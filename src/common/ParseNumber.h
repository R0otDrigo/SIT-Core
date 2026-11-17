#pragma once
#include <charconv>
#include <cctype>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace sit {

// Conversion independiente del locale. std::stod usa el separador decimal del locale
// global, que en es_PE es la coma; eso truncaba "-16.37" a -16 al arrancar con Qt.
inline double parseDouble(std::string_view text) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;

    double value{};
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    if (ec != std::errc{} || ptr != end) throw std::invalid_argument("double invalido");
    return value;
}

inline int parseInt(std::string_view text) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;

    int value{};
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    if (ec != std::errc{} || ptr != end) throw std::invalid_argument("entero invalido");
    return value;
}

inline long long parseLongLong(std::string_view text) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;

    long long value{};
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    if (ec != std::errc{} || ptr != end) throw std::invalid_argument("entero invalido");
    return value;
}

inline unsigned long parseUnsigned(std::string_view text) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;

    unsigned long value{};
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    if (ec != std::errc{} || ptr != end) throw std::invalid_argument("entero invalido");
    return value;
}

} // namespace sit
