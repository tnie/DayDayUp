QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

msvc: QMAKE_CXXFLAGS += /utf-8

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cobject.cpp \
        main.cpp \
        test.cpp \
        workerthread.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    cobject.h \
    test.h \
    workerthread.h

DISTFILES += \
    README.md
