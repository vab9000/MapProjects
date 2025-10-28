#ifndef MECHANICS_DATA
#define MECHANICS_DATA
#include <unordered_map>
#include "life/character.hpp"
#include "society/culture.hpp"
#include "date.hpp"
#include "event.hpp"
#include "province/province.hpp"
#include "society/tag.hpp"

namespace mechanics {
    class data {
        std::vector<province> provinces_;
        std::unordered_map<unsigned int, utils::ref<province>> provinces_map_;
        std::vector<std::unique_ptr<tag>> tags_;
        std::vector<std::unique_ptr<character>> characters_;
        std::vector<std::unique_ptr<culture>> cultures_;
        std::priority_queue<std::unique_ptr<event>, std::vector<std::unique_ptr<event>>, std::greater<>> events_;
        date current_date_;

        data() = default;

    public:
        auto operator=(const data &) -> data & = delete;

        data(const data &) = delete;

        // Get the current date
        [[nodiscard]] auto current_date() const -> const date &;

        // Get the provinces
        [[nodiscard]] auto provinces() const -> const std::vector<province> &;

        auto provinces() -> std::vector<province> &;

        // Add a province
        template<typename... Args>
        auto emplace_province(Args &&... args) -> province & {
            auto &val = provinces_.emplace_back(std::forward<Args>(args)...);
            return val;
        }

        auto lock_provinces() -> void;

        [[nodiscard]] auto province_at(unsigned int color) const -> province &;

        auto add_event(event &&e) -> void;

        // Advance the simulation
        auto tick() -> void;

        // Get the singleton instance of data
        static auto instance() -> data &;
    };
}
#endif
