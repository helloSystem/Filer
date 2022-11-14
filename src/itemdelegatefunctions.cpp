#include "itemdelegatefunctions.h"

#include <QFileInfo>
#include <QPainter>
#include <QProcess>
#include <QString>
#include <QStyleOptionViewItem>

#include <libfm/fm-file-info.h>

void drawGitEmblem(QString path, QPainter* painter, QPoint iconPos, QStyleOptionViewItem opt, FmFileInfo* file, QIcon::Mode iconMode)
{
    QString mimetype = QString::fromUtf8(fm_mime_type_get_type(fm_mime_type_ref(fm_file_info_get_mime_type(file))));
    if (mimetype == "inode/directory" && QFileInfo(path + "/.git").exists()) {
        // Check git status
        QProcess p;
        QProcessEnvironment env;
        env.insert("LANG", "C");
        p.setProcessEnvironment(env);
        p.setProgram("git");
        p.setWorkingDirectory(path);
        p.setArguments({"status", "-s"});
        p.start();
        p.waitForFinished();
        bool gitHasChanges = p.readAllStandardOutput().length();

        QIcon emblemIcon;
        emblemIcon = QIcon::fromTheme("emblem-symbolic-git");
//        if(gitHasChanges) {
//            emblemIcon = QIcon::fromTheme("emblem-symbolic-git-red");
//        }
        QPoint emblemPos = iconPos;
        emblemPos.setX(emblemPos.x() + opt.decorationSize.width()/4);
        emblemPos.setY(emblemPos.y() + opt.decorationSize.height()/2.6);
        if(! gitHasChanges) {
             painter->setOpacity(0.5);
        } else {
            painter->setOpacity(0.1);
        }
        painter->drawPixmap(emblemPos, emblemIcon.pixmap(opt.decorationSize / 2, iconMode));
        painter->setOpacity(1);
    }
}
