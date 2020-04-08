TARGET = asddsa
TEMPLATE = app
CONFIG += console c++11
INCLUDEPATH += /opt/qtrpi/raspbian/sysroot/usr/include
LIBS += -L/opt/qtrpi/raspbian/sysroot/usr/lib -lbcm2835

CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    adda.cpp




HEADERS += \
    adda.h

target.path = home/pi
INSTALLS +=target
