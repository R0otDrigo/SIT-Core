#ifndef GUESTDASHBOARD_H
#define GUESTDASHBOARD_H

#include <QMainWindow>
#include <QLabel>

class GuestDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit GuestDashboard(QWidget *parent = nullptr);

private:
    QLabel *m_welcomeLabel;
};

#endif // GUESTDASHBOARD_H
