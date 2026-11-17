#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include "dashboard.h"

class SimulationWidget;

class UserDashboard : public Dashboard {
  Q_OBJECT
public:
  explicit UserDashboard(QWidget *parent = nullptr);

  SimulationWidget *simulation() const { return simulation_; }

protected:
  QString dashboardTitle() const override;
  QString welcomeMessage() const override;

private:
  SimulationWidget *simulation_ = nullptr;
};

#endif // USERDASHBOARD_H
