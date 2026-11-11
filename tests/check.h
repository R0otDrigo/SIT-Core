#pragma once
#include <cstdlib>
#include <iostream>

// Mini framework de pruebas (sin dependencias). Luego se puede migrar a GoogleTest/Qt Test.
#define CHECK(cond) do { if (!(cond)) { \
    std::cerr << "FALLO " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
    std::exit(1); } } while (0)

#define CHECK_THROWS(expr, ExType) do { bool ok_ = false; \
    try { expr; } catch (const ExType&) { ok_ = true; } \
    CHECK(ok_); } while (0)
