#include <QApplication>

#include "mainwindow.h"
#include "theme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("SIT-Core"));
    QApplication::setOrganizationName(QStringLiteral("SIT"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(Theme::styleSheet());

    MainWindow w;
    w.show();
    return app.exec();
}
