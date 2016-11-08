QT += widgets

HEADERS       = norwegianwoodstyle.h \
                widgetgallery.h
SOURCES       = main.cpp \
                norwegianwoodstyle.cpp \
                widgetgallery.cpp
RESOURCES     = styles.qrc

DESTDIR = $$_PRO_FILE_PWD_/../bin/

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/styles
# INSTALLS += target
