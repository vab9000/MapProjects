#pragma once
#include <flags.hpp>
#include <reference.hpp>
#include "ai.hpp"
#include "date.hpp"
#include "tickable.hpp"
#include "character/character_properties.hpp"

namespace mechanics {
    class army;
    class province;

    class character final : public ai, public tickable {
        const date birthday_;
        utils::ref<province> location_;
        utils::tied_flags<role_t> roles_;
        utils::flags<personality_trait_t> traits_;
        bool alive_{true};

    public:
        explicit character(const date &birthday, province &location);

        ~character() override;

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

        [[nodiscard]] auto roles() const -> const utils::tied_flags<role_t> &;

        auto roles() -> utils::tied_flags<role_t> &;

        [[nodiscard]] auto traits() const -> const utils::flags<personality_trait_t> &;

        auto traits() -> utils::flags<personality_trait_t> &;

        auto tick(tick_t tick_type) -> void override;
    };
}
