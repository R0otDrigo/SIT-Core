#include "view/MainWindow.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStatusBar>
#include <QVBoxLayout>
#include <algorithm>
#include <vector>
#include "model/TrafficEvent.h"
#include "model/VehicleFactory.h"

MainWindow::MainWindow(std::unique_ptr<sit::Network> network, QWidget* parent) : QMainWindow(parent) {
    controller_ = new SimulationController(std::move(network), this);
    buildUi();
    connect(controller_, &SimulationController::snapshotReady, this, &MainWindow::onSnapshot);
    connect(controller_, &SimulationController::errorOccurred, this, &MainWindow::onError);
    controller_->start();
}

void MainWindow::buildUi() {
    setWindowTitle("SIT-Core - Simulador de transporte publico (Arequipa)");
    resize(1320, 780);
    setStyleSheet(
        "QGroupBox{font-weight:bold;border:1px solid #cbd5e1;border-radius:6px;margin-top:10px;padding-top:6px;}"
        "QGroupBox::title{subcontrol-origin:margin;left:10px;padding:0 4px;}");

    auto* central = new QWidget;
    auto* root = new QHBoxLayout(central);
    map_ = new MapView;
    root->addWidget(map_, 1);

    auto* side = new QWidget;
    side->setFixedWidth(380);
    auto* col = new QVBoxLayout(side);
    col->setContentsMargins(0, 0, 0, 0);

    clockLabel_ = new QLabel("Hora simulada --:--");
    QFont big = clockLabel_->font(); big.setPointSize(20); big.setBold(true);
    clockLabel_->setFont(big);
    col->addWidget(clockLabel_);

    // --- Simulacion ---
    auto* simBox = new QGroupBox("Simulacion");
    auto* simGrid = new QGridLayout(simBox);
    pauseButton_ = new QPushButton("Pausar");
    pauseButton_->setCheckable(true);
    speedCombo_ = new QComboBox;
    for (double v : {1.0, 2.0, 5.0, 10.0, 20.0}) speedCombo_->addItem(QString("%1x").arg(v), v);
    speedCombo_->setCurrentIndex(1);
    simGrid->addWidget(pauseButton_, 0, 0);
    simGrid->addWidget(new QLabel("Velocidad:"), 0, 1, Qt::AlignRight);
    simGrid->addWidget(speedCombo_, 0, 2);
    col->addWidget(simBox);

    // --- Indicadores ---
    auto* kpiBox = new QGroupBox("Indicadores");
    auto* kpi = new QGridLayout(kpiBox);
    busesLabel_ = new QLabel("-");
    transportedLabel_ = new QLabel("-");
    abandonedLabel_ = new QLabel("-");
    saturationLabel_ = new QLabel("-");
    eventsLabel_ = new QLabel("Sin eventos");
    eventsLabel_->setWordWrap(true);
    kpi->addWidget(new QLabel("Buses en servicio:"), 0, 0);       kpi->addWidget(busesLabel_, 0, 1);
    kpi->addWidget(new QLabel("Pasajeros transportados:"), 1, 0); kpi->addWidget(transportedLabel_, 1, 1);
    kpi->addWidget(new QLabel("Pasajeros que desistieron:"), 2, 0); kpi->addWidget(abandonedLabel_, 2, 1);
    kpi->addWidget(new QLabel("Saturacion media:"), 3, 0);        kpi->addWidget(saturationLabel_, 3, 1);
    kpi->addWidget(eventsLabel_, 4, 0, 1, 2);
    col->addWidget(kpiBox);

    // --- Acciones del controlador de flota ---
    auto* actBox = new QGroupBox("Controlador de flota");
    auto* act = new QGridLayout(actBox);
    routeCombo_ = new QComboBox;
    vehicleTypeCombo_ = new QComboBox;
    eventCombo_ = new QComboBox;
    for (auto t : {sit::EventType::Accident, sit::EventType::Protest, sit::EventType::RoadWork})
        eventCombo_->addItem(QString::fromStdString(sit::TrafficEventFactory::label(t)), static_cast<int>(t));
    auto* dispatchBtn = new QPushButton("Despachar");
    auto* retireBtn = new QPushButton("Retirar bus vacio");
    auto* eventBtn = new QPushButton("Provocar");
    act->addWidget(new QLabel("Ruta:"), 0, 0);        act->addWidget(routeCombo_, 0, 1, 1, 2);
    act->addWidget(new QLabel("Tipo de bus:"), 1, 0); act->addWidget(vehicleTypeCombo_, 1, 1); act->addWidget(dispatchBtn, 1, 2);
    act->addWidget(retireBtn, 2, 1, 1, 2);
    act->addWidget(new QLabel("Evento:"), 3, 0);      act->addWidget(eventCombo_, 3, 1); act->addWidget(eventBtn, 3, 2);
    col->addWidget(actBox);

    // --- Paraderos ---
    stopsTable_ = new QTableWidget(0, 3);
    stopsTable_->setHorizontalHeaderLabels({"Paradero", "Esperan", "Saturacion"});
    stopsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    stopsTable_->verticalHeader()->hide();
    stopsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stopsTable_->setSelectionMode(QAbstractItemView::NoSelection);
    col->addWidget(stopsTable_, 1);

    // --- Registro ---
    logList_ = new QListWidget;
    logList_->setMaximumHeight(120);
    col->addWidget(logList_);

    root->addWidget(side);
    setCentralWidget(central);
    statusBar()->showMessage("Listo");

    // --- Conexiones vista -> controlador ---
    connect(pauseButton_, &QPushButton::toggled, this, [this](bool paused) {
        pauseButton_->setText(paused ? "Reanudar" : "Pausar");
        controller_->setPaused(paused);
    });
    connect(speedCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { controller_->setSpeed(speedCombo_->currentData().toDouble()); });
    connect(routeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { onRouteChanged(); });
    connect(dispatchBtn, &QPushButton::clicked, this, [this] {
        controller_->dispatchBus(routeCombo_->currentData().toString(), vehicleTypeCombo_->currentData().toInt());
    });
    connect(retireBtn, &QPushButton::clicked, this, [this] { controller_->retireBus(routeCombo_->currentData().toString()); });
    connect(eventBtn, &QPushButton::clicked, this, [this] {
        controller_->triggerEvent(eventCombo_->currentData().toInt(), routeCombo_->currentData().toString());
    });

    controller_->setSpeed(speedCombo_->currentData().toDouble());
}

