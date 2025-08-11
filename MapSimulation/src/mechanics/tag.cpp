#include "tag.hpp"
#include <algorithm>
#include "army.hpp"
#include "province.hpp"

namespace mechanics {
    utils::id_generator tag::id_gen_;

    tag::tag() : tag(0) {}

    tag::tag(const uint_fast32_t color) : color_(color), id_(id_gen_.next_id()) {}

    tag::~tag() { id_gen_.return_id(id_); }

    auto tag::id() const -> uint_fast32_t { return id_; }

    auto tag::color() const -> uint_fast32_t { return color_; }

    auto tag::set_color(const uint_fast32_t color) -> void { color_ = color; }

    auto tag::gold() const -> int_fast32_t { return gold_; }

    auto tag::add_gold(const int_fast32_t amount) -> void { gold_ += amount; }

    auto tag::remove_gold(const int_fast32_t amount) -> void { gold_ -= amount; }

    [[nodiscard]] auto tag::new_army() -> army & {
        armies_.emplace_back(*this);
        return armies_.back();
    }

    auto tag::add_province(province &added_province) -> void { provinces_.emplace_back(added_province); }

    auto tag::remove_province(const province &removed_province) -> void {
        std::erase(provinces_, std::ref(removed_province));
    }

    auto tag::has_province(const province &found_province) const -> bool {
        for (const auto &province_ref : provinces_) { if (&province_ref.get() == &found_province) { return true; } }
        return false;
    }

    [[nodiscard]] auto tag::provinces() const -> const std::list<std::reference_wrapper<province>> & {
        return provinces_;
    }

    auto tag::provinces() -> std::list<std::reference_wrapper<province>> & { return provinces_; }

    [[nodiscard]] auto tag::has_army_access(const province &access_province) const -> bool {
        if (!access_province.owner().has_value()) { return true; }
        return &access_province.owner()->get() == this;
    }

    auto tag::tick(tick_t tick_type) -> void {}
}
