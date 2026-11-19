#include "view/UserBroadcast.h"

UserBroadcast::UserBroadcast(QObject* parent) : QObject(parent) {}

void UserBroadcast::setAntecedentes(const QString& text)
{
    if (antecedentes_ == text) return;
    antecedentes_ = text;
    emit antecedentesChanged();
}

void UserBroadcast::setEvento(const QString& text)
{
    if (evento_ == text) return;
    evento_ = text;
    emit eventoChanged();
}

void UserBroadcast::clear()
{
    setAntecedentes(QString());
    setEvento(QString());
}
