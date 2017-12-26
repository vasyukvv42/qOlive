#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>

#include "BasicXMLSyntaxHighlighter.h"
#include "xmltreemodel.h"
#include "appendchilddialog.h"
#include "attributeswindow.h"
#include "Parser.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void saveFile();

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

    void on_actionEdit_Attributes_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_File_triggered();

private:
    Ui::MainWindow *ui;
    BasicXMLSyntaxHighlighter *xmlHighlighter;
    std::unique_ptr<XML::TreeModel> xmlTreeModel;
    QString currentFile;

    QAction *createSeparator();
};

#endif // MAINWINDOW_H
