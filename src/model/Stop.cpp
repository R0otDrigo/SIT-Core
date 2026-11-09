#include "model/Stop.h"
#include <algorithm>

namespace sit {

void Stop::addWaiting(int n) { waiting_ += std::max(0, n); }

int Stop::takeWaiting(int n) {
    int out = std::clamp(n, 0, waiting_);
    waiting_ -= out;
    return out;
}

double Stop::saturation() const {
    return maxWaiting_ > 0 ? static_cast<double>(waiting_) / maxWaiting_ : 0.0;
}

} // namespace sit
