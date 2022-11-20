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


#include "desktopitemdelegate.h"
#include "foldermodel.h"
#include <QApplication>
#include <QListView>
#include <QPainter>
#include <QIcon>
#include <QTextLayout>
#include <QTextOption>
#include <QTextLine>
#include <QPainterPath>
#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QStorageInfo>

#include "extattrs.h"
#include "itemdelegatefunctions.h"

using namespace Filer;

DesktopItemDelegate::DesktopItemDelegate(QListView* view, QObject* parent):
  QStyledItemDelegate(parent ? parent : view),
  view_(view),
  symlinkIcon_(QIcon::fromTheme("emblem-symbolic-link")),
  shadowColor_(0, 0, 0) {
}

// FIXME: we need to figure out a way to derive from Fm::FolderItemDelegate to avoid code duplication.
void DesktopItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  Q_ASSERT(index.isValid());
  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  /*
  // Draw shadow
  // qDebug() << "index:" << index;
  QPen origPen = painter->pen();
  QRect shadowRect(QPoint(0, 0), QSize(1600, 33));
  QLinearGradient linearGradient(0,0,0,33);
  linearGradient.setColorAt(0.00, QColor::fromRgbF(0, 0, 0, 0.3));
  linearGradient.setColorAt(0.33, QColor::fromRgbF(0, 0, 0, 0.2));
  linearGradient.setColorAt(1.00, QColor::fromRgbF(0, 0, 0, 0.0));
  painter->fillRect(shadowRect,  linearGradient);
  painter->setPen(origPen);
  painter->save();
  painter->setClipRect(option.rect);
  */

  opt.decorationAlignment = Qt::AlignHCenter | Qt::AlignTop;
  opt.displayAlignment = Qt::AlignTop | Qt::AlignHCenter;

  // draw the icon
  QIcon::Mode iconMode;
  if(opt.state & QStyle::State_Enabled) {
    if(opt.state & QStyle::State_Selected)
      iconMode = QIcon::Selected;
    else {
      iconMode = QIcon::Normal;
    }
  }
  else
    iconMode = QIcon::Disabled;
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
    painter->fillPath(path, QColor::fromRgbF(196, 196, 196, 0.5)); // Light gray, 50% alpha
  }

  FmFileInfo* file = static_cast<FmFileInfo*>(index.data(Fm::FolderModel::FileInfoRole).value<void*>());

  if(file) {
      QPixmap pixmap = opt.icon.pixmap(opt.decorationSize, iconMode);
      painter->drawPixmap(iconPos, pixmap);
  }

  if(file) {
      if(fm_file_info_is_symlink(file)) {
          // draw some emblems for the item if needed
      // we only support symlink emblem at the moment
      QPoint symlinkPos = iconPos;
      symlinkPos.setY(symlinkPos.y() + opt.decorationSize.height() / 2);
      painter->drawPixmap(symlinkPos, symlinkIcon_.pixmap(opt.decorationSize / 2, iconMode));
    }
  }

  QString path = QString::fromUtf8(fm_path_to_str(fm_file_info_get_path(file))); // argh! So complicated!

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

  // Calculate size of textRect
  int leftAndRightSpace = 20;
  QRectF textRect(opt.rect.x() + leftAndRightSpace,
                  opt.rect.y() + 3 + opt.decorationSize.height(),
                  opt.rect.width() - leftAndRightSpace * 2,
                  opt.rect.height() - opt.decorationSize.height());
  QFont desktopFont = opt.font;
  desktopFont.setBold(true);
  QTextLayout layout(opt.text, desktopFont);

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
    QFontMetrics fontMetrics(desktopFont);
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

  if((opt.state & QStyle::State_Selected) && opt.widget) {
    // qDebug("w: %f, h:%f, m:%f", boundRect.width(), boundRect.height(), layout.minimumWidth());
    painter->setFont(desktopFont);
    // painter->fillRect(boundRect, opt.palette.highlight());
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path = QPainterPath();
    path.addRoundedRect(boundRect, 8, 8);
    painter->fillPath(path, opt.palette.highlight());

  }

    // draw shadow, FIXME: is it possible to use QGraphicsDropShadowEffect here?
    QPen prevPen = painter->pen();
    painter->setPen(QPen(shadowColor_));
    for(int i = 0; i < visibleLines; ++i) {
      QTextLine line = layout.lineAt(i);
      if(i == (visibleLines - 1) && !elidedText.isEmpty()) { // the last line, draw elided text
        QPointF pos(textRect.x() + line.position().x() + 1, textRect.y() + line.y() + line.ascent() + 1);
        painter->setFont(desktopFont);
        painter->drawText(pos, elidedText);
      }
      else {
        line.draw(painter, textRect.topLeft() + QPointF(1, 1));
      }
    }
    painter->setPen(prevPen);


  // draw text
  for(int i = 0; i < visibleLines; ++i) {
    QTextLine line = layout.lineAt(i);
    QPen prevPen = painter->pen();
    painter->setPen(QPen(textColor_));
    if(i == (visibleLines - 1) && !elidedText.isEmpty()) { // the last line, draw elided text
      QPointF pos(textRect.x() + line.position().x(), textRect.y() + line.y() + line.ascent());
      painter->drawText(pos, elidedText);
    }
    else {
      line.draw(painter, textRect.topLeft());
    }
    painter->setPen(prevPen);
  }

  if(opt.state & QStyle::State_HasFocus) {
    // FIXME: draw focus rect
  }
  // painter->restore();
}

QSize DesktopItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QVariant value = index.data(Qt::SizeHintRole);
  if(value.isValid())
    return qvariant_cast<QSize>(value);
  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  // use grid size as size hint
  QSize gridSize = view_->gridSize();
  return QSize(gridSize.width() -2, gridSize.height() - 2);
}

DesktopItemDelegate::~DesktopItemDelegate() {

}
