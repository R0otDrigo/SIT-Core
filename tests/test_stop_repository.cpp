#include "check.h"
#include "common/Repository.h"
#include "model/Stop.h"
#include "model/Route.h"

using namespace teoto;

int main() {
    auto a = std::make_shared<Stop>(
        "S1", "Plaza", QGeoCoordinate(-16.3989, -71.5369), 20);
    auto b = std::make_shared<Stop>(
        "S2", "Terminal", QGeoCoordinate(-16.4290, -71.5210), 90);

    CHECK(a->waitingPassengers() == 20);
    CHECK(a->takeWaiting(50) == 20);
    CHECK(a->waitingPassengers() == 0);

    Repository<Stop> repo;
    repo.add(a);
    repo.add(b);
    const auto terminal = repo.findIf([](const Stop &stop) {
        return stop.name() == QStringLiteral("Terminal");
    });
    CHECK(repo.size() == 2);
    CHECK(terminal != nullptr);
    CHECK(terminal->id() == QStringLiteral("S2"));
    return 0;
}
