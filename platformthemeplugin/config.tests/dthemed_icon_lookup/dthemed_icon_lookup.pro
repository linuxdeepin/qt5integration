QT += core
QT -= gui

CONFIG += c++11

TARGET = dthemed_icon_lookup
CONFIG += console link_pkgconfig
CONFIG -= app_bundle

PKGCONFIG += til

TEMPLATE = app

SOURCES += main.cpp

DEFINES += QT_DEPRECATED_WARNINGS
