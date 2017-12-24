#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->setModel(xmlTreeModel.get());

    ui->treeView->addAction(ui->actionExpand_All);
    ui->treeView->addAction(ui->actionCollapse_All);
    ui->treeView->addAction(createSeparator());
    ui->treeView->addAction(ui->actionAppend_Child);
    ui->treeView->addAction(ui->actionRemove_Node);
    ui->treeView->addAction(createSeparator());
    ui->treeView->addAction(ui->actionEdit_Attributes);

    xmlHighlighter = new BasicXMLSyntaxHighlighter(ui->textEdit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showErrorMessage(const QString &title, const QString &info)
{
    QMessageBox::critical(this, title, info);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(nullptr);
}

void MainWindow::on_parseButton_clicked()
{
    auto text = ui->textEdit->toPlainText().toStdString();
    try {
        auto doc = XML::Parser::from_string(text);
        xmlTreeModel = std::make_unique<XML::TreeModel>(doc);
        ui->treeView->setModel(xmlTreeModel.get());
        connect(xmlTreeModel.get(), SIGNAL(errorOccurred(QString, QString)),
                this, SLOT(showErrorMessage(QString, QString)));
    } catch(XML::SyntaxError &e) {
        showErrorMessage("Syntax Error", e.what());
    } catch(XML::DOMError &e) {
        showErrorMessage("DOM Error", e.what());
    }
}

void MainWindow::on_serializeButton_clicked()
{
    ui->textEdit->setPlainText(QString::fromStdString(xmlTreeModel->getDocument()->serialize(4)));
}

void MainWindow::on_actionOpen_triggered()
{

}

void MainWindow::on_actionExpand_All_triggered()
{
    ui->treeView->expandAll();
}

QAction *MainWindow::createSeparator()
{
    auto *separator = new QAction(this);
    separator->setSeparator(true);
    return separator;
}

void MainWindow::on_actionRemove_Node_triggered()
{
    auto index = ui->treeView->selectionModel()->currentIndex();
    xmlTreeModel->removeRows(index.row(), 1, index.parent());
}

void MainWindow::on_actionAppend_Child_triggered()
{
    auto dialog = new AppendChildDialog(xmlTreeModel.get(),
                                        ui->treeView->selectionModel()->currentIndex(),
                                        this);
    dialog->show();
}

void MainWindow::on_actionCollapse_All_triggered()
{
    ui->treeView->collapseAll();
}
