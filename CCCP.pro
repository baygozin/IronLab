#-------------------------------------------------
#
# Project created by QtCreator 2016-10-26T12:15:52
#
#-------------------------------------------------


QT += core gui charts
QT += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IronLab
TEMPLATE = app

include(3rdparty/qtxlsx/src/xlsx/qtxlsx.pri)
include(3rdparty/qttelnet/src/qttelnet.pri)

SOURCES += main.cpp mainwindow.cpp \
	dialogsetup.cpp \
    chartview.cpp \
    ironchart.cpp

HEADERS  += mainwindow.h \
	dialogsetup.h \
    chartview.h \
    ironchart.h

FORMS    += mainwindow.ui \
	dialogsetup.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../qttelnet-2.1-opensource-master/lib/ -llibQtSolutions_Telnet-2.1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../qttelnet-2.1-opensource-master/lib/ -llibQtSolutions_Telnet-2.1d

INCLUDEPATH += $$PWD/../qttelnet-2.1-opensource-master
DEPENDPATH += $$PWD/../qttelnet-2.1-opensource-master
