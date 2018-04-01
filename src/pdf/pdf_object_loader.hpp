#ifndef PDF_OBJECT_LOADER_HPP
#define PDF_OBJECT_LOADER_HPP

#include <QObject>
#include "pdf_document.hpp"
#include "pdf_page.hpp"
#include "pdf_char.hpp"
#include "pdf_link.hpp"
#include "pdfium.hpp"

class FPDF_CHAR_INFO;
class CPDF_Link;

namespace dde {

class PdfObjectLoader
{
public:
    static QFuture<void>
    loadDocument(PdfDocument* doc);

protected:
    static PdfPage*
    loadPage(PdfDocument *doc, int page);

    static PdfChar*
    loadChar(PdfPage *page, FPDF_CHAR_INFO &info, QTransform ctm);

    static PdfLink*
    loadLink(PdfPage *page, CPDF_Link &link, QTransform ctm);
};

} // namespace dde

#endif // PDF_OBJECT_LOADER_HPP
