#include "apptheme.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QStyleFactory>

void applyAppTheme(QApplication &app) {
    // Los recursos viven en la libreria estatica sit_ui: hay que inicializarlos
    // explicitamente o el linker descarta el objeto generado por rcc.
    Q_INIT_RESOURCE(resources);

    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);

    QFile styleFile(QStringLiteral(":/styles/style.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
}
