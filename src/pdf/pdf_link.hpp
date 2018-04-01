#ifndef PDF_LINK_HPP
#define PDF_LINK_HPP

#include <QObject>
#include <QTransform>
#include <QString>
#include <QRectF>

namespace dde {

class PdfPage;

class PdfLink : public QObject
{
    Q_OBJECT
public:
    explicit PdfLink(PdfPage *page);

    inline QString
    uri() const
    { return m_uri; }

    inline QRectF
    bounds() const
    { return m_bounds; }

    inline int
    page() const
    { return m_page_index; }

    bool
    isInternal() const;

private:
    PdfPage *const m_page = nullptr;
    mutable QString m_uri;
    mutable QRectF m_bounds;
    mutable int m_page_index = -1;
    mutable quint32 m_obj_number = 0;

    friend class PdfObjectLoader;
};

} // namespace dde

#endif // PDF_LINK_HPP
