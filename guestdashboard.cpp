#include "guestdashboard.h"
#include <QVBoxLayout>
#include <QWidget>

GuestDashboard::GuestDashboard(QWidget *parent)
    : QMainWindow(parent)
    , m_welcomeLabel(new QLabel(this))
{
    setWindowTitle("Guest Dashboard");
    setMinimumSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    m_welcomeLabel->setText("Welcome Guest");
    m_welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(m_welcomeLabel);
    
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}
