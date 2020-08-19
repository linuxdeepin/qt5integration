TEMPLATE = subdirs
SUBDIRS += chameleon

!isEmpty(ENABLE_OLD_STYLE): SUBDIRS += dstyleplugin
