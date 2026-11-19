#include "mainwindow.h"
#include "admindashboard.h"
#include "controller/CallSignaling.h"
#include "controller/UnitRegistry.h"
#include "controller/firebaseauthmanager.h"
#include "guestdashboard.h"
#include "login.h"
#include "ui_mainwindow.h"
#include "userdashboard.h"
#include "view/MeetingDialog.h"
#include "view/SimulationWidget.h"
#include "view/UserBridge.h"
#include "view/UserBroadcast.h"
#include "view/UserWidget.h"
#include <QAbstractButton>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>
#include <QStackedWidget>
#include <QTranslator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      stack(new QStackedWidget(this)), loginPage(new Login(this)),
      adminPage(new AdminDashboard(this)), userPage(new UserDashboard(this)),
      guestPage(new GuestDashboard(this)), translator(new QTranslator(this)),
      broadcast(new UserBroadcast(this)), unitRegistry(&UnitRegistry::instance()) {
  ui->setupUi(this);
  adminPage->setBroadcast(broadcast);
  userPage->setBroadcast(broadcast);
  adminPage->setUnitRegistry(&UnitRegistry::instance());
  userPage->setUnitRegistry(&UnitRegistry::instance());

  // Mismo orden que el enum Page.
  stack->addWidget(loginPage);
  stack->addWidget(adminPage);
  stack->addWidget(userPage);
  stack->addWidget(guestPage);

  setCentralWidget(stack);
  showPage(LoginPage);

  // El menu "Sesion" solo aparece tras iniciar sesion
  ui->menuLogout->menuAction()->setVisible(false);
  connect(ui->actionLogout, &QAction::triggered, this, &MainWindow::onLogoutRequested);
  connect(ui->actionMoreInfo, &QAction::triggered, this, &MainWindow::onMoreInfoRequested);
  connect(ui->actionCall, &QAction::triggered, this, &MainWindow::onCallRequested);
  connect(ui->actionVehicleStatus, &QAction::triggered, this,
          &MainWindow::onVehicleStatusRequested);
  connect(loginPage, &Login::loginSuccessful, this, &MainWindow::onLoginSuccessful);
  // Una sola vez: conectarlas en cada login duplicaba los slots tras logout/login.
  connect(adminPage, &AdminDashboard::callAnswered, this, &MainWindow::onCallAnswered);
  connect(adminPage, &AdminDashboard::callRequested, this,
          &MainWindow::onAdminCallRequested);

  // Idiomas: grupo exclusivo, una accion por locale
  auto *languageGroup = new QActionGroup(this);
  const QList<QPair<QAction *, QString>> languages = {
      {ui->actionEspa_ol, "es_ES"},
      {ui->actionIngles, "en_US"},
      {ui->actionChino, "zh_CN"},
      {ui->actionArabe, "ar_SA"}};
  for (const auto &language : languages) {
    const QString locale = language.second;
    languageGroup->addAction(language.first);
    connect(language.first, &QAction::triggered, this,
            [this, locale] { onLanguageSelected(locale); });
  }

  // Restaurar el idioma guardado o usar el del sistema
  QSettings settings("TecnologiaObjetos", "appTecnologiaObjetos");
  const QString savedLanguage = settings.value("language").toString();
  applyLanguage(savedLanguage.isEmpty() ? QLocale::system().name()
                                        : savedLanguage);
}

void MainWindow::setApiConfig(const QString &api_key,
                              const QString &project_id) {
  FirebaseAuthManager::instance().setApiKey(api_key);
  FirebaseAuthManager::instance().setProjectId(project_id);
  UnitRegistry::instance().configure(project_id, QString());
  loginPage->setFirebaseConfig(api_key, project_id);
}

MainWindow::~MainWindow() { delete ui; }


void MainWindow::onLanguageSelected(const QString &locale) {
  applyLanguage(locale);
  QSettings().setValue("language", supportedLocale(locale));
}

void MainWindow::applyLanguage(const QString &locale) {
  const QString normalized = supportedLocale(locale);

  qApp->removeTranslator(translator);
  if (translator->load(":/i18n/TecnologiaObjetos_" + normalized + ".qm"))
    qApp->installTranslator(translator);

  qApp->setLayoutDirection(normalized == "ar_SA" ? Qt::RightToLeft : Qt::LeftToRight);
  selectLanguageAction(normalized);
}

void MainWindow::selectLanguageAction(const QString &locale) {
  if (locale == "en_US")
    ui->actionIngles->setChecked(true);
  else if (locale == "zh_CN")
    ui->actionChino->setChecked(true);
  else if (locale == "ar_SA")
    ui->actionArabe->setChecked(true);
  else
    ui->actionEspa_ol->setChecked(true);
}

QString MainWindow::supportedLocale(const QString &locale) {
  const QString language = locale.toLower();
  if (language.startsWith("en"))
    return "en_US";
  if (language.startsWith("zh"))
    return "zh_CN";
  if (language.startsWith("ar"))
    return "ar_SA";
  return "es_ES";
}

