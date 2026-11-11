#pragma once

#include <memory>
#include <QString>

#include "model/Vehicle.h"
#include "model/Route.h"

namespace teoto {

/// Crea los tipos de vehiculo disponibles sin acoplar al cliente a sus clases.
class VehicleFactory {
public:
    /// Devuelve nullptr cuando el tipo no es valido.
    static std::unique_ptr<Vehicle> create(
        VehicleType type, QString id, std::shared_ptr<Route> route);

    /// Variante para tipos recibidos como texto desde escenarios o la interfaz.
    static std::unique_ptr<Vehicle> create(
        const QString &type, QString id, std::shared_ptr<Route> route);
};

} // namespace teoto
