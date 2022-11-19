#include "trash.h"
#include "fileoperation.h"
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include "sound.h"

void Fm::Trash::emptyTrash(){
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/Trash/files";
    QDir dir(path);

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    for(const QString dirItem : dir.entryList())
        dir.remove( dirItem );

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    for(const QString dirItem : dir.entryList())
    {
        QDir subDir(dir.absoluteFilePath(dirItem));
        subDir.removeRecursively();
    }
}
