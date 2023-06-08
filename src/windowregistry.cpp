/*
 *    Copyright (C) 2021 Chris Moore <chris@mooreonline.org>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "windowregistry.h"

#include <QDebug>
#include <QGuiApplication>
#include <QWindow>
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

WindowRegistry::~WindowRegistry() {
  // do nothing
}

bool WindowRegistry::checkPathAndRaise(const QString &path) {
  qDebug() << "probono: WindowRegistry::checkPathAndRaise(" << path << ")";

  QList<QWindow *> windows = QGuiApplication::topLevelWindows();

  // Iterate through the windows and check if the "_FILER_PATH" atom is set
  for (QWindow *window : windows) {
    if (checkWindowAtomValue(window, "_FILER_PATH",
                             path.toUtf8().constData())) {
      qDebug() << "Window" << window << "has a matching _FILER_PATH atom set.";
      Q_EMIT raiseWindow(path);
      return true;
    }
  }
  return false;
}

bool WindowRegistry::checkPathAndClose(const QString &path) {

  qDebug() << "probono: WindowRegistry::checkPathAndClose(" << path << ")";

  QList<QWindow *> windows = QGuiApplication::topLevelWindows();

  // Iterate through the windows and check if the "_FILER_PATH" atom is set
  for (QWindow *window : windows) {
    if (checkWindowAtomValue(window, "_FILER_PATH",
                             path.toUtf8().constData())) {
      qDebug() << "Window" << window << "has a matching _FILER_PATH atom set.";
      Q_EMIT closeWindow(path);
      return true;
    }
  }
  return false;
}

bool WindowRegistry::checkPathAndSelectItems(const QString &path,
                                             const QStringList &items) {

  qDebug() << "probono: WindowRegistry::checkPathAndSelectItems(" << path
           << ", " << items << ")";

  QList<QWindow *> windows = QGuiApplication::topLevelWindows();

  // Iterate through the windows and check if the "_FILER_PATH" atom is set
  for (QWindow *window : windows) {
    if (checkWindowAtomValue(window, "_FILER_PATH",
                             path.toUtf8().constData())) {
      qDebug() << "Window" << window << "has a matching _FILER_PATH atom set.";
      Q_EMIT raiseWindowAndSelectItems(path, items);
      return true;
    }
  }
  return false;
}

WindowRegistry::WindowRegistry(QObject *parent) : QObject() {
  // do nothing
}

// Check if the given window has the given atom set to the given value
// Returns true if the atom is set and the value matches
bool WindowRegistry::checkWindowAtomValue(QWindow *window,
                                          const QString &atomName,
                                          const QString &atomValue) {
  Display *display = QX11Info::display();
  WId x11Window = window->winId();

  // Get the atom for the given name
  Atom atom = XInternAtom(display, atomName.toUtf8().constData(), false);

  // Check if the atom is set on the window
  Atom actualType;
  int actualFormat;
  unsigned long numItems, bytesAfter;
  unsigned char *propValue = nullptr;

  int result = XGetWindowProperty(display, x11Window, atom, 0, 1024, false,
                                  XCB_ATOM_STRING, &actualType, &actualFormat,
                                  &numItems, &bytesAfter, &propValue);

  if (result == Success && propValue) {
    // Compare the atom value and return true if it matches
    if (atomValue.isEmpty() || atomValue == QString((char *)propValue)) {
      XFree(propValue);
      return true;
    }
  }

  return false;
}