#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace teoto {

/// Repositorio genérico para colecciones de entidades compartidas.
template <typename T>
class Repository
{
public:
    void add(std::shared_ptr<T> item)
    {
        if (item)
            m_items.push_back(std::move(item));
    }

    std::size_t size() const noexcept { return m_items.size(); }
    bool empty() const noexcept { return m_items.empty(); }

    const std::vector<std::shared_ptr<T>> &all() const noexcept { return m_items; }

    template <typename Predicate>
    std::shared_ptr<T> findIf(Predicate predicate) const
    {
        const auto it = std::find_if(m_items.cbegin(), m_items.cend(),
            [&predicate](const std::shared_ptr<T> &item) { return predicate(*item); });
        return it == m_items.cend() ? nullptr : *it;
    }

private:
    std::vector<std::shared_ptr<T>> m_items;
};

} // namespace teoto
