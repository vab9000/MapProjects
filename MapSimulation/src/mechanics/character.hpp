#pragma once
#include <set>
#include "ai.hpp"
#include "date.hpp"
#include "../utils/id_generator.hpp"
#include "character/character_properties.hpp"

namespace mechanics {
    class army;
    class province;

    class character final : public ai {
        date birthday_;
        std::reference_wrapper<province> location_;
        std::set<character_flag_t> flags_;
        std::set<personality_trait_t> traits_;

        static utils::id_generator id_gen_;

    public:
        const uint_fast32_t id;

        explicit character(date birthday, province &location);

        virtual ~character() override;

        // The number of years since the character's birthday
        [[nodiscard]] auto age(const date &current_date) const -> uint_fast32_t;

        // The character's birthday
        [[nodiscard]] auto birthday() const -> const date &;

        // The province where the character is located
        [[nodiscard]] auto location() const -> province &;

        // Set the character's location to a new province
        auto set_location(province &location) -> void;

        virtual auto update_ai() -> void override;

        // Add a flag to the character
        auto add_flag(character_flag_t flag) -> void;

        // Remove a flag from the character
        auto remove_flag(character_flag_t flag) -> void;

        // Check if the character has a specific flag
        [[nodiscard]] auto has_flag(character_flag_t flag) const -> bool;

        // Get all flags of the character
        [[nodiscard]] auto flags() const -> const std::set<character_flag_t> &;

        // Add a personality trait to the character
        auto add_trait(personality_trait_t trait) -> void;

        // Remove a personality trait from the character
        auto remove_trait(personality_trait_t trait) -> void;

        // Check if the character has a specific personality trait
        [[nodiscard]] auto has_trait(personality_trait_t trait) const -> bool;

        // Get all personality traits of the character
        [[nodiscard]] auto traits() const -> const std::set<personality_trait_t> &;
    };
}
