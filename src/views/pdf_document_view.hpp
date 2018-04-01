#ifndef PDF_VIEWER_HPP
#define PDF_VIEWER_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFutureWatcher>
#include <QElapsedTimer>

namespace dde {

class PdfDocument;
class Pdfium;
class PdfPageView;

class PdfDocumentView : public QGraphicsView
{
    Q_OBJECT
public:
    using PageVector = QVector<PdfPageView *>;

    explicit PdfDocumentView(QWidget *parent = nullptr);
    ~PdfDocumentView();

    void
    setDocument(PdfDocument *doc);

protected:
    bool
    event(QEvent *event);

    void
    scrollContentsBy(int dx, int dy);

signals:
    void
    nextPageReady(int page, int offset);

private slots:
    void
    updateLoadProgress(int total, int progress);

    void
    renderNextPage(int page, int offset);

private:
    void
    updatePageRender();

    Pdfium *const m_pdfium = nullptr;
    PdfDocument *m_doc = nullptr;
    QGraphicsScene m_scene;
    PageVector m_pageviews;

    qreal m_pixelratio = 1;
    qreal m_zoom = 1;
    bool isZooming = false;

    QMetaObject::Connection m_conn_load_progress;

    QElapsedTimer m_page_load_timer;

    QGraphicsRectItem *m_load_bar = nullptr;
};

} // namespace dde

#endif // PDF_VIEWER_HPP
