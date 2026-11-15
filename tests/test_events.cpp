#include "check.h"
#include "common/Exceptions.h"
#include "model/EventManager.h"
#include "model/TrafficEvent.h"

using namespace sit;

int main() {
    // --- Factory de eventos: cada tipo con su comportamiento (polimorfismo) ---
    auto accident = TrafficEventFactory::create(EventType::Accident, "T");
    auto roadWork = TrafficEventFactory::create(EventType::RoadWork, "T");
    auto protest  = TrafficEventFactory::create(EventType::Protest, "T", "B");
    CHECK(accident->name() == "Accidente");
    CHECK_NEAR(accident->speedFactor(), 0.2, 1e-9);
    CHECK_NEAR(roadWork->speedFactor(), 0.5, 1e-9);
    CHECK_THROWS(TrafficEventFactory::create(EventType::Protest, "T"), InvalidEventException);

    auto stopB = StopFactory::create("regular", "B", "B", {0, 0}, 10);
    auto stopC = StopFactory::create("regular", "C", "C", {1, 0}, 10);
    CHECK(protest->closesStop(*stopB));
    CHECK(!protest->closesStop(*stopC));
    CHECK(!accident->closesStop(*stopB));

    // --- EventManager: combina efectos y hace expirar eventos ---
    EventManager manager;
    CHECK(manager.empty());
    CHECK_NEAR(manager.speedFactorFor("T"), 1.0, 1e-9);
    manager.add(TrafficEventFactory::create(EventType::Accident, "T", "", 10.0));
    manager.add(TrafficEventFactory::create(EventType::RoadWork, "T", "", 30.0));
    manager.add(TrafficEventFactory::create(EventType::Protest, "X", "B", 30.0));
    CHECK_NEAR(manager.speedFactorFor("T"), 0.1, 1e-9);
    CHECK_NEAR(manager.speedFactorFor("OTRA"), 1.0, 1e-9);
    CHECK(manager.isStopClosed(*stopB));

    CHECK(manager.tick(5.0).empty());
    CHECK(manager.tick(6.0).size() == 1);                      // el accidente (10 s) termina
    CHECK_NEAR(manager.speedFactorFor("T"), 0.5, 1e-9);
    manager.tick(100.0);
    CHECK(manager.empty());
    CHECK(!manager.isStopClosed(*stopB));

    return 0;
}
