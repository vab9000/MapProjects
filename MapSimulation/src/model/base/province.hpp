#pragma once

#include <array>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "utils.hpp"
#include "../populations/pop.hpp"


class tag;
enum class map_mode;

class province {
    std::vector<std::array<int, 2> > pixels_;
    std::vector<std::pair<province *, std::array<int, 2> > > outline_;
    std::vector<std::unique_ptr<pop> > pops_;
    bool distances_processed_ = false;
    tag *owner_;
    std::unordered_map<province *, double> neighbors_;
    unsigned int num_pixels_;
    unsigned int num_outline_;
    std::array<int, 4> bounds_;
    std::array<int, 2> center_;
    unsigned int base_color_;
    std::string name_;

public:
    unsigned int color;

    province();

    province(std::string name, unsigned int color, int i, int j);

    void finalize();

    void process_distances();

    std::string_view name();

    void set_owner(tag &new_owner);

    void remove_owner();

    [[nodiscard]] tag &get_owner() const;

    [[nodiscard]] bool has_owner() const;

    void add_pixel(int x, int y);

    void add_outline(int x, int y, province &other);

    [[nodiscard]] const std::vector<std::array<int, 2> > &get_pixels() const;

    [[nodiscard]] const std::vector<std::pair<province *, std::array<int, 2> > > &get_outline() const;

    void expand_bounds(int x, int y);

    void recolor(map_modes mode);

    [[nodiscard]] unsigned int get_base_color() const;

    [[nodiscard]] double distance(const province &other) const;

    [[nodiscard]] std::vector<province *> get_path_to(const province &destination,
                                                      bool (*accessible)(const province &, void *),
                                                      double (*cost_modifier)(const province &, void *), void *param);

    [[nodiscard]] unsigned int get_num_pixels() const;

    [[nodiscard]] unsigned int get_num_outline() const;

    [[nodiscard]] const std::array<int, 4> &get_bounds() const;

    [[nodiscard]] const std::array<int, 2> &get_center() const;

    [[nodiscard]] const std::vector<std::unique_ptr<pop> > &get_pops() const;

    [[nodiscard]] const std::unordered_map<province *, double> &get_neighbors() const;

    void tick();
};
