#include "pdf_object_loader.hpp"
#include <QtConcurrent>
#include <QDebug>
#include <QApplication>
#include <QFutureInterface>

#include <public/fpdf_text.h>
#include <public/fpdf_edit.h>
#include <public/fpdf_doc.h>
#include <core/fpdfapi/font/cpdf_font.h>
#include <core/fpdfapi/page/cpdf_page.h>
#include <core/fpdfapi/page/cpdf_pageobject.h>
#include <core/fpdfapi/page/cpdf_path.h>
#include <core/fpdfapi/page/cpdf_pathobject.h>
#include <core/fpdfapi/page/cpdf_textobject.h>
#include <core/fpdfapi/parser/cpdf_document.h>
#include <core/fpdfapi/parser/cpdf_object.h>
#include <core/fpdfdoc/cpdf_link.h>
#include <core/fpdftext/cpdf_textpage.h>

namespace dde {

QFuture<void>
PdfObjectLoader::loadDocument(PdfDocument* doc)
{
    return QtConcurrent::run([doc]()
    {
        FPDF_DOCUMENT fpdf = FPDF()->load_document(doc->m_filepath.absoluteFilePath());
        const int pageCount = FPDF()->get_page_count(fpdf);
        doc->m_fpdf = fpdf;
        doc->m_pageCount = pageCount;
        emit doc->loadProgress(pageCount, 0);

        for (int ii = 0; ii < pageCount; ii++)
        {
            PdfPage *page = loadPage(doc, ii);
            page->moveToThread(QApplication::instance()->thread());
            page->setParent(doc);
            doc->m_pages.push_back(page);
            emit doc->loadProgress(pageCount, ii + 1);
        }
    });
}

PdfPage*
PdfObjectLoader::loadPage(PdfDocument *doc, int pageNumber)
{
    PdfPage *page = new PdfPage(doc, pageNumber);
    QMutexLocker l(FPDF()->mutex());

    FPDF_PAGE fpdfpage = FPDF_LoadPage(doc->m_fpdf, pageNumber);
    CPDF_Page *pdf_page = static_cast<CPDF_Page *>(fpdfpage);
    page->m_page = fpdfpage;

    // Get page size
    FPDF_GetPageSizeByIndex(doc->m_fpdf, pageNumber, &page->m_pageSize.rwidth(), &page->m_pageSize.rheight());
    // get page rotation
    page->m_rotation = pdf_page->GetPageRotation() * 90;
    // get page matrix
    const CFX_Matrix& m = pdf_page->GetDisplayMatrix(0,0, page->pageSize().width(), page->pageSize().height(), 0);
    page->m_matrix = QTransform(m.a, m.b, m.c, m.d, m.e, m.f);
    // get flipped matrix?
//    QTransform ctmm = page->m_matrix * QTransform().scale(1, -1).translate(0, -page->pageSize().height());
    QTransform ctmm = page->m_matrix;

    // get all page polygons
    const int objcount = FPDFPage_CountObject(fpdfpage);
    for (int ii=0; ii < objcount; ii++)
    {
        CPDF_PageObject *object = static_cast<CPDF_PageObject *>(FPDFPage_GetObject(fpdfpage, ii));
        // cache polygons too
        if (object->IsPath())
        {
            CFX_Matrix &m = object->AsPath()->m_Matrix;
            CPDF_Path &path = object->AsPath()->m_Path;
            const std::vector<FX_PATHPOINT> &points = path.GetPoints();
            QPolygonF poly;
            for (const FX_PATHPOINT &p: points)
            {
                poly << QPointF(p.m_Point.x, p.m_Point.y);
            }
            QTransform ctm = QTransform(m.a, m.b, m.c, m.d, m.e, m.f) * ctmm;
            page->m_polygons.push_back(ctm.map(poly));
        }
    }

    // get all page characters
    CPDF_TextPage *textpage = static_cast<CPDF_TextPage *>(FPDFText_LoadPage(fpdfpage));
    const int charcount = textpage->CountChars();
    page->m_chars.reserve(charcount);
    for (int ii=0; ii < charcount; ii++)
    {
        FPDF_CHAR_INFO charinfo;
        textpage->GetCharInfo(ii, &charinfo);
        page->m_chars.push_back(loadChar(page, charinfo, ctmm));
    }
    FPDFText_ClosePage(textpage);

    // get all page links
    FPDF_LINK rawlink;
    for(int pos = 0; FPDFLink_Enumerate(fpdfpage, &pos, &rawlink);)
    {
        CPDF_Link link(ToDictionary(static_cast<CPDF_Object*>(rawlink)));
        page->m_links.push_back(loadLink(page, link, ctmm));
    }

    return page;
}

PdfChar*
PdfObjectLoader::loadChar(PdfPage *page, FPDF_CHAR_INFO &i, QTransform ctm)
{
    PdfChar *pdfchar = new PdfChar(page);

    pdfchar->m_unicode = i.m_Unicode;
    pdfchar->m_charcode = i.m_Charcode;
    pdfchar->m_flags = i.m_Flag;
    pdfchar->m_fontsize = i.m_FontSize;
    pdfchar->m_origin = ctm.map(QPointF(i.m_Origin.x, i.m_Origin.y));
    pdfchar->m_bounds = ctm.mapRect(QRectF(i.m_CharBox.left, i.m_CharBox.bottom, i.m_CharBox.right - i.m_CharBox.left, i.m_CharBox.top - i.m_CharBox.bottom));
    pdfchar->m_ctm = ctm * QTransform(i.m_Matrix.a, i.m_Matrix.b, i.m_Matrix.c, i.m_Matrix.d, i.m_Matrix.e, i.m_Matrix.f);
    if (i.m_pTextObj) {
        CPDF_Font *const m_font = i.m_pTextObj->GetFont();
//        const FXFT_Face m_face = pdfchar->m_font->GetFace();

        // get basename (raw font name)
        pdfchar->m_basename = QString::fromLocal8Bit(m_font->GetBaseFont().c_str());
        // get fontname
        auto names = pdfchar->m_basename.split("+");
        Q_ASSERT(names.length() >= 2);
        names.pop_front();
        auto name = names.join("");
        pdfchar->m_fontname = name.remove("-Italic").remove("-Bold");
        // get stylename
        if (pdfchar->m_basename.contains("Italic")) {
            pdfchar->m_stylename = "italic";
        }
        else if (pdfchar->m_basename.contains("Bold")) {
            pdfchar->m_stylename = "bold";
        }
    }

    return pdfchar;
}

PdfLink*
PdfObjectLoader::loadLink(PdfPage *page, CPDF_Link &link, QTransform ctm)
{
    PdfLink *pdflink = new PdfLink(page);

    CPDF_Document *cdoc = static_cast<CPDF_Document*>(page->m_doc->m_fpdf);
    CPDF_Action action = link.GetAction();
    CPDF_Dest dest = link.GetDest(cdoc);
    CFX_FloatRect ar = link.GetRect();

    pdflink->m_bounds = ctm.mapRect(QRectF(ar.left, ar.bottom, ar.right - ar.left, ar.top - ar.bottom));
    pdflink->m_uri = QString::fromLocal8Bit(action.GetURI(cdoc).c_str());
    pdflink->m_obj_number = dest.GetPageObjNum();
    pdflink->m_page_index = pdflink->m_obj_number ? dest.GetDestPageIndex(cdoc) : -1;

    return pdflink;
}

} // namespace dde
