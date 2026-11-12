#pragma once
#include <QString>

// Roles soportados por SIT-Core.
enum class Role {
    FleetManager,               // fleet_manager (antes "administrador")
    User,                       // user (conductores afiliados)
    JefeDeTrafico,              // jefe_de_trafico
    ResponsableDesplazamientos, // responsable_de_desplazamientos
    Guess                       // guess (invitado / sandbox)
};

namespace Roles {

inline QString id(Role r)
{
    switch (r) {
    case Role::FleetManager:               return QStringLiteral("fleet_manager");
    case Role::User:                       return QStringLiteral("user");
    case Role::JefeDeTrafico:              return QStringLiteral("jefe_de_trafico");
    case Role::ResponsableDesplazamientos: return QStringLiteral("responsable_de_desplazamientos");
    case Role::Guess:                      return QStringLiteral("guess");
    }
    return QStringLiteral("user");
}

inline QString label(Role r)
{
    switch (r) {
    case Role::FleetManager:               return QStringLiteral("Gerente de Flota");
    case Role::User:                       return QStringLiteral("Conductor");
    case Role::JefeDeTrafico:              return QStringLiteral("Jefe de Tráfico");
    case Role::ResponsableDesplazamientos: return QStringLiteral("Resp. de Desplazamientos");
    case Role::Guess:                      return QStringLiteral("Invitado (Sandbox)");
    }
    return QStringLiteral("Conductor");
}

// Convierte el identificador de texto ("fleet_manager", ...) en Role.
// "administrador" se acepta como alias heredado de fleet_manager.
inline Role fromId(const QString &s, bool *ok = nullptr)
{
    const QString k = s.trimmed().toLower();
    bool found = true;
    Role r = Role::User;
    if (k == QLatin1String("fleet_manager") || k == QLatin1String("administrador")) r = Role::FleetManager;
    else if (k == QLatin1String("user"))                                             r = Role::User;
    else if (k == QLatin1String("jefe_de_trafico"))                                  r = Role::JefeDeTrafico;
    else if (k == QLatin1String("responsable_de_desplazamientos"))                   r = Role::ResponsableDesplazamientos;
    else if (k == QLatin1String("guess") || k == QLatin1String("guest"))             r = Role::Guess;
    else found = false;
    if (ok) *ok = found;
    return r;
}

} // namespace Roles
