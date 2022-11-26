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

    // TODO: This deletes the files but does not manage the trashinfo and directorysizes "database"
    // as per https://specifications.freedesktop.org/trash-spec/trashspec-1.0.html
    // Possibly there was never code for this in PCManFM-Qt for this because it was reyling on gvfs-trash
    // With xdg, everything is complicated. Even emptying the Trash.
    // Because it never stores information on the objects themselves (i.e., in extattrs).
    // Is there a Qt function that we could call to empty the Trash for us?
    // Or, as a last resort, a libfm function?

    QString trashPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/Trash";
    QString trashFilesPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/Trash/files";
    QString trashInfoPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/Trash/info";
    QDir trashDir(trashPath);
    QDir trashFilesDir(trashFilesPath);
    QDir trashInfoDir(trashInfoPath);

    trashFilesDir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::System);
    for(const QString dirItem : trashFilesDir.entryList()) {
        qDebug() << "Deleting:" << trashFilesDir.path() + dirItem;
        trashFilesDir.remove(dirItem);
    }

    trashFilesDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    for(const QString dirItem : trashFilesDir.entryList())
    {
        QDir subDir(trashFilesDir.absoluteFilePath(dirItem));
        if(QFileInfo(subDir.path()).isSymbolicLink()) {
            qDebug() << "Deleting:" << trashFilesDir.path() + subDir.path();
            trashFilesDir.remove(subDir.path());
        } else {
            qDebug() << "Deleting recursively:" << trashFilesDir.absoluteFilePath(dirItem);
            subDir.removeRecursively();
        }
    }

    // As long as we don't use it, we can just as well delete it. FIXME: Implement proper handling.
    qDebug() << "Deleting:" << trashInfoDir;
    trashInfoDir.removeRecursively();
    qDebug() << "Deleting:" << trashDir.path() + "/directorysizes";
    trashDir.remove("/directorysizes");
}
