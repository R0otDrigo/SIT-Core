#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QMainWindow>
#include <QLabel>

class UserDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit UserDashboard(QWidget *parent = nullptr);

private:
    QLabel *m_welcomeLabel;
};

#endif // USERDASHBOARD_H
