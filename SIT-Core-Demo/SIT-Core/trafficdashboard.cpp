#include "trafficdashboard.h"
#include "loadingdialog.h"
#include "roles.h"
#include "theme.h"
#include "uihelpers.h"
#include "arequipamapwidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

TrafficDashboard::TrafficDashboard(const QString &userName, bool embedded, QWidget *parent)
    : DashboardBase(Roles::label(Role::JefeDeTrafico), userName, embedded, parent)
{
    addPage(tr("Monitoreo de rutas"), buildMonitoring());
    addPage(tr("Delegación IA / RPA"), buildDelegation());
}

QWidget *TrafficDashboard::buildMonitoring()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);

    v->addWidget(UI::label(tr("Rutas activas"), "h1"));
    v->addLayout(UI::hbox({
        UI::statCard(tr("Rutas activas"), "24", tr("En este momento"), Theme::Accent),
        UI::statCard(tr("A tiempo"), "17", tr("71% de las rutas"), Theme::Green),
        UI::statCard(tr("Con retraso"), "5", tr("Retraso medio: 28 min"), Theme::Amber),
        UI::statCard(tr("Incidencias"), "2", tr("Requieren acción"), Theme::Red),
    }));

    struct Route { QString id, route, driver, unit; int progress; QString eta, status; };
    const QList<Route> routes = {
        { "C-1A-01", "Troncal C-1A",  "Carlos Mendoza", "V8A-912", 78, "14:20", "En ruta" },
        { "A-25-01", "Alimentadora A-25", "Ana María", "V4T-751", 91, "11:05", "En ruta" },
        { "T-14-01", "Estructurante T-14", "Miguel Ríos", "V87X-234", 54, "18:45", "En ruta" },
        { "A-5-01", "Alimentadora A-5", "Diego Álvarez", "V2B-819", 37, "17:30", "Retraso" },
        { "T-18-01", "Estructurante T-18", "Sara Mamani", "V6Q-890", 69, "12:40", "En ruta" },
        { "C-1A-02", "Troncal C-1A (reverse)", "Luis Quispe", "V9M-703", 22, "—",     "Detenido" },
        { "A-25-02", "Alimentadora A-25 (return)", "José Carlos", "V6K-405", 45, "15:10", "En ruta" },
    };

    // Filtros de ruta
    auto *filters = new QWidget;
    auto *fv = new QHBoxLayout(filters);
    fv->setContentsMargins(0, 0, 0, 0);
    fv->setSpacing(8);
    QPushButton *btnAll = UI::button(tr("Todas"));
    QPushButton *btnC1A = UI::button(tr("C-1A Troncal"));
    QPushButton *btnA25 = UI::button(tr("A-25 Cayma"));
    QPushButton *btnT14 = UI::button(tr("T-14 Paucarpata"));
    QPushButton *btnA5 = UI::button(tr("A-5 Cono Norte"));
    QPushButton *btnT18 = UI::button(tr("T-18 Miraflores"));
    // Establecer C-1A como activo por defecto
    btnAll->setCheckable(true);
    btnC1A->setCheckable(true);
    btnA25->setCheckable(true);
    btnT14->setCheckable(true);
    btnA5->setCheckable(true);
    btnT18->setCheckable(true);
    btnC1A->setChecked(true);
    fv->addWidget(btnAll);
    fv->addWidget(btnC1A);
    fv->addWidget(btnA25);
    fv->addWidget(btnT14);
    fv->addWidget(btnA5);
    fv->addWidget(btnT18);

    v->addWidget(filters);

    // Mapa urbano de Arequipa
    ArequipaMapWidget *mapWidget = new ArequipaMapWidget;
    // Por defecto todas las rutas visibles
    for (int i = 0; i < 5; ++i) {
        mapWidget->setRouteVisible(
            i == 0 ? "C-1A" : i == 1 ? "A-25" : i == 2 ? "T-14" : i == 3 ? "A-5" : "T-18", true);
    }
    v->addWidget(mapWidget, 1); // stretch factor 1 para que ocupe espacio disponible

    auto *table = UI::makeTable({ tr("Ruta"), tr("Trayecto"), tr("Conductor"), tr("Unidad"),
                                  tr("Avance"), tr("ETA"), tr("Estado") }, {}, 6);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    for (const Route &r : routes) {
        const int row = UI::appendRow(table, { r.id, r.route, r.driver, r.unit, QString(), r.eta, r.status });
        auto *cell = new QWidget;
        auto *h = new QHBoxLayout(cell);
        h->setContentsMargins(8, 0, 12, 0);
        auto *bar = new QProgressBar;
        bar->setRange(0, 100);
        bar->setValue(r.progress);
        bar->setTextVisible(false);
        bar->setStyleSheet(QStringLiteral("QProgressBar::chunk{background:%1;}")
                               .arg(UI::statusColor(r.status)));
        h->addWidget(bar);
        table->setCellWidget(row, 4, cell);
    }
    v->addWidget(UI::panel(tr("Monitoreo en tiempo real"), table));
    return page;
}

