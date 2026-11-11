#ifndef GUESTDASHBOARD_H
#define GUESTDASHBOARD_H

#include <QMainWindow>
#include <QLabel>

class QEvent;

class GuestDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit GuestDashboard(QWidget *parent = nullptr);
    void retranslate();

protected:
    void changeEvent(QEvent *event) override;

private:
    QLabel *m_welcomeLabel;
};

#endif // GUESTDASHBOARD_H
