#include "pdf_document_view.hpp"
#include <QDebug>
#include <QtConcurrent>
#include <QNativeGestureEvent>
#include <QApplication>

#include <pdf/pdf_document.hpp>
#include <pdf/pdfium.hpp>
#include <pdf/pdf_page.hpp>
#include "pdf_page_view.hpp"

namespace dde {

PdfDocumentView::PdfDocumentView(QWidget *parent) :
    QGraphicsView(parent), m_pdfium(new Pdfium)
{
    setScene(&m_scene);
    setBackgroundBrush(QBrush(QColor(Qt::lightGray)));
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHints(QPainter::Antialiasing);
    connect(this, &PdfDocumentView::nextPageReady, this, &PdfDocumentView::renderNextPage, Qt::QueuedConnection);
}

PdfDocumentView::~PdfDocumentView()
{
    qDebug() << "~PdfDocumentView";
    for (auto *const i: m_scene.items())
    {
        m_scene.removeItem(i);
    }
}

void
PdfDocumentView::setDocument(PdfDocument *doc)
{
    if (m_doc) {
        disconnect(m_conn_load_progress);
    }
    if (doc == nullptr) return;
    m_doc = doc;
    m_conn_load_progress = connect(m_doc, &PdfDocument::loadProgress, this, &PdfDocumentView::updateLoadProgress);
}

void
PdfDocumentView::updateLoadProgress(int total, int progress)
{
    if (progress == 0)
    {
        // clear and reset everything
        m_scene.clear();
        m_pageviews.clear();
        m_pixelratio = devicePixelRatioF();
        m_zoom = 1;
        resetTransform();
        m_pageviews.reserve(total);
        // setup the background load bar
        m_scene.addRect(-250, 500, 500, 10, Qt::NoPen, Qt::darkGray);
        // and the foregrould load bar
        m_load_bar = m_scene.addRect(0, 0, 0, 0, Qt::NoPen, Qt::white);
        m_scene.setSceneRect(m_scene.itemsBoundingRect());
        centerOn(m_load_bar);
    }
    else if (total == progress)
    {
        m_scene.clear();
        m_scene.setSceneRect(QRectF());
        resetTransform();
        m_page_load_timer.restart();
        emit nextPageReady(0, 0);
    }
    else
    {
        m_load_bar->setRect(QRectF(-250, 500, 500.0 * progress / total, 10));
    }
}

void
PdfDocumentView::renderNextPage(int index, int offset)
{
    if (index >= m_doc->pages().size()) {
        m_scene.update();
        updatePageRender();
        qDebug() << "Finished displaying" << m_pageviews.size() << "pages in" << m_page_load_timer.elapsed() << "ms!";
        return;
    }
    PdfPage *const page = m_doc->pages()[index];
    auto *page_view = new PdfPageView(m_pdfium, page, offset, this);
    page_view->addToView(this);
    // cache this information
    m_pageviews.push_back(page_view);
    // increase offset for next page
    offset += page->pageSize().height();
    offset += 7;
    if (index % 100 == 3) {
        m_scene.update();
        updatePageRender();
        qDebug() << "Rendered page" << index + 1 << "in" << m_page_load_timer.elapsed() << "ms...";
    }

    emit nextPageReady(index + 1, offset);
}

void
PdfDocumentView::updatePageRender()
{
    QRectF sceneRect = mapToScene(viewport()->rect()).boundingRect();
    for (PdfPageView *const page_view: m_pageviews)
    {
        if (page_view->isVisibleInSceneRect(sceneRect))
        {
            page_view->show();
            if(not isZooming) {
                page_view->repaint(m_zoom, m_pixelratio);
            }
        }
        else page_view->hide();
    }
}

bool
PdfDocumentView::event(QEvent *event)
{
    bool processed = QGraphicsView::event(event);
//    qDebug() << event->type() << processed;
    switch(event->type())
    {
    case QEvent::NativeGesture:
    {
        QNativeGestureEvent *gesture = static_cast<QNativeGestureEvent *>(event);
        if (gesture->gestureType() == Qt::ZoomNativeGesture) {
            isZooming = true;
        }
        qreal multiplier = (1 + gesture->value());
        qreal new_zoom = m_zoom * multiplier;
        if (0.1 <= new_zoom and new_zoom <= 10.0)
        {
            scale(multiplier, multiplier);
            m_zoom = new_zoom;
        }
        if (gesture->gestureType() == Qt::EndNativeGesture) {
            updatePageRender();
            isZooming = false;
        }
        return true;
    }
    case QEvent::ScreenChangeInternal:
    {
        if (not qFuzzyCompare(m_pixelratio, devicePixelRatioF()))
        {
            m_pixelratio = devicePixelRatioF();
        }
        // fallthrough
    }
    case QEvent::LayoutRequest:
    case QEvent::Resize:
    {
        updatePageRender();
        return true;
    }
    default: break;
    }
    return processed;
}

void
PdfDocumentView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    updatePageRender();
}

} // namespace dde
