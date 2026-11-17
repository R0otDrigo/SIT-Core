#include "view/SimulationWidget.h"
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <algorithm>
#include <vector>
#include "model/TrafficEvent.h"
#include "model/VehicleFactory.h"

SimulationWidget::SimulationWidget(std::unique_ptr<sit::Network> network, QWidget* parent) : QWidget(parent) {
    controller_ = new SimulationController(std::move(network), this);
    buildUi();
    connect(controller_, &SimulationController::snapshotReady, this, &SimulationWidget::onSnapshot);
    connect(controller_, &SimulationController::errorOccurred, this, &SimulationWidget::onError);
    controller_->start();
}

void SimulationWidget::buildUi() {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    map_ = new MapView;
    root->addWidget(map_, 1);
    connect(map_, &MapView::busClicked, this,
            [this](int id, const QString& type, const QString& route,
                   int passengers, int capacity, bool dwelling) {
        selectedBusId_ = id;
        showBusInfo(id, type, route, passengers, capacity, dwelling);
    });

    auto* side = new QWidget;
    side->setFixedWidth(340);
    auto* col = new QVBoxLayout(side);
    col->setContentsMargins(0, 0, 0, 0);

    clockLabel_ = new QLabel("Hora simulada --:--");
    QFont big = clockLabel_->font(); big.setPointSize(14); big.setBold(true);
    clockLabel_->setFont(big);
    col->addWidget(clockLabel_);

    busInfoPanel_ = new QGroupBox(tr("Información del bus"));
    busInfoPanel_->setObjectName("busInfoPanel");
    auto* busLayout = new QVBoxLayout(busInfoPanel_);
    busInfoHint_ = new QLabel(tr("Selecciona un bus en el mapa"));
    busLayout->addWidget(busInfoHint_);
    busInfoDetails_ = new QWidget;
    busInfoDetails_->setObjectName("busInfoDetails");
    auto* busFields = new QFormLayout(busInfoDetails_);
    busTypeLabel_ = new QLabel;
    busRouteLabel_ = new QLabel;
    busRouteLabel_->setWordWrap(true);
    busPassengersLabel_ = new QLabel;
    busPassengersLabel_->setObjectName("busPassengersLabel");
    busFreeSeatsLabel_ = new QLabel;
    busFreeSeatsLabel_->setObjectName("busFreeSeatsLabel");
    busStatusLabel_ = new QLabel;
    busOccupancyBar_ = new QProgressBar;
    busOccupancyBar_->setObjectName("busOccupancyBar");
    busOccupancyBar_->setRange(0, 100);
    busOccupancyBar_->setFormat(tr("%p% ocupado"));
    busFields->addRow(tr("Tipo:"), busTypeLabel_);
    busFields->addRow(tr("Ruta:"), busRouteLabel_);
    busFields->addRow(tr("Pasajeros:"), busPassengersLabel_);
    busFields->addRow(tr("Asientos libres:"), busFreeSeatsLabel_);
    busFields->addRow(tr("Estado:"), busStatusLabel_);
    busFields->addRow(tr("Ocupación:"), busOccupancyBar_);
    busLayout->addWidget(busInfoDetails_);
    busInfoDetails_->hide();
    col->addWidget(busInfoPanel_);

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
    kpi->addWidget(new QLabel("Buses en servicio:"), 0, 0);       kpi->addWidget(busesLabel_, 0, 1);
    kpi->addWidget(new QLabel("Pasajeros transportados:"), 1, 0); kpi->addWidget(transportedLabel_, 1, 1);
    kpi->addWidget(new QLabel("Pasajeros que desistieron:"), 2, 0); kpi->addWidget(abandonedLabel_, 2, 1);
    kpi->addWidget(new QLabel("Saturacion media:"), 3, 0);        kpi->addWidget(saturationLabel_, 3, 1);
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

void SimulationWidget::populateRoutes(const sit::Snapshot& snap) {
    for (const auto& r : snap.routes) {
        const QString id = QString::fromStdString(r.id);
        routeIsTrunk_[id] = r.trunk;
        routeCombo_->addItem(QString("%1 - %2").arg(id, QString::fromStdString(r.name)), id);
    }
    routesLoaded_ = true;
}

void SimulationWidget::onRouteChanged() {
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

void SimulationWidget::onSnapshot(const sit::Snapshot& s) {
    if (!routesLoaded_) populateRoutes(s);
    map_->refresh(s);
    if (selectedBusId_ >= 0) {
        const auto bus = std::find_if(s.vehicles.begin(), s.vehicles.end(),
                                      [this](const sit::VehicleView& v) { return v.id == selectedBusId_; });
        if (bus == s.vehicles.end()) {
            selectedBusId_ = -1;
            clearBusInfo(tr("El bus seleccionado fue retirado"));
        } else {
            QString route = QString::fromStdString(bus->routeId);
            const auto routeInfo = std::find_if(s.routes.begin(), s.routes.end(),
                                                [bus](const sit::RouteView& r) { return r.id == bus->routeId; });
            if (routeInfo != s.routes.end())
                route += QStringLiteral(" - ") + QString::fromStdString(routeInfo->name);
            showBusInfo(bus->id, QString::fromStdString(bus->type), route,
                        bus->passengers, bus->capacity, bus->dwelling);
        }
    }

    clockLabel_->setText(QString("Hora simulada %1%2").arg(QString::fromStdString(s.clock), s.paused ? "  (pausa)" : ""));
    busesLabel_->setText(QString("%1 / %2").arg(s.stats.activeVehicles).arg(s.stats.fleetLimit));
    transportedLabel_->setText(QString::number(s.stats.transported));
    abandonedLabel_->setText(QString::number(s.stats.abandoned));
    saturationLabel_->setText(QString("%1 %").arg(s.stats.avgSaturation * 100.0, 0, 'f', 0));

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

void SimulationWidget::showBusInfo(int id, const QString& type, const QString& route,
                                   int passengers, int capacity, bool dwelling) {
    busInfoPanel_->setTitle(tr("Bus #%1").arg(id));
    busInfoHint_->hide();
    busInfoDetails_->show();
    busTypeLabel_->setText(type);
    busRouteLabel_->setText(route);
    busPassengersLabel_->setText(tr("%1 / %2").arg(passengers).arg(capacity));
    busFreeSeatsLabel_->setText(QString::number(std::max(0, capacity - passengers)));
    busStatusLabel_->setText(dwelling ? tr("Detenido en paradero") : tr("En marcha"));
    busOccupancyBar_->setValue(capacity > 0 ? std::clamp(100 * passengers / capacity, 0, 100) : 0);
}

void SimulationWidget::clearBusInfo(const QString& message) {
    busInfoPanel_->setTitle(tr("Información del bus"));
    busInfoDetails_->hide();
    busInfoHint_->setText(message);
    busInfoHint_->show();
}

void SimulationWidget::addLog(const QString& line) {
    logList_->insertItem(0, line);
    while (logList_->count() > 200) delete logList_->takeItem(logList_->count() - 1);
}

void SimulationWidget::onError(const QString& message) {
    addLog("ERROR: " + message);
}
