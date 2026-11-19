#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include "dashboard.h"

class QGroupBox;
class QLineEdit;
class SimulationWidget;
class UserBroadcast;

class AdminDashboard : public Dashboard
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    ~AdminDashboard() override;

    // El mapa general se crea solo cuando inicia sesion un administrador.
    void initialize();
    void resetSession();
    void setBroadcast(UserBroadcast *broadcast);
    void sendMessages(const QString &antecedentes, const QString &evento);

    SimulationWidget *simulation() const { return simulation_; }

protected:
    QString dashboardTitle() const override;
    QString welcomeMessage() const override;

private:
    SimulationWidget *simulation_ = nullptr;
    UserBroadcast *broadcast_ = nullptr;
    QGroupBox *messagesBox_ = nullptr;
    QLineEdit *antecedentesEdit_ = nullptr;
    QLineEdit *eventoEdit_ = nullptr;
};

#endif // ADMINDASHBOARD_H
