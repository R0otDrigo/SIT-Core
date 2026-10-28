#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/TecnologiaObjetos/main.qml")));
  return a.exec();
}
