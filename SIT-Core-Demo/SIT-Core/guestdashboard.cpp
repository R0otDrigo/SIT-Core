#include "guestdashboard.h"
#include "admindashboard.h"
#include "movementdashboard.h"
#include "roles.h"
#include "theme.h"
#include "trafficdashboard.h"
#include "uihelpers.h"
#include "userdashboard.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

GuestDashboard::GuestDashboard(const QString &userName, QWidget *parent)
    : QWidget(parent)
{
    setObjectName("dashRoot");
    setAttribute(Qt::WA_StyledBackground, true);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ---------- Barra superior con selector de rol ----------
    auto *bar = new QFrame;
    bar->setObjectName("header");
    bar->setFixedHeight(64);
    auto *bh = new QHBoxLayout(bar);
    bh->setContentsMargins(20, 0, 20, 0);
    bh->setSpacing(8);

    bh->addWidget(UI::label(QStringLiteral("🚚  SIT-Core"), "headerTitle"));
    bh->addWidget(UI::label(tr("Sandbox"), "roleChip"), 0, Qt::AlignVCenter);
    bh->addSpacing(20);

    auto *stack = new QStackedWidget;
    auto *group = new QButtonGroup(this);
    group->setExclusive(true);

    struct Entry { Role role; QWidget *view; };
    const QList<Entry> entries = {
        { Role::FleetManager,               new AdminDashboard(userName, true) },
        { Role::User,                       new UserDashboard(userName, true) },
        { Role::JefeDeTrafico,              new TrafficDashboard(userName, true) },
        { Role::ResponsableDesplazamientos, new MovementDashboard(userName, true) },
    };

    for (int i = 0; i < entries.size(); ++i) {
        stack->addWidget(entries[i].view);
        auto *btn = UI::button(Roles::label(entries[i].role), "roleTab");
        btn->setToolTip(Roles::id(entries[i].role));
        btn->setCheckable(true);
        group->addButton(btn, i);
        bh->addWidget(btn);
        connect(btn, &QPushButton::clicked, stack, [stack, i]() { stack->setCurrentIndex(i); });
        if (i == 0)
            btn->setChecked(true);
    }

    bh->addStretch();
    bh->addWidget(UI::label(userName, "headerUser"));
    auto *logout = UI::button(tr("Cerrar sesión"), "logout");
    connect(logout, &QPushButton::clicked, this, &GuestDashboard::logoutRequested);
    bh->addWidget(logout);
    root->addWidget(bar);

    // ---------- Aviso Sandbox ----------
    auto *note = new QLabel(tr("Entorno de pruebas: previsualiza cada rol con datos simulados. "
                               "Los cambios no se guardan."));
    note->setAlignment(Qt::AlignCenter);
    note->setFixedHeight(30);
    note->setStyleSheet(QStringLiteral("background:#FEF3C7; color:#92400E; font-weight:600;"));
    root->addWidget(note);

    root->addWidget(stack, 1);
}
