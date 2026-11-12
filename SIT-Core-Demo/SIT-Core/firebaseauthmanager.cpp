#include "firebaseauthmanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QSettings>
#include <QTimer>
#include <QUrl>

FirebaseAuthManager::FirebaseAuthManager(QObject *parent)
    : QObject(parent)
{
    QString key = QProcessEnvironment::systemEnvironment().value(QStringLiteral("SIT_FIREBASE_API_KEY"));
    if (key.isEmpty()) {
        QSettings s(QStringLiteral("SIT"), QStringLiteral("SIT-Core"));
        key = s.value(QStringLiteral("firebase/apiKey")).toString();
    }
    m_apiKey = key;
}

void FirebaseAuthManager::setApiKey(const QString &key)
{
    m_apiKey = key.trimmed();
}

QList<DemoAccount> FirebaseAuthManager::demoAccounts()
{
    return {
        { QStringLiteral("manager@sit.com"),       QStringLiteral("Marcela Paredes"), Role::FleetManager,               QStringLiteral("Gerente de Flota") },
        { QStringLiteral("conductor@sit.com"),     QStringLiteral("Carlos Mendoza"),  Role::User,                       QStringLiteral("Conductor") },
        { QStringLiteral("trafico@sit.com"),       QStringLiteral("Lucía Valdivia"),  Role::JefeDeTrafico,              QStringLiteral("Jefe de Tráfico") },
        { QStringLiteral("desplazamiento@sit.com"),QStringLiteral("Raúl Benavente"),  Role::ResponsableDesplazamientos, QStringLiteral("Resp. de Desplazamientos") },
        { QStringLiteral("guest@sit.com"),         QStringLiteral("Invitado"),        Role::Guess,                      QStringLiteral("Invitado / Sandbox") },
    };
}

bool FirebaseAuthManager::findDemoAccount(const QString &email, DemoAccount *out)
{
    const QString e = email.trimmed().toLower();
    const auto accounts = demoAccounts();
    for (const DemoAccount &a : accounts) {
        if (a.email == e) {
            if (out) *out = a;
            return true;
        }
    }
    return false;
}

void FirebaseAuthManager::signIn(const QString &email, const QString &password)
{
    DemoAccount demo;
    if (findDemoAccount(email, &demo)) {
        // Mock Auth: valida de forma local sin depender de Firebase.
        QTimer::singleShot(350, this, [this, demo]() {
            AuthResult r;
            r.email = demo.email;
            r.displayName = demo.displayName;
            r.role = demo.role;
            r.idToken = QStringLiteral("demo-token");
            r.localId = QStringLiteral("demo-") + Roles::id(demo.role);
            r.demo = true;
            emit authenticated(r);
        });
        return;
    }
    signInWithFirebase(email.trimmed(), password);
}

void FirebaseAuthManager::signInWithFirebase(const QString &email, const QString &password)
{
    if (m_apiKey.isEmpty()) {
        QTimer::singleShot(0, this, [this]() {
            emit authFailed(tr("Firebase no está configurado (falta la API Key). "
                               "Usa un usuario del Modo Demo o define SIT_FIREBASE_API_KEY."));
        });
        return;
    }

    QUrl url(QStringLiteral("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword"));
    url.setQuery(QStringLiteral("key=") + m_apiKey);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    req.setTransferTimeout(10000);

    const QJsonObject body{
        { QStringLiteral("email"), email },
        { QStringLiteral("password"), password },
        { QStringLiteral("returnSecureToken"), true },
    };

    QNetworkReply *reply = m_nam.post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        const QByteArray data = reply->readAll();
        const QJsonObject obj = QJsonDocument::fromJson(data).object();

        if (reply->error() != QNetworkReply::NoError) {
            const QString code = obj.value(QStringLiteral("error")).toObject()
                                     .value(QStringLiteral("message")).toString();
            if (code.isEmpty())
                emit authFailed(tr("No se pudo conectar con Firebase: %1").arg(reply->errorString()));
            else
                emit authFailed(translateFirebaseError(code));
            return;
        }

        AuthResult r;
        r.email = obj.value(QStringLiteral("email")).toString();
        r.displayName = obj.value(QStringLiteral("displayName")).toString();
        if (r.displayName.isEmpty())
            r.displayName = r.email.section(QLatin1Char('@'), 0, 0);
        r.idToken = obj.value(QStringLiteral("idToken")).toString();
        r.localId = obj.value(QStringLiteral("localId")).toString();
        r.role = roleFromIdToken(r.idToken);
        r.demo = false;
        emit authenticated(r);
    });
}

// El rol se lee del custom claim "role" del idToken (JWT). Si no existe, "user".
Role FirebaseAuthManager::roleFromIdToken(const QString &idToken)
{
    const QStringList parts = idToken.split(QLatin1Char('.'));
    if (parts.size() < 2)
        return Role::User;
    const QByteArray payload = QByteArray::fromBase64(parts.at(1).toLatin1(),
                                                      QByteArray::Base64UrlEncoding
                                                          | QByteArray::OmitTrailingEquals);
    const QJsonObject claims = QJsonDocument::fromJson(payload).object();
    bool ok = false;
    const Role r = Roles::fromId(claims.value(QStringLiteral("role")).toString(), &ok);
    return ok ? r : Role::User;
}

QString FirebaseAuthManager::translateFirebaseError(const QString &code)
{
    if (code.startsWith(QLatin1String("EMAIL_NOT_FOUND"))
        || code.startsWith(QLatin1String("INVALID_PASSWORD"))
        || code.startsWith(QLatin1String("INVALID_LOGIN_CREDENTIALS")))
        return tr("Correo o contraseña incorrectos.");
    if (code.startsWith(QLatin1String("USER_DISABLED")))
        return tr("Esta cuenta ha sido deshabilitada.");
    if (code.startsWith(QLatin1String("TOO_MANY_ATTEMPTS_TRY_LATER")))
        return tr("Demasiados intentos. Inténtalo más tarde.");
    if (code.startsWith(QLatin1String("INVALID_EMAIL")))
        return tr("El correo electrónico no es válido.");
    if (code.contains(QLatin1String("API key")))
        return tr("La API Key de Firebase no es válida.");
    return tr("Error de autenticación: %1").arg(code);
}
