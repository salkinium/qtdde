#ifndef ASYNC_PDFIUM_HPP
#define ASYNC_PDFIUM_HPP

#include <QObject>
#include <functional>
#include <QPixmap>
#include <QThread>
#include <QMutex>
#include <public/fpdfview.h>
#include <atomic>

Q_DECLARE_METATYPE(const char *)

namespace dde {

class PdfPageView;

class Pdfium : public QObject
{
    Q_OBJECT
public:
    explicit Pdfium(QObject *parent = nullptr);
    virtual ~Pdfium();

    // NOT THREAD-SAFE, DAMMIT!!
    static Pdfium *
    instance();

    void
    init();

    inline QMutex*
    mutex()
    { return &m; }

    FPDF_DOCUMENT
    load_document(QString filename);

    void
    close_document(FPDF_DOCUMENT doc);

    int
    get_page_count(FPDF_DOCUMENT doc);

    FPDF_PAGE
    load_page(FPDF_DOCUMENT doc, int pageNumber);

    void
    close_page(FPDF_PAGE page);

    void
    renderAsync(PdfPageView *page_view, qreal scale, const char *slot);

private:
    Q_INVOKABLE void
    renderAsyncPage(dde::PdfPageView *page, qreal scale, const char *slot);

    QThread t;
    QMutex m;

    bool m_inited = false;
};

inline Pdfium*
FPDF()
{
    return Pdfium::instance();
}

} // namespace dde

#endif // ASYNC_PDFIUM_HPP