void MainWindow::populateRoutes(const sit::Snapshot& snap) {
    for (const auto& r : snap.routes) {
        const QString id = QString::fromStdString(r.id);
        routeIsTrunk_[id] = r.trunk;
        routeCombo_->addItem(QString("%1 - %2").arg(id, QString::fromStdString(r.name)), id);
    }
    routesLoaded_ = true;
}

void MainWindow::onRouteChanged() {
    // Solo se ofrecen los tipos de bus compatibles con la ruta elegida.
    vehicleTypeCombo_->clear();
    const auto it = routeIsTrunk_.find(routeCombo_->currentData().toString());
    if (it == routeIsTrunk_.end()) return;
    using sit::VehicleType;
    std::vector<VehicleType> types;
    if (it->second) types = {VehicleType::Conventional, VehicleType::Articulated};
    else types = {VehicleType::Feeder};
    for (auto t : types)
        vehicleTypeCombo_->addItem(QString::fromStdString(sit::VehicleFactory::label(t)), static_cast<int>(t));
}

void MainWindow::onSnapshot(const sit::Snapshot& s) {
    if (!routesLoaded_) populateRoutes(s);
    map_->refresh(s);

    clockLabel_->setText(QString("Hora simulada %1%2").arg(QString::fromStdString(s.clock), s.paused ? "  (pausa)" : ""));
    busesLabel_->setText(QString("%1 / %2").arg(s.stats.activeVehicles).arg(s.stats.fleetLimit));
    transportedLabel_->setText(QString::number(s.stats.transported));
    abandonedLabel_->setText(QString::number(s.stats.abandoned));
    saturationLabel_->setText(QString("%1 %").arg(s.stats.avgSaturation * 100.0, 0, 'f', 0));
    if (s.events.empty()) {
        eventsLabel_->setText("Sin eventos de trafico activos");
    } else {
        QStringList parts;
        for (const auto& e : s.events)
            parts << QString("%1 en %2 (%3 s)").arg(QString::fromStdString(e.name), QString::fromStdString(e.routeId))
                         .arg(static_cast<int>(e.remaining));
        eventsLabel_->setText("Eventos: " + parts.join(", "));
    }

    // Tabla de paraderos: los mas saturados primero (lambda de ordenamiento).
    std::vector<sit::StopView> stops = s.stops;
    std::sort(stops.begin(), stops.end(), [](const sit::StopView& a, const sit::StopView& b) {
        return a.saturation > b.saturation;
    });
    stopsTable_->setRowCount(static_cast<int>(stops.size()));
    for (int r = 0; r < static_cast<int>(stops.size()); ++r) {
        const auto& st = stops[static_cast<std::size_t>(r)];
        const QString cells[3] = {QString::fromStdString(st.name) + (st.closed ? " (cerrado)" : ""),
                                  QString::number(st.waiting),
                                  QString("%1 %").arg(st.saturation * 100.0, 0, 'f', 0)};
        for (int c = 0; c < 3; ++c) {
            auto* item = stopsTable_->item(r, c);
            if (!item) { item = new QTableWidgetItem; stopsTable_->setItem(r, c, item); }
            item->setText(cells[c]);
            if (c > 0) item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            item->setBackground(st.saturation >= 0.8 ? QColor("#fecaca") : st.saturation >= 0.5 ? QColor("#fef3c7") : QColor("#ffffff"));
        }
    }

    for (const auto& line : s.log) addLog(QString::fromStdString(line));
}

void MainWindow::addLog(const QString& line) {
    logList_->insertItem(0, line);
    while (logList_->count() > 200) delete logList_->takeItem(logList_->count() - 1);
}

void MainWindow::onError(const QString& message) {
    addLog("ERROR: " + message);
    statusBar()->showMessage(message, 5000);
}
