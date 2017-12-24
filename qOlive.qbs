import qbs

Project {
    minimumQbsVersion: "1.7.1"

    CppApplication {
        Depends { name: "Qt"; submodules: ["core", "gui", "widgets"] }

        cpp.cxxLanguageVersion: "c++17"

        cpp.defines: [
            // The following define makes your compiler emit warnings if you use
            // any feature of Qt which as been marked deprecated (the exact warnings
            // depend on your compiler). Please consult the documentation of the
            // deprecated API in order to know how to port your code away from it.
            "QT_DEPRECATED_WARNINGS",

            // You can also make your code fail to compile if you use deprecated APIs.
            // In order to do so, uncomment the following line.
            // You can also select to disable deprecated APIs only up to a certain version of Qt.
            //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
        ]

        consoleApplication: true
        files: [
            "GUI/BasicXMLSyntaxHighlighter.cpp",
            "GUI/BasicXMLSyntaxHighlighter.h",
            "GUI/appendchilddialog.cpp",
            "GUI/appendchilddialog.h",
            "GUI/appendchilddialog.ui",
            "GUI/mainwindow.cpp",
            "GUI/mainwindow.h",
            "GUI/mainwindow.ui",
            "GUI/xmltreemodel.cpp",
            "GUI/xmltreemodel.h",
            "XML/DOM.cpp",
            "XML/DOM.hpp",
            "XML/Errors.cpp",
            "XML/Errors.hpp",
            "XML/Lexer.cpp",
            "XML/Lexer.hpp",
            "XML/Parser.cpp",
            "XML/Parser.hpp",
            "XML/Token.cpp",
            "XML/Token.hpp",
            "main.cpp",
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: "application"
            qbs.install: true
        }
    }
}
