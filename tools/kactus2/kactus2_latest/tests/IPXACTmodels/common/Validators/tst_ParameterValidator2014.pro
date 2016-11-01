#-----------------------------------------------------------------------------
# File: tst_ParameterValidator2014.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 11.12.2014
#
# Description:
# Qt project file template for running unit tests for SystemVerilogExpressionParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ParameterValidator2014

QT += core xml gui testlib
CONFIG += testcase console

DEFINES += IPXACTMODELS_LIB

win32:CONFIG(release, debug|release) {
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    DESTDIR = ./debug
}
else:unix {
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$PWD/../../../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles

include(tst_ParameterValidator2014.pri)
