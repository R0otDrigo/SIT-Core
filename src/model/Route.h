#pragma once
#include <memory>
#include <string>
#include <vector>
#include "model/Stop.h"

namespace sit {

enum class RouteKind { Trunk, Feeder };

// Recorrido ordenado de paraderos. Los paraderos son compartidos (shared_ptr) entre rutas:
// asi una estacion de transferencia pertenece a varias rutas a la vez.
class Route {
public:
    Route(std::string id, std::string name, RouteKind kind, std::string color)
        : id_(std::move(id)), name_(std::move(name)), kind_(kind), color_(std::move(color)) {}

    void addStop(std::shared_ptr<Stop> stop) { stops_.push_back(std::move(stop)); }

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    RouteKind kind() const { return kind_; }
    const std::string& color() const { return color_; }
    const std::vector<std::shared_ptr<Stop>>& stops() const { return stops_; }
    std::size_t size() const { return stops_.size(); }

    double length() const;
    std::vector<GeoCoordinate> waypoints() const;
    bool hasGeo() const;
    double geoLength() const;      // metros aproximados sobre el trazado geo

    bool setPath(std::vector<Position> points);
    bool hasPath() const { return path_.size() >= 2; }
    const std::vector<Position>& path() const { return path_; }
    double pathLength() const { return cumulative_.empty() ? 0.0 : cumulative_.back(); }
    Position pointAtDistance(double distance) const;
    double stopAnchor(std::size_t index) const;

private:
    std::string id_;
    std::string name_;
    RouteKind kind_;
    std::string color_;
    std::vector<std::shared_ptr<Stop>> stops_;
    std::vector<Position> path_;
    std::vector<double> cumulative_;
    std::vector<double> stopAnchors_;
};

} // namespace sit
