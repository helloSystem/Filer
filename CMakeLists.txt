cmake_minimum_required(VERSION 3.0.2)
project(Filer)

set(CMAKE_INSTALL_PREFIX "/System/Filer.app")

# Do not show deprecated warnings
add_definitions(-DQT_NO_DEPRECATED_WARNINGS)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x000000)

# Do not put qDebug() into Release builds
if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

set(FILER_VERSION_MAJOR 0)
set(FILER_VERSION_MINOR 10)
set(FILER_VERSION_PATCH 0)
set(FILER_VERSION ${FILER_VERSION_MAJOR}.${FILER_VERSION_MINOR}.${FILER_VERSION_PATCH})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Qt5Widgets 5.2 REQUIRED)
find_package(Qt5DBus 5.2 REQUIRED)
find_package(Qt5LinguistTools 5.2 REQUIRED)
find_package(Qt5X11Extras 5.2 REQUIRED)
find_package(Qt5Multimedia 5.2 REQUIRED)

find_package(PkgConfig)
pkg_check_modules(SYSTEM_LIBS REQUIRED
    glib-2.0
    gio-2.0
    gio-unix-2.0
    xcb
    x11
)

pkg_check_modules(LIBFM REQUIRED libfm>=1.2.0)
pkg_check_modules(LIBMENUCACHE REQUIRED libmenu-cache>=0.4.0)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

add_definitions(-DQT_NO_KEYWORDS)

# Do not put qDebug() into Release builds
if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    # set visibility to hidden to hide symbols, unless they're exported manually in the code
    set(CMAKE_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -fno-exceptions ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)

# manpage for filer-qt
#configure_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/filer-qt.1.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/filer-qt.1"
#    @ONLY
#)
#install(FILES
#    "${CMAKE_CURRENT_BINARY_DIR}/filer-qt.1"
#    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
#)

install(FILES
    "src/icons/filer-128.png"
    RENAME "Filer.png"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/Resources")
