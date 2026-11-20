#pragma once

class QApplication;

// Aplica el tema visual global (Fusion + hoja de estilos). Se usa en main() y
// tambien en las pruebas graficas para que las capturas reflejen la app real.
void applyAppTheme(QApplication &app);