void MainWindow::onLoginSuccessful(const QString &userId,
                                   const QString &idToken,
                                   const QString &role) {
  // El mapa general solo se crea para administradores. Los demas roles nunca
  // reciben MapBridge: el rol "user" (o cualquier rol por defecto) ve
  // unicamente user.qml.
  const QString r = role.trimmed().toUpper();
  const bool userFeatures = !isAdminRole(r) && !isGuestRole(r);
  currentUserId = userId;
  currentVehicleId.clear();

  if (callSignaling) {
    callSignaling->stop();
    delete callSignaling;
  }
  const auto signalingRole =
      isAdminRole(r) ? CallSignaling::Role::Admin : CallSignaling::Role::User;
  callSignaling = new CallSignaling(signalingRole, this);
  callSignaling->start();
  connect(callSignaling, &CallSignaling::messageReceived, this,
          &MainWindow::onIncomingCall);

  UnitRegistry::instance().configure(UnitRegistry::instance().projectId(), idToken);

  if (isAdminRole(r)) {
    adminPage->initialize(idToken);
    showPage(AdminPage);
  } else if (isGuestRole(r)) {
    showPage(GuestPage);
  } else {
    userPage->initializeSession(userId, idToken);
    if (UserWidget *widget = userPage->userWidget()) {
      currentVehicleId = widget->bridge()->vehicleId();
      connect(widget->bridge(), &UserBridge::vehicleChanged, this, [this] {
        if (UserWidget *active = userPage->userWidget())
          currentVehicleId = active->bridge()->vehicleId();
      });
    }
    showPage(UserPage);
  }
  setUserActionsEnabled(userFeatures);

  ui->menuLogout->menuAction()->setVisible(true);
}

void MainWindow::onIncomingCall(const CallInfo &info) {
  if (stack->currentIndex() == AdminPage) {
    adminPage->showIncomingCall(info.vehicleId, info.displayName, info.status);
    return;
  }

  QMessageBox prompt(this);
  prompt.setIcon(QMessageBox::Information);
  prompt.setWindowTitle(tr("Llamada entrante"));
  prompt.setText(
      tr("El administrador solicita una reunión con cámara y audio."));
  prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  prompt.button(QMessageBox::Yes)->setText(tr("Atender Llamada"));
  prompt.button(QMessageBox::No)->setText(tr("Rechazar"));
  if (prompt.exec() == QMessageBox::Yes)
    openMeeting(tr("Administrador"));
}

void MainWindow::onCallAnswered(const QString &vehicleId) {
  unitRegistry->setStatus(vehicleId, tr("Llamada atendida"));
  if (adminPage->simulation())
    adminPage->simulation()->showIncomingCall(QString(), QString());
  openMeeting(tr("Usuario"));
}

void MainWindow::onAdminCallRequested(const QString &vehicleId) {
  if (callSignaling)
    callSignaling->sendCall(currentUserId, vehicleId, tr("Administrador"),
                            tr("Llamada del administrador"));
  unitRegistry->setStatus(vehicleId, tr("Llamada del administrador"));
  openMeeting(tr("Usuario"));
}

void MainWindow::openMeeting(const QString &peerName) {
  if (meetingDialog)
    meetingDialog->close();
  meetingDialog = new MeetingDialog(peerName, this);
  meetingDialog->setAttribute(Qt::WA_DeleteOnClose);
  connect(meetingDialog, &QDialog::destroyed, this,
          [this] { meetingDialog = nullptr; });
  meetingDialog->show();
}

bool MainWindow::isAdminRole(const QString &role) {
  return role == "ADMIN" || role == "ADMINISTRATOR";
}

bool MainWindow::isGuestRole(const QString &role) { return role == "GUESS"; }

void MainWindow::onLogoutRequested() {
  // Descartar la sesion anterior para que otro usuario no herede su bus
  // asignado.
  adminPage->resetSession();
  userPage->resetSession();
  broadcast->clear();
  FirebaseAuthManager::instance().clearSession();
  UnitRegistry::instance().clearSession();
  if (callSignaling) {
    callSignaling->stop();
    delete callSignaling;
    callSignaling = nullptr;
  }
  if (meetingDialog) {
    meetingDialog->close();
    meetingDialog = nullptr;
  }
  currentUserId.clear();
  currentVehicleId.clear();
  setUserActionsEnabled(false);
  showPage(LoginPage);
  ui->menuLogout->menuAction()->setVisible(false);
}

void MainWindow::setUserActionsEnabled(bool enabled) {
  ui->actionMoreInfo->setEnabled(enabled);
  ui->actionCall->setEnabled(enabled);
  ui->actionVehicleStatus->setEnabled(enabled);
}

void MainWindow::onMoreInfoRequested() {
  showUserDialog(UserInfoDialog::Mode::MoreInfo);
}

void MainWindow::onCallRequested() {
  if (!callSignaling || stack->currentIndex() != UserPage)
    return;
  callSignaling->sendCall(currentUserId, currentVehicleId, tr("Usuario"),
                          tr("Llamada entrante"));
  if (!currentVehicleId.isEmpty())
    unitRegistry->setStatus(currentVehicleId, tr("Llamada entrante"));
  openMeeting(tr("Administrador"));
}

void MainWindow::onVehicleStatusRequested() {
  showUserDialog(UserInfoDialog::Mode::VehicleStatus);
}

void MainWindow::showUserDialog(UserInfoDialog::Mode mode) {
  UserWidget *widget = userPage->userWidget();
  if (!widget)
    return;
  UserInfoDialog dialog(mode, widget->bridge(), this);
  dialog.exec();
}

void MainWindow::showPage(Page page) { stack->setCurrentIndex(page); }

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
  QMainWindow::changeEvent(event);
}
