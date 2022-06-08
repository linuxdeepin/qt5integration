greaterThan(QT_MAJOR_VERSION, 4) {
  QT += gui-private
}

LIBS += -ldl

TARGET = qdeepin

PLUGIN_TYPE = platformthemes
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QDeepinTheme

PLUGIN_TYPE=platformthemes
JSON_FILE=deepin.json
include($$PWD/../../chooser.prf)
