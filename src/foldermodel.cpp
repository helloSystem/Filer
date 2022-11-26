/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Hong Jen Yee (PCMan) <pcman.tw@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "foldermodel.h"
#include "icontheme.h"
#include <iostream>
#include <QtAlgorithms>
#include <QVector>
#include <qmimedata.h>
#include <QMimeData>
#include <QByteArray>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QFileInfo>
#include <QStorageInfo>
#include <QProcess>
#include <QMessageBox>
#include "utilities.h"
#include "fileoperation.h"
#include "thumbnailloader.h"
#include "folderview.h"

#include "application.h"
#include "fm-path.h"

using namespace Fm;

FolderModel::FolderModel() :
  folder_(NULL),
  computerFolder_(NULL) {
/*
    ColumnIcon,
    ColumnName,
    ColumnFileType,
    ColumnMTime,
    NumOfColumns
*/
  qDebug("probono: FolderModel created");
  thumbnailRefCounts.reserve(4);

  // reload all icons when the icon theme is changed
  connect(IconTheme::instance(), &IconTheme::changed, this, &FolderModel::updateIcons);
}

FolderModel::~FolderModel() {
  qDebug("probono: FolderModel destroyed");

  if(folder_)
    setFolder(NULL);

  if (computerFolder_)
    g_object_unref(computerFolder_);

  // if the thumbnail requests list is not empty, cancel them
  if(!thumbnailResults.empty()) {
    Q_FOREACH(FmThumbnailLoader* res, thumbnailResults) {
      ThumbnailLoader::cancel(res);
    }
  }
}

void FolderModel::setFolder(FmFolder* new_folder, bool add_devices) {
  if(folder_) {
    removeAll();        // remove old items
    g_signal_handlers_disconnect_by_func(folder_, gpointer(onStartLoading), this);
    g_signal_handlers_disconnect_by_func(folder_, gpointer(onFinishLoading), this);
    g_signal_handlers_disconnect_by_func(folder_, gpointer(onFilesAdded), this);
    g_signal_handlers_disconnect_by_func(folder_, gpointer(onFilesChanged), this);
    g_signal_handlers_disconnect_by_func(folder_, gpointer(onFilesRemoved), this);
    g_object_unref(folder_);
  }
  if (computerFolder_) {
      // these need to get added again
      g_signal_handlers_disconnect_by_func(computerFolder_, gpointer(onStartLoading), this);
      g_signal_handlers_disconnect_by_func(computerFolder_, gpointer(onFinishLoading), this);
      g_signal_handlers_disconnect_by_func(computerFolder_, gpointer(onFilesAdded), this);
      g_signal_handlers_disconnect_by_func(computerFolder_, gpointer(onFilesChanged), this);
      g_signal_handlers_disconnect_by_func(computerFolder_, gpointer(onFilesRemoved), this);
      g_object_unref(computerFolder_);
  }
  if(new_folder) {
    folder_ = FM_FOLDER(g_object_ref(new_folder));
    g_signal_connect(folder_, "start-loading", G_CALLBACK(onStartLoading), this);
    g_signal_connect(folder_, "finish-loading", G_CALLBACK(onFinishLoading), this);
    g_signal_connect(folder_, "files-added", G_CALLBACK(onFilesAdded), this);
    g_signal_connect(folder_, "files-changed", G_CALLBACK(onFilesChanged), this);
    g_signal_connect(folder_, "files-removed", G_CALLBACK(onFilesRemoved), this);
    // handle the case if the folder is already loaded
    if(fm_folder_is_loaded(folder_))
      insertFiles(0, fm_folder_get_files(folder_));
  }
  else
    folder_ = NULL;

  if (add_devices) {
    computerFolder_ = fm_folder_from_path(fm_path_new_for_path("/media")); // No more computer:///
    if (computerFolder_) {
      g_signal_connect(computerFolder_, "start-loading", G_CALLBACK(onStartLoading), this);
      g_signal_connect(computerFolder_, "finish-loading", G_CALLBACK(onFinishLoading), this);
      g_signal_connect(computerFolder_, "files-added", G_CALLBACK(onFilesAdded), this);
      g_signal_connect(computerFolder_, "files-changed", G_CALLBACK(onFilesChanged), this);
      g_signal_connect(computerFolder_, "files-removed", G_CALLBACK(onFilesRemoved), this);
      // handle the case if the folder is already loaded
      if(fm_folder_is_loaded(computerFolder_))
        insertFiles(0, fm_folder_get_files(computerFolder_));
    }
  }
  else {
      computerFolder_ = NULL;
  }
}

