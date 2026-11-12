#pragma once
#include <memory>
#include <string>
#include "common/Position.h"

namespace sit {

class Stop {
public:
    Stop(std::string id, std::string name, Position pos, int maxWaiting)
        : id_(std::move(id)), name_(std::move(name)), pos_(pos), maxWaiting_(maxWaiting) {}
    virtual ~Stop() = default;

    // Pasajeros nuevos por segundo (depende del tipo de paradero).
    virtual double boardingRate() const = 0;
    virtual std::string typeName() const = 0;

    // Agrega pasajeros esperando. Devuelve cuantos se rechazaron (paradero desbordado).
    int addWaiting(int n);
    // Genera pasajeros segun la tasa del paradero. Devuelve cuantos se rechazaron.
    int generate(double dt, double factor);
    // Sube pasajeros a un bus, devuelve cuantos salieron de la cola.
    int takeWaiting(int n);

    int waiting() const { return waiting_; }
    int maxWaiting() const { return maxWaiting_; }
    double saturation() const;              // 0.0 a 2.0 (el paradero rechaza gente sobre 2x)

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const Position& position() const { return pos_; }

    // Para ordenar paraderos por saturacion.
    bool operator<(const Stop& o) const { return saturation() < o.saturation(); }

private:
    std::string id_;
    std::string name_;
    Position pos_;
    int maxWaiting_;
    int waiting_{0};
    double accumulator_{0.0};
};

class RegularStop : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 0.4; }
    std::string typeName() const override { return "regular"; }
};

class Terminal : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 1.0; }
    std::string typeName() const override { return "terminal"; }
};

class TransferStation : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 0.75; }
    std::string typeName() const override { return "transfer"; }
};

// Patron creacional: Factory Method para paraderos ("regular", "terminal", "transfer").
class StopFactory {
public:
    static std::shared_ptr<Stop> create(const std::string& type, std::string id, std::string name,
                                        Position pos, int maxWaiting);
};

} // namespace sit
