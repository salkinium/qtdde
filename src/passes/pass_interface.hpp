#ifndef PASS_INTERFACE_HPP
#define PASS_INTERFACE_HPP

#include <QStringList>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace dde {

class PdfDocument;

class PassInterface
{
public:
    inline virtual
    ~PassInterface() {}

    virtual int
    version() const = 0;

    virtual QString
    name() const = 0;

    virtual QStringList
    depends() const = 0;

public slots:
    virtual bool
    start(PdfDocument *const doc, QSqlDatabase db) = 0;

    virtual bool
    abort() = 0;

signals:
    virtual void
    progress(qreal) = 0;

    virtual void
    finished() = 0;
};

} // namespace dde

#define dde_pass_interface_iid "com.salkinium.silvia.dde.pass_interface.v1"
Q_DECLARE_INTERFACE(dde::PassInterface, dde_pass_interface_iid)
#define dde_pass_interface_version ((int)1)

#endif // PASS_INTERFACE_HPP
