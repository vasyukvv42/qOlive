#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    xmlHighlighter = new BasicXMLSyntaxHighlighter(ui->textEdit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(nullptr);
}

void MainWindow::on_parseButton_clicked()
{
    auto text = ui->textEdit->toPlainText().toStdString();
    auto doc = XML::Parser::from_string(text);
    xmlTreeModel = new XML::TreeModel(doc);
    ui->treeView->setModel(xmlTreeModel);
}

void MainWindow::on_serializeButton_clicked()
{
    ui->textEdit->setPlainText(QString::fromStdString(xmlTreeModel->getDocument()->serialize(4)));
}