QWidget *TrafficDashboard::buildDelegation()
{
    auto *page = new QWidget;
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(18);

    v->addWidget(UI::label(tr("Delegación de tareas"), "h1"));

    // Banner IA / RPA
    auto *banner = new QFrame;
    banner->setStyleSheet(QStringLiteral(
        "QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 %1,stop:1 %2);"
        "border-radius:12px;} QLabel{background:transparent;color:white;}")
        .arg(Theme::Navy, Theme::Accent));
    auto *bv = new QVBoxLayout(banner);
    bv->setContentsMargins(22, 18, 22, 18);
    bv->setSpacing(4);
    auto *bt = UI::label(QStringLiteral("✨  ") + tr("Sugerencia de delegación automática (IA / RPA)"));
    bt->setStyleSheet(QStringLiteral("font-size:18px;font-weight:700;color:white;"));
    bv->addWidget(bt);
    bv->addWidget(UI::label(tr("Motor predictivo v2.3 · Bot RPA activo · 3 sugerencias basadas en retrasos, "
                               "carga de trabajo y horas de manejo de los conductores. (Simulación demo)")));
    v->addWidget(banner);

    m_delegated = UI::makeTable({ tr("Tarea"), tr("Delegada a"), tr("Origen"), tr("Estado") }, {}, 3);

    struct Suggestion { QString title, reason; int confidence; QString task, to, from; };
    const QList<Suggestion> suggestions = {
        { tr("Reasignar C-1A-01 a conductor disponible"),
          tr("Carlos Mendoza presenta reporte de frenos. Unidad V8A-912 requiere revisión."),
          92, "Reasignar C-1A-01", "Conductor disponible", "IA" },
        { tr("Enviar unidad de reemplazo por retraso A-25"),
          tr("Ana María acumula 35 min de retraso. La unidad V4T-751 puede ser reasignada."),
          88, "Unidad de reemplazo A-25", "V4T-751 / Ana María", "IA" },
        { tr("Notificar automáticamente retrasos a clientes"),
          tr("El bot RPA puede enviar el aviso de nueva ETA actualizar el sistema."),
          97, "Aviso de ETA", "Bot RPA", "RPA" },
    };

    auto *list = new QWidget;
    auto *lv = new QVBoxLayout(list);
    lv->setContentsMargins(0, 0, 0, 0);
    lv->setSpacing(12);

    for (const Suggestion &s : suggestions) {
        QFrame *c = UI::card();
        auto *cv = new QVBoxLayout(c);
        cv->setContentsMargins(18, 14, 18, 14);
        cv->setSpacing(8);

        auto *title = UI::label(s.title, "h2");
        title->setWordWrap(true);
        cv->addWidget(title);
        auto *reason = UI::label(s.reason, "muted");
        reason->setWordWrap(true);
        cv->addWidget(reason);
        cv->addWidget(UI::meter(tr("Confianza del modelo"), s.confidence, Theme::Green));

        auto *btns = new QHBoxLayout;
        auto *status = UI::label(QString());
        status->setStyleSheet(QStringLiteral("font-weight:700;"));
        auto *accept = UI::button(tr("Aceptar y delegar"));
        auto *dismiss = UI::button(tr("Descartar"), "ghost");
        btns->addWidget(status);
        btns->addStretch();
        btns->addWidget(dismiss);
        btns->addWidget(accept);
        cv->addLayout(btns);

        connect(accept, &QPushButton::clicked, this, [=]() {
            LoadingDialog::run(this, tr("Delegando..."), tr("Aplicando la sugerencia"), 1500);
            accept->setEnabled(false);
            dismiss->setEnabled(false);
            status->setText(QStringLiteral("✔ ") + tr("Tarea delegada"));
            status->setStyleSheet(QStringLiteral("font-weight:700;color:%1;").arg(Theme::Green));
            UI::appendRow(m_delegated, { s.task, s.to, s.from, "Delegada" });
        });
        connect(dismiss, &QPushButton::clicked, this, [=]() {
            accept->setEnabled(false);
            dismiss->setEnabled(false);
            status->setText(tr("Sugerencia descartada"));
            status->setStyleSheet(QStringLiteral("font-weight:700;color:%1;").arg(Theme::Muted));
        });
        lv->addWidget(c);
    }
    v->addWidget(list);
    v->addWidget(UI::panel(tr("Tareas delegadas"), m_delegated));
    return page;
}