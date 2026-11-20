.pragma library

// Tokens visuales compartidos por las vistas QML.
var bg = "#F5F7FB"
var surface = "#FFFFFF"
var border = "#E2E8F0"
var text = "#0F172A"
var muted = "#64748B"
var primary = "#2563EB"
var primaryDark = "#1D4ED8"
var success = "#16A34A"
var warning = "#F59E0B"
var danger = "#DC2626"
var headerBg = "#FFFFFF"
var mapBg = "#E8EEF5"

var radius = 12
var radiusSmall = 8
var spacing = 12

function occupancyColor(ratio) {
    if (ratio >= 0.8) return danger
    if (ratio >= 0.5) return warning
    return success
}
