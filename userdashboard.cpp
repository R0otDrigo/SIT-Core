#include "userdashboard.h"

UserDashboard::UserDashboard(QWidget *parent)
    : Dashboard(parent)
{
    retranslate();
}

QString UserDashboard::dashboardTitle() const
{
    return tr("Espacio de Trabajo");
}

QString UserDashboard::welcomeMessage() const
{
    return tr("Bienvenido Usuario");
}
