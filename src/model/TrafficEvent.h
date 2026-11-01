#pragma once
#include <memory>
#include <string>
#include "model/Stop.h"

namespace sit {

enum class EventType { Accident, Protest, RoadWork };

// Jerarquia polimorfica de eventos de trafico. Cada evento afecta una ruta (velocidad)
// y/o cierra un paradero.
class TrafficEvent {
public:
    TrafficEvent(std::string routeId, std::string stopId, double duration)
        : routeId_(std::move(routeId)), stopId_(std::move(stopId)), remaining_(duration) {}
    virtual ~TrafficEvent() = default;

    virtual std::string name() const = 0;
    virtual double speedFactor() const { return 1.0; }                 // 1.0 = sin efecto
    virtual bool closesStop(const Stop&) const { return false; }

    void tick(double dt) { remaining_ -= dt; }
    bool expired() const { return remaining_ <= 0.0; }

    const std::string& routeId() const { return routeId_; }
    const std::string& stopId() const { return stopId_; }
    double remaining() const { return remaining_; }

private:
    std::string routeId_;
    std::string stopId_;
    double remaining_;
};

class Accident : public TrafficEvent {
public:
    using TrafficEvent::TrafficEvent;
    std::string name() const override { return "Accidente"; }
    double speedFactor() const override { return 0.2; }
};

class Protest : public TrafficEvent {
public:
    using TrafficEvent::TrafficEvent;
    std::string name() const override { return "Protesta"; }
    double speedFactor() const override { return 0.7; }
    bool closesStop(const Stop& s) const override { return s.id() == stopId(); }
};

class RoadWork : public TrafficEvent {
public:
    using TrafficEvent::TrafficEvent;
    std::string name() const override { return "Obra vial"; }
    double speedFactor() const override { return 0.5; }
};

// Patron creacional: Factory Method tambien para eventos.
class TrafficEventFactory {
public:
    // duration < 0 usa la duracion por defecto de cada tipo (segundos simulados).
    static std::unique_ptr<TrafficEvent> create(EventType type, const std::string& routeId,
                                                const std::string& stopId = "", double duration = -1.0);
    static std::string label(EventType type);
};

} // namespace sit
