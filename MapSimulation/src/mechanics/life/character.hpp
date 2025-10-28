#ifndef MECHANICS_CHARACTER
#define MECHANICS_CHARACTER
#include <flags.hpp>
#include <numbered.hpp>
#include <reference.hpp>
#include <magic_enum/magic_enum_containers.hpp>
#include "../ai/ai.hpp"
#include "../date.hpp"
#include "character_properties.hpp"

namespace mechanics {
    class army;
    class province;

    class character final : public ai, public utils::numbered<character> {
        utils::ref<province> location_;
        utils::tied_flags<role_t, role_type> roles_;
        magic_enum::containers::set<personality_trait_t> traits_;
        const date birthday_;
        bool alive_ = true;

    public:
        explicit character(const date &birthday, province &location);

        [[nodiscard]] auto is_alive() const -> bool;

        auto kill() -> void;

        // The number of years since the character's birthday
        [[nodiscard]] auto age(const date &current_date) const -> int;

        // The character's birthday
        [[nodiscard]] auto birthday() const -> const date &;

        // The province where the character is located
        [[nodiscard]] auto location() const -> province &;

        // Set the character's location to a new province
        auto set_location(province &location) -> void;

        [[nodiscard]] auto roles() const -> const utils::tied_flags<role_t, role_type> &;

        auto roles() -> utils::tied_flags<role_t, role_type> &;

        [[nodiscard]] auto traits() const -> const magic_enum::containers::set<personality_trait_t> &;

        auto traits() -> magic_enum::containers::set<personality_trait_t> &;

        auto tick(tick_t tick_type) -> void;
    };
}
#endif
