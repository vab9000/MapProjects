#pragma once

#include <vector>
#include "../base/ai.hpp"
#include "../base/province.hpp"
#include "../populations/character.hpp"


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

    unit(const army &parent_army, const province &location);

    void set_destination(const province &destination);

    void move();
};

class army {
    army_directive directive_{};
    character *commander_;

public:
    std::vector<std::unique_ptr<unit> > units;
    tag *parent_tag;

    explicit army(const tag &parent_tag);

    [[nodiscard]] unit *new_unit(const province &location);
};
