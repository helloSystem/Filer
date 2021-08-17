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

#import "airyx.h"
#import <LaunchServices/LaunchServices.h>

NSFileManager *fm = [NSFileManager defaultManager];

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

int launchFilesWithLS(QList<QString> files)
{
    LSLaunchURLSpec spec;
    spec.appURL = NULL;
    NSMutableArray *itemURLs = [NSMutableArray new];
    for(QList<QString>::iterator iter = files.begin(); iter != files.end(); ++iter) {

        NSURL *url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:iter->toUtf8()]];
        [itemURLs addObject:url];
    }
    spec.itemURLs = (CFArrayRef)itemURLs;
    return LSOpenFromURLSpec(&spec, NULL);
}

bool checkWhetherAppDirOrBundle(QString path)
{
    NSURL *nsurl = [NSURL fileURLWithPath:
        [NSString stringWithUTF8String:path.toUtf8()]];
    return LSIsNSBundle(nsurl) | LSIsAppDir(nsurl);
}

QString displayNameForBundle(QString path)
{
    NSString *nspath = [NSString stringWithUTF8String:path.toUtf8()];

    // This function is only called for App Bundles or
    // AppDirs, so just a quick check to tell them apart...
    if(path.toLower().endsWith(".app"))
        return QString::fromUtf8([[fm displayNameAtPath:nspath] UTF8String]);

    // Handle AppDirs by looking for .desktop files. This is ugly.
    QString name;
    NSMutableArray *fileList = [NSMutableArray new];
    NSEnumerator *entries = [[fm directoryContentsAtPath:nspath] objectEnumerator];
    while(NSString *entry = [entries nextObject])
        [fileList addObject:[nspath stringByAppendingPathComponent:entry]];

    NSString *share = [nspath stringByAppendingString:@"/usr/share/applications"];
    entries = [[fm directoryContentsAtPath:share] objectEnumerator];
    while(NSString *entry = [entries nextObject])
        [fileList addObject:[share stringByAppendingPathComponent:entry]];

    NSEnumerator *files = [fileList objectEnumerator];
    while(NSString *filepath = [files nextObject]) {
        if([filepath hasSuffix:@".desktop"]) {
            XdgDesktopFile df;
            df.load(QString::fromUtf8([filepath UTF8String]));
            if(df.type() == XdgDesktopFile::ApplicationType) {
                name = df.name();
                if(name != QString())
                    return name;
            }
        }
    }

    return QString::fromUtf8([[[nspath lastPathComponent] 
        stringByDeletingPathExtension] UTF8String]);
}

int registerApplicationWithLS(QString path)
{
    return LSRegisterURL((CFURLRef)[NSURL fileURLWithPath:
        [NSString stringWithUTF8String:path.toUtf8()]], false);
}

QIcon getIconForBundle(QString path)
{
    NSString *nspath = [NSString stringWithUTF8String:path.toUtf8()];

    QIcon icon = QIcon::fromTheme("do"); // default "executable folder" icon

    // This function is only called for App Bundles or
    // AppDirs, so just a quick check to tell them apart...
    if(path.toLower().endsWith(".app")) {
        NSBundle *bundle = [NSBundle bundleWithPath:nspath];
        NSString *iconfile = [bundle objectForInfoDictionaryKey:@"CFBundleIconFile"];
        if(iconfile == nil) // try the NeXTStep/GNUstep legacy key
            iconfile = [bundle objectForInfoDictionaryKey:@"NSIcon"];
        if(iconfile != nil) {
            NSString *iconpath = [[bundle resourcePath] stringByAppendingPathComponent:iconfile];
            if([fm fileExistsAtPath:iconpath]) {
                icon = QIcon(QString::fromUtf8([iconpath UTF8String]));
            }
        }
        return icon;
    }

    // Look for a .DirIcon in AppDir
    NSString *iconpath = [nspath stringByAppendingPathComponent:@".DirIcon"];
    if([fm fileExistsAtPath:iconpath])
        icon = QIcon(QString::fromUtf8([iconpath UTF8String]));
    return icon;
}

