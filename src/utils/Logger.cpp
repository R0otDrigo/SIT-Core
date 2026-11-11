#include "utils/Logger.h"

#include <cstdio>

#include <QDateTime>
#include <QFile>
#include <QMessageLogContext>
#include <QMutex>

namespace {

QMutex g_mutex;
QFile g_logFile;

void fileMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QMutexLocker locker(&g_mutex);
    if (!g_logFile.isOpen())
        return;

    const char *level = "INFO";
    switch (type) {
    case QtDebugMsg:
        level = "DEBUG";
        break;
    case QtInfoMsg:
        level = "INFO";
        break;
    case QtWarningMsg:
        level = "WARN";
        break;
    case QtCriticalMsg:
        level = "CRIT";
        break;
    case QtFatalMsg:
        level = "FATAL";
        break;
    }

    const QString line = QStringLiteral("%1 [%2] %3%4\n")
                             .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs),
                                  QString::fromLatin1(level),
                                  message,
                                  context.file ? QStringLiteral(" (%1:%2)")
                                                     .arg(QString::fromUtf8(context.file))
                                                     .arg(context.line)
                                               : QString());

    g_logFile.write(line.toUtf8());
    g_logFile.flush();
}

} // namespace

namespace teoto {

bool installFileLogger(const QString &filePath)
{
    g_logFile.setFileName(filePath);
    if (!g_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    qInstallMessageHandler(fileMessageHandler);
    return true;
}

} // namespace teoto
