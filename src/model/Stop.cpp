#include "model/Stop.h"
#include <algorithm>
#include "common/Exceptions.h"

namespace sit {

int Stop::addWaiting(int n) {
    n = std::max(0, n);
    const int room = std::max(0, maxWaiting_ * 2 - waiting_);
    const int accepted = std::min(n, room);
    waiting_ += accepted;
    return n - accepted;
}

int Stop::generate(double dt, double factor) {
    accumulator_ += boardingRate() * factor * dt;
    const int n = static_cast<int>(accumulator_);
    accumulator_ -= n;
    return addWaiting(n);
}

int Stop::takeWaiting(int n) {
    const int out = std::clamp(n, 0, waiting_);
    waiting_ -= out;
    return out;
}

double Stop::saturation() const {
    return maxWaiting_ > 0 ? static_cast<double>(waiting_) / maxWaiting_ : 0.0;
}

std::shared_ptr<Stop> StopFactory::create(const std::string& type, std::string id, std::string name,
                                          Position pos, int maxWaiting) {
    if (type == "regular")  return std::make_shared<RegularStop>(std::move(id), std::move(name), pos, maxWaiting);
    if (type == "terminal") return std::make_shared<Terminal>(std::move(id), std::move(name), pos, maxWaiting);
    if (type == "transfer") return std::make_shared<TransferStation>(std::move(id), std::move(name), pos, maxWaiting);
    throw DataLoadException("tipo de paradero desconocido: " + type);
}

} // namespace sit
