#pragma once
#include "dashboardbase.h"

// Rol user: conductor afiliado. Estadísticas personales, próxima ruta y unidad.
class UserDashboard : public DashboardBase
{
    Q_OBJECT
public:
    explicit UserDashboard(const QString &userName, bool embedded = false,
                           QWidget *parent = nullptr);

private:
    QWidget *buildPanel();
    QWidget *buildTrips();
};
