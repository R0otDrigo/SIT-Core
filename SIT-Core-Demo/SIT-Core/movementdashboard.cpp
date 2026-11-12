#include "movementdashboard.h"
#include "loadingdialog.h"
#include "roles.h"
#include "theme.h"
#include "uihelpers.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

MovementDashboard::MovementDashboard(const QString &userName, bool embedded, QWidget *parent)
    : DashboardBase(Roles::label(Role::ResponsableDesplazamientos), userName, embedded, parent)
{
    addPage(tr("Horarios"), buildTimetables());
    addPage(tr("Asignaciones"), buildAssignments());
}

QWidget *MovementDashboard::buildTimetables()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);

    v->addWidget(UI::label(tr("Control de horarios"), "h1"));
    v->addLayout(UI::hbox({
        UI::statCard(tr("Salidas hoy"), "38", tr("26 ya despachadas"), Theme::Accent),
        UI::statCard(tr("Llegadas hoy"), "31", tr("22 registradas"), Theme::Green),
        UI::statCard(tr("Con retraso"), "4", tr("Más de 15 min"), Theme::Amber),
        UI::statCard(tr("Canceladas"), "1", tr("Por mantenimiento"), Theme::Red),
    }));

    auto *departures = UI::makeTable(
        { tr("Hora"), tr("Ruta"), tr("Unidad"), tr("Conductor"), tr("Andén"), tr("Estado") },
        {
            { "05:30", "A-5 Alimentadora", "V7M-551", "Diego Álvarez", "1", "Completado" },
            { "06:00", "C-1A Troncal", "V8A-912", "Carlos Mendoza", "2", "Completado" },
            { "07:00", "T-14 Estructurante", "V87X-234", "Miguel Ríos", "3", "Completado" },
            { "08:00", "A-25 Alimentadora", "V2B-819", "Ana María", "4", "Por salir" },
            { "09:00", "T-18 Estructurante", "V6Q-890", "Sara Mamani", "5", "Programado" },
            { "10:30", "A-5 Alimentadora (reverse)", "V5N-333", "Patricia Soto", "1", "Programado" },
            { "12:00", "C-1A Troncal", "V4T-751", "Luis Quispe", "2", "Completado" },
            { "14:00", "T-14 Estructurante", "V3K-112", "Fernanda", "1", "Completado" },
        }, 5);
    v->addWidget(UI::panel(tr("Salidas de hoy"), departures));

    auto *arrivals = UI::makeTable(
        { tr("Hora"), tr("Procedencia"), tr("Unidad"), tr("Conductor"), tr("Andén"), tr("Estado") },
        {
            { "08:10", "C-1A Troncal (terminal)", "V8A-912", "Carlos Mendoza", "3", "Completado" },
            { "10:45", "A-25 Alimentadora (desde Cayma)", "V4T-751", "Ana María", "2", "Programado" },
            { "12:20", "T-14 Estructurante (desde Terminal)", "V87X-234", "Miguel Ríos", "1", "Retraso" },
            { "14:05", "A-5 Alimentadora (desde Cerro Colorado)", "V2B-819", "Ana María", "4", "Programado" },
            { "16:30", "T-18 Estructurante (desde Miraflores)", "V6Q-890", "Sara Mamani", "5", "Programado" },
        }, 5);
    v->addWidget(UI::panel(tr("Llegadas de hoy"), arrivals));
    return page;
}

QWidget *MovementDashboard::buildAssignments()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);

    v->addWidget(UI::label(tr("Asignación de recursos"), "h1"));
    v->addLayout(UI::hbox({
        UI::statCard(tr("Vehículos libres"), "9", tr("Listos para asignar"), Theme::Green),
        UI::statCard(tr("Conductores libres"), "12", tr("Con descanso cumplido"), Theme::Accent),
        UI::statCard(tr("Asignaciones hoy"), "36", tr("Confirmadas"), Theme::Accent),
    }));

    auto *table = UI::makeTable(
        { tr("Vehículo"), tr("Conductor"), tr("Ruta"), tr("Salida"), tr("Estado") },
        {
            { "V8A-912", "Carlos Mendoza", "C-1A Troncal", "06:30", "Confirmado" },
            { "V4T-751", "Ana María", "A-25 Alimentadora", "09:00", "Confirmado" },
            { "V87X-234", "Miguel Ríos", "T-14 Estructurante", "11:30", "Confirmado" },
            { "V2B-819", "Ana María", "A-5 Alimentadora", "13:00", "Pendiente" },
            { "V6Q-890", "Sara Mamani", "T-18 Estructurante", "15:45", "Pendiente" },
        }, 4);

    // Formulario de asignación
    auto *form = new QWidget;
    auto *fl = new QFormLayout(form);
    fl->setContentsMargins(0, 0, 0, 0);
    fl->setVerticalSpacing(10);

    auto *vehicle = new QComboBox;
    vehicle->addItems({ "V8A-912", "V4T-751", "V87X-234", "V2B-819", "V6Q-890" });
    auto *driver = new QComboBox;
    driver->addItems({ "Carlos Mendoza", "Ana María", "Miguel Ríos", "Sara Mamani", "Fernanda" });
    auto *route = new QComboBox;
    route->addItems({ "C-1A Troncal", "A-25 Alimentadora", "T-14 Estructurante", "A-5 Cono Norte", "T-18 Miraflores" });
    auto *time = new QLineEdit;
    time->setPlaceholderText(QStringLiteral("HH:mm"));
    time->setText(QStringLiteral("18:00"));

    fl->addRow(tr("Vehículo"), vehicle);
    fl->addRow(tr("Conductor"), driver);
    fl->addRow(tr("Ruta"), route);
    fl->addRow(tr("Hora de salida"), time);

    auto *assign = UI::button(tr("Asignar recurso"));
    fl->addRow(QString(), assign);
    connect(assign, &QPushButton::clicked, this, [=]() {
        LoadingDialog::run(this, tr("Asignando..."), tr("Registrando la asignación"), 1500);
        UI::appendRow(table, { vehicle->currentText(), driver->currentText(), route->currentText(),
                               time->text().trimmed().isEmpty() ? QStringLiteral("--:--") : time->text().trimmed(),
                               "Confirmado" });
    });

    v->addWidget(UI::panel(tr("Nueva asignación"), form));
    v->addWidget(UI::panel(tr("Asignaciones del día"), table));
    return page;
}