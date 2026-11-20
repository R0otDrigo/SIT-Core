#pragma once
#include <memory>
#include <string>
#include "common/GeoCoordinate.h"
#include "common/Position.h"

namespace sit {

class Stop {
public:
    Stop(std::string id, std::string name, Position pos, int maxWaiting,
         GeoCoordinate geo = {})
        : id_(std::move(id)), name_(std::move(name)), pos_(pos), maxWaiting_(maxWaiting),
          geo_(geo) {}
    virtual ~Stop() = default;

    // Pasajeros nuevos por segundo simulado (1 s = 30 s de reloj). 0.06 ~ 7 personas/hora
    // fuera de hora punta: la demanda total queda al alcance de la flota disponible.
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
    double saturation() const;              // 0.0 a 1.0 (lleno: los que llegan desisten)

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const Position& position() const { return pos_; }
    const GeoCoordinate& geoPosition() const { return geo_; }
    bool hasGeo() const { return geo_.isValid(); }
    void setGeoPosition(GeoCoordinate geo) { geo_ = geo; }

    // Para ordenar paraderos por saturacion.
    bool operator<(const Stop& o) const { return saturation() < o.saturation(); }

private:
    std::string id_;
    std::string name_;
    Position pos_;
    int maxWaiting_;
    GeoCoordinate geo_;
    int waiting_{0};
    double accumulator_{0.0};
};

class RegularStop : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 0.06; }
    std::string typeName() const override { return "regular"; }
};

class Terminal : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 0.15; }
    std::string typeName() const override { return "terminal"; }
};

class TransferStation : public Stop {
public:
    using Stop::Stop;
    double boardingRate() const override { return 0.12; }
    std::string typeName() const override { return "transfer"; }
};

// Patron creacional: Factory Method para paraderos ("regular", "terminal", "transfer").
class StopFactory {
public:
    static std::shared_ptr<Stop> create(const std::string& type, std::string id, std::string name,
                                        Position pos, int maxWaiting,
                                        GeoCoordinate geo = {});
};

} // namespace sit
