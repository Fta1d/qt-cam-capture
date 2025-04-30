QT += core gui multimedia multimediawidgets

CONFIG += c++17
CONFIG += link_pkgconfig
PKGCONFIG += opencv4
PKGCONFIG += gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0

TARGET = prog
TEMPLATE = app
SOURCES += ../src/*
HEADERS += ../inc/*

OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = ui
RCC_DIR = rcc
