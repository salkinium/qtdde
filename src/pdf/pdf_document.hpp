#ifndef PDF_DOCUMENT_HPP
#define PDF_DOCUMENT_HPP

#include <QObject>
#include <QVector>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QTimer>
#include <QFileInfo>

#include "pdfium.hpp"

namespace dde {

class PdfPage;

class PdfDocument : public QObject
{
    Q_OBJECT
public:
    using PageVector = QVector<PdfPage *>;

    explicit PdfDocument(QObject *parent = nullptr);
    explicit PdfDocument(QFileInfo filepath, QObject *parent = nullptr);

    ~PdfDocument();

    QString
    name() const;

    const PageVector&
    pages() const;

    int
    pageCount() const;

    bool
    isLoaded() const;

signals:
    void
    loadProgress(int total, int current);

private:
    void
    purgeDoc();

    const QFileInfo m_filepath;
    int m_pageCount = 0;
    PageVector m_pages;
    FPDF_DOCUMENT m_fpdf;

    friend class PdfObjectLoader;
};

} // namespace dde

#endif // PDF_DOCUMENT_HPP
