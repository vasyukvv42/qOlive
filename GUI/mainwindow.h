#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "BasicXMLSyntaxHighlighter.h"
#include "xmltreemodel.h"
#include "appendchilddialog.h"
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

public slots:
    void showErrorMessage(const QString &title, const QString &info);

private slots:
    void on_actionAbout_Qt_triggered();

    void on_parseButton_clicked();

    void on_serializeButton_clicked();

    void on_actionOpen_triggered();

    void on_actionExpand_All_triggered();


    void on_actionRemove_Node_triggered();

    void on_actionAppend_Child_triggered();

    void on_actionCollapse_All_triggered();

private:
    Ui::MainWindow *ui;
    BasicXMLSyntaxHighlighter *xmlHighlighter;
    std::unique_ptr<XML::TreeModel> xmlTreeModel;

    QAction *createSeparator();
};

#endif // MAINWINDOW_H
