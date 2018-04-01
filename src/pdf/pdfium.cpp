#include "pdfium.hpp"

#include <QList>
#include <QMetaObject>
#include <QDebug>
#include <views/pdf_page_view.hpp>
#include "pdf_page.hpp"
#include <memory>

namespace {
static std::unique_ptr<dde::Pdfium> pfdium_instance;
}

namespace dde {

Pdfium::Pdfium(QObject *parent)
{
    qRegisterMetaType<PdfPageView*>("dde::PdfPageView*");
    qRegisterMetaType<const char*>("const char*");
}

Pdfium::~Pdfium()
{
    if (m_inited)
    {
        t.quit();
        t.wait();
        {
            QMutexLocker l(mutex());
            FPDF_DestroyLibrary();
        }
//        qDebug() << "~Pdfium";
    }
}

Pdfium *
Pdfium::instance()
{
    if (pfdium_instance.get() == nullptr) {
        pfdium_instance = std::make_unique<Pdfium>();
        pfdium_instance->init();
//        qDebug() << "new Pdfium()";
    }
    Pdfium *ptr = pfdium_instance.get();
    return ptr;
}

void
Pdfium::init()
{
    QMutexLocker l(mutex());
    if (not m_inited)
    {
//        qDebug() << "Pdfium::init()";
        moveToThread(&t);
        t.start();
        FPDF_InitLibrary();
        FPDF_SetSandBoxPolicy(FPDF_POLICY_MACHINETIME_ACCESS, false);
        m_inited = true;
    }
}

FPDF_DOCUMENT
Pdfium::load_document(QString filename)
{
    QMutexLocker l(mutex());
//    qDebug() << this << filename;
    return FPDF_LoadDocument(filename.toStdString().c_str(), nullptr);
}

void
Pdfium::close_document(FPDF_DOCUMENT doc)
{
    QMutexLocker l(mutex());
//    qDebug() << this << doc;
    FPDF_CloseDocument(doc);
}

int
Pdfium::get_page_count(FPDF_DOCUMENT doc)
{
    QMutexLocker l(mutex());
//    qDebug() << this << doc;
    return FPDF_GetPageCount(doc);
}

FPDF_PAGE
Pdfium::load_page(FPDF_DOCUMENT doc, int pageNumber)
{
    QMutexLocker l(mutex());
//    qDebug() << this << doc;
    return FPDF_LoadPage(doc, pageNumber);
}

void
Pdfium::close_page(FPDF_PAGE page)
{
    QMutexLocker l(mutex());
    FPDF_ClosePage(page);
}

void
Pdfium::renderAsync(PdfPageView *const page_view, qreal scale, const char *slot)
{
//        qDebug() << "Requesting async render for page" << page_view->page()->pageNumber();
    const bool b = QMetaObject::invokeMethod(this, "renderAsyncPage", Qt::QueuedConnection,
                                             Q_ARG(dde::PdfPageView*, page_view),
                                             Q_ARG(qreal, scale),
                                             Q_ARG(const char*, slot));
    Q_ASSERT(b);
    Q_UNUSED(b);
}

void
Pdfium::renderAsyncPage(PdfPageView *page_view, qreal scale, const char *slot)
{
//    qDebug() << "Executing async render for page" << page_view->page()->pageNumber();
    QSizeF size = page_view->page()->pageSize() * scale;
    QImage render(size.toSize(), QImage::Format_ARGB32);
    render.fill(Qt::white);
    {
        QMutexLocker l(mutex());
        FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(render.width(), render.height(), FPDFBitmap_BGRA, render.bits(), render.bytesPerLine());
        FPDF_RenderPageBitmap(bitmap, page_view->page()->m_page, 0, 0, render.width(), render.height(), 0, FPDF_LCD_TEXT | FPDF_DEBUG_INFO);
        FPDFBitmap_Destroy(bitmap);
    }

    QPixmap map = QPixmap::fromImage(render);
    map.setDevicePixelRatio(scale);

    const bool b = QMetaObject::invokeMethod(page_view, slot, Qt::QueuedConnection,
                                             Q_ARG(QPixmap, map));
    Q_ASSERT(b);
    Q_UNUSED(b);
}

} // namespace dde
