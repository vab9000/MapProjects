#pragma once

#include <list>
#include <memory>
#include "army.hpp"

class province;

class tag {
    std::list<province *> provinces_;
    uint_fast32_t color_;
    std::list<std::unique_ptr<army> > armies_;
    int_fast32_t gold_ = 0;

public:
    const uint_fast32_t id;

    tag();

    explicit tag(uint_fast32_t color);

    ~tag();

    [[nodiscard]] uint_fast32_t color() const;

    void set_color(uint_fast32_t color);

    [[nodiscard]] int32_t gold() const;

    void add_gold(int32_t amount);

    void remove_gold(int32_t amount);

    [[nodiscard]] army &new_army();

    void add_province(province &added_province);

    void remove_province(const province &removed_province);

    [[nodiscard]] bool has_province(const province &found_province) const;

    [[nodiscard]] const std::list<province *> &provinces() const;

    [[nodiscard]] bool has_army_access(const province &access_province) const;

    void tick();
};
