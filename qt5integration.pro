TEMPLATE = subdirs
SUBDIRS += dstyleplugin

CONFIG(debug, debug|release) {
    SUBDIRS += styles
    styles.depends += dstyleplugin
}
