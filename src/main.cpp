#include <QApplication>
#include <QLabel>
#include <QMainWindow>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QMainWindow w;
    w.setWindowTitle("SIT-Core");
    w.setCentralWidget(new QLabel("SIT-Core: simulador de transporte urbano (Arequipa)"));
    w.resize(900, 600);
    w.show();
    return app.exec();
}
