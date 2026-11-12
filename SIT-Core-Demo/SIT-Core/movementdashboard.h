#pragma once
#include "dashboardbase.h"

// Rol responsable_de_desplazamientos: horarios, salidas/llegadas y asignación de recursos.
class MovementDashboard : public DashboardBase
{
    Q_OBJECT
public:
    explicit MovementDashboard(const QString &userName, bool embedded = false,
                               QWidget *parent = nullptr);

private:
    QWidget *buildTimetables();
    QWidget *buildAssignments();
};
