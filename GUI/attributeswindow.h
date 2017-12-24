#ifndef ATTRIBUTESWINDOW_H
#define ATTRIBUTESWINDOW_H

#include <QWidget>
#include "../XML/DOM.hpp"

namespace Ui {
class AttributesWindow;
}

class AttributesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AttributesWindow(XML::DOM::Element *element, QWidget *parent = 0);
    ~AttributesWindow();
    void updateTable();

signals:
    void errorOccurred(const QString &title, const QString &info);

private slots:
    void on_setAttributeButton_clicked();

private:
    Ui::AttributesWindow *ui;
    XML::DOM::Element *element;
};

#endif // ATTRIBUTESWINDOW_H
