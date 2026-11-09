#pragma once
#include <ostream>
#include <string>
#include "common/Position.h"

namespace sit {

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

    int id() const { return id_; }
    int passengers() const { return passengers_; }
    const Position& position() const { return pos_; }

private:
    int id_;
    Position pos_;
    int passengers_{0};
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
