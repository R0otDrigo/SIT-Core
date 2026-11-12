#pragma once
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "common/Repository.h"
#include "model/Route.h"
#include "model/EventManager.h"
#include "model/Snapshot.h"
#include "model/Stop.h"
#include "model/TrafficEvent.h"
#include "model/Vehicle.h"
#include "model/VehicleFactory.h"

namespace sit {

// Raiz del modelo: contiene paraderos, rutas, buses y eventos, y sabe avanzar el tiempo.
// No usa Qt ni hilos; la sincronizacion la hace SimulationEngine.
class Network {
public:
    explicit Network(unsigned seed = 42) : rng_(seed) {}

    void addStop(std::shared_ptr<Stop> stop) { stops_.add(std::move(stop)); }
    void addRoute(std::shared_ptr<Route> route) { routes_.add(std::move(route)); }
    void setFleetLimit(int limit) { fleetLimit_ = limit; }

    // Crea un bus con la Factory y lo coloca en la ruta. Lanza InvalidRouteException o
    // FleetLimitExceededException.
    Vehicle& addVehicle(VehicleType type, const std::string& routeId, std::size_t startIndex = 0);
    // Retira un bus vacio de la ruta. Devuelve false si no hay ninguno vacio.
    bool retireVehicle(const std::string& routeId);

    void triggerEvent(EventType type, const std::string& routeId, const std::string& stopId = "");

    void step(double dt);                       // avanza dt segundos simulados

    // Consultas
    double speedFactorFor(const Route& route) const;
    bool isStopClosed(const Stop& stop) const;
    double demandMultiplier() const;            // hora punta = mas pasajeros
    std::string clockString() const;
    int activeVehicles() const { return static_cast<int>(vehicles_.size()); }
    int vehiclesOnRoute(const std::string& routeId) const;
    int fleetLimit() const { return fleetLimit_; }
    double simTime() const { return simTime_; }
    const Repository<Stop>& stops() const { return stops_; }
    const Repository<Route>& routes() const { return routes_; }
    const std::vector<std::unique_ptr<Vehicle>>& vehicles() const { return vehicles_; }
    const std::vector<std::unique_ptr<TrafficEvent>>& events() const { return events_.all(); }
    long long transported() const { return transported_; }
    long long abandoned() const { return abandoned_; }

    void log(const std::string& message);
    std::vector<std::string> drainLog();
    Snapshot snapshot() const;

    static bool compatible(VehicleType type, RouteKind kind);

private:
    void serve(Vehicle& v);                     // atiende al bus en el paradero actual

    Repository<Stop> stops_;
    Repository<Route> routes_;
    std::vector<std::unique_ptr<Vehicle>> vehicles_;
    EventManager events_;                       // eventos de trafico (clase de C)
    std::vector<std::string> log_;
    std::mt19937 rng_;
    int fleetLimit_{10};
    int nextVehicleId_{1};
    double simTime_{0.0};
    long long transported_{0};
    long long abandoned_{0};
};

} // namespace sit
