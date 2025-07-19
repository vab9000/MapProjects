#pragma once
#include <unordered_map>
#include "character.hpp"
#include "date.hpp"
#include "province.hpp"
#include "tag.hpp"

namespace mechanics {
    class data {
        date current_date_;
        std::unordered_map<uint_fast32_t, province> provinces_;
        std::unordered_map<uint_fast32_t, tag> tags_;
        std::unordered_map<uint_fast32_t, character> characters_;

        static data instance_;

        data() = default;

    public:
        auto operator=(const data &) -> data & = delete;

        data(const data &) = delete;

        // Get the current date
        [[nodiscard]] auto current_date() const -> const date &;

        // Get the provinces
        [[nodiscard]] auto provinces() const -> const std::unordered_map<uint_fast32_t, province> &;

        // Get the provinces
        auto provinces() -> std::unordered_map<uint_fast32_t, province> &;

        // Add a province
        auto add_province(province &&p) -> void;

        // Get the tags
        [[nodiscard]] auto tags() const -> const std::unordered_map<uint_fast32_t, tag> &;

        // Get the tags
        auto tags() -> std::unordered_map<uint_fast32_t, tag> &;

        // Get the characters
        [[nodiscard]] auto characters() const -> const std::unordered_map<uint_fast32_t, character> &;

        // Get the characters
        auto characters() -> std::unordered_map<uint_fast32_t, character> &;

        // Advance the simulation
        auto tick() -> void;

        // Get the singleton instance of data
        static auto instance() -> data &;
    };
}
