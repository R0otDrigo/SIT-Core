#include "admindashboard.h"
#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

AdminDashboard::AdminDashboard(QWidget *parent)
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

void AdminDashboard::retranslate()
{
    setWindowTitle(tr("Admin Dashboard"));
    m_welcomeLabel->setText(tr("Welcome Administrator"));
}

void AdminDashboard::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QMainWindow::changeEvent(event);
}
