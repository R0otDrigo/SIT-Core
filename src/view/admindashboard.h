#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include "dashboard.h"

class AdminDashboard : public Dashboard
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);

protected:
    QString dashboardTitle() const override;
    QString welcomeMessage() const override;
};

#endif // ADMINDASHBOARD_H