void FolderModel::onStartLoading(FmFolder* folder, gpointer user_data) {
  FolderModel* model = static_cast<FolderModel*>(user_data);
  // remove all items
  model->removeAll();
}

void FolderModel::onFinishLoading(FmFolder* folder, gpointer user_data) {
  Q_UNUSED(folder)
  //Q_UNUSED(user_data)
  ((FolderModel *)user_data)->onFinishedLoading();
}

void FolderModel::onFinishedLoading() {
  if(filesToSelect.count() > 0)
    ((FolderView *)filesToSelectView)->selectFiles(filesToSelect, filesToSelectAdd);
  filesToSelect.clear();
}

void FolderModel::wantToSelect(QStringList files, bool add, void *view) {
  filesToSelect = files;
  filesToSelectAdd = add;
  filesToSelectView = view;
}

void FolderModel::onFilesAdded(FmFolder* folder, GSList* files, gpointer user_data) {
  qDebug() << "FolderModel::onFilesAdded";
  FolderModel* model = static_cast<FolderModel*>(user_data);

  // OK, in this very specific case where the files are refreshed
  // the model is emptied in onStartLoading and we need to add the
  // device files again when it is reconstructed
  if (model->computerFolder()
      && (folder != model->computerFolder())
      && (model->items.count() == 0)) {
    FmFileInfoList* computerFiles = fm_folder_get_files(model->computerFolder());
    int n_files = fm_file_info_list_get_length(computerFiles);
    model->beginInsertRows(QModelIndex(), 0, n_files - 1);
    for(GList* l = fm_file_info_list_peek_head_link(computerFiles); l; l = l->next) {
      FolderModelItem item(FM_FILE_INFO(l->data));
      model->items.append(item);
    }
    model->endInsertRows();
  }

  int n_files = g_slist_length(files);
  model->beginInsertRows(QModelIndex(), model->items.count(), model->items.count() + n_files - 1);
  for(GSList* l = files; l; l = l->next) {
    FmFileInfo* info = FM_FILE_INFO(l->data);
    FolderModelItem item(info);
/*
    if(fm_file_info_is_hidden(info)) {
      model->hiddenItems.append(item);
      continue;
    }
*/
    model->items.append(item);
  }
  model->endInsertRows();
}

//static
void FolderModel::onFilesChanged(FmFolder* folder, GSList* files, gpointer user_data) {
  qDebug() << "FolderModel::onFilesChanged";
  FolderModel* model = static_cast<FolderModel*>(user_data);
  for(GSList* l = files; l; l = l->next) {
    FmFileInfo* info = FM_FILE_INFO(l->data);
    int row;
    QList<FolderModelItem>::iterator it = model->findItemByFileInfo(info, &row);
    if(it != model->items.end()) {
      FolderModelItem& item = *it;
      // try to update the item
      item.displayName = QString::fromUtf8(fm_file_info_get_disp_name(info));
      item.updateIcon();
      item.thumbnails.clear();
      QModelIndex index = model->createIndex(row, 0, &item);
      Q_EMIT model->dataChanged(index, index);
    }
  }
}

//static
void FolderModel::onFilesRemoved(FmFolder* folder, GSList* files, gpointer user_data) {  
  qDebug() << "FolderModel::onFilesRemoved";
  FolderModel* model = static_cast<FolderModel*>(user_data);
  for(GSList* l = files; l; l = l->next) {
    FmFileInfo* info = FM_FILE_INFO(l->data);
    const char* name = fm_file_info_get_name(info);
    int row;
    QList<FolderModelItem>::iterator it = model->findItemByName(name, &row);
    if(it != model->items.end()) {
      model->beginRemoveRows(QModelIndex(), row, row);
      model->items.erase(it);
      model->endRemoveRows();
    }
  }
}

void FolderModel::insertFiles(int row, FmFileInfoList* files) {
  int n_files = fm_file_info_list_get_length(files);
  beginInsertRows(QModelIndex(), row, row + n_files - 1);
  for(GList* l = fm_file_info_list_peek_head_link(files); l; l = l->next) {
    FolderModelItem item(FM_FILE_INFO(l->data));
    items.append(item);
  }
  endInsertRows();
}

