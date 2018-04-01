#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <pdf/pdf_document.hpp>
#include <views/pdf_document_view.hpp>
#include <passes/pass_manager.hpp>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void
    loadPdfAction();

private:
    void
    loadPdf(QString filename);

    Ui::MainWindow *const m_ui;
    dde::PdfDocument *m_doc;
    dde::PdfDocumentView *const m_doc_view;
//    dde::PassManager *m_manager = nullptr;
    QSqlDatabase m_db;
};

#endif // MAINWINDOW_HPP
