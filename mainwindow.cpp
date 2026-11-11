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

  // Agregar todas las páginas al stack
  stack->addWidget(loginPage); // índice 0
  stack->addWidget(adminPage); // índice 1
  stack->addWidget(userPage);  // índice 2
  stack->addWidget(guestPage); // índice 3

  setCentralWidget(stack);
  showPage(0); // Iniciar en Login

  // Conectar señales de las páginas
  connect(loginPage, &Login::loginSuccessful, this,
          &MainWindow::onLoginSuccessful);
  // connect(adminPage, &AdminDashboard::logoutRequested, this, &MainWindow::onLogoutRequested);
  // connect(userPage, &UserDashboard::logoutRequested, this, &MainWindow::onLogoutRequested);
  // connect(guestPage, &GuestDashboard::logoutRequested, this,&MainWindow::onLogoutRequested);

  // Agrupar las acciones de idioma para que solo una esté seleccionada
  QActionGroup *languageGroup = new QActionGroup(this);
  languageGroup->setExclusive(true);
  const QList<QAction *> languageActions = {
      ui->actionEspa_ol, ui->actionIngles, ui->actionChino,
      ui->actionArabe};
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

void MainWindow::onLoginSuccessful(const QString &role) {
  QString r = role.toLower();
  if (r == "admin" || r == "administrator")
    showPage(1);
  else if (r == "guest")
    showPage(3);
  else
    showPage(2);
}

void MainWindow::onLogoutRequested() {
  showPage(0);
  loginPage->reset(); // limpiar campos si es necesario
}

void MainWindow::showPage(int index) { stack->setCurrentIndex(index); }

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
  QMainWindow::changeEvent(event);
}
