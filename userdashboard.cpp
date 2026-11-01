#include "userdashboard.h"
#include <QVBoxLayout>
#include <QWidget>

UserDashboard::UserDashboard(QWidget *parent)
    : QMainWindow(parent)
    , m_welcomeLabel(new QLabel(this))
{
    setWindowTitle("User Dashboard");
    setMinimumSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    m_welcomeLabel->setText("Welcome User");
    m_welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(m_welcomeLabel);
    
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}
