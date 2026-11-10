#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace SITCore {
    namespace Roles {
        // Roles del sistema actualizados
        const QString FLEET_MANAGER = "fleet_manager";
        const QString DRIVER = "user";
        const QString GUEST = "guess"; // TODO: Eliminar en producción

        // Futuros roles a implementar
        const QString TRAFFIC_CHIEF = "jefe_de_trafico";
        const QString DISPLACEMENT_MANAGER = "responsable_de_desplazamientos";
    }
}

#endif // CONSTANTS_H