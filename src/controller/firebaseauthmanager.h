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

    void signUp(const QString &email, const QString &password, const QString &role);
    void signIn(const QString &email, const QString &password);
    void setApiKey(const QString &apiKey);
    void setProjectId(const QString &projectId);
    void createFirestoreUser();

signals:
    void signUpSucceeded(const QString &uid);
    void signUpFailed(const QString &error);
    void signInSuccess(const QString &localId, const QString &idToken, const QString &role);
    void signInFailed(const QString &error);

private slots:
    void onSignUpReplyFinished();
    void onSignInReplyFinished();
    void onFirestoreUserCreated();

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_projectId;
    QString m_user_role;
    QString m_idToken;
    QString m_uid;
};

#endif // FIREBASEAUTHMANAGER_H
