#include "admindashboard.h"
#include "loadingdialog.h"
#include "roles.h"
#include "theme.h"
#include "uihelpers.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

QList<QStringList> unitRows()
{
    return {
        { "V8A-912", "Carlos Mendoza",  "C-1A Troncal",        "En ruta",       "Pasajeros: 47/60" },
        { "V4T-751", "Luis Quispe",     "C-1A Troncal",        "En ruta",       "Pasajeros: 52/60" },
        { "V9M-703", "Rosa Flores",     "A-25 Alimentadora",   "En ruta",       "Pasajeros: 38/50" },
        { "V2B-819", "Ana María",       "A-5 Alimentadora",    "En ruta",       "Pasajeros: 41/55" },
        { "V6K-405", "José Carlos",     "T-14 Estructurante",  "En ruta",       "Pasajeros: 55/65" },
        { "V87X-234", "Miguel Ríos",     "T-18 Estructurante",  "Retraso",       "Pasajeros: 39/60, Av. Ejército" },
        { "V3K-112", "Fernanda",        "C-1A Troncal",        "Operativo",     "Pasajeros: 58/60" },
        { "V7M-551", "Diego Álvarez",   "A-5 Alimentadora",    "Operativo",     "Pasajeros: 33/55" },
        { "V5N-333", "Patricia Soto",   "T-18 Estructurante",  "En ruta",       "Pasajeros: 46/60" },
        { "V6Q-890", "Sara Mamani",     "A-25 Alimentadora",   "Detenido",      "Pasajeros: 12/50, vías" },
    };
}

QList<QStringList> driverRows()
{
    return {
        { "Carlos Mendoza",  "Q40213878", "V8A-912",   "Conductor C-1A",  "A tiempo" },
        { "Luis Quispe",     "Q41982215", "V4T-751",   "Conductor C-1A",  "A tiempo" },
        { "Rosa Flores",     "Q43120964", "V9M-703",   "Conductor A-25",  "A tiempo" },
        { "Jorge Huamán",    "Q38874120", "V2B-819",   "Conductor A-5",   "Retraso ligero" },
        { "Miguel Condori",  "Q42267301", "V6K-405",   "Conductor T-14",  "A tiempo" },
        { "Ana Salas",       "Q44509873", "V87X-234",  "Conductor T-18",  "A tiempo" },
        { "Pedro Cáceres",   "Q39651472", "V3K-112",   "Conductor C-1A",  "A tiempo" },
        { "Juan Mamani",     "Q40778026", "V7M-551",   "Conductor A-5",   "En pausas" },
    };
}

} // namespace

AdminDashboard::AdminDashboard(const QString &userName, bool embedded, QWidget *parent)
    : DashboardBase(Roles::label(Role::FleetManager), userName, embedded, parent)
{
    addPage(tr("Resumen"), buildOverview());
    addPage(tr("Unidades"), buildUnits());
    addPage(tr("Conductores"), buildDrivers());
}

