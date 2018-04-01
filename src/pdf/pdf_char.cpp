#include "pdf_char.hpp"
#include "pdf_page.hpp"
#include <QDebug>

namespace dde {

PdfChar::PdfChar(PdfPage *page) :
    QObject(page), m_page(page)
{}

} // namespace dde
