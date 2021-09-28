QT += core gui widgets

LIBS += -ldl

TARGET = chameleon

PLUGIN_TYPE=styles
JSON_FILE=chameleon.json
include($$PWD/../../chooser.prf)
