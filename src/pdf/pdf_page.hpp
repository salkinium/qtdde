#ifndef PDF_PAGE_HPP
#define PDF_PAGE_HPP

#include <QObject>
#include <QVector>
#include <QSizeF>
#include <QTransform>
#include "pdf_char.hpp"
#include "pdf_link.hpp"
#include "pdfium.hpp"

namespace dde {

class PdfDocument;

class PdfPage : public QObject
{
    Q_OBJECT
public:
    explicit PdfPage(PdfDocument *doc, int pageNumber);
    virtual ~PdfPage();

    inline QSizeF
    pageSize() const
    { return m_pageSize; }

    inline int
    pageNumber() const
    { return m_pageNumber; }

    inline int
    rotation() const
    { return m_rotation; }

    inline const QVector<QPolygonF>&
    polygons() const
    { return m_polygons; }

    inline const QVector<PdfChar*>&
    chars() const
    { return m_chars; }

private:
    inline QTransform&
    matrix() const
    { return m_matrix; }

    const PdfDocument *m_doc = nullptr;
    const int m_pageNumber = 0;
    FPDF_PAGE m_page;

    mutable QTransform m_matrix;
    mutable int m_rotation = 0;
    mutable QSizeF m_pageSize;

    mutable QVector<QPolygonF> m_polygons;
    mutable QVector<PdfChar *> m_chars;
    mutable QVector<PdfLink *> m_links;

    friend class PdfPageView;
    friend class Pdfium;
    friend class PdfLink;
    friend class PdfObjectLoader;
};

} // namespace dde

#endif // PDF_PAGE_HPP
