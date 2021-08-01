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

#define COCOA
#import "cocoa.h"

QString resourcePath()
{
    return QString::fromUtf8( [[[NSBundle mainBundle] resourcePath] UTF8String] );
}

QString executablePath()
{
    return QString::fromUtf8( [[[NSBundle mainBundle] executablePath] UTF8String] );
}

NSDictionary *infoDictionary()
{
    return [[NSBundle mainBundle] infoDictionary];
}

QList<QString> programArguments()
{
    NSArray *args = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"ProgramArguments"];
    NSEnumerator *e = [args objectEnumerator];
    id object;
    QList<QString> list;
    while(object = [e nextObject]) {
        list.append(QString::fromUtf8([object UTF8String]));
    }
    return list;
}

