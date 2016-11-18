TEMPLATE = subdirs
SUBDIRS += \
    dstyleplugin\
    platformplugin/qt5dxcb-plugin.pro\
    platformthemeplugin/qt5deepintheme-plugin.pro

CONFIG(debug, debug|release) {
    SUBDIRS += styles
    styles.depends += dstyleplugin
}
