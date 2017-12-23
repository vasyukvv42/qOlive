#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "BasicXMLSyntaxHighlighter.h"
#include "xmltreemodel.h"
#include "../XML/Parser.hpp"

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
    void on_actionAbout_Qt_triggered();

    void on_parseButton_clicked();

    void on_serializeButton_clicked();

private:
    Ui::MainWindow *ui;
    BasicXMLSyntaxHighlighter *xmlHighlighter;
    XML::TreeModel *xmlTreeModel;
};

#endif // MAINWINDOW_H
