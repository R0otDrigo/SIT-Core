#include "model/TrafficEvent.h"
#include "common/Exceptions.h"

namespace sit {

std::unique_ptr<TrafficEvent> TrafficEventFactory::create(EventType type, const std::string& routeId,
                                                          const std::string& stopId, double duration) {
    switch (type) {
        case EventType::Accident:
            return std::make_unique<Accident>(routeId, stopId, duration < 0 ? 45.0 : duration);
        case EventType::Protest:
            if (stopId.empty()) throw InvalidEventException("una protesta necesita un paradero");
            return std::make_unique<Protest>(routeId, stopId, duration < 0 ? 60.0 : duration);
        case EventType::RoadWork:
            return std::make_unique<RoadWork>(routeId, stopId, duration < 0 ? 90.0 : duration);
    }
    throw InvalidEventException("tipo desconocido");
}

std::string TrafficEventFactory::label(EventType type) {
    switch (type) {
        case EventType::Accident: return "Accidente";
        case EventType::Protest:  return "Protesta";
        case EventType::RoadWork: return "Obra vial";
    }
    return "?";
}

} // namespace sit
