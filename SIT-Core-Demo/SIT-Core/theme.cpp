#include "theme.h"

QString Theme::styleSheet()
{
    QString qss = QStringLiteral(R"QSS(
QWidget {
    font-family: "Segoe UI", "Noto Sans", "Ubuntu", "Helvetica Neue", sans-serif;
    font-size: 13px;
    color: @TEXT@;
}
QMainWindow, QStackedWidget { background: @BG@; }
QLabel { background: transparent; }
QToolTip { background: @NAVY@; color: white; border: none; padding: 4px 8px; }

#dashRoot, #pageArea { background: @BG@; }
#pageScroll { background: @BG@; border: none; }
#pageScroll > QWidget { background: @BG@; }
#pageContent { background: @BG@; }

/* ---------- Sidebar y header ---------- */
#sidebar { background: @NAVY@; }
#sidebar QLabel { color: white; }
#logo { font-size: 22px; font-weight: 700; color: white; }
#sidebarSub { color: #BFDBFE; font-size: 11px; }
#sidebarUser { font-weight: 600; color: white; }
#sidebarRole { color: #BFDBFE; font-size: 11px; }
#sidebarSep { background: rgba(255,255,255,0.18); max-height: 1px; min-height: 1px; border: none; }

QPushButton#navButton {
    background: transparent; color: #DBEAFE; text-align: left;
    padding: 11px 16px; border: none; border-radius: 8px; font-weight: 600;
}
QPushButton#navButton:hover { background: rgba(255,255,255,0.10); }
QPushButton#navButton:checked { background: @ACCENT@; color: white; }

#header { background: @NAVY@; }
#headerTitle { color: white; font-size: 18px; font-weight: 700; }
#headerUser { color: #DBEAFE; }
#roleChip {
    background: rgba(255,255,255,0.16); color: white;
    border-radius: 10px; padding: 4px 12px; font-weight: 600;
}

/* ---------- Tarjetas ---------- */
QFrame#card { background: @CARD@; border: 1px solid @BORDER@; border-radius: 12px; }
#h1 { font-size: 22px; font-weight: 700; color: @TEXT@; }
#h2 { font-size: 15px; font-weight: 700; color: @TEXT@; }
#muted { color: @MUTED@; }
#statTitle { color: @MUTED@; font-weight: 600; font-size: 12px; }
#statValue { font-size: 28px; font-weight: 700; }

/* ---------- Botones ---------- */
QPushButton {
    background: @ACCENT@; color: white; border: none; border-radius: 8px;
    padding: 9px 18px; font-weight: 600;
}
QPushButton:hover { background: #2563EB; }
QPushButton:pressed { background: #1D4ED8; }
QPushButton:disabled { background: #9CA3AF; color: #F3F4F6; }
QPushButton#ghost { background: white; color: @ACCENT@; border: 1px solid @ACCENT@; }
QPushButton#ghost:hover { background: #EFF6FF; }
QPushButton#danger { background: @RED@; }
QPushButton#danger:hover { background: #DC2626; }
QPushButton#logout {
    background: rgba(255,255,255,0.12); color: white;
    border: 1px solid rgba(255,255,255,0.30);
}
QPushButton#logout:hover { background: rgba(255,255,255,0.22); }
QPushButton#roleTab {
    background: rgba(255,255,255,0.10); color: #DBEAFE;
    border: 1px solid rgba(255,255,255,0.20); padding: 8px 14px;
}
QPushButton#roleTab:hover { background: rgba(255,255,255,0.20); }
QPushButton#roleTab:checked { background: white; color: @NAVY@; border: 1px solid white; }
QPushButton#demoButton {
    background: white; color: @TEXT@; border: 1px solid @BORDER@;
    text-align: left; padding: 10px 14px; font-weight: 500;
}
QPushButton#demoButton:hover { border: 1px solid @ACCENT@; background: #EFF6FF; }

/* ---------- Entradas ---------- */
QLineEdit, QComboBox, QSpinBox {
    background: white; border: 1px solid #D1D5DB; border-radius: 8px;
    padding: 8px 10px; min-height: 18px;
}
QComboBox::drop-down { border: none; width: 26px; }
QComboBox::down-arrow { width: 10px; height: 10px; }
QLineEdit:focus, QComboBox:focus, QSpinBox:focus { border: 1px solid @ACCENT@; }
QComboBox QAbstractItemView {
    background: white; border: 1px solid @BORDER@;
    selection-background-color: #DBEAFE; selection-color: @TEXT@;
}
#errorLabel { color: @RED@; font-weight: 600; }

/* ---------- Tablas ---------- */
QTableWidget {
    background: white; border: 1px solid @BORDER@; border-radius: 10px;
    gridline-color: transparent;
    selection-background-color: #DBEAFE; selection-color: @TEXT@;
}
QTableWidget::item { padding: 6px; border-bottom: 1px solid #F3F4F6; }
QHeaderView { background: #F9FAFB; border: none; }
QHeaderView::section {
    background: #F9FAFB; color: @MUTED@; padding: 8px 10px; border: none;
    border-bottom: 1px solid @BORDER@; font-weight: 600;
}

/* ---------- Barras de progreso ---------- */
QProgressBar {
    background: #E5E7EB; border: none; border-radius: 5px;
    min-height: 10px; max-height: 10px; color: transparent;
}
QProgressBar::chunk { background: @ACCENT@; border-radius: 5px; }

/* ---------- Scrollbars ---------- */
QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 4px; min-height: 30px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px; }
QScrollBar::handle:horizontal { background: #CBD5E1; border-radius: 4px; min-width: 30px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ---------- Login ---------- */
#loginHero { background: @NAVY@; }
#loginHero QLabel { color: white; }
#heroTitle { font-size: 38px; font-weight: 800; color: white; }
#heroSub { color: #BFDBFE; font-size: 15px; }
#heroBullet { color: #DBEAFE; font-size: 14px; }
#loginRight { background: @BG@; }

/* ---------- Pantalla de carga ---------- */
#loadingCard { background: white; border: 1px solid @BORDER@; border-radius: 14px; }
#loadingTitle { font-size: 20px; font-weight: 700; color: @NAVY@; }
)QSS");

    qss.replace(QLatin1String("@NAVY@"), Navy);
    qss.replace(QLatin1String("@ACCENT@"), Accent);
    qss.replace(QLatin1String("@BG@"), Bg);
    qss.replace(QLatin1String("@CARD@"), Card);
    qss.replace(QLatin1String("@GREEN@"), Green);
    qss.replace(QLatin1String("@AMBER@"), Amber);
    qss.replace(QLatin1String("@RED@"), Red);
    qss.replace(QLatin1String("@TEXT@"), Text);
    qss.replace(QLatin1String("@MUTED@"), Muted);
    qss.replace(QLatin1String("@BORDER@"), Border);
    return qss;
}
