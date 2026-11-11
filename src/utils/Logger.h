#pragma once

#include <QString>

namespace teoto {

/// Instala un manejador de mensajes de Qt que escribe en \a filePath.
/// Devuelve false si no se pudo abrir el archivo.
bool installFileLogger(const QString &filePath);

} // namespace teoto
