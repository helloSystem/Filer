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

#include "folderitemdelegate.h"
#include "foldermodel.h"
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QIcon>
#include <QTextLayout>
#include <QTextOption>
#include <QTextLine>
#include <QDebug>
#include <QPainterPath>
#include <QColor>
#include <QFileInfo>
#include <QProcess>

# include "extattrs.h"

namespace Fm {

FolderItemDelegate::FolderItemDelegate(QAbstractItemView* view, QObject* parent):
  QStyledItemDelegate(parent ? parent : view),
  symlinkIcon_(QIcon::fromTheme("emblem-symbolic-link")),
  view_(view) {
    qDebug() << "probono: FolderItemDelegate::FolderItemDelegate created";
}

FolderItemDelegate::~FolderItemDelegate() {
    qDebug() << "probono: FolderItemDelegate::FolderItemDelegate destroyed";
}

QSize FolderItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QVariant value = index.data(Qt::SizeHintRole);
  if(value.isValid())
    return qvariant_cast<QSize>(value);
  if(option.decorationPosition == QStyleOptionViewItem::Top ||
    option.decorationPosition == QStyleOptionViewItem::Bottom) {

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.decorationAlignment = Qt::AlignHCenter|Qt::AlignTop;
    opt.displayAlignment = Qt::AlignTop|Qt::AlignHCenter;

    // FIXME: there're some problems in this size hint calculation.
    Q_ASSERT(gridSize_ != QSize());
    QRectF textRect(0, 0, gridSize_.width() - 4, gridSize_.height() - opt.decorationSize.height() - 4);
    drawText(NULL, opt); // passing NULL for painter will calculate the bounding rect only.
    int width = qMax((int)textRect.width(), opt.decorationSize.width()) + 4;
    int height = opt.decorationSize.height() + textRect.height() + 4;
    return QSize(width, height);
  }
  return QStyledItemDelegate::sizeHint(option, index);
}

QIcon::Mode FolderItemDelegate::iconModeFromState(QStyle::State state) {
  QIcon::Mode iconMode;
  if(state & QStyle::State_Enabled) {
    if(state & QStyle::State_Selected)
      iconMode = QIcon::Selected;
    else {
      iconMode = QIcon::Normal;
    }
  }
  else
    iconMode = QIcon::Disabled;
  return iconMode;
}

// special thanks to Razor-qt developer Alec Moskvin(amoskvin) for providing the fix!
void FolderItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  Q_ASSERT(index.isValid());
  FmFileInfo* file = static_cast<FmFileInfo*>(index.data(FolderModel::FileInfoRole).value<void*>());
  bool isSymlink = file && fm_file_info_is_symlink(file);

  if(option.decorationPosition == QStyleOptionViewItem::Top ||
    option.decorationPosition == QStyleOptionViewItem::Bottom) {
    painter->save();
    painter->setClipRect(option.rect);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.decorationAlignment = Qt::AlignHCenter|Qt::AlignTop;
    opt.displayAlignment = Qt::AlignTop|Qt::AlignHCenter;

    QString path = QString::fromUtf8(fm_path_to_str(fm_file_info_get_path(file))); // probono: rgh! So complicated! TODO: Get rid of libfm

    // If the trash is full, use a different icon for the trash can
    if(QFileInfo(path).fileName() == "trash-can.desktop") {

    }


    // draw the icon
    QIcon::Mode iconMode = iconModeFromState(opt.state);
    QPoint iconPos(opt.rect.x() + (opt.rect.width() - opt.decorationSize.width()) / 2, opt.rect.y());

    // probono: If the icon is selected, draw a light rounded rect in the background
    if(opt.state & QStyle::State_Selected) {
      QRectF boundRect = QRectF();
      boundRect.setTop(iconPos.y());
      boundRect.setLeft(iconPos.x());
      boundRect.setHeight(opt.decorationSize.height());
      boundRect.setWidth(opt.decorationSize.width());
      painter->setRenderHint(QPainter::Antialiasing);
      QPainterPath path = QPainterPath();
      path.addRoundedRect(boundRect, 4, 4);
      painter->fillPath(path, QColor(196, 196, 196)); // Light gray
    }

    QPixmap pixmap = opt.icon.pixmap(opt.decorationSize, iconMode);
    painter->drawPixmap(iconPos, pixmap);

    QPoint symlinkPos = iconPos;
    symlinkPos.setY(symlinkPos.y() + opt.decorationSize.height() / 2);

    // draw some emblems for the item if needed
    // we only support symlink emblem at the moment
    if(isSymlink)
      painter->drawPixmap(symlinkPos, symlinkIcon_.pixmap(opt.decorationSize / 2, iconMode));

    // probono: git labels
    // FIXME: Why doesn't this work?
    // Fm::drawGitEmblem(path, painter, iconPos, opt, file ,iconMode);

    // ======================================= Start ugly code duplication

    // probono: git labels
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
        if(gitHasChanges) {
             painter->setOpacity(0.5);
        } else {
            painter->setOpacity(0.1);
        }
        painter->drawPixmap(emblemPos, emblemIcon.pixmap(opt.decorationSize / 2, iconMode));
        painter->setOpacity(1);
    }

    // ======================================= End ugly code duplication

    // probono: Draw label emblems
    bool ok;
    QString emblem = Fm::getAttributeValueQString(path, "EMBLEM", ok);
    if (ok) {
        QIcon emblemIcon = QIcon::fromTheme(emblem);
        QPoint emblemIconPos(opt.decorationSize.width() * 0.75 + opt.rect.x() + (opt.rect.width() - opt.decorationSize.width()) / 2, opt.rect.y());
        painter->drawPixmap(emblemIconPos, emblemIcon.pixmap(opt.decorationSize / 2, iconMode));
    }

    // draw the text
    drawText(painter, opt);
    painter->restore();
  }
  else {
    // let QStyledItemDelegate does its default painting
    QStyledItemDelegate::paint(painter, option, index);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QIcon::Mode iconMode = iconModeFromState(opt.state);

    // draw emblems if needed
    if(isSymlink) {
      QPoint iconPos(opt.rect.x(), opt.rect.y() + (opt.rect.height() - opt.decorationSize.height()) / 2);
      // draw some emblems for the item if needed
      // we only support symlink emblem at the moment
      QPoint symlinkPos = iconPos;
      symlinkPos.setY(symlinkPos.y() + opt.decorationSize.height() / 2);
      painter->drawPixmap(symlinkPos, symlinkIcon_.pixmap(opt.decorationSize / 2, iconMode));
    }

    // probono: Draw label emblems
    QString path = QString::fromUtf8(fm_path_to_str(fm_file_info_get_path(file))); // argh! So complicated!
    bool ok;
    QString emblem = Fm::getAttributeValueQString(path, "EMBLEM", ok);
    if (ok) {
        QIcon emblemIcon = QIcon::fromTheme(emblem);
        QPoint emblemIconPos(opt.decorationSize.width() * 0.75 + opt.rect.x() + (opt.rect.width() - opt.decorationSize.width()) / 2, opt.rect.y());
        painter->drawPixmap(emblemIconPos, emblemIcon.pixmap(opt.decorationSize / 2, iconMode));
    }
  }
}

