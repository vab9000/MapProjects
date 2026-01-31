#include "data.hpp"
#include <algorithm>
#include <execution>

namespace mechanics {
    auto data::current_date() -> const date & { return current_date_; }

    auto data::provinces() -> std::vector<province> & { return provinces_; }

    auto data::lock_provinces() -> void {
        for (auto &province : provinces_) { provinces_map_.emplace(province.base_color(), province); }
    }

    auto data::province_at(const unsigned int color) -> province & { return provinces_map_.at(color); }

    auto data::add_event(event &&e) -> void { events_.emplace(std::make_unique<event>(std::move(e))); }

    auto data::tick() -> void {
        const auto tick_type = current_date_.advance();
        while (!events_.empty() && events_.top()->date() <= current_date_) {
            (*events_.top())();
            events_.pop();
        }
        std::for_each(std::execution::par_unseq, provinces_.begin(), provinces_.end(),
            [tick_type](province &p) { p.tick(tick_type); });
        std::for_each(std::execution::par_unseq, tags_.begin(), tags_.end(),
            [tick_type](const std::unique_ptr<tag> &t) { if (t != nullptr) { t->tick(tick_type); } });
        std::for_each(std::execution::par_unseq, characters_.begin(), characters_.end(),
            [tick_type](const std::unique_ptr<character> &c) { if (c != nullptr) { c->tick(tick_type); } });
        std::for_each(std::execution::seq, characters_.begin(), characters_.end(),
            [](const std::unique_ptr<character> &c) { if (c != nullptr) { c->run_ai(); } });
    }
}
