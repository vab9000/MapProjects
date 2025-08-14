#pragma once
#include <unordered_map>
#include "character.hpp"
#include "date.hpp"
#include "event.hpp"
#include "province.hpp"
#include "tag.hpp"

namespace mechanics {
    class data {
        date current_date_;
        std::vector<province> provinces_;
        std::unordered_map<unsigned int, std::reference_wrapper<province>> provinces_map_;
        std::vector<std::unique_ptr<tag>> tags_;
        std::vector<std::unique_ptr<character>> characters_;
        std::priority_queue<std::unique_ptr<event>, std::vector<std::unique_ptr<event>>, std::greater<>> events_;

        static data instance_;

        data() = default;

    public:
        auto operator=(const data &) -> data & = delete;

        data(const data &) = delete;

        // Get the current date
        [[nodiscard]] auto current_date() const -> const date &;

        // Get the provinces
        [[nodiscard]] auto provinces() const -> const std::vector<province> &;

        // Get the provinces
        auto provinces() -> std::vector<province> &;

        // Add a province
        template<typename... Args>
        auto emplace_province(Args &&... args) -> province & {
            // static_assert(std::is_constructible_v<province, Args...>, "Arguments must be able to construct a province");
            auto &val = provinces_.emplace_back(provinces_.size(), std::forward<Args>(args)...);
            return val;
        }

        auto lock_provinces() -> void;

        [[nodiscard]] auto province_at(unsigned int color) const -> province &;

        // Get the tags
        [[nodiscard]] auto tags() const -> const std::vector<std::unique_ptr<tag>> &;

        // Get the tags
        auto tags() -> std::vector<std::unique_ptr<tag>> &;

        // Get the characters
        [[nodiscard]] auto characters() const -> const std::vector<std::unique_ptr<character>> &;

        // Get the characters
        auto characters() -> std::vector<std::unique_ptr<character>> &;

        // Advance the simulation
        auto tick() -> void;

        // Get the singleton instance of data
        static auto instance() -> data &;
    };
}
