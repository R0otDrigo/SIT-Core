#ifndef GUESTDASHBOARD_H
#define GUESTDASHBOARD_H

#include "dashboard.h"

class GuestDashboard : public Dashboard
{
    Q_OBJECT
public:
    explicit GuestDashboard(QWidget *parent = nullptr);

protected:
    QString dashboardTitle() const override;
    QString welcomeMessage() const override;
};

#endif // GUESTDASHBOARD_H
