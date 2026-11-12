#include "model/VehicleFactory.h"
#include "common/Exceptions.h"

namespace sit {

std::unique_ptr<Vehicle> VehicleFactory::create(VehicleType type, int id, Position pos) {
    switch (type) {
        case VehicleType::Conventional: return std::make_unique<ConventionalBus>(id, pos);
        case VehicleType::Articulated:  return std::make_unique<ArticulatedBus>(id, pos);
        case VehicleType::Feeder:       return std::make_unique<FeederBus>(id, pos);
    }
    throw InvalidVehicleTypeException("desconocido");
}

std::unique_ptr<Vehicle> VehicleFactory::create(const std::string& type, int id, Position pos) {
    return create(parse(type), id, pos);
}

VehicleType VehicleFactory::parse(const std::string& type) {
    if (type == "convencional") return VehicleType::Conventional;
    if (type == "articulado")   return VehicleType::Articulated;
    if (type == "alimentador")  return VehicleType::Feeder;
    throw InvalidVehicleTypeException(type);
}

std::string VehicleFactory::label(VehicleType type) {
    switch (type) {
        case VehicleType::Conventional: return "Convencional";
        case VehicleType::Articulated:  return "Articulado";
        case VehicleType::Feeder:       return "Alimentador";
    }
    return "?";
}

} // namespace sit