void FolderModel::removeAll() {
  if(items.empty())
    return;
  beginRemoveRows(QModelIndex(), 0, items.size() - 1);
  items.clear();
  endRemoveRows();
}

int FolderModel::rowCount(const QModelIndex & parent) const {
  if(parent.isValid())
    return 0;
  return items.size();
}

int FolderModel::columnCount (const QModelIndex & parent = QModelIndex()) const {
  if(parent.isValid())
    return 0;
  return NumOfColumns;
}

FolderModelItem* FolderModel::itemFromIndex(const QModelIndex& index) const {
  return reinterpret_cast<FolderModelItem*>(index.internalPointer());
}

FmFileInfo* FolderModel::fileInfoFromIndex(const QModelIndex& index) const {
  FolderModelItem* item = itemFromIndex(index);
  return item ? item->info : NULL;
}

QVariant FolderModel::data(const QModelIndex & index, int role = Qt::DisplayRole) const {
  if(!index.isValid() || index.row() > items.size() || index.column() >= NumOfColumns) {
    return QVariant();
  }
  FolderModelItem* item = itemFromIndex(index);
  FmFileInfo* info = item->info;

  switch(role) {
    case Qt::ToolTipRole:
      return QVariant(item->displayName);
    case Qt::DisplayRole:  {
      switch(index.column()) {
        case ColumnFileName: {
          return QVariant(item->displayName);
        }
        case ColumnFileType: {
          FmMimeType* mime = fm_file_info_get_mime_type(info);
          const char* desc = fm_mime_type_get_desc(mime);
          return QString::fromUtf8(desc);
        }
        case ColumnFileMTime: {
          const char* name = fm_file_info_get_disp_mtime(info);
          return QString::fromUtf8(name);
        }
        case ColumnFileSize: {
          const char* name = fm_file_info_get_disp_size(info);
          return QString::fromUtf8(name);
        }
        case ColumnFileOwner: {
          const char* name = fm_file_info_get_disp_owner(info);
          return QString::fromUtf8(name);
        }
      }
    }
    case Qt::DecorationRole: {
      if(index.column() == 0) {
        // QPixmap pix = IconTheme::loadIcon(fm_file_info_get_icon(info), iconSize_);
        return QVariant(item->icon);
        // return QVariant(pix);
      }
      break;
    }
    case FileInfoRole:
      return qVariantFromValue((void*)info);
  }
  return QVariant();
}

QVariant FolderModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
  if(role == Qt::DisplayRole) {
    if(orientation == Qt::Horizontal) {
      QString title;
      switch(section) {
        case ColumnFileName:
          title = tr("Name");
          break;
        case ColumnFileType:
          title = tr("Type");
          break;
        case ColumnFileSize:
          title = tr("Size");
          break;
        case ColumnFileMTime:
          title = tr("Modified");
          break;
        case ColumnFileOwner:
          title = tr("Owner");
          break;
      }
      return QVariant(title);
    }
  }
  return QVariant();
}

QModelIndex FolderModel::index(int row, int column, const QModelIndex & parent) const {
  if(row <0 || row >= items.size() || column < 0 || column >= NumOfColumns)
    return QModelIndex();
  const FolderModelItem& item = items.at(row);
  return createIndex(row, column, (void*)&item);
}

QModelIndex FolderModel::parent(const QModelIndex & index) const {
  return QModelIndex();
}

Qt::ItemFlags FolderModel::flags(const QModelIndex& index) const {
  // FIXME: should not return same flags unconditionally for all columns
  Qt::ItemFlags flags;
  if(index.isValid()) {
    flags = Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    if(index.column() == ColumnFileName)
      flags |= (Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled);
  }
  else {
    flags = Qt::ItemIsDropEnabled;
  }
  return flags;
}

// FIXME: this is very inefficient and should be replaced with a
// more reasonable implementation later.
QList<FolderModelItem>::iterator FolderModel::findItemByPath(FmPath* path, int* row) {
  QList<FolderModelItem>::iterator it = items.begin();
  int i = 0;
  while(it != items.end()) {
    FolderModelItem& item = *it;
    FmPath* item_path = fm_file_info_get_path(item.info);
    if(fm_path_equal(item_path, path)) {
      *row = i;
      return it;
    }
    ++it;
    ++i;
  }
  return items.end();
}

