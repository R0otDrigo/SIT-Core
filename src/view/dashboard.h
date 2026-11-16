#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

class QEvent;

namespace Ui {
class Dashboard;
}

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard() override;

    void retranslate();

protected:
    void changeEvent(QEvent *event) override;

    virtual QString dashboardTitle() const;
    virtual QString welcomeMessage() const;

private:
    Ui::Dashboard *ui;
};

#endif // DASHBOARD_H
