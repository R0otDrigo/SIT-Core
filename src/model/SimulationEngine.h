#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "model/DispatchStrategy.h"
#include "model/Network.h"
#include "model/Snapshot.h"

namespace sit {

// Motor de simulacion con un hilo de trabajo:
//   * hilo de simulacion: cada 50 ms reales avanza el reloj (x velocidad), aplica la estrategia
//     de despacho y notifica a los observadores con un Snapshot (patron Observer).
// El estado compartido (Network, estrategia) se protege con un std::mutex; los flags simples
// son std::atomic. La interfaz publica es segura para llamarla desde el hilo de la UI.
// (Demo 2 agrega un segundo hilo que genera eventos de trafico aleatorios.)
class SimulationEngine {
public:
    using Observer = std::function<void(const Snapshot&)>;

    explicit SimulationEngine(std::unique_ptr<Network> network);
    ~SimulationEngine();
    SimulationEngine(const SimulationEngine&) = delete;
    SimulationEngine& operator=(const SimulationEngine&) = delete;

    void start();
    void stop();                                // idempotente; espera a que termine el hilo

    void setPaused(bool paused) { paused_ = paused; }
    void setSpeed(double speed) { speed_ = speed < 0.1 ? 0.1 : speed; }
    void setStrategy(std::shared_ptr<IDispatchStrategy> strategy);
    void setObserver(Observer observer);

    // Acciones del controlador de flota (lanzan SitException si no son validas).
    void dispatch(const std::string& routeId, VehicleType type);
    void retire(const std::string& routeId);
    void triggerEvent(EventType type, const std::string& routeId, const std::string& stopId = "");

    Snapshot snapshot();                        // copia consistente del estado actual
    void stepOnce(double dt);                   // avanza sin hilos (pruebas deterministas)
    bool running() const { return running_; }

private:
    void simLoop();
    void advanceLocked(double simDt);           // requiere mutex_ tomado
    void applyStrategyLocked();
    Snapshot makeSnapshotLocked(bool drainLog);

    std::unique_ptr<Network> network_;
    std::shared_ptr<IDispatchStrategy> strategy_;
    Observer observer_;
    mutable std::mutex mutex_;                  // protege network_, strategy_, observer_

    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
    std::atomic<double> speed_{2.0};

    std::thread simThread_;
};

} // namespace sit
