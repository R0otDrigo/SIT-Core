#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include "dashboard.h"

class UserDashboard : public Dashboard {
  Q_OBJECT
public:
  explicit UserDashboard(QWidget *parent = nullptr);

protected:
  QString dashboardTitle() const override;
  QString welcomeMessage() const override;
};

#endif // USERDASHBOARD_H
