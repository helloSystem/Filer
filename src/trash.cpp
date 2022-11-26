#include "trash.h"
#include "fileoperation.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include "sound.h"

void Fm::Trash::emptyTrash(){

    // TODO: This deletes the files but does not manage the trash "database"
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/Trash/files";
    QDir dir(path);

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::System);
    for(const QString dirItem : dir.entryList()) {
        qDebug() << "Deleting:" << dir.path() + dirItem;
        dir.remove(dirItem);
    }

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    for(const QString dirItem : dir.entryList())
    {
        QDir subDir(dir.absoluteFilePath(dirItem));
        if(QFileInfo(subDir.path()).isSymbolicLink()) {
            qDebug() << "Deleting:" << dir.path() + subDir.path();
            dir.remove(subDir.path());
        } else {
            qDebug() << "Deleting recursively:" << dir.absoluteFilePath(dirItem);
            subDir.removeRecursively();
        }
    }

}
