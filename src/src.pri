#-------------------------------------------------
#
# Project created by QtCreator 2017-08-25T23:02:42
#
#-------------------------------------------------

include($$PWD/../ext/pdfium.pri)

include($$PWD/passes/passes.pri)
include($$PWD/pdf/pdf.pri)
include($$PWD/views/views.pri)

INCLUDEPATH += $$PWD


CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
    QMAKE_CXXFLAGS += -stdlib=libc++ -mmacosx-version-min=10.12
    LIBS += -stdlib=libc++ -mmacosx-version-min=10.12
}

gcc {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}