// FIXME: this is very inefficient and should be replaced with a
// more reasonable implementation later.
QList<FolderModelItem>::iterator FolderModel::findItemByName(const char* name, int* row) {
  QList<FolderModelItem>::iterator it = items.begin();
  int i = 0;
  while(it != items.end()) {
    FolderModelItem& item = *it;
    const char* item_name = fm_file_info_get_name(item.info);
    if(strcmp(name, item_name) == 0) {
      *row = i;
      return it;
    }
    ++it;
    ++i;
  }
  return items.end();
}

QList< FolderModelItem >::iterator FolderModel::findItemByFileInfo(FmFileInfo* info, int* row) {
  QList<FolderModelItem>::iterator it = items.begin();
  int i = 0;
  while(it != items.end()) {
    FolderModelItem& item = *it;
    if(item.info == info) {
      *row = i;
      return it;
    }
    ++it;
    ++i;
  }
  return items.end();
}

QStringList FolderModel::mimeTypes() const {
  qDebug("FolderModel::mimeTypes");
  QStringList types = QAbstractItemModel::mimeTypes();
  // now types contains "application/x-qabstractitemmodeldatalist"
  types << "text/uri-list";
  // types << "x-special/gnome-copied-files";
  return types;
}

QMimeData* FolderModel::mimeData(const QModelIndexList& indexes) const {
  QMimeData* data = QAbstractItemModel::mimeData(indexes);
  qDebug("FolderModel::mimeData");
  // build a uri list
  QByteArray urilist;
  urilist.reserve(4096);

  QModelIndexList::const_iterator it;
  for(it = indexes.constBegin(); it != indexes.end(); ++it) {
    const QModelIndex index = *it;
    FolderModelItem* item = itemFromIndex(index);
    if(item) {
      FmPath* path = fm_file_info_get_path(item->info);
      char* uri = fm_path_to_uri(path);
      urilist.append(uri);
      urilist.append('\n');
      g_free(uri);
    }
  }
  data->setData("text/uri-list", urilist);

  return data;
}

