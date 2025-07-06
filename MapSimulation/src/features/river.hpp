#pragma once

#include <vector>

class province;

class river {
    std::vector<province *> provinces_;

public:
    river() = default;

    // Adds a province to the river
    void add_province(province *prov);

    // Checks if the river contains a specific province
    bool contains(const province *prov) const;
};