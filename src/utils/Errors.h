#pragma once

#include <QString>

namespace teoto {

/// Códigos de error de dominio. Se usan para reportar fallos sin recurrir a
/// excepciones para el control de flujo normal (ver requerimientos §11.5).
enum class ErrorCode {
    Ok,
    InvalidIndex,
    StopNotFound,
    RouteTooShort,
    VehicleFull,
    VehicleUnavailable,
    InvalidData,
};

inline QString errorToString(ErrorCode code)
{
    switch (code) {
    case ErrorCode::Ok:
        return QStringLiteral("sin error");
    case ErrorCode::InvalidIndex:
        return QStringLiteral("indice invalido");
    case ErrorCode::StopNotFound:
        return QStringLiteral("paradero no encontrado");
    case ErrorCode::RouteTooShort:
        return QStringLiteral("ruta con menos de dos paraderos");
    case ErrorCode::VehicleFull:
        return QStringLiteral("vehiculo lleno");
    case ErrorCode::VehicleUnavailable:
        return QStringLiteral("vehiculo no disponible");
    case ErrorCode::InvalidData:
        return QStringLiteral("datos invalidos");
    }
    return QStringLiteral("error desconocido");
}

} // namespace teoto
