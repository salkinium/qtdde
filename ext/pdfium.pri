INCLUDEPATH += $$PWD/pdfium/fpdfsdk/include
INCLUDEPATH += $$PWD/pdfium
INCLUDEPATH += $$PWD/pdfium/third_party/freetype/src/include
QMAKE_LIBDIR += $$PWD/../lib
CONFIG( debug, debug|release ) {
    LIBS += -lpdfium_debug
} else {
    LIBS += -lpdfium_release
}
macx {
    LIBS_PRIVATE += -framework AppKit -framework CoreFoundation
}
