#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QMainWindow>
#include <QLabel>

class QEvent;

class UserDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit UserDashboard(QWidget *parent = nullptr);
    void retranslate();

protected:
    void changeEvent(QEvent *event) override;

private:
    QLabel *m_welcomeLabel;
};

#endif // USERDASHBOARD_H
