#include "model/EventManager.h"
#include <algorithm>

namespace sit {

std::vector<std::string> EventManager::tick(double dt) {
    for (auto& e : events_) e->tick(dt);

    std::vector<std::string> finished;
    for (auto it = events_.begin(); it != events_.end();) {
        if ((*it)->expired()) {
            finished.push_back("Fin de evento: " + (*it)->name() + " en " + (*it)->routeId());
            it = events_.erase(it);
        } else {
            ++it;
        }
    }
    return finished;
}

double EventManager::speedFactorFor(const std::string& routeId) const {
    double factor = 1.0;
    for (const auto& e : events_)
        if (e->routeId() == routeId) factor *= e->speedFactor();
    return factor;
}

bool EventManager::isStopClosed(const Stop& stop) const {
    return std::any_of(events_.begin(), events_.end(),
                       [&](const std::unique_ptr<TrafficEvent>& e) { return e->closesStop(stop); });
}

} // namespace sit
