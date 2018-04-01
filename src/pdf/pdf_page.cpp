#include "pdf_page.hpp"
#include "pdf_document.hpp"
#include <QTransform>
#include <QRectF>
#include <QDebug>

namespace dde {

PdfPage::PdfPage(PdfDocument *doc, int pageNumber) :
    QObject(nullptr),
    m_doc(doc), m_pageNumber(pageNumber)
{
}

PdfPage::~PdfPage()
{
//    qDebug() << "~PdfPage";
}

} // namespace dde
