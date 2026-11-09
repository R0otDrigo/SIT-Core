#pragma once
#include <algorithm>
#include <memory>
#include <vector>

namespace sit {

// Template: almacen generico de entidades (buses, paraderos, rutas...).
template <typename T>
class Repository {
public:
    void add(std::shared_ptr<T> item) { items_.push_back(std::move(item)); }

    std::size_t size() const { return items_.size(); }

    const std::vector<std::shared_ptr<T>>& all() const { return items_; }

    // Busqueda con lambda: repo.findIf([](const Stop& s){ return s.saturation() > 0.8; });
    template <typename Pred>
    std::shared_ptr<T> findIf(Pred pred) const {
        auto it = std::find_if(items_.begin(), items_.end(),
                               [&](const std::shared_ptr<T>& p) { return pred(*p); });
        return it == items_.end() ? nullptr : *it;
    }

private:
    std::vector<std::shared_ptr<T>> items_;
};

} // namespace sit
