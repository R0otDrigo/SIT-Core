#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QMainWindow>
#include <QLabel>

class AdminDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);

private:
    QLabel *m_welcomeLabel;
};

#endif // ADMINDASHBOARD_H
