#ifndef DARKSIDECONVERTER_H
#define DARKSIDECONVERTER_H

#include <QStringList>
#include <glib.h>
#include <libfm/fm.h>

/*
 * Helper class with class methods to convert objects from the dark side (glib/libfm/...) to Qt native objects.
 * Each time we encounter one of those objects from the dark side, we should add a converter for it here.
 * Eventually, our code will be free of objects from the dark side hopefully.
 * Any contributions toward this objective would be highly welcome.
 */

class DarkSideConverter
{
public:
    DarkSideConverter();

    static QStringList qStringListFromFmPathList(FmPathList* srcFiles);
};

#endif // DARKSIDECONVERTER_H
