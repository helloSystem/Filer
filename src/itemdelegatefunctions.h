// File added by probono

#ifndef ITEMDELEGATEFUNCS_H
#define ITEMDELEGATEFUNCS_H

#include <QFileInfo>
#include <QPainter>
#include <QProcess>
#include <QString>
#include <QStyleOptionViewItem>

#include <libfm/fm-file-info.h>


namespace Fm {

    void drawGitEmblem(QString path, QPainter* painter, QPoint iconPos, QStyleOptionViewItem opt, FmFileInfo* file, QIcon::Mode iconMode);
}

#endif // ITEMDELEGATEFUNCS_H
