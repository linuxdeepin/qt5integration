TEMPLATE = subdirs
SUBDIRS += \
    dstyleplugin\
    platformplugin/qt5platform-plugin.pro\
    platformthemeplugin/qt5deepintheme-plugin.pro \
    iconengineplugins/iconengines.pro \
    imageformatplugins/imageformats.pro

CONFIG(debug, debug|release) {
    SUBDIRS += styles
    styles.depends += dstyleplugin
}
