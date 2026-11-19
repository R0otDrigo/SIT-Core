#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTranslator>

class Login;
class AdminDashboard;
class UserDashboard;
class GuestDashboard;
class QEvent;
class UserBroadcast;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    void setApiConfig(const QString &api, const QString &id);
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onLoginSuccessful(const QString &userId, const QString &role);
    void onLogoutRequested();
    void onLanguageSelected(const QString &locale);
    void onMoreInfoRequested();
    void onCallRequested();
    void onVehicleStatusRequested();

private:
    Ui::MainWindow *ui;
    QStackedWidget *stack;
    Login *loginPage;
    AdminDashboard *adminPage;
    UserDashboard *userPage;
    GuestDashboard *guestPage;
    QTranslator *translator;
    UserBroadcast *broadcast;

    void showPage(int index);
    void showUserDialog(const QString &mode);
    void setUserActionsEnabled(bool enabled);
    void applyLanguage(const QString &locale);
    void selectLanguageAction(const QString &locale);
    static QString supportedLocale(const QString &locale);
    static bool isAdminRole(const QString &role);
    static bool isGuestRole(const QString &role);
};

#endif
