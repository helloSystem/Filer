# Filer is built by cmake. This dummy makefile builds just the Obj-C++
# parts and creates the .app bundle.

APP=Filer
SRCS=
MK_DEBUG_FILES=no
RESOURCES=
FRAMEWORKS=

QTFLAGS!= pkg-config --cflags Qt5Core
CXXFLAGS+= ${QTFLAGS} -fPIC -F${BUILDROOT}/System/Library/Frameworks \
    -framework Foundation -framework CoreFoundation

build: ${APP_DIR} ${.CURDIR}/build/src/cocoa.o
clean:
	rm -f ${.CURDIR}/build/src/*.o

${.CURDIR}/build/src/cocoa.o: ${.CURDIR}/src/cocoa.mm
	${CXX} ${CXXFLAGS} -o ${.TARGET} -c ${.ALLSRC}

.include <airyx.app.mk>
