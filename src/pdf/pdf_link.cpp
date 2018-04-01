#include "pdf_link.hpp"
#include "pdf_page.hpp"

namespace dde {

PdfLink::PdfLink(PdfPage *page) :
    QObject(page), m_page(page)
{}

bool
PdfLink::isInternal() const
{
    return m_page_index >= 0;
}

} // namespace dde
