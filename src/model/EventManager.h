#pragma once
#include <memory>
#include <string>
#include <vector>
#include "model/Stop.h"
#include "model/TrafficEvent.h"

namespace sit {

// Guarda los eventos de trafico activos y responde dos preguntas a la red:
// "que tan lento va esta ruta?" y "este paradero esta cerrado?".
class EventManager {
public:
    void add(std::unique_ptr<TrafficEvent> event) { events_.push_back(std::move(event)); }

    // Avanza dt segundos. Devuelve un mensaje por cada evento que acaba de terminar.
    std::vector<std::string> tick(double dt);

    double speedFactorFor(const std::string& routeId) const;
    bool isStopClosed(const Stop& stop) const;

    bool empty() const { return events_.empty(); }
    const std::vector<std::unique_ptr<TrafficEvent>>& all() const { return events_; }

private:
    std::vector<std::unique_ptr<TrafficEvent>> events_;
};

} // namespace sit
