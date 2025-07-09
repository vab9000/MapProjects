#pragma once

#include <list>
#include "../utils/id_generator.hpp"
#include "army.hpp"

class province;

class tag {
    std::list<province *> provinces_;
    std::list<army> armies_;
    uint_fast32_t color_;
    int_fast32_t gold_ = 0;

    static id_generator id_gen_;

public:
    const uint_fast32_t id;

    tag();

    explicit tag(uint_fast32_t color);

    ~tag();

    // Get the color of the tag
    [[nodiscard]] uint_fast32_t color() const;

    // Set the color of the tag
    void set_color(uint_fast32_t color);

    // Get the amount of gold the tag has
    [[nodiscard]] int_fast32_t gold() const;

    // Add gold to the tag
    void add_gold(int_fast32_t amount);

    // Remove gold from the tag
    void remove_gold(int_fast32_t amount);

    // Create a new army for the tag
    army &new_army();

    // Add a province to the tag
    void add_province(province &added_province);

    // Remove a province from the tag
    void remove_province(const province &removed_province);

    // Check if the tag has a specific province
    [[nodiscard]] bool has_province(const province &found_province) const;

    // Get the list of provinces owned by the tag
    [[nodiscard]] const std::list<province *> &provinces() const;

    // Get the list of provinces owned by the tag
    std::list<province *> &provinces();

    // Check if the tag has army access to a specific province
    [[nodiscard]] bool has_army_access(const province &access_province) const;

    // Tick the tag
    void tick();
};
