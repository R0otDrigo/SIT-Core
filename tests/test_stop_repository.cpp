#include "check.h"
#include "common/Repository.h"
#include "model/Stop.h"

using namespace sit;

int main() {
    auto a = std::make_shared<RegularStop>("Plaza", Position{0, 0}, 100);
    auto b = std::make_shared<Terminal>("Terminal", Position{10, 0}, 100);
    a->addWaiting(20);
    b->addWaiting(90);

    CHECK(a->saturation() == 0.2);
    CHECK(*a < *b);
    CHECK(a->takeWaiting(50) == 20);

    Repository<Stop> repo;
    repo.add(a);
    repo.add(b);
    auto saturated = repo.findIf([](const Stop& s) { return s.saturation() > 0.8; });
    CHECK(saturated && saturated->name() == "Terminal");
    return 0;
}
