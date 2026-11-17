#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTranslator>

class Login;
class AdminDashboard;
class UserDashboard;
class GuestDashboard;

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
    void onLoginSuccessful(const QString &role);
    void onLogoutRequested();
    void onLanguageSelected(const QString &locale);

private:
    enum Page { LoginPage, AdminPage, UserPage, GuestPage };

    Ui::MainWindow *ui;
    QStackedWidget *stack;
    Login *loginPage;
    AdminDashboard *adminPage;
    UserDashboard *userPage;
    GuestDashboard *guestPage;
    QTranslator *translator;

    void showPage(Page page);
    void applyLanguage(const QString &locale);
    void selectLanguageAction(const QString &locale);
    static QString supportedLocale(const QString &locale);
};

#endif
