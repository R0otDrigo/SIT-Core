#include "controller/SimulationController.h"
#include <QMetaType>
#include "model/CsvRouteAdapter.h"
#include "common/Exceptions.h"
#include "model/DispatchStrategy.h"
#include "model/NetworkBuilder.h"

SimulationController::SimulationController(std::unique_ptr<sit::Network> network, QObject* parent)
    : QObject(parent), engine_(std::move(network)) {
    qRegisterMetaType<sit::Snapshot>("sit::Snapshot");
    // Observer: el hilo de simulacion llama a este lambda; al emitir la senal desde otro hilo,
    // Qt la entrega a la vista de forma encolada (segura) en el hilo de la interfaz.
    engine_.setObserver([this](const sit::Snapshot& s) { emit snapshotReady(s); });
}

SimulationController::~SimulationController() { engine_.stop(); }

std::unique_ptr<sit::Network> SimulationController::loadNetwork(const QString& path) {
    sit::CsvRouteAdapter source(path.toStdString());
    return sit::NetworkBuilder().withData(source.load()).build();
}

void SimulationController::start() { engine_.start(); }
void SimulationController::setPaused(bool paused) { engine_.setPaused(paused); }
void SimulationController::setSpeed(double speed) { engine_.setSpeed(speed); }

void SimulationController::dispatchBus(const QString& routeId, int vehicleType) {
    try {
        engine_.dispatch(routeId.toStdString(), static_cast<sit::VehicleType>(vehicleType));
    } catch (const sit::SitException& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void SimulationController::retireBus(const QString& routeId) {
    try {
        engine_.retire(routeId.toStdString());
    } catch (const sit::SitException& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void SimulationController::triggerEvent(int eventType, const QString& routeId) {
    try {
        engine_.triggerEvent(static_cast<sit::EventType>(eventType), routeId.toStdString());
    } catch (const sit::SitException& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}
