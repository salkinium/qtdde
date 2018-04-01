#-------------------------------------------------
#
# Project created by QtCreator 2017-08-25T23:02:42
#
#-------------------------------------------------

QT += core concurrent

include($$PWD/../ext/pdfium.pri)

HEADERS += \
    $$PWD/pdf_char.hpp \
    $$PWD/pdf_document.hpp \
    $$PWD/pdf_link.hpp \
    $$PWD/pdf_object_loader.hpp \
    $$PWD/pdf_page.hpp \
    $$PWD/pdfium.hpp

SOURCES += \
    $$PWD/pdf_document.cpp \
    $$PWD/pdf_page.cpp \
    $$PWD/pdf_char.cpp \
    $$PWD/pdf_link.cpp \
    $$PWD/pdf_object_loader.cpp \
    $$PWD/pdfium_js_stub.cpp \
    $$PWD/pdfium.cpp
