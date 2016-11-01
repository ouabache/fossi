#-----------------------------------------------------------------------------
# File: tst_ModelParameterComparator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 16.10.2014
#
# Description:
# Qt project file template for running unit tests for a ComponentComparator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ModelParameterComparator

QT += core xml gui testlib
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$PWD/../../../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../executable

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ModelParameterComparator.pri)
