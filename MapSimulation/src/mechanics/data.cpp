#include "data.hpp"
#include <algorithm>
#include <execution>
#include <ranges>

namespace mechanics {
    data data::instance_ = data();

    auto data::current_date() const -> const date & { return current_date_; }

    auto data::provinces() const -> const std::vector<province> & { return provinces_; }

    auto data::provinces() -> std::vector<province> & { return provinces_; }

    auto data::lock_provinces() -> void {
        for (auto &province : provinces_) { provinces_map_.emplace(province.base_color(), province); }
    }

    auto data::province_at(const uint_fast32_t color) const -> province & { return provinces_map_.at(color); }

    auto data::tags() const -> const std::vector<std::unique_ptr<tag>> & { return tags_; }

    auto data::tags() -> std::vector<std::unique_ptr<tag>> & { return tags_; }

    auto data::characters() const -> const std::vector<std::unique_ptr<character>> & { return characters_; }

    auto data::characters() -> std::vector<std::unique_ptr<character>> & { return characters_; }

    auto data::tick() -> void {
        const auto tick_type = current_date_.advance();
        while (!events_.empty() && events_.top()->date() <= current_date_) {
            (*events_.top())();
            events_.pop();
        }
        std::for_each(std::execution::unseq, provinces_.begin(), provinces_.end(),
            [tick_type](province &p) { p.tick(tick_type); });
        std::for_each(std::execution::unseq, tags_.begin(), tags_.end(),
            [tick_type](const std::unique_ptr<tag> &t) { t->tick(tick_type); });
        std::for_each(std::execution::unseq, characters_.begin(), characters_.end(),
            [tick_type](const std::unique_ptr<character> &c) { c->tick(tick_type); });
        std::for_each(std::execution::seq, characters_.begin(), characters_.end(),
            [](const std::unique_ptr<character> &c) { c->run_ai(); });
    }

    auto data::instance() -> data & { return instance_; }
}
