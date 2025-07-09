#pragma once

#include <set>
#include "ai.hpp"
#include "date.hpp"
#include "character/character_properties.hpp"
#include "../utils/id_generator.hpp"

class army;
class province;

class character final : public ai {
    date birthday_;
    province *location_;
    std::set<character_flag_t> flags_;
    std::set<personality_trait_t> traits_;

    static id_generator id_gen_;

public:
    const uint_fast32_t id;

    explicit character(date birthday, province *location);

    ~character() override;

    // The number of years since the character's birthday
    [[nodiscard]] uint_fast32_t age(const date &current_date) const;

    // The character's birthday
    [[nodiscard]] const date &birthday() const;

    // The province where the character is located
    [[nodiscard]] province &location() const;

    // Set the character's location to a new province
    void set_location(province *location);

    void update_ai() override;

    // Add a flag to the character
    void add_flag(character_flag_t flag);

    // Remove a flag from the character
    void remove_flag(character_flag_t flag);

    // Check if the character has a specific flag
    [[nodiscard]] bool has_flag(character_flag_t flag) const;

    // Get all flags of the character
    [[nodiscard]] const std::set<character_flag_t> &flags() const;

    // Add a personality trait to the character
    void add_trait(personality_trait_t trait);

    // Remove a personality trait from the character
    void remove_trait(personality_trait_t trait);

    // Check if the character has a specific personality trait
    [[nodiscard]] bool has_trait(personality_trait_t trait) const;

    // Get all personality traits of the character
    [[nodiscard]] const std::set<personality_trait_t> &traits() const;
};
