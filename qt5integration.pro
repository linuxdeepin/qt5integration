TEMPLATE = subdirs
SUBDIRS += \
    dstyleplugin\
    platformthemeplugin/qt5deepintheme-plugin.pro \
    iconengineplugins/iconengines.pro \
    imageformatplugins/imageformats.pro

CONFIG(debug, debug|release) {
    SUBDIRS += styles
    styles.depends += dstyleplugin
}
