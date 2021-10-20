#-------------------------------------------------
#
# Project created by QtCreator 2021-07-10T08:35:12
#
#-------------------------------------------------

QT       += core gui websockets sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    loadwindow.cpp \
    signinwindow.cpp \
    protocol.cpp \
    user.cpp \
    signupwindow.cpp \
    talkwindow.cpp \
    message.cpp \
    record.cpp \
    facesigninwindow.cpp \
    facesignupwindow.cpp

HEADERS += \
        mainwindow.h \
    loadwindow.h \
    signinwindow.h \
    protocol.h \
    user.h \
    signupwindow.h \
    talkwindow.h \
    message.h \
    record.h \
    facesigninwindow.h \
    facesignupwindow.h

FORMS += \
        mainwindow.ui \
    loadwindow.ui \
    signinwindow.ui \
    talkwindow.ui \
    facesigninwindow.ui \
    facesignupwindow.ui \
    facesignupwindow.ui \
    signupwindow.ui

RESOURCES += \
    assets.qrc \
    breeze.qrc
