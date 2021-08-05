# Filer is built by cmake. This dummy makefile builds just the Obj-C++
# parts and creates the .app bundle.

APP=Filer
SRCS=
MK_DEBUG_FILES=no
RESOURCES=
FRAMEWORKS=

QTFLAGS!= pkg-config --cflags Qt5Gui Qt5Xdg
CXXFLAGS+= -g -O0 ${QTFLAGS} -fPIC -F${BUILDROOT}/System/Library/Frameworks \
    -framework Foundation -framework CoreFoundation

build: Filer.app ${.CURDIR}/build/src/cocoa.o
	cp -fv ${.CURDIR}/src/icons/filer-256.png ${APP_DIR}/Contents/Resources/Icon.png
	cp -fv ${.CURDIR}/Info.plist ${APP_DIR}/Contents

clean:
	rm -f ${.CURDIR}/build/src/*.o

${.CURDIR}/build/src/cocoa.o: ${.CURDIR}/src/cocoa.mm
	${CXX} ${CXXFLAGS} -o ${.TARGET} -c ${.ALLSRC}

.include <airyx.app.mk>
