#include "pdf_document.hpp"
#include "pdf_page.hpp"
#include <QDebug>

namespace dde
{

PdfDocument::PdfDocument(QObject *parent) :
    PdfDocument(QFileInfo(), parent)
{
}

PdfDocument::PdfDocument(QFileInfo filename, QObject *parent) :
    QObject(parent), m_filepath(filename)
{
}

PdfDocument::~PdfDocument()
{
//    qDebug() << "~PdfDocument";
}

QString
PdfDocument::name() const
{
    return m_filepath.baseName();
}

bool
PdfDocument::isLoaded() const
{
    return m_pages.count() > 0;
}

int
PdfDocument::pageCount() const
{
    return m_pageCount;
}

const PdfDocument::PageVector&
PdfDocument::pages() const
{
    return m_pages;
}

} // namespace dde
