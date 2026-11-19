#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include "dashboard.h"

class UserBroadcast;
class UserWidget;

class UserDashboard : public Dashboard {
  Q_OBJECT
 public:
  explicit UserDashboard(QWidget *parent = nullptr);
  ~UserDashboard() override;

  // Carga user.qml para el uid autenticado; el simulador completo nunca se crea aqui.
  void initializeSession(const QString &userId);
  void setBroadcast(UserBroadcast *broadcast);
  void resetSession();

  UserWidget *userWidget() const { return userWidget_; }

 protected:
  QString dashboardTitle() const override;
  QString welcomeMessage() const override;

 private:
  UserWidget *userWidget_ = nullptr;
  UserBroadcast *broadcast_ = nullptr;
};

#endif // USERDASHBOARD_H