bool FolderModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {
  qDebug("FolderModel::dropMimeData");
  if(!folder_)
    return false;
  FmPath* destPath;
  if(parent.isValid()) { // drop on an item
    FmFileInfo* info;
    if(row == -1 && column == -1)
      info = fileInfoFromIndex(parent);
    else {
      QModelIndex itemIndex = parent.child(row, column);
      info = fileInfoFromIndex(itemIndex);
    }
    if(info)
      destPath = fm_file_info_get_path(info);
    else
      return false;
  }
  else { // drop on blank area of the folder
    destPath = path();
  }

  // FIXME: should we put this in dropEvent handler of FolderView instead?
  if(data->hasUrls()) {
      qDebug("drop outside of an item, action: %d", action);
      FmPathList* srcPaths = pathListFromQUrls(data->urls());
      bool identicalLocations = false;
      GList* l;
      if (action == Qt::CopyAction) {
          // probono: Do not attempt to do a copy in the filesystem if source and destination paths are identical
          // This is important for the Desktop, where apparently moving things on the desktop defaults to copying
          // FIXME: Replace things like FmPathList* with QList.
          // Why is it so complicated to do "For each in srcPaths, do..."?
          // This is the issue I have with C++. Instead of a list being a list, we have now a "list of paths" that behaves unlike any other list.
          // C++ is ok as long as one limits oneself to using what Qt provides.
          // But as soon as other libraries such as libfm creep in, it becomes insanely cumbersome.
          // We should drop all remnants of glib and libfm and do everything natively in Qt. Volunteers?
          for(l = fm_path_list_peek_head_link(srcPaths); l; l = l->next) {
              FmPath* path = FM_PATH(l->data);
              QString pathStr = QString::fromUtf8(fm_path_display_name(path, false)); // Unlike for MoveAction, do not use fm_path_get_parent here
              QString destPathStr = QString::fromUtf8(fm_path_display_name(destPath, false));
              qDebug() << "probono: pathStr" << pathStr;
              qDebug() << "probono: destPathStr" << destPathStr;

              if (pathStr == destPathStr){
                  identicalLocations = true;
                  break;
              }
          }
          if(identicalLocations == true) {
              qDebug() << "probono: Source and destination paths are identical";
          } else {
              FileOperation::copyFiles(srcPaths, destPath);
          }
      } else if (action == Qt::MoveAction) {
          bool sourcePathsContainMountpoints = false;
          // Bevore we move anything, do a couple of checks:
          // Are source and target identical? Are objects being moved to the Trash?
          QString destPathStr = QString(fm_path_to_str(destPath));
          for(l = fm_path_list_peek_head_link(srcPaths); l; l = l->next) {
              FmPath* path = FM_PATH(l->data);
              QString sourcePathStr =  QString(fm_path_to_str(path));
              qDebug() << "probono: pathStr" << sourcePathStr;
              qDebug() << "probono: destPathStr" << destPathStr;
              if (sourcePathStr == destPathStr){
                  // probono: Do not attempt to do a move in the filesystem if source and destination paths are identical.
                  // This is important for Filer windows
                  // FIXME: Same as above in "case Qt::CopyAction"
                  qDebug() << "probono: Source and destination paths are identical";
                  return false;
              }
              // probono: If destination is trash-can.desktop, then move to trash
              // using the proper facility
              if (QFileInfo(destPathStr).fileName() == "trash-can.desktop"){
                  for (const QStorageInfo storageInfo : QStorageInfo::mountedVolumes()) {
                      if(storageInfo.rootPath() == sourcePathStr) {
                          qDebug() << sourcePathStr << "is a mountpoint";
                          sourcePathsContainMountpoints = true;
                          break;
                      }
                  }
              }
          }

          if (QFileInfo(destPathStr).fileName() == "trash-can.desktop"){
              qDebug() << "sourcePathsContainMountpoints:" << sourcePathsContainMountpoints;
              if(sourcePathsContainMountpoints == false) {
                  Filer::Application* app = static_cast<Filer::Application*>(qApp);
                  FileOperation::trashFiles(srcPaths, app->settings().confirmTrash());
                  return true;
              } else {
                  // Similar code is in filemenu.cpp
                  // Do the unmounting natively in Qt without the need for an external program
                  // The dark side does this with something like
                  // GVolume* volume = volumeItem->volume();
                  // op->unmount(volumeItem->volume());
                  for(l = fm_path_list_peek_head_link(srcPaths); l; l = l->next) {
                      FmPath* path = FM_PATH(l->data);
                      QString sourcePathStr =  QString(fm_path_to_str(path));
                      QProcess p;
                      p.setProgram("eject-and-clean");
                      p.setArguments({sourcePathStr});
                      qDebug() << p.program() << p.arguments();
                      p.start();
                      p.waitForFinished();
                      qDebug() <<  "p.exitCode():" << p.exitCode();
                      if(p.exitCode() == 0) {
                          return true;
                      }
                      else {
                          QMessageBox::warning(nullptr, " ", QString("Cannot eject %1, 'eject-and-clean' command line tool missing or returned an error.").arg(sourcePathStr));
                          return false;
                      }
                  }
              }
          }

          FileOperation::moveFiles(srcPaths, destPath);
          return true;
      } else if (action == Qt::LinkAction) {
          FileOperation::symlinkFiles(srcPaths, destPath);
      } else {

          fm_path_list_unref(srcPaths);
          return false;

      }
      fm_path_list_unref(srcPaths);
      return true;
  }
  else if(data->hasFormat("application/x-qabstractitemmodeldatalist")) {
      return true;
  }
  return QAbstractListModel::dropMimeData(data, action, row, column, parent);
}

Qt::DropActions FolderModel::supportedDropActions() const {
  qDebug("FolderModel::supportedDropActions");
  return Qt::CopyAction|Qt::MoveAction|Qt::LinkAction;
}

// ask the model to load thumbnails of the specified size
void FolderModel::cacheThumbnails(int size) {
  QVector<QPair<int, int> >::iterator it;
  for(it = thumbnailRefCounts.begin(); it != thumbnailRefCounts.end(); ++it) {
    if(it->first == size) {
      break;
    }
  }
  if(it != thumbnailRefCounts.end())
    ++it->second;
  else
    thumbnailRefCounts.append(QPair<int, int>(size, 1));
}

