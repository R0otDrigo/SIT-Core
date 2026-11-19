#include "mainwindow.h"
#include "admindashboard.h"
#include "guestdashboard.h"
#include "login.h"
#include "ui_mainwindow.h"
#include "userdashboard.h"
#include "controller/CallSignaling.h"
#include "controller/UnitRegistry.h"
#include "view/MeetingDialog.h"
#include "view/SimulationWidget.h"
#include "view/UserBroadcast.h"
#include "view/UserInfoDialog.h"
#include "view/UserBridge.h"
#include "view/UserWidget.h"
#include <QAbstractButton>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>
#include <QDebug>

namespace {
constexpr int kLoginPage = 0;
constexpr int kAdminPage = 1;
constexpr int kUserPage = 2;
constexpr int kGuestPage = 3;
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      stack(new QStackedWidget(this)), loginPage(new Login(this)),
      adminPage(new AdminDashboard(this)), userPage(new UserDashboard(this)),
      guestPage(new GuestDashboard(this)), translator(new QTranslator(this)),
      broadcast(new UserBroadcast(this)), unitRegistry(new UnitRegistry(this)) {
  ui->setupUi(this);
  adminPage->setBroadcast(broadcast);
  userPage->setBroadcast(broadcast);
  adminPage->setUnitRegistry(unitRegistry);
  userPage->setUnitRegistry(unitRegistry);

  // Agregar todas las páginas al stack
  stack->addWidget(loginPage); // índice 0
  stack->addWidget(adminPage); // índice 1
  stack->addWidget(userPage);  // índice 2
  stack->addWidget(guestPage); // índice 3

  setCentralWidget(stack);
  showPage(kLoginPage); // Iniciar en Login

  // La acción "User" (Logout) solo está disponible tras iniciar sesión
  ui->menuLogout->menuAction()->setVisible(false);
  connect(ui->actionLogout, &QAction::triggered, this,
          &MainWindow::onLogoutRequested);

  // Conectar señales de las páginas
  connect(loginPage, &Login::loginSuccessful, this,
          &MainWindow::onLoginSuccessful);
  connect(ui->actionMoreInfo, &QAction::triggered, this,
          &MainWindow::onMoreInfoRequested);
  connect(ui->actionCall, &QAction::triggered, this,
          &MainWindow::onCallRequested);
  connect(ui->actionVehicleStatus, &QAction::triggered, this,
          &MainWindow::onVehicleStatusRequested);
  // connect(adminPage, &AdminDashboard::logoutRequested, this,
  // &MainWindow::onLogoutRequested); connect(userPage,
  // &UserDashboard::logoutRequested, this, &MainWindow::onLogoutRequested);
  // connect(guestPage, &GuestDashboard::logoutRequested,
  // this,&MainWindow::onLogoutRequested);

  // Agrupar las acciones de idioma para que solo una esté seleccionada
  QActionGroup *languageGroup = new QActionGroup(this);
  languageGroup->setExclusive(true);
  const QList<QAction *> languageActions = {ui->actionEspa_ol, ui->actionIngles,
                                            ui->actionChino, ui->actionArabe};
  for (QAction *action : languageActions) {
    action->setCheckable(true);
    languageGroup->addAction(action);
  }

  connect(ui->actionEspa_ol, &QAction::triggered, this,
          [this] { onLanguageSelected("es_ES"); });
  connect(ui->actionIngles, &QAction::triggered, this,
          [this] { onLanguageSelected("en_US"); });
  connect(ui->actionChino, &QAction::triggered, this,
          [this] { onLanguageSelected("zh_CN"); });
  connect(ui->actionArabe, &QAction::triggered, this,
          [this] { onLanguageSelected("ar_SA"); });

  // Restaurar el idioma guardado o usar el del sistema
  QSettings settings("TecnologiaObjetos", "appTecnologiaObjetos");
  const QString savedLanguage = settings.value("language").toString();
  applyLanguage(savedLanguage.isEmpty() ? QLocale::system().name()
                                        : savedLanguage);
}

void MainWindow::setApiConfig(const QString &api_key,
                              const QString &project_id) {

  loginPage->setFirebaseConfig(api_key, project_id);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onLanguageSelected(const QString &locale) {
  applyLanguage(locale);

  QSettings settings("TecnologiaObjetos", "appTecnologiaObjetos");
  settings.setValue("language", supportedLocale(locale));
}

void MainWindow::applyLanguage(const QString &locale) {
  const QString normalized = supportedLocale(locale);

  qApp->removeTranslator(translator);
  if (translator->load(":/i18n/TecnologiaObjetos_" + normalized + ".qm"))
    qApp->installTranslator(translator);

  qApp->setLayoutDirection(normalized == "ar_SA" ? Qt::RightToLeft
                                                 : Qt::LeftToRight);
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

void MainWindow::onLoginSuccessful(const QString &userId, const QString &idToken,
                                   const QString &role) {
  // El mapa general solo se crea para administradores. Los demas roles nunca
  // reciben MapBridge: el rol "user" (o cualquier rol por defecto) ve
  // unicamente user.qml.
  const QString r = role.trimmed().toUpper();
  const bool userFeatures = !isAdminRole(r) && !isGuestRole(r);
  currentUserId = userId;
  currentVehicleId.clear();
  currentUserName = r;

  if (callSignaling) {
    callSignaling->stop();
    delete callSignaling;
  }
  const auto signalingRole = isAdminRole(r) ? CallSignaling::Role::Admin
                                            : CallSignaling::Role::User;
  callSignaling = new CallSignaling(signalingRole, this);
  callSignaling->start();
  connect(callSignaling, &CallSignaling::messageReceived, this, &MainWindow::onIncomingCall);

  if (isAdminRole(r)) {
    adminPage->initialize(idToken);
    connect(adminPage, &AdminDashboard::callAnswered, this, &MainWindow::onCallAnswered);
    connect(adminPage, &AdminDashboard::callRequested, this, &MainWindow::onAdminCallRequested);
    showPage(kAdminPage);
  } else if (isGuestRole(r)) {
    showPage(kGuestPage);
  } else {
    userPage->initializeSession(userId, idToken);
    if (UserWidget *widget = userPage->userWidget()) {
      currentVehicleId = widget->bridge()->vehicleId();
      connect(widget->bridge(), &UserBridge::vehicleChanged, this, [this] {
        if (UserWidget *active = userPage->userWidget())
          currentVehicleId = active->bridge()->vehicleId();
      });
    }
    showPage(kUserPage);
  }
  setUserActionsEnabled(userFeatures);

  // Mostrar el menú "User" con la acción de Logout una vez autenticado
  ui->menuLogout->menuAction()->setVisible(true);
}

void MainWindow::onIncomingCall(const CallInfo &info) {
  if (stack->currentIndex() == kAdminPage) {
    adminPage->showIncomingCall(info.vehicleId, info.displayName, info.status);
    return;
  }

  QMessageBox prompt(this);
  prompt.setIcon(QMessageBox::Information);
  prompt.setWindowTitle(tr("Llamada entrante"));
  prompt.setText(tr("El administrador solicita una reunión con cámara y audio."));
  prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  prompt.button(QMessageBox::Yes)->setText(tr("Atender Llamada"));
  prompt.button(QMessageBox::No)->setText(tr("Rechazar"));
  if (prompt.exec() == QMessageBox::Yes) openMeeting(tr("Administrador"));
}

void MainWindow::onCallAnswered(const QString &vehicleId) {
  if (unitRegistry) unitRegistry->setStatus(vehicleId, tr("Llamada atendida"));
  if (adminPage->simulation()) adminPage->simulation()->showIncomingCall(QString(), QString());
  openMeeting(tr("Usuario"));
}

void MainWindow::onAdminCallRequested(const QString &vehicleId) {
  if (callSignaling)
    callSignaling->sendCall(currentUserId, vehicleId, tr("Administrador"), tr("Llamada del administrador"));
  if (unitRegistry) unitRegistry->setStatus(vehicleId, tr("Llamada del administrador"));
  openMeeting(tr("Usuario"));
}

void MainWindow::openMeeting(const QString &peerName) {
  if (meetingDialog) meetingDialog->close();
  meetingDialog = new MeetingDialog(peerName, this);
  meetingDialog->setAttribute(Qt::WA_DeleteOnClose);
  connect(meetingDialog, &QDialog::destroyed, this, [this] { meetingDialog = nullptr; });
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
  showPage(kLoginPage);
  ui->menuLogout->menuAction()->setVisible(false);
  loginPage->reset(); // limpiar campos si es necesario
}

void MainWindow::setUserActionsEnabled(bool enabled) {
  ui->actionMoreInfo->setEnabled(enabled);
  ui->actionCall->setEnabled(enabled);
  ui->actionVehicleStatus->setEnabled(enabled);
}

void MainWindow::onMoreInfoRequested() {
  showUserDialog(QStringLiteral("info"));
}

void MainWindow::onCallRequested() {
  if (!callSignaling || stack->currentIndex() != kUserPage) return;
  callSignaling->sendCall(currentUserId, currentVehicleId, tr("Usuario"), tr("Llamada entrante"));
  if (unitRegistry && !currentVehicleId.isEmpty())
    unitRegistry->setStatus(currentVehicleId, tr("Llamada entrante"));
  openMeeting(tr("Administrador"));
}

void MainWindow::onVehicleStatusRequested() {
  showUserDialog(QStringLiteral("vehicle"));
}

void MainWindow::showUserDialog(const QString &mode) {
  UserWidget *widget = userPage->userWidget();
  if (!widget)
    return;

  UserInfoDialog::Mode dialogMode = UserInfoDialog::Mode::MoreInfo;
  if (mode == QStringLiteral("call"))
    dialogMode = UserInfoDialog::Mode::Call;
  else if (mode == QStringLiteral("vehicle"))
    dialogMode = UserInfoDialog::Mode::VehicleStatus;

  UserInfoDialog dialog(dialogMode, widget->bridge(), this);
  dialog.exec();
}

void MainWindow::showPage(int index) { stack->setCurrentIndex(index); }

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
  QMainWindow::changeEvent(event);
}
