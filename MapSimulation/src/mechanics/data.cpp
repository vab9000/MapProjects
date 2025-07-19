#include "data.hpp"

namespace mechanics {
    data data::instance_ = data();

    auto data::current_date() const -> const date & { return current_date_; }

    auto data::provinces() const -> const std::unordered_map<uint_fast32_t, province> & { return provinces_; }

    auto data::provinces() -> std::unordered_map<uint_fast32_t, province> & { return provinces_; }

    auto data::add_province(province &&p) -> void { provinces_[p.base_color()] = std::move(p); }

    auto data::tags() const -> const std::unordered_map<uint_fast32_t, tag> & { return tags_; }

    auto data::tags() -> std::unordered_map<uint_fast32_t, tag> & { return tags_; }

    auto data::characters() const -> const std::unordered_map<uint_fast32_t, character> & { return characters_; }

    auto data::characters() -> std::unordered_map<uint_fast32_t, character> & { return characters_; }

    auto data::tick() -> void { current_date_ = current_date_ + 1; }

    auto data::instance() -> data & { return instance_; }
}
