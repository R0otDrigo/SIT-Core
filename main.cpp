#include <QApplication>
#include "logindialog.h"
#include "admindashboard.h"
#include "userdashboard.h"
#include "guestdashboard.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    LoginDialog loginDialog;
    // Configure Firebase - replace with your actual values
    loginDialog.setFirebaseConfig("YOUR_FIREBASE_API_KEY", "YOUR_FIREBASE_PROJECT_ID");
    
    QObject::connect(&loginDialog, &LoginDialog::loginSuccessful, [&](const QString &role) {
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
    
    if (loginDialog.exec() == QDialog::Accepted) {
        return a.exec();
    }
    
    return 0;
}
