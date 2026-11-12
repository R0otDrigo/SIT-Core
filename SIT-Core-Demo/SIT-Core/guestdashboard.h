#pragma once
#include <QWidget>

class QStackedWidget;

// Rol guess: entorno Sandbox para probar todos los dashboards sin cerrar sesión.
class GuestDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit GuestDashboard(const QString &userName, QWidget *parent = nullptr);

signals:
    void logoutRequested();
};
