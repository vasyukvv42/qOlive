#include "attributeswindow.h"
#include "ui_attributeswindow.h"

AttributesWindow::AttributesWindow(XML::DOM::Element *element, QWidget *parent) :
    QWidget(parent, Qt::Dialog),
    ui(new Ui::AttributesWindow),
    element(element)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    updateTable();

}

AttributesWindow::~AttributesWindow()
{
    delete ui;
}

void AttributesWindow::updateTable()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(element->attributes().size());
    int row = 0;
    for (auto &&pair : element->attributes()) {
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(pair.first)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(pair.second)));
        row++;
    }
}

void AttributesWindow::on_setAttributeButton_clicked()
{
    auto name = ui->nameEdit->text().toStdString();
    auto value = ui->valueEdit->text().toStdString();

    if (not value.empty()) {
        try {
            element->set_attribute(name, value);
        } catch (XML::SyntaxError &e) {
            emit errorOccurred("Syntax Error", e.what());
        }
    } else if (not name.empty()) {
        if (element->has_attribute(name)) {
            element->remove_attribute(name);
        }
    }

    updateTable();
}
