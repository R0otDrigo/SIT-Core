#pragma once
#include <QObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QString>
#include "roles.h"

// Resultado de una autenticación (Firebase real o Modo Demo).
struct AuthResult {
    QString email;
    QString displayName;
    QString idToken;
    QString localId;
    Role    role = Role::User;
    bool    demo = false;
};

// Usuario de prueba del Modo Demo / Mock Auth.
struct DemoAccount {
    QString email;
    QString displayName;
    Role    role;
    QString description;
};

class FirebaseAuthManager : public QObject
{
    Q_OBJECT
public:
    explicit FirebaseAuthManager(QObject *parent = nullptr);

    // API Key de Firebase. Por defecto se lee de la variable de entorno
    // SIT_FIREBASE_API_KEY o de QSettings("SIT", "SIT-Core") -> firebase/apiKey.
    void setApiKey(const QString &key);
    QString apiKey() const { return m_apiKey; }

    // Inicia sesión. Si el correo es uno de los usuarios demo, se valida
    // localmente (cualquier contraseña). Si no, se hace la petición REST a Firebase.
    void signIn(const QString &email, const QString &password);

    static QList<DemoAccount> demoAccounts();
    static bool findDemoAccount(const QString &email, DemoAccount *out = nullptr);

signals:
    void authenticated(const AuthResult &result);
    void authFailed(const QString &message);

private:
    void signInWithFirebase(const QString &email, const QString &password);
    static Role roleFromIdToken(const QString &idToken);
    static QString translateFirebaseError(const QString &code);

    QNetworkAccessManager m_nam;
    QString m_apiKey;
};
