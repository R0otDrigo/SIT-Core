#pragma once
#include <stdexcept>
#include <string>

namespace sit {

// Jerarquia de errores propia del sistema.
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

class DataLoadException : public SitException {
public:
    explicit DataLoadException(const std::string& f)
        : SitException("No se pudo cargar el archivo: " + f) {}
};

} // namespace sit
