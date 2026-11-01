#include "firebaseauthmanager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>

FirebaseAuthManager::FirebaseAuthManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

void FirebaseAuthManager::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void FirebaseAuthManager::setProjectId(const QString &projectId)
{
    m_projectId = projectId;
}

void FirebaseAuthManager::signIn(const QString &email, const QString &password)
{
    if (m_apiKey.isEmpty()) {
        emit signInFailed("Firebase API key not configured");
        return;
    }

    QNetworkRequest request(QUrl(QString("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=%1").arg(m_apiKey)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;
    json["returnSecureToken"] = true;

    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, &FirebaseAuthManager::onSignInReplyFinished);
}

void FirebaseAuthManager::onSignInReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        QString localId = obj["localId"].toString();
        QString idToken = obj["idToken"].toString();

        // Fetch user role from Firestore/Realtime Database
        if (!m_projectId.isEmpty()) {
            QNetworkRequest roleRequest(QUrl(QString("https://firestore.googleapis.com/v1/projects/%1/databases/(default)/documents/users/%2").arg(m_projectId).arg(localId)));
            roleRequest.setRawHeader("Authorization", QString("Bearer %1").arg(idToken).toUtf8());
            QNetworkReply *roleReply = m_networkManager->get(roleRequest);
            connect(roleReply, &QNetworkReply::finished, this, [this, localId, idToken, roleReply]() {
                if (roleReply->error() == QNetworkReply::NoError) {
                    QByteArray roleData = roleReply->readAll();
                    QJsonDocument roleDoc = QJsonDocument::fromJson(roleData);
                    QJsonObject roleObj = roleDoc.object();
                    QJsonObject fields = roleObj["fields"].toObject();
                    QString role = "user";
                    if (fields.contains("role") && fields["role"].isObject()) {
                        QJsonObject roleField = fields["role"].toObject();
                        if (roleField.contains("stringValue")) {
                            role = roleField["stringValue"].toString();
                        }
                    }
                    if (role.isEmpty()) role = "user";
                    emit signInSuccess(localId, idToken, role);
                } else {
                    emit signInSuccess(localId, idToken, "user");
                }
                roleReply->deleteLater();
            });
        } else {
            emit signInSuccess(localId, idToken, "user");
        }
    } else {
        QByteArray errorData = reply->readAll();
        QJsonDocument errorDoc = QJsonDocument::fromJson(errorData);
        QString errorMsg = reply->errorString();
        if (errorDoc.isObject()) {
            QJsonObject errObj = errorDoc.object();
            if (errObj.contains("error") && errObj["error"].isObject()) {
                QJsonObject errorDetail = errObj["error"].toObject();
                if (errorDetail.contains("message")) {
                    errorMsg = errorDetail["message"].toString();
                }
            }
        }
        emit signInFailed(errorMsg);
    }
    reply->deleteLater();
}
