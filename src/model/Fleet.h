#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "model/Vehicle.h"

namespace teoto {

/// Contenedor propietario de los vehiculos de la flota (RF-01, RF-02, OBJ-06).
class Fleet
{
public:
    void add(std::unique_ptr<Vehicle> vehicle);

    std::size_t size() const { return m_vehicles.size(); }
    bool empty() const { return m_vehicles.empty(); }

    /// Devuelve el vehiculo en \a index, o nullptr si el indice es invalido.
    Vehicle *at(std::size_t index) const;

    void updateAll(double deltaTime);

    const std::vector<std::unique_ptr<Vehicle>> &vehicles() const { return m_vehicles; }

private:
    std::vector<std::unique_ptr<Vehicle>> m_vehicles;
};

} // namespace teoto
