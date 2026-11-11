#include "userdashboard.h"
#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

UserDashboard::UserDashboard(QWidget *parent)
    : QMainWindow(parent)
    , m_welcomeLabel(new QLabel(this))
{
    setMinimumSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    m_welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(m_welcomeLabel);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    retranslate();
}

void UserDashboard::retranslate()
{
    setWindowTitle(tr("Espacio de Trabajo"));
    m_welcomeLabel->setText(tr("Bienvenido Usuario"));
}

void UserDashboard::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QMainWindow::changeEvent(event);
}
