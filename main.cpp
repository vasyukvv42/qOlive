#include "GUI/mainwindow.h"
#include <QApplication>
#include <iostream>
#include "XML/Parser.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto doc = "<test>text<xd><test/></xd></test>"_xml;

    for (auto&& ele : doc->get_elements_by_tag_name("test")){
        ele->set_attribute("a", "b");
    }

    std::cout << doc->serialize(4);
    MainWindow w;
    w.show();

    return a.exec();
}
