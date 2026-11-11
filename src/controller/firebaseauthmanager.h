#ifndef FIREBASEAUTHMANAGER_H
#define FIREBASEAUTHMANAGER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class FirebaseAuthManager : public QObject {
  Q_OBJECT
public:
  static FirebaseAuthManager &instance();
  explicit FirebaseAuthManager(QObject *parent = nullptr);

  FirebaseAuthManager(const FirebaseAuthManager &) = delete;
  FirebaseAuthManager &operator=(const FirebaseAuthManager &) = delete;

  void signUp(const QString &email, const QString &password,
              const QString &role, const QString &username);
  void signIn(const QString &email, const QString &password);
  void setApiKey(const QString &apiKey);
  void setProjectId(const QString &projectId);
  void createFirestoreUser();
  void clearSession();

  QString apiKey() const { return m_apiKey; }
  QString projectId() const { return m_projectId; }
  QString idToken() const { return m_idToken; }
  QString uid() const { return m_uid; }
  QString role() const { return m_user_role; }
  QString username() const { return m_user_username; }
  bool isAuthenticated() const { return !m_idToken.isEmpty(); }

signals:
  void signUpSucceeded(const QString &uid);
  void signUpFailed(const QString &error);
  void signInSuccess(const QString &localId, const QString &idToken,
                     const QString &role);
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
  QString m_user_username;
  QString m_idToken;
  QString m_uid;
};

#endif // FIREBASEAUTHMANAGER_H
