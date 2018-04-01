#ifndef PDF_PAGE_VIEW_HPP
#define PDF_PAGE_VIEW_HPP

#include <QObject>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPixmap>

namespace dde {

class Pdfium;
class PdfPage;

class PdfPageView : public QObject
{
    Q_OBJECT
public:
    PdfPageView(QObject *parent = nullptr);
    PdfPageView(Pdfium *pdfium, const PdfPage *page, qreal y_offset = 0, QObject *parent = nullptr);
    PdfPageView(const PdfPageView &view);
    virtual ~PdfPageView();

    bool
    isVisibleInSceneRect(QRectF rect) const;

    const PdfPage *
    page();

    void
    addToView(QGraphicsView *view);

    void
    show();

    void
    hide();

public slots:
    void
    repaint(qreal zoom, qreal pixelratio);

private:
    Q_INVOKABLE void
    setPixmap(QPixmap map);

    Pdfium *const m_pdfium = nullptr;
    const PdfPage *const m_page = nullptr;
    QGraphicsItemGroup *const m_group = nullptr;
    QGraphicsPixmapItem *const m_image = nullptr;
    QGraphicsItemGroup *const m_chars = nullptr;
    QGraphicsItemGroup *const m_polygons = nullptr;
    QPointF m_offset;
    qreal m_zoom = 1;
    qreal m_pixelratio = 1;
    bool m_initialRender = true;

    friend class Pdfium;
};

} // namespace dde

Q_DECLARE_METATYPE(dde::PdfPageView)

#endif // PDF_PAGE_VIEW_HPP
