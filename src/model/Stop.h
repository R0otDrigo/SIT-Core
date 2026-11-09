#pragma once
#include <string>
#include "common/Position.h"

namespace sit {

class Stop {
public:
    Stop(std::string name, Position pos, int maxWaiting)
        : name_(std::move(name)), pos_(pos), maxWaiting_(maxWaiting) {}
    virtual ~Stop() = default;

    // Pasajeros nuevos por segundo (depende del tipo de paradero).
    virtual double boardingRate() const = 0;

    void addWaiting(int n);
    int takeWaiting(int n);                 // sube pasajeros a un bus, devuelve cuantos salieron
    int waiting() const { return waiting_; }
    double saturation() const;              // 0.0 a 1.0+

    const std::string& name() const { return name_; }
    const Position& position() const { return pos_; }

    // Para ordenar paraderos por saturacion.
    bool operator<(const Stop& o) const { return saturation() < o.saturation(); }

private:
    std::string name_;
    Position pos_;
    int maxWaiting_;
    int waiting_{0};
};

class RegularStop : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 0.5; }
};

class Terminal : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 2.0; }
};

} // namespace sit
