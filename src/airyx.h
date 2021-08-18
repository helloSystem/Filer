/*
 * Glue code to integrate Filer with Airyx Cocoa frameworks
 *
 * Copyright (C) 2021 Zoe Knox <zoe@pixin.net>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QString>
#include <QList>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <XdgDesktopFile>

#import <CoreFoundation/CoreFoundation.h>
#import <LaunchServices/LaunchServices.h>

extern "C" void __NSInitializeProcess(int argc, const char **argv);

// Returns the list of strings from key "ProgramArguments" in 
// the bundle's Info.plist
QList<QString> programArguments();

// Returns true if path is an App Bundle or AppDir
bool checkWhetherAppDirOrBundle(QString path);

// Returns the value of CFBundleDisplayName or CFBundleName
// and falls back to folder name on error
QString displayNameForBundle(QString path);

// Returns an icon for a bundle or a generic default
QIcon getIconForBundle(QString path);


