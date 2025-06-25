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
enum class koppen : unsigned int;
enum class elevation : unsigned int;
enum class vegetation : unsigned int;

std::string_view koppen_to_string(koppen value);
std::string_view elevation_to_string(elevation value);
std::string_view vegetation_to_string(vegetation value);

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
    koppen koppen_;
    elevation elevation_;
    vegetation vegetation_;

public:
    unsigned int color;

    province();

    province(std::string name, unsigned int color, koppen koppen, elevation elevation, vegetation vegetation);

    void finalize();

    void process_distances();

    std::string_view name();

    void set_owner(tag &new_owner);

    void remove_owner();

    [[nodiscard]] tag *owner() const;

    [[nodiscard]] bool has_owner() const;

    void add_pixel(int x, int y);

    void add_outline(int x, int y, province &other);

    [[nodiscard]] const std::vector<std::array<int, 2> > &pixels() const;

    [[nodiscard]] const std::vector<std::pair<province *, std::array<int, 2> > > &outline() const;

    void expand_bounds(int x, int y);

    void recolor(map_modes mode);

    [[nodiscard]] unsigned int base_color() const;

    [[nodiscard]] koppen koppen() const;

    [[nodiscard]] elevation elevation() const;

    [[nodiscard]] vegetation vegetation() const;

    [[nodiscard]] double distance(const province &other) const;

    [[nodiscard]] std::vector<province *> path_to(const province &destination,
                                                      bool (*accessible)(const province &, void *),
                                                      double (*cost_modifier)(const province &, void *), void *param);

    [[nodiscard]] unsigned int num_pixels() const;

    [[nodiscard]] unsigned int num_outline() const;

    [[nodiscard]] const std::array<int, 4> &bounds() const;

    [[nodiscard]] const std::array<int, 2> &center() const;

    [[nodiscard]] const std::vector<std::unique_ptr<pop> > &pops() const;

    [[nodiscard]] const std::unordered_map<province *, double> &neighbors() const;

    void tick();
};
