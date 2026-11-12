#include "userdashboard.h"
#include "roles.h"
#include "theme.h"
#include "uihelpers.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

UserDashboard::UserDashboard(const QString &userName, bool embedded, QWidget *parent)
    : DashboardBase(Roles::label(Role::User), userName, embedded, parent)
{
    addPage(tr("Mi panel"), buildPanel());
    addPage(tr("Mis recorridos"), buildTrips());
}

QWidget *UserDashboard::buildPanel()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);

    v->addWidget(UI::label(tr("Mi actividad de hoy"), "h1"));
    v->addLayout(UI::hbox({
        UI::statCard(tr("Pasajeros validados"), "2 840", tr("Esta semana: 12 840"), Theme::Accent),
        UI::statCard(tr("Vueltas completadas"), "23", tr("Límite diario: 30"), Theme::Green),
        UI::statCard(tr("Frecuencia de paso"), "4 min", tr("Promedio widget SIT"), Theme::Amber),
        UI::statCard(tr("Puntualidad"), "93%", tr("Promedio mensual"), Theme::Green),
    }));

    // Próxima ruta
    auto *route = new QWidget;
    auto *rv = new QVBoxLayout(route);
    rv->setContentsMargins(0, 0, 0, 0);
    rv->setSpacing(8);
    auto *rt = UI::label(tr("Arequipa  →  Cayma / Centro"));
    rt->setStyleSheet(QStringLiteral("font-size:20px; font-weight:700; color:%1;").arg(Theme::Navy));
    rv->addWidget(rt);
    rv->addWidget(UI::label(tr("Salida programada: mañana 06:30  ·  Andén B SIT")));
    rv->addSpacing(6);
    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(24);
    grid->setVerticalSpacing(6);
    const QList<QPair<QString, QString>> info = {
        { tr("Distancia"), "8.5 km" }, { tr("Duración estimada"), "22 min" },
        { tr("Desembarques"), tr("42 / 60 pasajeros") }, { tr("Cliente"), tr("Servicio SIT") },
        { tr("Ruta ID"), tr("A-25 Cayma → Centro") },
    };
    for (int i = 0; i < info.size(); ++i) {
        grid->addWidget(UI::label(info[i].first, "muted"), i / 2 * 2, i % 2);
        auto *val = UI::label(info[i].second);
        val->setStyleSheet(QStringLiteral("font-weight:600;"));
        grid->addWidget(val, i / 2 * 2 + 1, i % 2);
    }
    rv->addLayout(grid);

    // Estado de la unidad
    auto *unit = new QWidget;
    auto *uv = new QVBoxLayout(unit);
    uv->setContentsMargins(0, 0, 0, 0);
    uv->setSpacing(10);
    auto *head = new QHBoxLayout;
    auto *plate = UI::label(QStringLiteral("V8A-912"));
    plate->setStyleSheet(QStringLiteral("font-size:16px; font-weight:700;"));
    head->addWidget(plate);
    head->addStretch();
    auto *badge = UI::label(QStringLiteral("● A tiempo"));
    badge->setStyleSheet(QStringLiteral("color:%1; font-weight:700;").arg(Theme::Green));
    head->addWidget(badge);
    uv->addLayout(head);
    uv->addWidget(UI::meter(tr("Pasajeros validados (tarjetas)"), 72, Theme::Green));
    uv->addWidget(UI::meter(tr("Vueltas completadas"), 88, Theme::Green));
    uv->addWidget(UI::meter(tr("Frecuencia de paso (min)"), 41, Theme::Amber));
    uv->addWidget(UI::meter(tr("Próximo mantenimiento"), 64, Theme::Accent, "2 500 km"));

    auto *row = new QHBoxLayout;
    row->setSpacing(16);
    row->addWidget(UI::panel(tr("Próxima ruta asignada"), route), 1);
    row->addWidget(UI::panel(tr("Estado de mi unidad"), unit), 1);
    v->addLayout(row);
    return page;
}

QWidget *UserDashboard::buildTrips()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);
    v->addWidget(UI::label(tr("Mis recorridos"), "h1"));
    v->addLayout(UI::hbox({
        UI::statCard(tr("Km este mes"), "5 120", tr("Meta: 5 500 km"), Theme::Accent),
        UI::statCard(tr("Viajes completados"), "18", tr("2 en progreso"), Theme::Green),
        UI::statCard(tr("Puntualidad"), "94%", tr("Promedio mensual"), Theme::Green),
    }));
    auto *table = UI::makeTable(
        { tr("Fecha"), tr("Ruta"), tr("Pasajeros"), tr("Tarjeta SIT"), tr("Estado") },
        {
            { "10/11/2026", "C-1A Troncal",    "156", "V8A-912", "Completado" },
            { "08/11/2026", "A-25 Alimentadora", "89", "V4T-751", "Completado" },
            { "06/11/2026", "T-14 Estructurante", "210", "V6K-405", "Retraso" },
            { "04/11/2026", "A-5 Alimentadora", "128", "V2B-819", "Completado" },
            { "02/11/2026", "T-18 Estructurante", "95", "V87X-234", "Completado" },
            { "31/10/2026", "C-1A Troncal", "203", "V9M-703", "Completado" },
        }, 4);
    v->addWidget(UI::panel(tr("Historial reciente"), table));
    return page;
}
