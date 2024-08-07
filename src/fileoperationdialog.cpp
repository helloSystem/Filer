/*

    Copyright (C) 2013  Hong Jen Yee (PCMan) <pcman.tw@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "fileoperationdialog.h"
#include "fileoperation.h"
#include "renamedialog.h"
#include <QMessageBox>
#include "ui_file-operation-dialog.h"

using namespace Fm;

FileOperationDialog::FileOperationDialog(FileOperation* _operation):
  QDialog(NULL),
  operation(_operation),
  defaultOption(-1) {

  ui = new Ui::FileOperationDialog();
  ui->setupUi(this);

  this->setFixedSize(this->width(),this->height());

  QString title;
  QString message;
  switch(_operation->type()) {
  case FM_FILE_OP_MOVE:
      title = tr("Move files");
      message = tr("Moving the following files to destination folder:");
      break;
  case FM_FILE_OP_COPY:
      title = tr("Copy Files");
      message = tr("Copying the following files to destination folder:");
      break;
  case FM_FILE_OP_TRASH:
      title = tr("Trash Files");
      message = tr("Moving the following files to trash can:");
      break;
  case FM_FILE_OP_DELETE:
      title = tr("Delete Files");
      message = tr("Deleting the following files");
      ui->dest->hide();
      ui->destLabel->hide();
      break;
  case FM_FILE_OP_LINK:
      title = tr("Create Symlinks");
      message = tr("Creating symlinks for the following files:");
      break;
  case FM_FILE_OP_CHANGE_ATTR:
      title = tr("Change Attributes");
      message = tr("Changing attributes of the following files:");
      ui->dest->hide();
      ui->destLabel->hide();
      break;
  case FM_FILE_OP_UNTRASH:
      title = tr("Restore Trashed Files");
      message = tr("Restoring the following files from trash can:");
      ui->dest->hide();
      ui->destLabel->hide();
      break;
  }
  // ui->message->setText(message);
  setWindowTitle(title);
}


FileOperationDialog::~FileOperationDialog() {
  delete ui;
}

void FileOperationDialog::setDestPath(FmPath* dest) {
  char* pathStr = fm_path_display_name(dest, false);
  ui->dest->setText(QString::fromUtf8(pathStr));
  g_free(pathStr);
}

void FileOperationDialog::setSourceFiles(FmPathList* srcFiles) {
  /* probono: Do not show list of source files in file operation dialog
  GList* l;
  for(l = fm_path_list_peek_head_link(srcFiles); l; l = l->next) {
    FmPath* path = FM_PATH(l->data);
    char* pathStr = fm_path_display_name(path, false);
    ui->sourceFiles->addItem(QString::fromUtf8(pathStr));
    g_free(pathStr);
  }
  */
}

int FileOperationDialog::ask(QString question, char*const* options) {
  // TODO: implement FileOperationDialog::ask()
  return 0;
}

int FileOperationDialog::askRename(FmFileInfo* src, FmFileInfo* dest, QString& new_name) {
  int ret;
  if(defaultOption == -1) { // default action is not set, ask the user
    RenameDialog dlg(src, dest, this);
    dlg.exec();
    switch(dlg.action()) {
      case RenameDialog::ActionOverwrite:
        ret = FM_FILE_OP_OVERWRITE;
        if(dlg.applyToAll())
          defaultOption = ret;
        break;
      case RenameDialog::ActionRename:
        ret = FM_FILE_OP_RENAME;
        new_name = dlg.newName();
        break;
      case RenameDialog::ActionIgnore:
        ret = FM_FILE_OP_SKIP;
        if(dlg.applyToAll())
          defaultOption = ret;
        break;
      default:
        ret = FM_FILE_OP_CANCEL;
        break;
    }
  }
  else
    ret = defaultOption;
  return ret;
}

FmJobErrorAction FileOperationDialog::error(GError* err, FmJobErrorSeverity severity) {
  if(severity >= FM_JOB_ERROR_MODERATE) {
    QMessageBox::critical(this, tr("Error"), QString::fromUtf8(err->message));
    if(severity == FM_JOB_ERROR_CRITICAL)
      return FM_JOB_ABORT;
  }
  return FM_JOB_CONTINUE;
}

void FileOperationDialog::setCurFile(QString cur_file) {
  ui->curFile->setText(cur_file);
}

void FileOperationDialog::setPercent(unsigned int percent) {
  ui->progressBar->setValue(percent);
  // probono: Show the dialog once we have some progress since we don't show it from the beginning to allow any question/error boxes to appear first
  if(percent != 0){
      this->show();
  }
}

void FileOperationDialog::setRemainingTime(unsigned int sec) {
  unsigned int min = 0;
  unsigned int hr = 0;
  if(sec > 60) {
    min = sec / 60;
    sec %= 60;
    if(min > 60) {
      hr = min / 60;
      min %= 60;
    }
  }
  ui->timeRemaining->setText(QString("%1:%2:%3")
                             .arg(hr, 2, 10, QChar('0'))
                             .arg(min, 2, 10, QChar('0'))
                             .arg(sec, 2, 10, QChar('0')));
}

void FileOperationDialog::setPrepared() {
}

void FileOperationDialog::reject() {
  operation->cancel();
  QDialog::reject();
}

