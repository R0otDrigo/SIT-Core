#include "dashboard.h"
#include "ui_dashboard.h"

#include <QEvent>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Dashboard)
{
    ui->setupUi(this);
    setMinimumSize(800, 600);
    retranslate();
}

Dashboard::~Dashboard()
{
    delete ui;
}

QString Dashboard::dashboardTitle() const
{
    return tr("Dashboard");
}

QString Dashboard::welcomeMessage() const
{
    return tr("Welcome");
}

void Dashboard::retranslate()
{
    setWindowTitle(dashboardTitle());
    ui->titleLabel->setText(dashboardTitle());
    ui->welcomeLabel->setText(welcomeMessage());
}

void Dashboard::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWidget::changeEvent(event);
}
