TEMPLATE = subdirs
SUBDIRS += dstyleplugin

CONFIG(debug) {
    SUBDIRS += styles
    styles.depends += dstyleplugin
}
