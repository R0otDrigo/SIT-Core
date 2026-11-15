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

private:
    std::string id_;
    std::string name_;
    RouteKind kind_;
    std::string color_;
    std::vector<std::shared_ptr<Stop>> stops_;
};

} // namespace sit
