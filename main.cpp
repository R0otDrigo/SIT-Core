#include <QQmlApplicationEngine>
#include <QApplication>
#include "login.h"
#include "admindashboard.h"
#include "userdashboard.h"
#include "guestdashboard.h"
#include <QSettings>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Login loginDialog;
    // Configure Firebase - replace with your actual values
    const QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    QString API_KEY = settings.value("FIREBASE_API_KEY").toString();

    QString PROJECT_ID = settings.value("FIREBASE_PROJECT_ID").toString();
    loginDialog.setFirebaseConfig(API_KEY, PROJECT_ID);
    
    QObject::connect(&loginDialog, &Login::loginSuccessful, [&](const QString &role) {
        loginDialog.hide();
        if (role.toLower() == "admin" || role.toLower() == "administrator") {
            AdminDashboard *adminDash = new AdminDashboard();
            adminDash->show();
        } else if (role.toLower() == "user") {
            UserDashboard *userDash = new UserDashboard();
            userDash->show();
        } else if (role.toLower() == "guest") {
            GuestDashboard *guestDash = new GuestDashboard();
            guestDash->show();
        } else {
            // Default to user dashboard
            UserDashboard *userDash = new UserDashboard();
            userDash->show();
        }
    });

    loginDialog.show();
    return a.exec();
}
