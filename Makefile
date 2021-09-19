# Filer is built by cmake. This dummy makefile just creates the .app bundle.

APP=Filer
SRCS=
MK_DEBUG_FILES=no
RESOURCES=
FRAMEWORKS=

build: Filer.app
	cp -fv ${.CURDIR}/src/icons/filer-256.png ${APP_DIR}/Contents/Resources/Icon.png
	cp -fv ${.CURDIR}/Info.plist ${APP_DIR}/Contents

clean:
	rm -f ${.CURDIR}/build/src/*.o

.include <airyx.app.mk>
