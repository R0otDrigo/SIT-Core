#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QMainWindow>
#include <QLabel>

class QEvent;

class AdminDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    void retranslate();

protected:
    void changeEvent(QEvent *event) override;

private:
    QLabel *m_welcomeLabel;
};

#endif // ADMINDASHBOARD_H
