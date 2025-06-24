#pragma once

#include <vector>
#include <memory>

class ai;
class province;
class character;
class tag;
class army;

enum class army_directive_type { attack };

struct army_directive {
    army_directive_type type;
    void *target;
};

class unit {
public:
    unsigned int size;
    province *location;
    army *parent_army;
    std::vector<province *> path;
    double travel_progress;
    character *general;
    bool retreating;
    double speed;

    unit(army &parent_army, province &location);

    void set_destination(const province &destination);

    void move();
};

class army {
    army_directive directive_{};
    character *commander_;

public:
    std::vector<std::unique_ptr<unit>> units;
    tag *parent_tag;

    explicit army(tag &parent_tag);

    [[nodiscard]] unit &new_unit(province &location);

    void remove_commander();

    void set_commander(character &commander);

    [[nodiscard]] character &get_commander() const;

    [[nodiscard]] army_directive get_directive() const;
};
