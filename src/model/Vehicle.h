#pragma once
#include <memory>
#include <ostream>
#include <string>
#include "common/Position.h"
#include "model/Route.h"

namespace sit {

enum class VehicleState { Idle, Travelling, Dwelling };

// Clase abstracta: herencia + polimorfismo.
class Vehicle {
public:
    Vehicle(int id, Position pos) : id_(id), pos_(pos) {}
    virtual ~Vehicle() = default;

    virtual int capacity() const = 0;
    virtual double speed() const = 0;      // unidades de mapa por segundo
    virtual std::string typeName() const = 0;

    // Sube n pasajeros. Lanza CapacityExceededException si no caben.
    void board(int n);
    int alight(int n);                     // baja hasta n pasajeros, devuelve cuantos bajaron

    // Avanza hacia un destino durante dt segundos.
    void moveTowards(const Position& target, double dt);

    // --- Recorrido sobre una ruta (ida y vuelta entre los extremos) ---
    void assignRoute(const std::shared_ptr<Route>& route, std::size_t startIndex = 0);
    std::shared_ptr<Route> route() const { return route_.lock(); }
    std::shared_ptr<Stop> targetStop() const;   // proximo paradero (o el actual si esta detenido)
    bool atRouteEnd() const;                    // el paradero objetivo es un extremo de la ruta

    // Avanza dt segundos (speedFactor <1 = trafico lento). Devuelve true si acaba de llegar
    // a un paradero y debe ser atendido (bajar/subir pasajeros).
    bool update(double dt, double speedFactor);
    // Define cuanto tiempo se queda detenido en el paradero antes de continuar.
    void finishService(double dwellSeconds);

    int id() const { return id_; }
    int passengers() const { return passengers_; }
    int freeSeats() const { return capacity() - passengers_; }
    const Position& position() const { return pos_; }
    VehicleState state() const { return state_; }

private:
    void advanceIndex(std::size_t routeSize);

    int id_;
    Position pos_;
    int passengers_{0};
    std::weak_ptr<Route> route_;            // la ruta es de la Network; el bus solo la observa
    std::size_t index_{0};                  // indice del paradero objetivo
    int dir_{1};                            // +1 ida, -1 vuelta
    VehicleState state_{VehicleState::Idle};
    double dwell_{0.0};
};

class ConventionalBus : public Vehicle {
public:
    using Vehicle::Vehicle;
    int capacity() const override { return 60; }
    double speed() const override { return 10.0; }
    std::string typeName() const override { return "Convencional"; }
};

class ArticulatedBus : public Vehicle {
public:
    using Vehicle::Vehicle;
    int capacity() const override { return 160; }
    double speed() const override { return 8.0; }
    std::string typeName() const override { return "Articulado"; }
};

class FeederBus : public Vehicle {
public:
    using Vehicle::Vehicle;
    int capacity() const override { return 35; }
    double speed() const override { return 12.0; }
    std::string typeName() const override { return "Alimentador"; }
};

std::ostream& operator<<(std::ostream& os, const Vehicle& v);

} // namespace sit
