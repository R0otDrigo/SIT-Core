#include "model/Route.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace sit {

double Route::length() const {
    double total = 0.0;
    for (std::size_t i = 1; i < stops_.size(); ++i)
        total += stops_[i - 1]->position().distanceTo(stops_[i]->position());
    return total;
}

std::vector<GeoCoordinate> Route::waypoints() const {
    std::vector<GeoCoordinate> out;
    out.reserve(stops_.size());
    for (const auto& stop : stops_) out.push_back(stop->geoPosition());
    return out;
}

bool Route::hasGeo() const {
    if (stops_.empty()) return false;
    for (const auto& stop : stops_)
        if (!stop->hasGeo()) return false;
    return true;
}

double Route::geoLength() const {
    double total = 0.0;
    for (std::size_t i = 1; i < stops_.size(); ++i)
        total += stops_[i - 1]->geoPosition().distanceTo(stops_[i]->geoPosition());
    return total;
}

bool Route::setPath(std::vector<Position> points) {
    path_.clear();
    cumulative_.clear();
    stopAnchors_.clear();
    if (points.size() < 2) return false;

    cumulative_.resize(points.size(), 0.0);
    for (std::size_t i = 1; i < points.size(); ++i)
        cumulative_[i] = cumulative_[i - 1] + points[i - 1].distanceTo(points[i]);

    for (const auto& stop : stops_) {
        const Position target = stop->position();
        double best = std::numeric_limits<double>::infinity();
        double anchor = -1.0;
        for (std::size_t i = 0; i + 1 < points.size(); ++i) {
            const double sx = points[i + 1].x - points[i].x;
            const double sy = points[i + 1].y - points[i].y;
            const double len2 = sx * sx + sy * sy;
            if (len2 <= 0.0) continue;
            double t = ((target.x - points[i].x) * sx + (target.y - points[i].y) * sy) / len2;
            t = std::clamp(t, 0.0, 1.0);
            const double dx = target.x - (points[i].x + t * sx);
            const double dy = target.y - (points[i].y + t * sy);
            const double dist = std::hypot(dx, dy);
            if (dist < best) {
                best = dist;
                anchor = cumulative_[i] + t * (cumulative_[i + 1] - cumulative_[i]);
            }
        }
        if (anchor < 0.0) {
            path_.clear();
            cumulative_.clear();
            stopAnchors_.clear();
            return false;
        }
        stopAnchors_.push_back(anchor);
    }

    if (stopAnchors_.size() != stops_.size()) {
        path_.clear();
        cumulative_.clear();
        return false;
    }
    for (std::size_t i = 1; i < stopAnchors_.size(); ++i) {
        if (stopAnchors_[i] <= stopAnchors_[i - 1] + 1e-9) {
            path_.clear();
            cumulative_.clear();
            stopAnchors_.clear();
            return false;
        }
    }

    path_ = std::move(points);
    return true;
}

Position Route::pointAtDistance(double distance) const {
    if (!hasPath()) return {};
    const double d = std::clamp(distance, 0.0, cumulative_.back());
    const auto it = std::upper_bound(cumulative_.begin(), cumulative_.end(), d);
    if (it == cumulative_.begin()) return path_.front();
    const std::size_t i = static_cast<std::size_t>(it - cumulative_.begin()) - 1;
    if (i + 1 >= path_.size()) return path_.back();
    const double span = cumulative_[i + 1] - cumulative_[i];
    const double t = span > 0.0 ? (d - cumulative_[i]) / span : 0.0;
    return {path_[i].x + (path_[i + 1].x - path_[i].x) * t,
            path_[i].y + (path_[i + 1].y - path_[i].y) * t};
}

double Route::stopAnchor(std::size_t index) const {
    return index < stopAnchors_.size() ? stopAnchors_[index] : -1.0;
}

} // namespace sit
