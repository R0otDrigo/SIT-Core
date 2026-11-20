#include "guestdashboard.h"

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

GuestDashboard::GuestDashboard(QWidget *parent)
    : Dashboard(parent)
{
    auto *card = new QFrame(this);
    card->setObjectName("emptyState");
    card->setMaximumWidth(520);
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(32, 32, 32, 32);
    layout->setSpacing(10);

    auto *title = new QLabel(tr("Estás explorando como invitado"), card);
    title->setObjectName("emptyTitle");
    auto *text = new QLabel(
        tr("Inicia sesión con una cuenta de usuario para seguir tu bus en el mapa "
           "o con una cuenta de administrador para controlar la flota."), card);
    text->setObjectName("emptyText");
    text->setWordWrap(true);

    layout->addWidget(title);
    layout->addWidget(text);

    auto *row = new QHBoxLayout;
    row->addStretch();
    row->addWidget(card);
    row->addStretch();
    contentLayout()->addStretch();
    contentLayout()->addLayout(row);
    contentLayout()->addStretch();

    retranslate();
}

QString GuestDashboard::dashboardTitle() const
{
    return tr("Modo invitado");
}

QString GuestDashboard::welcomeMessage() const
{
    return tr("Bienvenido, invitado");
}
