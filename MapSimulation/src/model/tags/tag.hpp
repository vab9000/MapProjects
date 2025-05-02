#pragma once

#include <string>
#include <vector>
#include "../warfare/army.hpp"

class province;

class tag {
    std::vector<province *> provinces_;
    std::string name_;
    unsigned int color_;
    std::vector<army> armies_;
    int gold_ = 0;

public:
    tag();

    tag(std::string name, unsigned int color);

    ~tag();

    [[nodiscard]] unsigned int get_color() const;

    void set_color(unsigned int color);

    [[nodiscard]] int get_gold() const;

    void add_gold(int amount);

    void remove_gold(int amount);

    [[nodiscard]] std::string_view get_name() const;

    void set_name(const std::string &name);

    [[nodiscard]] army &new_army();

    void add_province(province &added_province);

    void remove_province(const province &removed_province);

    [[nodiscard]] bool has_province(const province &found_province) const;

    [[nodiscard]] const std::vector<province *> &get_provinces() const;

    [[nodiscard]] bool has_army_access(const province &access_province) const;

    void tick();
};
