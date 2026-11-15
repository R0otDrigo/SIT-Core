#include "guestdashboard.h"

GuestDashboard::GuestDashboard(QWidget *parent)
    : Dashboard(parent)
{
    retranslate();
}

QString GuestDashboard::dashboardTitle() const
{
    return tr("Guest Dashboard");
}

QString GuestDashboard::welcomeMessage() const
{
    return tr("Welcome Guest");
}