void FolderItemDelegate::drawText(QPainter* painter, QStyleOptionViewItem& opt) const {

    // Calculate size of textRect
    int leftAndRightSpace = 20;
    QRectF textRect(opt.rect.x() + leftAndRightSpace,
                    opt.rect.y() + 3 + opt.decorationSize.height(),
                    opt.rect.width() - leftAndRightSpace * 2,
                    opt.rect.height() - opt.decorationSize.height());
    QTextLayout layout(opt.text, opt.font);

    QTextOption textOption;
    textOption.setAlignment(opt.displayAlignment);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textOption.setTextDirection(opt.direction);
    layout.setTextOption(textOption);
    qreal height = 0;
    qreal width = 0;
    int visibleLines = 0;
    layout.beginLayout();
    QString elidedText;

  for(;;) {
    QTextLine line = layout.createLine();
    if(!line.isValid())
      break;
    line.setLineWidth(textRect.width());
    QFontMetrics fontMetrics(opt.font);
    height += fontMetrics.leading();
    line.setPosition(QPointF(0, height));
    if((height + line.height() + textRect.y()) + 3 > textRect.bottom()) {
        // if part of this line falls outside the textRect, ignore it and quit.
        QTextLine lastLine = layout.lineAt(visibleLines - 1);
        elidedText = opt.text.mid(lastLine.textStart());
        opt.textElideMode = Qt::ElideMiddle; // probono: Put ... in the middle, not at the end so that we can see the suffix
        elidedText = fontMetrics.elidedText(elidedText, opt.textElideMode, textRect.width());
        break;
    }
    height += line.height();
    ++ visibleLines;
    width = qMax(width, line.naturalTextWidth());
  }
  layout.endLayout();

  // probono: draw background rounded rect for selected item
  QRectF boundRect = layout.boundingRect();
  int additionalSpace = 1;
  boundRect.setWidth(width + 16 * additionalSpace);

  // boundRect.setHeight(boundRect.height() + 2 * additionalSpace); // probono: This was wrong, resulting in some background rounded rects being too high
  boundRect.setHeight(height + additionalSpace);
  boundRect.moveTo(textRect.x() - 8*additionalSpace + (textRect.width() - width)/2,
                   textRect.y() - additionalSpace);

  if(!painter) { // no painter, calculate the bounding rect only
    textRect = boundRect;
    return;
  }

  QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
  if(opt.state & QStyle::State_Selected) {
    // painter->fillRect(boundRect, opt.palette.highlight());
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path = QPainterPath();
    path.addRoundedRect(boundRect, 8, 8);
    painter->fillPath(path, opt.palette.highlight());
    painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
  }
  else
    painter->setPen(opt.palette.color(cg, QPalette::Text));

  // draw text
  for(int i = 0; i < visibleLines; ++i) {
    QTextLine line = layout.lineAt(i);
    if(i == (visibleLines - 1) && !elidedText.isEmpty()) { // the last line, draw elided text
      QPointF pos(textRect.x() + line.position().x(), textRect.y() + line.y() + line.ascent());
      painter->drawText(pos, elidedText);
    }
    else {
      line.draw(painter, textRect.topLeft());
    }
  }

  /* probono: We don't want FocusRects
  if(opt.state & QStyle::State_HasFocus) {
    // draw focus rect
    QStyleOptionFocusRect o;
    o.QStyleOption::operator=(opt);
    o.rect = boundRect.toRect(); // subElementRect(SE_ItemViewItemFocusRect, vopt, widget);
    o.state |= QStyle::State_KeyboardFocusChange;
    o.state |= QStyle::State_Item;
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
                  ? QPalette::Normal : QPalette::Disabled;
    o.backgroundColor = opt.palette.color(cg, (opt.state & QStyle::State_Selected)
                                  ? QPalette::Highlight : QPalette::Window);
    if (const QWidget* widget = opt.widget) {
      QStyle* style = widget->style() ? widget->style() : qApp->style();
      style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);
    }
  }
  */

}


} // namespace Fm
