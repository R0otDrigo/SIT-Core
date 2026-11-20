#include "view/SimulationWidget.h"
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlError>
#include <QScrollArea>
#include <QSplitter>
#include <QStringList>
#include <QStyle>
#include <QTabWidget>
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

namespace {

QFrame* makeKpiCard(const QString& title, QLabel*& value, const QString& accent,
                    QWidget* parent) {
    auto* card = new QFrame(parent);
    card->setObjectName("kpiCard");
    card->setProperty("accent", accent);
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 11, 14, 11);
    layout->setSpacing(2);
    auto* titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("kpiTitle");
    value = new QLabel(QStringLiteral("-"), card);
    value->setObjectName("kpiValue");
    layout->addWidget(titleLabel);
    layout->addWidget(value);
    return card;
}

} // namespace

void SimulationWidget::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setObjectName("simSplitter");
    splitter->setChildrenCollapsible(false);

    auto* mapCard = new QFrame(splitter);
    mapCard->setObjectName("mapCard");
    auto* mapLayout = new QVBoxLayout(mapCard);
    mapLayout->setContentsMargins(0, 0, 0, 0);
    mapLayout->setSpacing(0);

    map_ = new QQuickWidget(mapCard);
    map_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    map_->setClearColor(QColor("#e8eef5"));
    map_->setMinimumSize(480, 360);
    map_->rootContext()->setContextProperty("mapBridge", mapBridge_);
    mapLayout->addWidget(map_);
    splitter->addWidget(mapCard);

    connect(mapBridge_, &MapBridge::busSelected, this,
            [this](int id, const QString& type, const QString& route,
                   int passengers, int capacity, bool dwelling, const QString& nextStop) {
        selectedBusId_ = id;
        showBusInfo(id, type, route, passengers, capacity, dwelling, nextStop);
    });

    auto* scroll = new QScrollArea(splitter);
    scroll->setObjectName("sideScroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setMinimumWidth(370);
    scroll->setMaximumWidth(460);

    auto* side = new QFrame;
    side->setObjectName("sidePanel");
    side->setMinimumWidth(350);
    auto* col = new QVBoxLayout(side);
    col->setContentsMargins(14, 14, 14, 14);
    col->setSpacing(12);

    // --- Reloj ---
    auto* clockCard = new QFrame(side);
    clockCard->setObjectName("panelCard");
    auto* clockLayout = new QVBoxLayout(clockCard);
    clockLayout->setContentsMargins(16, 12, 16, 12);
    clockLayout->setSpacing(0);
    auto* clockHint = new QLabel(tr("Hora simulada"), clockCard);
    clockHint->setObjectName("clockHint");
    clockLabel_ = new QLabel(QStringLiteral("--:--"), clockCard);
    clockLabel_->setObjectName("clockLabel");
    clockLayout->addWidget(clockHint);
    clockLayout->addWidget(clockLabel_);
    col->addWidget(clockCard);

    // --- Ficha del bus seleccionado ---
    busInfoPanel_ = new QGroupBox(tr("Información del bus"), side);
    busInfoPanel_->setObjectName("busInfoPanel");
    auto* busLayout = new QVBoxLayout(busInfoPanel_);
    busLayout->setSpacing(10);
    backToOverviewButton_ = new QPushButton(tr("Volver al resumen"), busInfoPanel_);
    backToOverviewButton_->setObjectName("backToOverviewButton");
    backToOverviewButton_->setCursor(Qt::PointingHandCursor);
    busLayout->addWidget(backToOverviewButton_, 0, Qt::AlignRight);
    connect(backToOverviewButton_, &QPushButton::clicked, this, [this] { clearBusInfo(); });

    busInfoDetails_ = new QWidget(busInfoPanel_);
    busInfoDetails_->setObjectName("busInfoDetails");
    auto* busFields = new QFormLayout(busInfoDetails_);
    busFields->setHorizontalSpacing(12);
    busFields->setVerticalSpacing(8);
    busTypeLabel_ = new QLabel;
    busRouteLabel_ = new QLabel;
    busRouteLabel_->setObjectName("busRouteText");
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
    busOccupancyBar_->setFormat(tr("%p% ocupado"));
    busFields->addRow(tr("Tipo:"), busTypeLabel_);
    busFields->addRow(tr("Ruta:"), busRouteLabel_);
    busFields->addRow(tr("Pasajeros:"), busPassengersLabel_);
    busFields->addRow(tr("Asientos libres:"), busFreeSeatsLabel_);
    busFields->addRow(tr("Estado:"), busStatusLabel_);
    busFields->addRow(tr("Próximo paradero:"), busNextStopLabel_);
    busFields->addRow(tr("Ocupación:"), busOccupancyBar_);
    busLayout->addWidget(busInfoDetails_);
    col->addWidget(busInfoPanel_);
    busInfoPanel_->hide();

    // --- Simulación ---
    auto* simBox = new QGroupBox(tr("Simulación"), side);
    auto* simGrid = new QGridLayout(simBox);
    simGrid->setHorizontalSpacing(8);
    simGrid->setVerticalSpacing(8);
    pauseButton_ = new QPushButton(tr("Pausar"), simBox);
    pauseButton_->setCheckable(true);
    pauseButton_->setCursor(Qt::PointingHandCursor);
    speedCombo_ = new QComboBox(simBox);
    for (double v : {1.0, 2.0, 5.0, 10.0, 20.0})
        speedCombo_->addItem(QString("%1x").arg(v), v);
    speedCombo_->setCurrentIndex(1);
    simGrid->addWidget(pauseButton_, 0, 0);
    simGrid->addWidget(new QLabel(tr("Velocidad:"), simBox), 0, 1, Qt::AlignRight);
    simGrid->addWidget(speedCombo_, 0, 2);
    col->addWidget(simBox);

    // --- Indicadores ---
    auto* kpiGrid = new QGridLayout;
    kpiGrid->setHorizontalSpacing(8);
    kpiGrid->setVerticalSpacing(8);
    kpiGrid->addWidget(makeKpiCard(tr("Buses en servicio"), busesLabel_, "blue", side), 0, 0);
    kpiGrid->addWidget(makeKpiCard(tr("Transportados"), transportedLabel_, "green", side), 0, 1);
    kpiGrid->addWidget(makeKpiCard(tr("Desistieron"), abandonedLabel_, "amber", side), 1, 0);
    kpiGrid->addWidget(makeKpiCard(tr("Saturación media"), saturationLabel_, "red", side), 1, 1);
    kpiGrid->setColumnStretch(0, 1);
    kpiGrid->setColumnStretch(1, 1);
    col->addLayout(kpiGrid);

    // --- Controlador de flota ---
    auto* actBox = new QGroupBox(tr("Controlador de flota"), side);
    auto* act = new QGridLayout(actBox);
    act->setHorizontalSpacing(8);
    act->setVerticalSpacing(8);
    routeCombo_ = new QComboBox(actBox);
    vehicleTypeCombo_ = new QComboBox(actBox);
    eventCombo_ = new QComboBox(actBox);
    for (auto t : {sit::EventType::Accident, sit::EventType::Protest, sit::EventType::RoadWork})
        eventCombo_->addItem(QString::fromStdString(sit::TrafficEventFactory::label(t)),
                             static_cast<int>(t));
    auto* dispatchBtn = new QPushButton(tr("Despachar"), actBox);
    dispatchBtn->setObjectName("primaryButton");
    dispatchBtn->setCursor(Qt::PointingHandCursor);
    auto* retireBtn = new QPushButton(tr("Retirar bus vacío"), actBox);
    auto* eventBtn = new QPushButton(tr("Provocar evento"), actBox);
    eventBtn->setObjectName("dangerButton");
    eventBtn->setCursor(Qt::PointingHandCursor);
    act->addWidget(new QLabel(tr("Ruta:"), actBox), 0, 0);
    act->addWidget(routeCombo_, 0, 1, 1, 2);
    act->addWidget(new QLabel(tr("Tipo de bus:"), actBox), 1, 0);
    act->addWidget(vehicleTypeCombo_, 1, 1);
    act->addWidget(dispatchBtn, 1, 2);
    act->addWidget(retireBtn, 2, 1, 1, 2);
    act->addWidget(new QLabel(tr("Evento:"), actBox), 3, 0);
    act->addWidget(eventCombo_, 3, 1);
    act->addWidget(eventBtn, 3, 2);
    col->addWidget(actBox);

    // --- Paraderos y registro ---
    stopsTable_ = new QTableWidget(0, 3, side);
    stopsTable_->setHorizontalHeaderLabels({tr("Paradero"), tr("Esperan"), tr("Saturación")});
    stopsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    stopsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    stopsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    stopsTable_->verticalHeader()->hide();
    stopsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stopsTable_->setSelectionMode(QAbstractItemView::NoSelection);
    stopsTable_->setAlternatingRowColors(true);
    stopsTable_->setShowGrid(false);
    stopsTable_->setMinimumHeight(220);

    logList_ = new QListWidget(side);
    logList_->setAlternatingRowColors(false);

    auto* tabs = new QTabWidget(side);
    tabs->setObjectName("dataTabs");
    tabs->addTab(stopsTable_, tr("Paraderos"));
    tabs->addTab(logList_, tr("Registro"));
    col->addWidget(tabs, 1);

    scroll->setWidget(side);
    splitter->addWidget(scroll);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setSizes({880, 400});
    root->addWidget(splitter, 1);

    // --- Conexiones vista -> controlador ---
    connect(pauseButton_, &QPushButton::toggled, this, [this](bool paused) {
        pauseButton_->setText(paused ? tr("Reanudar") : tr("Pausar"));
        controller_->setPaused(paused);
    });
    connect(speedCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { controller_->setSpeed(speedCombo_->currentData().toDouble()); });
    connect(routeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { onRouteChanged(); });
    connect(dispatchBtn, &QPushButton::clicked, this, [this] {
        controller_->dispatchBus(routeCombo_->currentData().toString(),
                                 vehicleTypeCombo_->currentData().toInt());
    });
    connect(retireBtn, &QPushButton::clicked, this,
            [this] { controller_->retireBus(routeCombo_->currentData().toString()); });
    connect(eventBtn, &QPushButton::clicked, this, [this] {
        controller_->triggerEvent(eventCombo_->currentData().toInt(),
                                  routeCombo_->currentData().toString());
    });

    connect(map_, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if (status != QQuickWidget::Error) return;
        QStringList messages;
        for (const QQmlError& error : map_->errors()) messages << error.toString();
        addLog("ERROR mapa QML: " + messages.join(" | "));
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

    clockLabel_->setText(QString::fromStdString(s.clock) + (s.paused ? tr(" · pausa") : QString()));
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
            if (st.saturation >= 0.8)
                item->setBackground(QColor("#FEE2E2"));
            else if (st.saturation >= 0.5)
                item->setBackground(QColor("#FEF3C7"));
            else
                item->setBackground(QBrush());
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
    busOccupancyBar_->setProperty("level", occupancy >= 80 ? "high" : occupancy >= 50 ? "medium" : "low");
    busOccupancyBar_->style()->unpolish(busOccupancyBar_);
    busOccupancyBar_->style()->polish(busOccupancyBar_);
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
