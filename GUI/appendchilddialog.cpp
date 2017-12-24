#include "appendchilddialog.h"
#include "ui_appendchilddialog.h"

AppendChildDialog::AppendChildDialog(XML::TreeModel *model, const QModelIndex &index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppendChildDialog),
    xmlTreeModel(model),
    index(index)
{
    ui->setupUi(this);
}

AppendChildDialog::~AppendChildDialog()
{
    delete ui;
}

void AppendChildDialog::on_buttonBox_accepted()
{
    XML::DOM::Node *node = nullptr;
    auto textContent = ui->textEdit->toPlainText().toStdString();
    auto nodeName = ui->nameEdit->text().toStdString();
    try {
        switch (ui->comboBox->currentIndex()) {
        case 0: {
            if (not textContent.empty())
                node = new XML::DOM::Text(textContent);
            break;
        }
        case 1: {
            if (not textContent.empty())
                node = new XML::DOM::Comment(textContent);
            break;
        }
        case 2: {
            if (not textContent.empty())
                node = new XML::DOM::CDATASection(textContent);
            break;
        }
        case 3: {
            if (not nodeName.empty()) {
                XML::Lexer::validate_name(nodeName);
                node = new XML::DOM::Element(nodeName);
                if (not textContent.empty())
                    node->set_text_content(textContent);
            }
            break;
        }
        default:
            break;
        }
    } catch (XML::SyntaxError &e) {
        emit xmlTreeModel->errorOccurred("Syntax Error", e.what());
    } catch (XML::DOMError &e) {
        emit xmlTreeModel->errorOccurred("DOM Error", e.what());
    }
    if (node)
        xmlTreeModel->appendChild(index, node);
}

void AppendChildDialog::on_buttonBox_rejected()
{
    close();
}
