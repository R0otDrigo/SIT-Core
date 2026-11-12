#pragma once
#include "dashboardbase.h"
#include <QTableWidget>

// Rol jefe_de_trafico: monitoreo de rutas activas y delegación de tareas (IA / RPA).
class TrafficDashboard : public DashboardBase
{
    Q_OBJECT
public:
    explicit TrafficDashboard(const QString &userName, bool embedded = false,
                              QWidget *parent = nullptr);

private:
    QWidget *buildMonitoring();
    QWidget *buildDelegation();

    class QTableWidget *m_delegated = nullptr;
};
