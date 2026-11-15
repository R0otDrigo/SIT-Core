#include "guestdashboard.h"

#include <QLabel>
#include <QVBoxLayout>

GuestDashboard::GuestDashboard(QWidget *parent)
    : Dashboard(parent)
{
    retranslate();

    auto *info = new QLabel(
        tr("Como invitado solo puedes ver esta pantalla. Para ver tu bus, recibir mensajes "
           "y llamar al administrador, pide a un administrador que te cree una cuenta."),
        this);
    info->setProperty("role", "muted");
    info->setWordWrap(true);
    contentLayout()->addWidget(info);
    contentLayout()->addStretch();
}

QString GuestDashboard::dashboardTitle() const
{
    return tr("Guest Dashboard");
}

QString GuestDashboard::welcomeMessage() const
{
    return tr("Welcome Guest");
}
