#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP

#include "pass_interface.hpp"
#include <pdf/pdf_document.hpp>
#include <QSqlDatabase>
#include <QObject>
#include <memory>

namespace dde {

class PassManager : public QObject
{
    Q_OBJECT
public:
    explicit PassManager(QObject *parent = nullptr);

    bool
    init(QString pdf_file, QSqlDatabase db);

    int
    reloadPasses();

public slots:
    bool
    start();

    bool
    startAt(QString pass_name);

private:
    int
    loadPlugins();

    void
    topologicalSort();

    struct PassNode
    {
        dde::PassInterface *pass = nullptr;
        bool isDirty = true;
        QStringList parents;
        QStringList children;
    };

    QMap<QString, PassNode> m_passes;
    std::unique_ptr<dde::PdfDocument> m_doc;
    QSqlDatabase m_db;
};

} // namespace dde

#endif // PASS_MANAGER_HPP
