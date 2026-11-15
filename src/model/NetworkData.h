#pragma once
#include <string>
#include <vector>

namespace sit {

// Datos "crudos" de una red de transporte, independientes del formato de origen.
struct StopData {
    std::string id, name, type;     // type: regular | terminal | transfer
    double x{}, y{};
    int capacity{80};
};

struct RouteData {
    std::string id, name, kind, color;   // kind: trunk | feeder
    std::vector<std::string> stops;      // ids de paraderos en orden
};

struct FleetData {
    std::string routeId, vehicleType;    // convencional | articulado | alimentador
    std::size_t startIndex{0};
};

struct NetworkData {
    std::vector<StopData> stops;
    std::vector<RouteData> routes;
    std::vector<FleetData> fleet;
    int fleetLimit{10};
};

// Interfaz objetivo del patron estructural Adapter: cada formato de archivo se "adapta" a
// esta interfaz, y el resto del sistema no sabe de donde vienen los datos.
class IRouteSource {
public:
    virtual ~IRouteSource() = default;
    virtual NetworkData load() = 0;             // lanza DataLoadException si falla
    virtual std::string describe() const = 0;
};

} // namespace sit
