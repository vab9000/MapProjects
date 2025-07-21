#pragma once
#include "ai.hpp"
#include "date.hpp"
#include "../utils/flags.hpp"
#include "../utils/id_generator.hpp"
#include "character/character_properties.hpp"

namespace mechanics {
    class army;
    class province;

    class character final : public ai {
        date birthday_;
        std::reference_wrapper<province> location_;
        utils::flags<character_flag_t> flags_;
        utils::flags<personality_trait_t> traits_;

        static utils::id_generator id_gen_;

    public:
        const uint_fast32_t id;

        explicit character(date birthday, province &location);

        ~character() override;

        // The number of years since the character's birthday
        [[nodiscard]] auto age(const date &current_date) const -> uint_fast32_t;

        // The character's birthday
        [[nodiscard]] auto birthday() const -> const date &;

        // The province where the character is located
        [[nodiscard]] auto location() const -> province &;

        // Set the character's location to a new province
        auto set_location(province &location) -> void;

        auto update_ai() -> void override;

        [[nodiscard]] auto flags() const -> const utils::flags<character_flag_t> &;

        auto flags() -> utils::flags<character_flag_t> &;

        [[nodiscard]] auto traits() const -> const utils::flags<personality_trait_t> &;

        auto traits() -> utils::flags<personality_trait_t> &;
    };
}
