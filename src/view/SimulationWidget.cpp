#include "view/SimulationWidget.h"
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlError>
#include <QScrollArea>
#include <QStringList>
#include <QUrl>
#include <QVBoxLayout>
#include <algorithm>
#include <vector>
#include "model/TrafficEvent.h"
#include "model/VehicleFactory.h"
#include "view/MapBridge.h"

SimulationWidget::SimulationWidget(std::unique_ptr<sit::Network> network, QWidget* parent) : QWidget(parent) {
    controller_ = new SimulationController(std::move(network), this);
    mapBridge_ = new MapBridge(this);
    mapBridge_->loadGtfs();
    buildUi();
    connect(controller_, &SimulationController::snapshotReady, this, &SimulationWidget::onSnapshot);
    connect(controller_, &SimulationController::errorOccurred, this, &SimulationWidget::onError);
    connect(mapBridge_, &MapBridge::callAnswered, this, &SimulationWidget::callAnswered);
    connect(mapBridge_, &MapBridge::callRequested, this, &SimulationWidget::callRequested);
    controller_->start();
}

void SimulationWidget::buildUi() {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    map_ = new QQuickWidget;
    map_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    map_->setClearColor(QColor("#e8eef5"));
    map_->setMinimumSize(480, 360);
    map_->rootContext()->setContextProperty("mapBridge", mapBridge_);
    root->addWidget(map_, 1);
    connect(mapBridge_, &MapBridge::busSelected, this,
            [this](int id, const QString& type, const QString& route,
                   int passengers, int capacity, bool dwelling, const QString& nextStop) {
        selectedBusId_ = id;
        showBusInfo(id, type, route, passengers, capacity, dwelling, nextStop);
    });

    busInfoPanel_ = new QGroupBox(tr("Información del bus"));
    busInfoPanel_->setObjectName("busInfoPanel");
    auto* busLayout = new QVBoxLayout(busInfoPanel_);
    backToOverviewButton_ = new QPushButton(tr("Volver al resumen"));
    backToOverviewButton_->setObjectName("backToOverviewButton");
    backToOverviewButton_->setProperty("variant", "ghost");
    backToOverviewButton_->setToolTip(tr("Quita la selección del bus en el mapa"));
    busLayout->addWidget(backToOverviewButton_, 0, Qt::AlignRight);
    connect(backToOverviewButton_, &QPushButton::clicked, this, [this] { clearBusInfo(); });
    busInfoDetails_ = new QWidget;
    busInfoDetails_->setObjectName("busInfoDetails");
    auto* busFields = new QFormLayout(busInfoDetails_);
    busFields->setContentsMargins(0, 0, 0, 0);
    busTypeLabel_ = new QLabel;
    busRouteLabel_ = new QLabel;
    busRouteLabel_->setWordWrap(true);
    busPassengersLabel_ = new QLabel;
    busPassengersLabel_->setObjectName("busPassengersLabel");
    busFreeSeatsLabel_ = new QLabel;
    busFreeSeatsLabel_->setObjectName("busFreeSeatsLabel");
    busStatusLabel_ = new QLabel;
    busNextStopLabel_ = new QLabel;
    busNextStopLabel_->setObjectName("busNextStopLabel");
    busNextStopLabel_->setWordWrap(true);
    busOccupancyBar_ = new QProgressBar;
    busOccupancyBar_->setObjectName("busOccupancyBar");
    busOccupancyBar_->setRange(0, 100);
    busFields->addRow(tr("Ocupación"), busOccupancyBar_);
    busFields->addRow(tr("Pasajeros"), busPassengersLabel_);
    busFields->addRow(tr("Asientos libres"), busFreeSeatsLabel_);
    busFields->addRow(tr("Estado"), busStatusLabel_);
    busFields->addRow(tr("Próximo paradero"), busNextStopLabel_);
    busFields->addRow(tr("Ruta"), busRouteLabel_);
    busFields->addRow(tr("Tipo"), busTypeLabel_);
    busLayout->addWidget(busInfoDetails_);
    busInfoPanel_->hide();

    // Panel lateral con jerarquia fija: reloj > reproduccion > bus seleccionado > flota >
    // eventos > indicadores > paraderos > registro. Va dentro de un QScrollArea para que
    // quepa en pantallas pequenas.
    auto* side = new QWidget;
    auto* col = new QVBoxLayout(side);
    col->setContentsMargins(12, 12, 12, 12);
    col->setSpacing(12);

    // --- Reloj y reproduccion ---
    auto* clockCard = new QFrame;
    clockCard->setProperty("role", "card");
    auto* clockCol = new QVBoxLayout(clockCard);
    clockCaption_ = new QLabel(tr("Hora simulada"));
    clockCaption_->setProperty("role", "muted");
    clockLabel_ = new QLabel("--:--");
    clockLabel_->setProperty("role", "stat");
    clockCol->addWidget(clockCaption_);
    clockCol->addWidget(clockLabel_);

    auto* playRow = new QHBoxLayout;
    pauseButton_ = new QPushButton(tr("Pausar"));
    pauseButton_->setCheckable(true);
    pauseButton_->setProperty("variant", "ghost");
    pauseButton_->setToolTip(tr("Detiene o reanuda el avance del reloj simulado"));
    speedCombo_ = new QComboBox;
    for (double v : {1.0, 2.0, 5.0, 10.0, 20.0}) speedCombo_->addItem(QString("%1x").arg(v), v);
    speedCombo_->setCurrentIndex(1);
    speedCombo_->setToolTip(tr("Velocidad de la simulación respecto al tiempo real"));
    auto* speedLabel = new QLabel(tr("Velocidad"));
    speedLabel->setProperty("role", "muted");
    playRow->addWidget(pauseButton_, 1);
    playRow->addWidget(speedLabel);
    playRow->addWidget(speedCombo_);
    clockCol->addLayout(playRow);
    col->addWidget(clockCard);

    // --- Bus seleccionado (solo visible al hacer clic en un bus del mapa) ---
    col->addWidget(busInfoPanel_);

    // --- Flota ---
    auto* fleetBox = new QGroupBox(tr("Flota"));
    auto* fleet = new QFormLayout(fleetBox);
    routeCombo_ = new QComboBox;
    routeCombo_->setToolTip(tr("Ruta sobre la que actúan la flota y los eventos de tráfico"));
    vehicleTypeCombo_ = new QComboBox;
    vehicleTypeCombo_->setToolTip(tr("Solo se listan los tipos de bus compatibles con la ruta"));
    auto* dispatchBtn = new QPushButton(tr("Despachar bus"));
    dispatchBtn->setProperty("variant", "primary");
    dispatchBtn->setToolTip(tr("Pone en servicio un bus nuevo en la ruta elegida"));
    auto* retireBtn = new QPushButton(tr("Retirar bus vacío"));
    retireBtn->setProperty("variant", "danger");
    retireBtn->setToolTip(tr("Saca de servicio un bus sin pasajeros de la ruta elegida"));
    auto* fleetButtons = new QHBoxLayout;
    fleetButtons->addWidget(dispatchBtn);
    fleetButtons->addWidget(retireBtn);
    fleet->addRow(tr("Ruta"), routeCombo_);
    fleet->addRow(tr("Tipo de bus"), vehicleTypeCombo_);
    fleet->addRow(fleetButtons);
    col->addWidget(fleetBox);

    // --- Eventos de trafico ---
    auto* eventBox = new QGroupBox(tr("Eventos de tráfico"));
    auto* events = new QFormLayout(eventBox);
    eventCombo_ = new QComboBox;
    for (auto t : {sit::EventType::Accident, sit::EventType::Protest, sit::EventType::RoadWork})
        eventCombo_->addItem(QString::fromStdString(sit::TrafficEventFactory::label(t)), static_cast<int>(t));
    auto* eventBtn = new QPushButton(tr("Provocar evento"));
    eventBtn->setProperty("variant", "danger");
    eventBtn->setToolTip(tr("Genera el evento elegido en la ruta seleccionada en Flota"));
    auto* eventHint = new QLabel(tr("Afecta a la ruta seleccionada en Flota."));
    eventHint->setProperty("role", "muted");
    eventHint->setWordWrap(true);
    events->addRow(tr("Evento"), eventCombo_);
    events->addRow(eventHint);
    events->addRow(eventBtn);
    col->addWidget(eventBox);

    // --- Indicadores ---
    auto* kpiBox = new QGroupBox(tr("Indicadores"));
    auto* kpi = new QFormLayout(kpiBox);
    busesLabel_ = new QLabel("-");
    transportedLabel_ = new QLabel("-");
    abandonedLabel_ = new QLabel("-");
    saturationLabel_ = new QLabel("-");
    abandonedLabel_->setToolTip(tr("Pasajeros que se cansaron de esperar y dejaron el paradero"));
    saturationLabel_->setToolTip(tr("Ocupación media de los paraderos"));
    kpi->addRow(tr("Buses en servicio"), busesLabel_);
    kpi->addRow(tr("Pasajeros transportados"), transportedLabel_);
    kpi->addRow(tr("Pasajeros que desistieron"), abandonedLabel_);
    kpi->addRow(tr("Saturación media"), saturationLabel_);
    col->addWidget(kpiBox);

    // --- Paraderos ---
    auto* stopsBox = new QGroupBox(tr("Paraderos (más saturados primero)"));
    auto* stopsCol = new QVBoxLayout(stopsBox);
    stopsTable_ = new QTableWidget(0, 3);
    stopsTable_->setHorizontalHeaderLabels({tr("Paradero"), tr("Esperan"), tr("Saturación")});
    stopsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    stopsTable_->verticalHeader()->hide();
    stopsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stopsTable_->setSelectionMode(QAbstractItemView::NoSelection);
    stopsTable_->setMinimumHeight(220);
    stopsCol->addWidget(stopsTable_);
    col->addWidget(stopsBox);

    // --- Registro ---
    auto* logBox = new QGroupBox(tr("Registro"));
    auto* logCol = new QVBoxLayout(logBox);
    logList_ = new QListWidget;
    logList_->setMinimumHeight(120);
    logList_->setToolTip(tr("Eventos más recientes arriba"));
    logCol->addWidget(logList_);
    col->addWidget(logBox);
    col->addStretch(1);

    auto* scroll = new QScrollArea;
    scroll->setWidget(side);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setFixedWidth(340);
    root->addWidget(scroll);

    // --- Conexiones vista -> controlador ---
    connect(pauseButton_, &QPushButton::toggled, this, [this](bool paused) {
        pauseButton_->setText(paused ? tr("Reanudar") : tr("Pausar"));
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

    connect(map_, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if (status != QQuickWidget::Error) return;
        QStringList messages;
        for (const QQmlError& error : map_->errors()) messages << error.toString();
        addLog(tr("ERROR mapa QML: ") + messages.join(" | "));
    });
    map_->setSource(QUrl(QStringLiteral("qrc:/qml/SimulationMap.qml")));

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
    mapBridge_->update(s);

    if (selectedBusId_ >= 0) {
        const auto bus = std::find_if(s.vehicles.begin(), s.vehicles.end(),
                                      [this](const sit::VehicleView& v) {
                                          return v.id == selectedBusId_;
                                      });
        if (bus == s.vehicles.end()) {
            clearBusInfo();
        } else {
            QString route = QString::fromStdString(bus->routeId);
            const auto routeInfo = std::find_if(
                s.routes.begin(), s.routes.end(),
                [bus](const sit::RouteView& r) { return r.id == bus->routeId; });
            if (routeInfo != s.routes.end()) {
                route = QStringLiteral("%1 - %2")
                            .arg(QString::fromStdString(routeInfo->id),
                                 QString::fromStdString(routeInfo->name));
            }
            showBusInfo(bus->id, QString::fromStdString(bus->type), route,
                        bus->passengers, bus->capacity, bus->dwelling,
                        QString::fromStdString(bus->nextStopName));
        }
    }

    clockLabel_->setText(QString::fromStdString(s.clock));
    clockCaption_->setText(s.paused ? tr("Hora simulada · en pausa") : tr("Hora simulada"));
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
            // Tinte suave sobre el fondo oscuro; el porcentaje sigue siendo el dato principal.
            item->setBackground(st.saturation >= 0.8 ? QColor(239, 68, 68, 70)
                                : st.saturation >= 0.5 ? QColor(245, 158, 11, 60)
                                : QBrush());
        }
    }

    for (const auto& line : s.log) addLog(QString::fromStdString(line));
}

