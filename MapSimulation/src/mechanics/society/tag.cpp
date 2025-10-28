#include "tag.hpp"
#include "../military/army.hpp"
#include "../province/province.hpp"

namespace mechanics {
    tag::tag() : tag(0U) {}

    tag::tag(const unsigned int color) : color_(color) {}

    auto tag::color() const -> unsigned int { return color_; }

    auto tag::set_color(const unsigned int color) -> void { color_ = color; }

    auto tag::gold() const -> int { return gold_; }

    auto tag::add_gold(const int amount) -> void { gold_ += amount; }

    auto tag::remove_gold(const int amount) -> void { gold_ -= amount; }

    [[nodiscard]] auto tag::new_army() -> army & {
        armies_.emplace_back(std::make_unique<army>(*this));
        return *armies_.back();
    }

    auto tag::add_province(province &added_province) -> void { provinces_.emplace_back(added_province); }

    auto tag::remove_province(const province &removed_province) -> void {
        std::erase(provinces_, utils::ref(removed_province));
    }

    auto tag::has_province(const province &found_province) const -> bool {
        for (const auto &province_ref : provinces_) { if (&province_ref.get() == &found_province) { return true; } }
        return false;
    }

    [[nodiscard]] auto tag::provinces() const -> const std::vector<utils::ref<province>> & { return provinces_; }

    auto tag::provinces() -> std::vector<utils::ref<province>> & { return provinces_; }

    [[nodiscard]] auto tag::has_army_access(const province &access_province) const -> bool {
        if (access_province.owner() == nullptr) { return true; }
        return access_province.owner() == this;
    }

    auto tag::tick(tick_t tick_type) -> void {}
}
