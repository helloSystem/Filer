/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "dnddest.h"
#include "fileoperation.h"
#include "utilities.h"

#include <QDebug>

using namespace Fm;

const char* supportedMimeTypes[] = {
  "text/uri-list"
  "XdndDirectSave0"/* X direct save */
  /* TODO: add more targets to support: text types, _NETSCAPE_URL, property/bgimage ... */
};

DndDest::DndDest() {

}

DndDest::~DndDest() {

}

bool DndDest::dropMimeData(const QMimeData* data, Qt::DropAction action) {
  // FIXME: should we put this in dropEvent handler of FolderView instead?
  qDebug() << "DndDest::dropMimeData";
  if(data->hasUrls()) {
    qDebug("drop action: %d", action);
    FmPathList* srcPaths = pathListFromQUrls(data->urls());
    switch(action) {
      case Qt::CopyAction:
        FileOperation::copyFiles(srcPaths, destPath_.data());
        break;
      case Qt::MoveAction:
        FileOperation::moveFiles(srcPaths, destPath_.data());
        break;
      case Qt::LinkAction:
        FileOperation::symlinkFiles(srcPaths, destPath_.data());
      default:
        fm_path_list_unref(srcPaths);
        return false;
    }
    fm_path_list_unref(srcPaths);
    return true;
  }
  return false;
}

bool DndDest::isSupported(const QMimeData* data) {
  return false;
}

bool DndDest::isSupported(QString mimeType) {
  return false;
}
