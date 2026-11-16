#include "admindashboard.h"

AdminDashboard::AdminDashboard(QWidget *parent)
    : Dashboard(parent)
{
    retranslate();
}

QString AdminDashboard::dashboardTitle() const
{
    return tr("Admin Dashboard");
}

QString AdminDashboard::welcomeMessage() const
{
    return tr("Welcome Administrator");
}
