#include "model/Route.h"

namespace sit {

double Route::length() const {
    double total = 0.0;
    for (std::size_t i = 1; i < stops_.size(); ++i)
        total += stops_[i - 1]->position().distanceTo(stops_[i]->position());
    return total;
}

} // namespace sit