void SimulationWidget::showBusInfo(int id, const QString& type, const QString& route,
                                   int passengers, int capacity, bool dwelling, const QString& nextStop) {
    busInfoPanel_->setTitle(tr("Bus #%1").arg(id));
    busInfoPanel_->show();
    busTypeLabel_->setText(type);
    busRouteLabel_->setText(route);
    busPassengersLabel_->setText(tr("%1 / %2").arg(passengers).arg(capacity));
    busFreeSeatsLabel_->setText(QString::number(std::max(0, capacity - passengers)));
    busStatusLabel_->setText(dwelling ? tr("Detenido en paradero") : tr("En marcha"));
    busNextStopLabel_->setText(nextStop.isEmpty() ? tr("No disponible") : nextStop);
    const int occupancy = capacity > 0 ? std::clamp(100 * passengers / capacity, 0, 100) : 0;
    busOccupancyBar_->setValue(occupancy);
    // Color + texto: nunca solo color.
    const bool full = occupancy >= 80, busy = occupancy >= 50;
    busOccupancyBar_->setFormat(full ? tr("%p% · lleno") : busy ? tr("%p% · concurrido") : tr("%p% · holgado"));
    busOccupancyBar_->setStyleSheet(QStringLiteral("QProgressBar::chunk { background: %1; border-radius: 3px; }")
                                        .arg(full ? "#EF4444" : busy ? "#F59E0B" : "#22C55E"));
}

void SimulationWidget::clearBusInfo() {
    selectedBusId_ = -1;
    mapBridge_->clearSelection();
    busInfoPanel_->setTitle(tr("Información del bus"));
    busInfoPanel_->hide();
}

void SimulationWidget::addLog(const QString& line) {
    if (!logList_) return;
    logList_->insertItem(0, line);
    while (logList_->count() > 200) delete logList_->takeItem(logList_->count() - 1);
}

void SimulationWidget::onError(const QString& message) {
    addLog("ERROR: " + message);
}

void SimulationWidget::showIncomingCall(const QString& vehicleId, const QString& name,
                                        const QString& status) {
    mapBridge_->setIncomingCall(vehicleId, name, status);
    addLog(QStringLiteral("Llamada entrante de %1 en vehículo %2").arg(name, vehicleId));
}
