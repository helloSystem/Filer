# Filer [![Build Status](https://api.cirrus-ci.com/github/helloSystem/Filer.svg)](https://cirrus-ci.com/github/helloSystem/Filer) [![Translation status](https://hosted.weblate.org/widgets/hellosystem/-/filer/svg-badge.svg)](https://hosted.weblate.org/engage/hellosystem/)

A file manager that also renders the desktop. A key component of [helloSystem](https://hellosystem.github.io/docs/).

## Features

Minimalistic simplicity, see [hello](https://github.com/probonopd/hello) for the background

* Appeal to users coming from macOS
* Cross platform: Builds for Linux, FreeBSD
* Spatial mode (each folder opens in its own window)
* Can handle [Application Bundles](https://hellosystem.github.io/docs/developer/application-bundles.html)
* Can handle ELF files that are lacking the executable bit (backlog)
* Can handle AppImages (backlog)
* Context menu can be extended using file manager actions

## Dependencies

On Alpine Linux:

```
apk add --no-cache qt5-qtbase-dev kwindowsystem-dev qt5-qttools-dev kdbusaddons-dev baloo-dev xcb-util-wm-dev libdbusmenu-qt-dev git cmake musl-dev alpine-sdk clang qt5-qtmultimedia-dev libfm-dev libfm-extra-dev menu-cache-dev
```

Not all of the above may actually be required.

## Build

```
mkdir build
cd build
cmake ..
make
sudo make install
```
