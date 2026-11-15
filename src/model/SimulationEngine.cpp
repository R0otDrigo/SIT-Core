#include "model/SimulationEngine.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include "common/Exceptions.h"

namespace sit {

SimulationEngine::SimulationEngine(std::unique_ptr<Network> network)
    : network_(std::move(network)), strategy_(std::make_shared<ManualDispatch>()) {}

SimulationEngine::~SimulationEngine() { stop(); }

void SimulationEngine::start() {
    if (running_.exchange(true)) return;
    simThread_ = std::thread([this] { simLoop(); });
}

void SimulationEngine::stop() {
    if (!running_.exchange(false)) return;
    if (simThread_.joinable()) simThread_.join();
}

void SimulationEngine::setStrategy(std::shared_ptr<IDispatchStrategy> strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    strategy_ = std::move(strategy);
}

void SimulationEngine::setObserver(Observer observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    observer_ = std::move(observer);
}

void SimulationEngine::dispatch(const std::string& routeId, VehicleType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    network_->addVehicle(type, routeId);
}

void SimulationEngine::retire(const std::string& routeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!network_->retireVehicle(routeId))
        throw SitException("No hay un bus vacio para retirar en la ruta " + routeId);
}

void SimulationEngine::triggerEvent(EventType type, const std::string& routeId, const std::string& stopId) {
    std::lock_guard<std::mutex> lock(mutex_);
    network_->triggerEvent(type, routeId, stopId);
}

Snapshot SimulationEngine::snapshot() {
    std::lock_guard<std::mutex> lock(mutex_);
    return makeSnapshotLocked(false);
}

void SimulationEngine::stepOnce(double dt) {
    std::lock_guard<std::mutex> lock(mutex_);
    advanceLocked(dt);
}

void SimulationEngine::advanceLocked(double simDt) {
    // Sub-pasos de max 0.25 s para que a velocidades altas los buses no "salten" paraderos.
    const int steps = std::max(1, static_cast<int>(std::ceil(simDt / 0.25)));
    const double h = simDt / steps;
    for (int i = 0; i < steps; ++i) network_->step(h);
    applyStrategyLocked();
}

void SimulationEngine::applyStrategyLocked() {
    for (const auto& order : strategy_->decide(*network_)) {
        try {
            if (order.action == DispatchAction::Dispatch) network_->addVehicle(order.type, order.routeId);
            else network_->retireVehicle(order.routeId);
        } catch (const SitException& e) {
            network_->log(std::string("Despacho automatico fallo: ") + e.what());
        }
    }
}

Snapshot SimulationEngine::makeSnapshotLocked(bool drainLog) {
    Snapshot s = network_->snapshot();
    s.paused = paused_;
    s.speed = speed_;
    s.strategy = strategy_->name();
    if (drainLog) s.log = network_->drainLog();
    return s;
}

void SimulationEngine::simLoop() {
    using clock = std::chrono::steady_clock;
    constexpr auto period = std::chrono::milliseconds(50);
    auto next = clock::now();
    while (running_) {
        next += period;
        std::this_thread::sleep_until(next);
        if (!running_) break;

        Snapshot snap;
        Observer observer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!paused_) advanceLocked(0.05 * speed_);
            snap = makeSnapshotLocked(true);
            observer = observer_;
        }
        if (observer) observer(snap);           // fuera del lock: el observador puede tardar
    }
}

} // namespace sit
