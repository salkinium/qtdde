
#include "pdf_page_view.hpp"

#include <QObject>
#include <QPen>
#include <QMetaObject>
#include <QDebug>

#include <pdf/pdfium.hpp>
#include <pdf/pdf_page.hpp>

namespace dde {

PdfPageView::PdfPageView(QObject *parent) :
    QObject(parent)
{
}

PdfPageView::PdfPageView(Pdfium *const pdfium, const PdfPage *const page, qreal y_offset, QObject *parent) :
    QObject(parent),
    m_pdfium(pdfium),
    m_page(page),
    m_group(new QGraphicsItemGroup),
    m_image(new QGraphicsPixmapItem),
    m_chars(new QGraphicsItemGroup),
    m_polygons(new QGraphicsItemGroup),
    m_offset(-page->pageSize().width()/2, y_offset)
{
    QPixmap empty(page->pageSize().toSize());
    empty.fill(Qt::white);
    m_image->setPixmap(empty);
    m_image->setOffset(m_offset);
    m_group->addToGroup(m_image);
    m_group->addToGroup(m_polygons);
    m_group->addToGroup(m_chars);
    m_group->hide();

//    m_polygons->hide();
//    m_chars->hide();
}

PdfPageView::PdfPageView(const PdfPageView &view) :
    m_pdfium(view.m_pdfium), m_page(view.m_page),
    m_image(view.m_image), m_chars(view.m_chars),
    m_polygons(view.m_polygons), m_offset(view.m_offset),
    m_zoom(view.m_zoom), m_pixelratio(view.m_pixelratio),
    m_initialRender(view.m_initialRender)
{
}

PdfPageView::~PdfPageView()
{
    if (this->parent() == nullptr) {
        // only if our QGraphicsItemGroup has no owner do we need to delete it
        // but it should be owned by the QGraphicsScene and deleted by it
        qDebug() << "~PdfPageView{delete m_group;}";
        delete m_group;
    }
    else qDebug() << "~PdfPageView";
}

void
PdfPageView::addToView(QGraphicsView *view)
{
    view->scene()->addItem(m_group);
}

void
PdfPageView::show()
{
    m_group->show();
}

void
PdfPageView::hide()
{
    m_group->hide();
}

bool
PdfPageView::isVisibleInSceneRect(QRectF rect) const
{
    QRectF itemRect = m_image->mapRectFromScene(rect);
    return itemRect.intersects(m_image->boundingRect());
}

const PdfPage *
PdfPageView::page()
{
    return m_page;
}

void
PdfPageView::setPixmap(QPixmap map)
{
    m_image->setPixmap(map);
}

void
PdfPageView::repaint(qreal zoom, qreal pixelratio)
{
    if (m_initialRender)
    {
        // add the vector data
        for (PdfChar *const c: m_page->chars())
        {
            auto *rect = new QGraphicsRectItem(c->bounds().translated(m_offset));
            rect->setPen(QPen(QColor(255, 0, 0, 100)));
            m_chars->addToGroup(rect);
        }
        for (QPolygonF const &p: m_page->polygons())
        {
            auto *poly = new QGraphicsPolygonItem(p.translated(m_offset));
            poly->setPen(QPen(QColor(0, 0, 255, 50)));
            m_polygons->addToGroup(poly);
        }
        for (PdfLink *const l: m_page->m_links)
        {
            auto *rect = new QGraphicsRectItem(l->bounds().translated(m_offset));
            rect->setPen(QPen(QColor(0, 255, 0, 130)));
            m_chars->addToGroup(rect);
        }
    }
    if (m_initialRender or not qFuzzyCompare(m_zoom, zoom) or not qFuzzyCompare(m_pixelratio, pixelratio))
    {
        m_pixelratio = pixelratio;
        m_zoom = zoom;
        m_pdfium->renderAsync(this, zoom * pixelratio, "setPixmap");

        if (m_zoom < 0.5) {
            m_polygons->hide();
            m_chars->hide();
        }
        else {
            m_polygons->show();
            m_chars->show();
        }
        m_initialRender = false;
    }
}

} // namespace dde
