QT += core
QT -= gui

CONFIG += c++11

TARGET = dthemed_icon_lookup
CONFIG += console
CONFIG -= app_bundle

LIBS += -lthemed_icon_lookup

TEMPLATE = app

SOURCES += main.cpp

DEFINES += QT_DEPRECATED_WARNINGS