QWidget *AdminDashboard::buildOverview()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);

    v->addWidget(UI::label(tr("Resumen de la flota"), "h1"));

    v->addLayout(UI::hbox({
        UI::statCard(tr("Unidades totales"), "48", tr("Flota registrada"), Theme::Accent),
        UI::statCard(tr("En ruta"), "29", tr("60% de la flota"), Theme::Green),
        UI::statCard(tr("En mantenimiento"), "7", tr("2 con entrega hoy"), Theme::Amber),
        UI::statCard(tr("Detenidas"), "3", tr("Requieren atención"), Theme::Red),
    }));

    // Estado de la flota + alertas
    auto *fleet = new QWidget;
    auto *fv = new QVBoxLayout(fleet);
    fv->setContentsMargins(0, 0, 0, 0);
    fv->setSpacing(12);
    fv->addWidget(UI::meter(tr("Pasajeros validados hoy"), 72, Theme::Green, "3425 / 4800"));
    fv->addWidget(UI::meter(tr("Frecuencia media (min)"), 15, Theme::Accent, "4.2 min"));
    fv->addWidget(UI::meter(tr("Cumplimiento de horario"), 85, Theme::Green));
    fv->addWidget(UI::meter(tr("Conductores disponibles"), 19, Theme::Accent, "9 / 48"));

    auto *alerts = new QWidget;
    auto *av = new QVBoxLayout(alerts);
    av->setContentsMargins(0, 0, 0, 0);
    av->setSpacing(4);
    av->addWidget(UI::alertItem(tr("V8A-912 presenta fallo en sistema de frenos"), "hace 12 min", Theme::Red));
    av->addWidget(UI::alertItem(tr("V2B-819 acumula 35 min de retraso en ruta A-5, congestión en Umacollo"), "hace 25 min", Theme::Amber));
    av->addWidget(UI::alertItem(tr("V87X-234 completó servicio diario - revisión preventiva"), "hace 1 h", Theme::Green));
    av->addWidget(UI::alertItem(tr("V5N-333 combustible por debajo del 40%"), "hace 2 h", Theme::Amber));
    av->addWidget(UI::alertItem(tr("Licencia de Miguel Condori vence en 15 días"), "hace 3 h", Theme::Amber));
    av->addStretch();

    auto *row = new QHBoxLayout;
    row->setSpacing(16);
    row->addWidget(UI::panel(tr("Estado de la flota"), fleet), 1);
    row->addWidget(UI::panel(tr("Alertas recientes"), alerts), 1);
    v->addLayout(row);

    auto *table = UI::makeTable({ tr("Placa"), tr("Conductor"), tr("Ruta / ubicación"), tr("Estado"), tr("Combustible") },
                                unitRows().mid(0, 6), 3);
    v->addWidget(UI::panel(tr("Unidades en operación"), table));
    return page;
}

QWidget *AdminDashboard::buildUnits()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);
    v->addWidget(UI::label(tr("Estado de unidades"), "h1"));
    v->addLayout(UI::hbox({
        UI::statCard(tr("Operativas"), "36", tr("En ruta o disponibles"), Theme::Green),
        UI::statCard(tr("En mantenimiento"), "7", tr("Taller central"), Theme::Amber),
        UI::statCard(tr("Con fallo"), "3", tr("Detenidas"), Theme::Red),
        UI::statCard(tr("Pasajeros validados hoy"), "3 420", tr("Total flota SIT Arequipa"), Theme::Accent),
    }));
    auto *table = UI::makeTable({ tr("Placa"), tr("Conductor"), tr("Ruta / destino"), tr("Estado"), tr("Ocupación") },
                                unitRows(), 3);
    v->addWidget(UI::panel(tr("Listado de unidades"), table));
    return page;
}

QWidget *AdminDashboard::buildDrivers()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);
    v->addWidget(UI::label(tr("Gestión de conductores"), "h1"));

    auto *table = UI::makeTable({ tr("Conductor"), tr("Licencia"), tr("Unidad"), tr("Ruta"), tr("Estado") },
                                driverRows(), 4);

    auto *addBtn = UI::button(tr("+ Registrar conductor"));
    connect(addBtn, &QPushButton::clicked, this, [this, table]() {
        QStringList vals;
        const QList<UI::FormField> fields = {
            { tr("Nombre completo"), {}, tr("Ej. Marco Aguilar") },
            { tr("N° de licencia"), {}, tr("Ej. Q45012345") },
            { tr("Unidad"), { "V8A-912", "V4T-751", "V9M-703", "V2B-819" }, {} },
            { tr("Ruta"), { "C-1A", "A-25", "T-14", "A-5", "T-18" }, {} },
        };
        if (!UI::formDialog(this, tr("Registrar conductor"), fields, vals))
            return;
        if (vals.value(0).isEmpty() || vals.value(1).isEmpty())
            return;
        LoadingDialog::run(this, tr("Registrando..."), tr("Guardando datos del conductor"), 1500);
        UI::appendRow(table, { vals[0], vals[1], vals[2], vals[3], "Activo" });
    });

    v->addWidget(UI::panel(tr("Conductores afiliados"), table, addBtn));
    return page;
}
