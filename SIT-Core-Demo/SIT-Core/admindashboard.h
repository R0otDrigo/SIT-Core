#pragma once
#include "dashboardbase.h"

// Rol fleet_manager (antes "administrador"): administración central de la flota.
class AdminDashboard : public DashboardBase
{
    Q_OBJECT
public:
    explicit AdminDashboard(const QString &userName, bool embedded = false,
                            QWidget *parent = nullptr);

private:
    QWidget *buildOverview();
    QWidget *buildUnits();
    QWidget *buildDrivers();
};
