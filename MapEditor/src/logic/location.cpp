#include "location.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace logic {
    constexpr size_t max_size_t = std::numeric_limits<size_t>::max();

    std::vector<location> location::locations_ = {};

    auto location::free_color() -> std::array<unsigned char, 4> {
        constexpr unsigned char max = std::numeric_limits<unsigned char>::max();
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution dist(0, max - 1);
        auto unique = false;
        std::array<unsigned char, 4> color = {};
        while (!unique) {
            color = {
                static_cast<unsigned char>(dist(rng)), static_cast<unsigned char>(dist(rng)),
                static_cast<unsigned char>(dist(rng)), max
            };
            unique = true;
            for (const auto &loc : locations_) {
                if (loc.base_color_ == color) {
                    unique = false;
                    break;
                }
            }
        }
        return color;
    }

    location::location() : idx_{max_size_t}, base_color_{} {}

    location::location(const size_t idx) : idx_{idx}, property_values_(properties_.size()),
        base_color_{free_color()} {}

    location::location(location &&other) noexcept : size_{other.size_},
        idx_{other.idx_}, property_values_(std::move(other.property_values_)), base_color_{other.base_color_} {
        other.idx_ = max_size_t;
        other.base_color_ = {};
        other.size_ = 0;
    }

    auto location::operator=(location &&other) noexcept -> location & {
        if (this != &other) {
            idx_ = other.idx_;
            property_values_ = std::move(other.property_values_);
            base_color_ = other.base_color_;
            size_ = other.size_;
            other.idx_ = max_size_t;
            other.base_color_ = {};
            other.size_ = 0;
        }
        return *this;
    }

    location::~location() = default;

    auto location::idx() const -> size_t { return idx_; }

    auto location::name() const -> const std::string & { return name_; }

    auto location::prop_value(const size_t prop_index) const -> size_t { return property_values_.at(prop_index); }

    auto location::prop_color(const size_t prop_index) const -> std::array<unsigned char, 4> {
        if (prop_index == base_color_index) { return base_color_; }
        const auto value_index = property_values_.at(prop_index);
        return properties_.at(prop_index).values.at(value_index).color;
    }

    auto location::size() const -> int { return size_; }

    auto location::base_color() const -> std::array<unsigned char, 4> { return base_color_; }

    auto location::set_property(const size_t location_idx, const size_t prop_index,
        const size_t value_index) -> void {
        auto &loc = locations_.at(location_idx);
        loc.property_values_.at(prop_index) = value_index;
        if (prop_index == current_property_) {
            if (on_color_change_) {
                const std::span<const location> locs = locations_;
                on_color_change_(locs.subspan(loc.idx_, 1));
            }
        }
    }

    auto location::locations() -> std::span<const location> { return locations_; }

    auto location::properties() -> std::span<const property> { return properties_; }

    auto location::prop(const size_t index) -> const property & { return properties_.at(index); }

    auto location::prop_value(const size_t prop_index, const size_t value_index) -> const value & {
        return properties_.at(prop_index).values.at(value_index);
    }

    auto location::add_property(std::string &&name) -> size_t {
        properties_.emplace_back(property{
            .values = std::vector{1, value{.color = {0, 0, 0, 255}, .name = "None"}}, .name = std::move(name)
        });
        for (auto &loc : locations_) { loc.property_values_.emplace_back(0); }
        return properties_.size() - 1;
    }

    auto location::remove_property(const size_t index) -> void {
        properties_.erase(properties_.begin() + static_cast<std::ptrdiff_t>(index));
        for (auto &loc : locations_) {
            loc.property_values_.erase(loc.property_values_.begin() +
                                       static_cast<std::ptrdiff_t>(index));
        }
    }

    auto location::add_property_value(std::string &&name, const std::array<unsigned char, 4> color) -> size_t {
        properties_.at(current_property_).values.emplace_back(value{.color = color, .name = std::move(name)});
        return properties_.at(current_property_).values.size() - 1;
    }

    auto location::remove_property_value(const size_t prop_index, const size_t value_index) -> void {
        properties_.at(prop_index).values.erase(
            properties_.at(prop_index).values.begin() + static_cast<std::ptrdiff_t>(value_index));
    }

    auto location::change_property_color(const size_t prop_index, const size_t value_index,
        const std::array<unsigned char, 4> color) -> void {
        properties_.at(prop_index).values.at(value_index).color = color;
        if (prop_index == current_property_) {
            if (on_color_change_) {
                const std::span<const location> locs = {locations_};
                on_color_change_(locs);
            }
        }
    }

    auto location::set_color_change_callback(
        std::function<void(std::span<const location>)> &&callback) -> void { on_color_change_ = std::move(callback); }

    auto location::new_location() -> const location & {
        size_t idx = {};
        if (!free_locations_.empty()) {
            idx = free_locations_.back();
            locations_.at(idx).property_values_.resize(properties_.size(), 0);
            if (on_color_change_) {
                const std::span<const location> locs = locations_;
                on_color_change_(locs.subspan(idx, 1));
            }
            return locations_.at(idx);
        }
        idx = locations_.size();
        locations_.emplace_back(idx);
        free_locations_.emplace_back(idx);
        if (on_color_change_) {
            const std::span<const location> locs = locations_;
            on_color_change_(locs.subspan(idx, 1));
        }
        return locations_.back();
    }

    auto location::change_size(const size_t location_idx, const int delta) -> void {
        auto &loc = locations_.at(location_idx);
        if (loc.size_ == 0 && delta >= 0) {
            std::erase(free_locations_, loc.idx_);
        }
        loc.size_ += delta;
        if (loc.size_ <= 0 && loc.idx_ != max_size_t) {
            loc.name_ = "";
            loc.size_ = 0;
            std::fill_n(loc.property_values_.begin(), properties_.size(), 0);
            free_locations_.push_back(loc.idx_);
        }
    }

    auto location::change_current_property(const size_t prop_index) -> void {
        current_property_ = prop_index;
        if (on_color_change_) { on_color_change_(locations_); }
    }

    auto location::current_property() -> size_t { return current_property_; }
} // namespace logic
