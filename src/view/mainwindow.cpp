#include "mainwindow.h"
#include "admindashboard.h"
#include "guestdashboard.h"
#include "login.h"
#include "ui_mainwindow.h"
#include "userdashboard.h"
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QLocale>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), stack(new QStackedWidget(this)),
      loginPage(new Login(this)), adminPage(new AdminDashboard(this)),
      userPage(new UserDashboard(this)), guestPage(new GuestDashboard(this)),
      translator(new QTranslator(this)) {
  ui->setupUi(this);

  // El orden debe coincidir con el enum Page de mainwindow.h
  stack->addWidget(loginPage);
  stack->addWidget(adminPage);
  stack->addWidget(userPage);
  stack->addWidget(guestPage);
  setCentralWidget(stack);
  showPage(LoginPage);

  // El menu "User" (Logout) solo aparece tras iniciar sesion
  ui->menuLogout->menuAction()->setVisible(false);
  connect(ui->actionLogout, &QAction::triggered, this, &MainWindow::onLogoutRequested);
  connect(loginPage, &Login::loginSuccessful, this, &MainWindow::onLoginSuccessful);

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
  const QString saved = QSettings().value("language").toString();
  applyLanguage(saved.isEmpty() ? QLocale::system().name() : saved);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setApiConfig(const QString &apiKey, const QString &projectId) {
  loginPage->setFirebaseConfig(apiKey, projectId);
}

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

void MainWindow::onLoginSuccessful(const QString &role) {
  const QString r = role.toLower();
  if (r == "admin" || r == "administrator")
    showPage(AdminPage);
  else if (r == "guest")
    showPage(GuestPage);
  else
    showPage(UserPage);
  ui->menuLogout->menuAction()->setVisible(true);
}

void MainWindow::onLogoutRequested() {
  loginPage->reset();
  showPage(LoginPage);
  ui->menuLogout->menuAction()->setVisible(false);
}

void MainWindow::showPage(Page page) { stack->setCurrentIndex(page); }

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
  QMainWindow::changeEvent(event);
}
