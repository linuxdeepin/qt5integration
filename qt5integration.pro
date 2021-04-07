TEMPLATE = subdirs
SUBDIRS += \
    styleplugins\
    platformthemeplugin/qt5deepintheme-plugin.pro \
    iconengineplugins/iconengines.pro \
    imageformatplugins/imageformats.pro \
    tests

!isEmpty(BASED_DTK_DIR): SUBDIRS += src

CONFIG(debug, debug|release) {
    SUBDIRS += styles
    styles.depends += styleplugins
}
