#pragma once

#include <unordered_set>
#include "../Base/Utils.hpp"
#include "../Base/AI.hpp"

enum class personality_trait {
    ambitious,
};

class army;

class character final : public has_ai {
    date birthday_{};
    bool commander_{};
    army *army_;

public:
    std::unordered_set<personality_trait> traits;

    explicit character(date birthday);

    [[nodiscard]] int age(date current_date) const;

    void update_ai() override;

    void make_commander(army *army);

    void remove_commander();

    [[nodiscard]] bool is_commander() const;
};
