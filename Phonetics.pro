QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

QT       += core gui multimedia
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audio.cpp \
    main.cpp \
    mainwindow.cpp \
    segmentgraph.cpp \
    waveformsegments.cpp \
    wavfile.cpp \
    wavform.cpp \
    zoom.cpp

HEADERS += \
    audio.h \
    mainwindow.h \
    segmentgraph.h \
    waveformsegments.h \
    wavfile.h \
    wavform.h \
    zoom.h

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# adding the FFTW library
# INCLUDEPATH += /usr/local/include
# LIBS += -L/usr/local/lib -lfftw3
