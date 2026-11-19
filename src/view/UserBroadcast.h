#pragma once
#include <QObject>
#include <QString>

// Datos que el administrador envia al usuario y que se comparten entre ambos dashboards.
class UserBroadcast : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString antecedentes READ antecedentes WRITE setAntecedentes NOTIFY antecedentesChanged)
    Q_PROPERTY(QString evento READ evento WRITE setEvento NOTIFY eventoChanged)

public:
    explicit UserBroadcast(QObject* parent = nullptr);

    QString antecedentes() const { return antecedentes_; }
    QString evento() const { return evento_; }

public slots:
    void setAntecedentes(const QString& text);
    void setEvento(const QString& text);
    void clear();

signals:
    void antecedentesChanged();
    void eventoChanged();

private:
    QString antecedentes_;
    QString evento_;
};
