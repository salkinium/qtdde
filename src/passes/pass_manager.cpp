#include "pass_manager.hpp"
#include <QPluginLoader>
#include <QDir>

namespace dde {

PassManager::PassManager(QObject *parent) : QObject(parent)
{
    loadPlugins();
    topologicalSort();
}

bool
PassManager::init(QString pdf_file, QSqlDatabase db)
{
    bool ok = true;
    m_db = db;

    // Create the metadata table and add the file path
    QSqlQuery query(m_db);
    ok &= query.exec("CREATE TABLE metadata("
               "key TEXT PRIMARY KEY,"
               "value TEXT)");
    if (not ok) {
        qCritical("Failed to create 'metadata' table!");
        return false;
    }
    query.prepare("INSERT INTO metadata VALUES ('filepath', :path)");
    query.bindValue(":path", QFileInfo(pdf_file).absoluteFilePath());
    ok &= query.exec();
    if (not ok) {
        qCritical("Failed to insert 'filepath' into 'metadata' table!");
        return false;
    }

    m_doc = std::make_unique<PdfDocument>();
    Q_CHECK_PTR(m_doc.get());
    return ok;
}

bool
PassManager::start()
{
    return startAt("dde:origin");
}

bool
PassManager::startAt(QString pass_name)
{
    if (not m_passes.contains(pass_name)) {
        qCritical() << "Pass" << pass_name << "does not exist!";
        return false;
    }

    QSqlQuery query(m_db);
    query.exec("SELECT value FROM metadata WHERE key = 'filepath';");
    query.next();
    QString filepath = query.value(0).toString();
//    m_doc->load(filepath);
    qDebug() << "Loading in process";
//    while(not m_doc->isLoaded());

    // XXX: nope
    PassNode pd = m_passes.value("dde:load:pdf");
    m_doc = std::make_unique<PdfDocument>();
    pd.pass->start(m_doc.get(), m_db);

    return true;
}

void
PassManager::topologicalSort()
{

}

int
PassManager::reloadPasses()
{
    // TODO: Only reload changed .dylibs and mark them as dirty
    m_passes.clear();
    return loadPlugins();
}

int
PassManager::loadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("lib");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        pluginLoader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            PassNode pd;
            pd.pass = qobject_cast<dde::PassInterface *>(plugin);
            QString pass_name = pd.pass->name();
            qDebug() << "Loading dde::Pass" << fileName << "called" << pass_name;
            m_passes[pass_name] = pd;
        }
    }

    // origin node is empty
    m_passes["dde:origin"] = PassNode();

    return m_passes.size();
}

} // namespace dde
