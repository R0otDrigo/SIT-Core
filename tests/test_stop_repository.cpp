#include "check.h"
#include "common/Exceptions.h"
#include "common/Repository.h"
#include "model/Stop.h"

using namespace sit;

int main() {
    auto a = std::make_shared<RegularStop>("PLZ", "Plaza", Position{0, 0}, 100);
    auto b = std::make_shared<Terminal>("TER", "Terminal", Position{10, 0}, 100);
    a->addWaiting(20);
    b->addWaiting(90);

    CHECK(a->saturation() == 0.2);
    CHECK(*a < *b);
    CHECK(a->takeWaiting(50) == 20);

    // Un paradero rechaza gente sobre 2x su capacidad.
    CHECK(b->addWaiting(500) == 500 - 110);
    CHECK(b->waiting() == 200);

    Repository<Stop> repo;
    repo.add(a);
    repo.add(b);
    auto saturated = repo.findIf([](const Stop& s) { return s.saturation() > 0.8; });
    CHECK(saturated && saturated->name() == "Terminal");
    CHECK(repo.byId("PLZ") == a);
    CHECK(repo.byId("nope") == nullptr);

    CHECK_THROWS(StopFactory::create("cohete", "X", "X", {}, 10), DataLoadException);
    CHECK(StopFactory::create("transfer", "T", "T", {}, 10)->typeName() == "transfer");
    return 0;
}
