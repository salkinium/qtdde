#include <QGraphicsView>
#include <QFile>
#include <QString>
#include <QDebug>
#include <iostream>
#include <QFileDialog>
#include <QSqlQuery>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <pdf/pdfium.hpp>
#include <pdf/pdf_object_loader.hpp>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_doc_view(new dde::PdfDocumentView)

    /*m_manager(new dde::PassManager(this))*/
{
    m_ui->setupUi(this);
    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::loadPdfAction);
    setCentralWidget(m_doc_view);

    // FIXME: This is temporary to load a default testing PDF quickly ***REMOVE***
    loadPdf("/Users/niklas/development/silvia/qtdde/test/test.pdf");
}

MainWindow::~MainWindow()
{
    delete m_ui;
    qDebug() << "~MainWindow";
}

void
MainWindow::loadPdf(QString filename)
{
    delete m_doc;
    m_doc = new dde::PdfDocument(filename);
    m_doc_view->setDocument(m_doc);
    dde::PdfObjectLoader::loadDocument(m_doc);
}

void
MainWindow::loadPdfAction()
{
    QString filename =
            QFileDialog::getOpenFileName(this,
                                         "Load PDF",
                                         "/Users/niklas/Downloads/uC/STM32",
                                         "PDF Files (*.pdf)");
    loadPdf(filename);
}
