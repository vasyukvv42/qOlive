#ifndef APPENDCHILDDIALOG_H
#define APPENDCHILDDIALOG_H

#include <QDialog>
#include "xmltreemodel.h"

namespace Ui {
class AppendChildDialog;
}

class AppendChildDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppendChildDialog(XML::TreeModel *model, const QModelIndex &index, QWidget *parent = 0);
    ~AppendChildDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::AppendChildDialog *ui;

    XML::TreeModel *xmlTreeModel;
    QModelIndex index;
};

#endif // APPENDCHILDDIALOG_H