// ask the model to free cached thumbnails of the specified size
void FolderModel::releaseThumbnails(int size) {
  QVector<QPair<int, int> >::iterator it;
  for(it = thumbnailRefCounts.begin(); it != thumbnailRefCounts.end(); ++it) {
    if(it->first == size) {
      break;
    }
  }
  if(it != thumbnailRefCounts.end()) {
    --it->second;
    if(it->second == 0) {
      thumbnailRefCounts.erase(it);

      // remove thumbnails that ara queued for loading from thumbnailResults
      QLinkedList<FmThumbnailLoader*>::iterator it;
      for(it = thumbnailResults.begin(); it != thumbnailResults.end();) {
        QLinkedList<FmThumbnailLoader*>::iterator next = it + 1;
        FmThumbnailLoader* res = *it;
        if(ThumbnailLoader::size(res) == size) {
          ThumbnailLoader::cancel(res);
          thumbnailResults.erase(it);
        }
        it = next;
      }

      // remove all cached thumbnails of the specified size
      QList<FolderModelItem>::iterator itemIt;
      for(itemIt = items.begin(); itemIt != items.end(); ++itemIt) {
        FolderModelItem& item = *itemIt;
        item.removeThumbnail(size);
      }
    }
  }
}

void FolderModel::onThumbnailLoaded(FmThumbnailLoader* res, gpointer user_data) {
  FolderModel* pThis = reinterpret_cast<FolderModel*>(user_data);
  QLinkedList<FmThumbnailLoader*>::iterator it;
  for(it = pThis->thumbnailResults.begin(); it != pThis->thumbnailResults.end(); ++it) {
    if(*it == res) { // the thumbnail result is in our list
      pThis->thumbnailResults.erase(it); // remove it from the list
      FmFileInfo* info = ThumbnailLoader::fileInfo(res);
      int row = -1;
      // find the model item this thumbnail belongs to
      QList<FolderModelItem>::iterator it = pThis->findItemByFileInfo(info, &row);
      if(it != pThis->items.end()) {
        // the file is found in our model
        FolderModelItem& item = *it;
        QModelIndex index = pThis->createIndex(row, 0, (void*)&item);
        // store the image in the folder model item.
        int size = ThumbnailLoader::size(res);
        QImage image = ThumbnailLoader::image(res);
        FolderModelItem::Thumbnail* thumbnail = item.findThumbnail(size);
        thumbnail->image = image;
        // qDebug("thumbnail loaded for: %s, size: %d", item.displayName.toUtf8().constData(), size);
        if(image.isNull())
          thumbnail->status = FolderModelItem::ThumbnailFailed;
        else {
          thumbnail->status = FolderModelItem::ThumbnailLoaded;
          // FIXME: due to bugs in Qt's QStyledItemDelegate, if the image width and height
          // are not the same, painting errors will happen. It's quite unfortunate.
          // Let's do some padding to make its width and height equals.
          // This greatly decrease performance :-(
          // Later if we can re-implement our own item delegate, this can be avoided.
          QPixmap pixmap = QPixmap(size, size);
          pixmap.fill(QColor(0, 0, 0, 0)); // fill the pixmap with transparent color (alpha:0)
          QPainter painter(&pixmap);
          int x = (size - image.width()) / 2;
          int y = (size - image.height()) / 2;
          painter.drawImage(QPoint(x, y), image); // draw the image to the pixmap at center.
          // FIXME: should we cache QPixmap instead for performance reason?
          thumbnail->image = pixmap.toImage(); // convert it back to image

          // tell the world that we have the thumbnail loaded
          Q_EMIT pThis->thumbnailLoaded(index, size);
        }
      }
      break;
    }
  }
}

// get a thumbnail of size at the index
// if a thumbnail is not yet loaded, this will initiate loading of the thumbnail.
QImage FolderModel::thumbnailFromIndex(const QModelIndex& index, int size) {
  FolderModelItem* item = itemFromIndex(index);
  if(item) {
    FolderModelItem::Thumbnail* thumbnail = item->findThumbnail(size);
    // qDebug("FolderModel::thumbnailFromIndex: %d, %s", thumbnail->status, item->displayName.toUtf8().data());
    switch(thumbnail->status) {
      case FolderModelItem::ThumbnailNotChecked: {
        // load the thumbnail
        FmThumbnailLoader* res = ThumbnailLoader::load(item->info, size, onThumbnailLoaded, this);
        thumbnailResults.push_back(res);
        thumbnail->status = FolderModelItem::ThumbnailLoading;
        break;
      }
      case FolderModelItem::ThumbnailLoaded:
        return thumbnail->image;
      default:
        break;
    }
  }
  return QImage();
}

void FolderModel::updateIcons() {
  QList<FolderModelItem>::iterator it = items.begin();
  for(;it != items.end(); ++it) {
    (*it).updateIcon();
  }
}
