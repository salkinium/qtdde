#ifndef PDF_CHAR_HPP
#define PDF_CHAR_HPP

#include <QObject>
#include <QTransform>
#include <QRectF>
#include <QPointF>
#include <QString>

namespace dde {

class PdfPage;

class PdfChar : public QObject
{
    Q_OBJECT
public:
    explicit PdfChar(PdfPage *page);

    inline const QRectF&
    bounds() const
    { return m_bounds; }

    inline qreal
    fontsize() const
    { return m_fontsize; }

    inline wchar_t
    unicode() const
    { return m_unicode; }

    inline QString
    fontname() const
    { return m_fontname; }

    inline QString
    stylename() const
    { return m_stylename; }

private:
    PdfPage *const m_page = nullptr;
    mutable wchar_t m_unicode = 0;
    mutable wchar_t m_charcode = 0;
    mutable int m_flags = 0;
    mutable qreal m_fontsize = 0;
    mutable QPointF m_origin;
    mutable QRectF m_bounds;
    mutable QTransform m_ctm;
    mutable void *m_font = nullptr;
    mutable void *m_face = nullptr;

    mutable QString m_basename;
    mutable QString m_fontname;
    mutable QString m_stylename;

    friend class PdfObjectLoader;
};

} // namespace dde

#endif // PDF_CHAR_HPP
