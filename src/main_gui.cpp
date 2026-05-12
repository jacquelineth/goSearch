// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// GUI application entry point (port of org.javatb.search.ui.UILauncher.main)
#include "ui/UILauncher.hpp"

#include <QApplication>
#include <QIcon>
#include <QString>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("Find4j");
    app.setOrganizationName("JavaTB");
    app.setWindowIcon(QIcon("images/JavaTB-icon.gif"));

    QString rootPath;
    if (argc > 1)
        rootPath = QString::fromLocal8Bit(argv[1]);

    javatb::UILauncher launcher(rootPath);
    launcher.show();

    return app.exec();
}
