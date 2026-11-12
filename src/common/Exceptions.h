#pragma once
#include <stdexcept>
#include <string>

namespace sit {

// Jerarquia de errores propia del sistema: todo error "esperable" hereda de SitException,
// asi el controlador puede capturarlos con un solo catch y mostrarlos en la vista.
class SitException : public std::runtime_error {
public:
    explicit SitException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidVehicleTypeException : public SitException {
public:
    explicit InvalidVehicleTypeException(const std::string& t)
        : SitException("Tipo de vehiculo invalido: " + t) {}
};

class CapacityExceededException : public SitException {
public:
    CapacityExceededException() : SitException("Capacidad del vehiculo excedida") {}
};

class InvalidRouteException : public SitException {
public:
    explicit InvalidRouteException(const std::string& detail)
        : SitException("Ruta invalida: " + detail) {}
};

class FleetLimitExceededException : public SitException {
public:
    explicit FleetLimitExceededException(int limit)
        : SitException("Limite de flota alcanzado (" + std::to_string(limit) + " buses)") {}
};

class InvalidEventException : public SitException {
public:
    explicit InvalidEventException(const std::string& detail)
        : SitException("Evento invalido: " + detail) {}
};

class DataLoadException : public SitException {
public:
    explicit DataLoadException(const std::string& detail)
        : SitException("Error al cargar datos: " + detail) {}
};

} // namespace sit
