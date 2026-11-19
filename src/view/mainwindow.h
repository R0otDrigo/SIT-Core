#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "view/UserInfoDialog.h"

class QStackedWidget;
class QTranslator;

class Login;
class AdminDashboard;
class UserDashboard;
class GuestDashboard;
class QEvent;
class UserBroadcast;
class UnitRegistry;
class CallSignaling;
class MeetingDialog;
struct CallInfo;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Ventana principal: menu de idioma/sesion y un QStackedWidget con una pagina por rol.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setApiConfig(const QString &apiKey, const QString &projectId);

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onLoginSuccessful(const QString &userId, const QString &idToken, const QString &role);
    void onIncomingCall(const CallInfo &info);
    void onCallAnswered(const QString &vehicleId);
    void onAdminCallRequested(const QString &vehicleId);
    void openMeeting(const QString &peerName);
    void onLogoutRequested();
    void onLanguageSelected(const QString &locale);
    void onMoreInfoRequested();
    void onCallRequested();
    void onVehicleStatusRequested();

private:
    // Orden de las paginas dentro del QStackedWidget.
    enum Page { LoginPage, AdminPage, UserPage, GuestPage };

    Ui::MainWindow *ui;
    QStackedWidget *stack;
    Login *loginPage;
    AdminDashboard *adminPage;
    UserDashboard *userPage;
    GuestDashboard *guestPage;
    QTranslator *translator;
    UserBroadcast *broadcast;
    UnitRegistry *unitRegistry;
    CallSignaling *callSignaling{};
    MeetingDialog *meetingDialog{};
    QString currentUserId;
    QString currentVehicleId;

    void showPage(Page page);
    void showUserDialog(UserInfoDialog::Mode mode);
    void setUserActionsEnabled(bool enabled);
    void applyLanguage(const QString &locale);
    void selectLanguageAction(const QString &locale);
    static QString supportedLocale(const QString &locale);
    static bool isAdminRole(const QString &role);
    static bool isGuestRole(const QString &role);
};

#endif
