#include "dashboardbase.h"
#include "uihelpers.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

DashboardBase::DashboardBase(const QString &roleLabel, const QString &userName,
                             bool embedded, QWidget *parent)
    : QWidget(parent)
{
    setObjectName("dashRoot");
    setAttribute(Qt::WA_StyledBackground, true);

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ---------- Sidebar ----------
    auto *sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(232);
    auto *sv = new QVBoxLayout(sidebar);
    sv->setContentsMargins(16, 22, 16, 16);
    sv->setSpacing(6);

    sv->addWidget(UI::label(QStringLiteral("🚚  SIT-Core"), "logo"));
    sv->addWidget(UI::label(tr("Gestión y Monitoreo de Flotas"), "sidebarSub"));
    sv->addSpacing(20);

    m_navLayout = new QVBoxLayout;
    m_navLayout->setSpacing(4);
    sv->addLayout(m_navLayout);
    sv->addStretch();

    auto *sep = new QFrame;
    sep->setObjectName("sidebarSep");
    sv->addWidget(sep);
    sv->addSpacing(6);
    sv->addWidget(UI::label(userName, "sidebarUser"));
    sv->addWidget(UI::label(roleLabel, "sidebarRole"));
    if (!embedded) {
        sv->addSpacing(8);
        auto *logout = UI::button(tr("Cerrar sesión"), "logout");
        connect(logout, &QPushButton::clicked, this, &DashboardBase::logoutRequested);
        sv->addWidget(logout);
    }
    root->addWidget(sidebar);

    // ---------- Header + contenido ----------
    auto *right = new QVBoxLayout;
    right->setContentsMargins(0, 0, 0, 0);
    right->setSpacing(0);

    auto *header = new QFrame;
    header->setObjectName("header");
    header->setFixedHeight(64);
    auto *hh = new QHBoxLayout(header);
    hh->setContentsMargins(24, 0, 24, 0);
    m_pageTitle = UI::label(QString(), "headerTitle");
    hh->addWidget(m_pageTitle);
    hh->addStretch();
    hh->addWidget(UI::label(roleLabel, "roleChip"), 0, Qt::AlignVCenter);
    hh->addSpacing(8);
    hh->addWidget(UI::label(userName, "headerUser"));
    right->addWidget(header);

    m_stack = new QStackedWidget;
    m_stack->setObjectName("pageArea");
    right->addWidget(m_stack, 1);
    root->addLayout(right, 1);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);
}

void DashboardBase::addPage(const QString &navText, QWidget *page)
{
    const int index = m_stack->count();
    m_stack->addWidget(UI::pageWithScroll(page));

    auto *btn = new QPushButton(navText);
    btn->setObjectName("navButton");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    m_group->addButton(btn, index);
    m_navLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, [this, index, navText]() {
        selectPage(index, navText);
    });

    if (index == 0) {
        btn->setChecked(true);
        selectPage(0, navText);
    }
}

void DashboardBase::selectPage(int index, const QString &title)
{
    m_stack->setCurrentIndex(index);
    m_pageTitle->setText(title);
}
