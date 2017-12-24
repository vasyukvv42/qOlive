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

void MainWindow::saveFile()
{
    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showErrorMessage("Error", "Cannot open file");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
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
    ui->textEdit->setPlainText(QString::fromStdString(xmlTreeModel->getDocument()->serialize(2)));
}

void MainWindow::on_actionOpen_triggered()
{
    auto filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(),
                                                 "XML files (*.xml *.html *.xhtml)");
    if (filePath.size() != 0) {
        currentFile = filePath;
        QFile file(currentFile);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            showErrorMessage("Error", "Cannot open file");
            return;
        }
        QString data = file.readAll();

        ui->textEdit->setText(data);
    }
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
    auto index = ui->treeView->selectionModel()->currentIndex();
    auto node = xmlTreeModel->getItem(index);
    if (node->type() == XML::DOM::Node::Type::ELEMENT_NODE) {
        auto dialog = new AppendChildDialog(xmlTreeModel.get(), index, this);
        dialog->show();
    } else {
        showErrorMessage("Error", "Can only append child to Element nodes");
    }
}

void MainWindow::on_actionCollapse_All_triggered()
{
    ui->treeView->collapseAll();
}

void MainWindow::on_actionEdit_Attributes_triggered()
{
    auto index = ui->treeView->selectionModel()->currentIndex();
    auto node = xmlTreeModel->getItem(index);
    if (node->type() == XML::DOM::Node::Type::ELEMENT_NODE) {
        auto form = new AttributesWindow(dynamic_cast<XML::DOM::Element*>(node), this);
        connect(form, SIGNAL(errorOccurred(QString,QString)),
                this, SLOT(showErrorMessage(QString,QString)));
        form->show();
    } else {
        showErrorMessage("Error", "Only Element nodes can have attributes");
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFile.size() != 0) {
        saveFile();
    } else {
        on_actionSave_As_triggered();
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    auto filePath = QFileDialog::getSaveFileName(this, "Save File As", QDir::homePath(),
                                                 "XML files (*.xml *.html *.xhtml)");

    if (filePath.size() != 0) {
        currentFile = filePath;
        saveFile();
    }
}

void MainWindow::on_actionNew_File_triggered()
{
    if (ui->textEdit->toPlainText().size() != 0) {
        on_actionSave_As_triggered();
    }
    ui->textEdit->clear();
}
