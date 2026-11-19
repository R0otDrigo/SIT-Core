#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include "dashboard.h"

class QGroupBox;
class QLineEdit;
class SimulationWidget;
class UserBroadcast;
class UnitRegistry;

class AdminDashboard : public Dashboard
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    ~AdminDashboard() override;

    // El mapa general se crea solo cuando inicia sesion un administrador.
    void initialize(const QString &idToken = QString());
    void resetSession();
    void setBroadcast(UserBroadcast *broadcast);
    void setUnitRegistry(UnitRegistry *registry);
    void sendMessages(const QString &antecedentes, const QString &evento);
    void showIncomingCall(const QString &vehicleId, const QString &name,
                          const QString &status = QString());

    SimulationWidget *simulation() const { return simulation_; }

signals:
    void callAnswered(const QString &vehicleId);
    void callRequested(const QString &vehicleId);

protected:
    QString dashboardTitle() const override;
    QString welcomeMessage() const override;

private:
    SimulationWidget *simulation_ = nullptr;
    UserBroadcast *broadcast_ = nullptr;
    QGroupBox *messagesBox_ = nullptr;
    QLineEdit *antecedentesEdit_ = nullptr;
    QLineEdit *eventoEdit_ = nullptr;
    UnitRegistry *unitRegistry_ = nullptr;
    QString idToken_;
};

#endif // ADMINDASHBOARD_H
