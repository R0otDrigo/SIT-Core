#ifndef FIREBASEAUTHMANAGER_H
#define FIREBASEAUTHMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class FirebaseAuthManager : public QObject
{
    Q_OBJECT
public:
    explicit FirebaseAuthManager(QObject *parent = nullptr);

    void signIn(const QString &email, const QString &password);
    void setApiKey(const QString &apiKey);
    void setProjectId(const QString &projectId);

signals:
    void signInSuccess(const QString &localId, const QString &idToken, const QString &role);
    void signInFailed(const QString &error);

private slots:
    void onSignInReplyFinished();

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_projectId;
};

#endif // FIREBASEAUTHMANAGER_H
