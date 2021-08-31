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

#if defined(__AIRYX__)
#import "airyx.h"
#import <LaunchServices/LaunchServices.h>

QList<QString> programArguments()
{
    CFArrayRef args = (CFArrayRef)CFBundleGetValueForInfoDictionaryKey(
    	CFBundleGetMainBundle(), CFSTR("ProgramArguments" )); 
    QList<QString> list;
    if(!args)
    	return list;

    for(int x = 0; x < CFArrayGetCount(args); ++x) {
	CFStringRef value = (CFStringRef)CFArrayGetValueAtIndex(args, x);
        list.append(QString::fromUtf8( CFStringGetCStringPtr(value, kCFStringEncodingUTF8) ));
    }
    return list;
}

bool checkWhetherAppDirOrBundle(QString path)
{
    CFStringRef cfpath = CFStringCreateWithCString(NULL, path.toUtf8(), kCFStringEncodingUTF8);
    CFURLRef url = CFURLCreateWithFileSystemPath(NULL, cfpath, kCFURLPOSIXPathStyle, true);
    bool check = (LSIsNSBundle(url) | LSIsAppDir(url));
    CFRelease(url);
    CFRelease(cfpath);
    return check;
}

QString displayNameForBundle(QString path)
{
    // This function is only called for App Bundles or
    // AppDirs, so just a quick check to tell them apart...
    if(path.toLower().endsWith(".app")) {
    	CFStringRef cfpath = CFStringCreateWithCString(NULL, path.toUtf8(), kCFStringEncodingUTF8);
	CFURLRef url = CFURLCreateWithFileSystemPath(NULL, cfpath, kCFURLPOSIXPathStyle, true);
	CFRelease(cfpath);
	CFBundleRef bundle = CFBundleCreate(NULL, url);
	CFDictionaryRef infoDict = CFBundleGetInfoDictionary(bundle);
	CFStringRef name = (CFStringRef)CFDictionaryGetValue(infoDict, CFSTR("CFBundleDisplayName"));
	if(name == NULL)
	    name = (CFStringRef)CFDictionaryGetValue(infoDict, CFSTR("CFBundleName"));
	if(name == NULL) {
	    CFURLRef noExt = CFURLCreateCopyDeletingPathExtension(NULL, url);
	    name = CFURLCopyLastPathComponent(noExt);
	    CFRelease(noExt);
	}
	QString qname = QString::fromUtf8(CFStringGetCStringPtr(name, kCFStringEncodingUTF8));
	CFRelease(bundle);
	CFRelease(url);
	return qname;
    }

    // Handle AppDirs by looking for .desktop files. This is ugly.
    QStringList fileList;
    QDir folder(path);
    QStringList entryList(folder.entryList(QStringList(), QDir::Files));
    for(QStringList::iterator entry = entryList.begin(); entry != entryList.end(); entry++)
        fileList.append(folder.absoluteFilePath(*entry));

    QDir share(path.append("/usr/share/applications"));
    entryList = folder.entryList(QStringList(), QDir::Files);
    for(QStringList::iterator entry = entryList.begin(); entry != entryList.end(); entry++)
        fileList.append(share.absoluteFilePath(*entry));
    
    for(QStringList::iterator filepath = fileList.begin(); filepath != fileList.end(); filepath++) {
        if(filepath->toLower().endsWith(".desktop")) {
            XdgDesktopFile df;
            df.load(*filepath);
            if(df.type() == XdgDesktopFile::ApplicationType) {
                if(df.name() != QString()) {
                    return df.name();
		}
            }
        }
    }

    return QFileInfo(path).baseName();
}

QIcon getIconForBundle(QString path)
{
    QIcon icon = QIcon::fromTheme("do"); // default "executable folder" icon

    // This function is only called for App Bundles or
    // AppDirs, so just a quick check to tell them apart...
    if(path.toLower().endsWith(".app")) {
    	CFStringRef cfpath = CFStringCreateWithCString(NULL, path.toUtf8(), kCFStringEncodingUTF8);
	    CFURLRef url = CFURLCreateWithFileSystemPath(NULL, cfpath, kCFURLPOSIXPathStyle, true);
	    CFBundleRef bundle = CFBundleCreate(NULL, url);
	    CFDictionaryRef infoDict = CFBundleGetInfoDictionary(bundle);
	    CFStringRef iconfile = (CFStringRef)CFDictionaryGetValue(infoDict, CFSTR("CFBundleIconFile"));

	    if(iconfile == NULL)
	        iconfile = (CFStringRef)CFDictionaryGetValue(infoDict, CFSTR("NSIcon"));

        if(iconfile != NULL) {
            CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(bundle);
            CFStringRef resourcePath = CFURLCopyFileSystemPath(resourceURL, kCFURLPOSIXPathStyle);
            QDir resourceDir(QString::fromUtf8(CFStringGetCStringPtr(resourcePath, kCFStringEncodingUTF8)));
            CFRelease(resourcePath);
            CFRelease(resourceURL);

            QString iconPath(resourceDir.filePath(QString::fromUtf8(
                CFStringGetCStringPtr(iconfile, kCFStringEncodingUTF8))));
            if(resourceDir.exists(iconPath))
                    icon = QIcon(iconPath);
        }

	    CFRelease(bundle);
	    CFRelease(url);
	    CFRelease(cfpath);
        return icon;
    }

    // Look for a .DirIcon in AppDir
    QDir appdir(path);
    QString iconPath = appdir.filePath(".DirIcon");
    if(appdir.exists(iconPath))
        icon = QIcon(iconPath);
    return icon;
}
#endif
