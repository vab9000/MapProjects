#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Utils.hpp"


class country;
enum class map_mode;
class pop;

class province {
    std::vector<std::array<int, 2> > pixels_;
    std::vector<std::pair<province *, std::array<int, 2> > > outline_;
    std::vector<pop *> pops_;
    bool distances_processed_{};
    country *owner_;
    std::unordered_map<province *, double> neighbors_;
    unsigned int num_pixels_;
    unsigned int num_outline_;
    int bounds_[4]{};
    int center_[2]{};

public:
    unsigned int color;
    unsigned int base_color;
    std::string name;

    province(const std::string &name, unsigned int color, int i, int j);

    void finalize();

    void process_distances();

    void set_owner(const country &new_owner);

    [[nodiscard]] country *get_owner() const;

    void add_pixel(int x, int y);

    void add_outline(int x, int y, const province &other);

    [[nodiscard]] const std::array<int, 2> *get_pixels() const;

    [[nodiscard]] const std::pair<province *, std::array<int, 2> > *get_outline() const;

    void expand_bounds(int x, int y);

    void recolor(map_mode mode);

    [[nodiscard]] double distance(const province &other) const;

    [[nodiscard]] std::vector<province *> get_path_to(const province &destination,
                                                      bool (*accessible)(const province &, void *),
                                                      double (*cost_modifier)(const province &, void *), void *param);

    [[nodiscard]] unsigned int get_num_pixels() const;

    [[nodiscard]] unsigned int get_num_outline() const;

    [[nodiscard]] const int *get_bounds() const;

    [[nodiscard]] const int *get_center() const;

    [[nodiscard]] const std::vector<pop *> &get_pops() const;

    [[nodiscard]] const std::unordered_map<province *, double> &get_neighbors() const;

    void tick();
};
