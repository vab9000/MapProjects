#include "province.hpp"
#include <algorithm>
#include <array>
#include <queue>
#include <ranges>
#include <string>
#include <unordered_map>
#include <cmath>
#include <vector>
#include "../tags/tag.hpp"
#include "utils.hpp"

province::province() : owner_(nullptr), size_(0), bounds_({-1, -1, 0, 0}), center_({-1, -1}),
                       koppen_(koppen_t::none), elevation_(elevation_t::none), vegetation_(vegetation_t::none),
                       soil_(soil_t::none), sea_(sea_t::none), base_color_(0), color_(0) {
}

province::province(std::string name, const unsigned int color, const koppen_t koppen,
                   const elevation_t elevation,
                   const vegetation_t vegetation, const soil_t soil, const sea_t sea) : owner_(nullptr),
    size_(0),
    bounds_({-1, -1, 0, 0}), center_{-1, -1},
    name_(std::move(name)),
    koppen_(koppen), elevation_(elevation),
    vegetation_(vegetation),
    soil_(soil), sea_(sea), base_color_(color), color_(color) {
}

void province::finalize(const std::vector<std::array<int, 2> > &pixels) {
    size_ = pixels.size();

    auto x = std::vector<int>(size_);
    auto y = std::vector<int>(size_);

    for (const auto &pixel: pixels) {
        x.push_back(pixel[0]);
        y.push_back(pixel[1]);
    }

    std::ranges::sort(x);
    std::ranges::sort(y);

    int test_center[2] = {0, 0};

    if (size_ % 2 == 0) {
        test_center[0] = (x[size_ / 2] + x[size_ / 2 - 1]) / 2;
        test_center[1] = (y[size_ / 2] + y[size_ / 2 - 1]) / 2;
    } else {
        test_center[0] = x[size_ / 2];
        test_center[1] = y[size_ / 2];
    }

    for (int i = 0; i < size_; ++i) {
        if (pixels[i][0] == test_center[0] && pixels[i][1] == test_center[1]) {
            center_[0] = test_center[0];
            center_[1] = test_center[1];
            break;
        }
    }

    auto distance = [](const std::array<int, 2> &a, const std::array<int, 2> &b) {
        return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
    };

    if (center_[0] == -1) {
        double min_distance = (std::numeric_limits<double>::max)();
        for (int i = 0; i < size_; ++i) {
            if (const auto dist = distance(pixels[i], {test_center[0], test_center[1]}); dist < min_distance) {
                min_distance = dist;
                center_[0] = pixels[i][0];
                center_[1] = pixels[i][1];
            }
        }
    }
}

void province::process_distances() {
    for (const auto neighbor: neighbors_ | std::views::keys) {
        neighbors_[neighbor] = distance(*neighbor);
    }
    distances_processed_ = true;
}

std::string_view province::name() {
    return name_;
}

void province::set_owner(tag *new_owner) {
    if (has_owner() && owner_->has_province(*this)) {
        owner_->remove_province(*this);
    }
    owner_ = new_owner;
}

void province::remove_owner() {
    if (has_owner()) {
        owner_->remove_province(*this);
    }
    owner_ = nullptr;
}

tag *province::owner() const {
    return owner_;
}

bool province::has_owner() const {
    return owner_ != nullptr;
}

void province::add_river_boundary(province *neighbor, const int size) {
    river_boundaries_[neighbor] = size;
}

void province::add_river(river *r) {
    rivers_.emplace_back(r);
}

void province::add_neighbor(province *neighbor) {
    neighbors_.emplace(neighbor, 0.0);
}

void province::expand_bounds(const int x, const int y) {
    if (bounds_[0] == -1) {
        bounds_[0] = x;
        bounds_[1] = y;
        bounds_[2] = x;
        bounds_[3] = y;
    }
    if (x < bounds_[0]) {
        bounds_[0] = x;
    } else if (x > bounds_[2]) {
        bounds_[2] = x;
    }
    if (y < bounds_[1]) {
        bounds_[1] = y;
    } else if (y > bounds_[3]) {
        bounds_[3] = y;
    }
}

void province::recolor(const map_modes mode) {
    if (mode == map_modes::provinces) {
        color_ = base_color_;
    } else if (mode == map_modes::owner) {
        if (has_owner()) {
            color_ = owner_->color();
        } else {
            color_ = 0xFFFFFFFF;
        }
    }
    switch (mode) {
        case map_modes::provinces:
            color_ = base_color_;
            break;
        case map_modes::owner:
            if (has_owner()) {
                color_ = owner_->color();
            } else {
                color_ = 0xFFFFFFFF;
            }
            break;
        case map_modes::koppen:
            color_ = static_cast<unsigned int>(koppen_);
            break;
        case map_modes::elevation:
            color_ = static_cast<unsigned int>(elevation_);
            break;
        case map_modes::vegetation:
            color_ = static_cast<unsigned int>(vegetation_);
            break;
        case map_modes::soil:
            color_ = static_cast<unsigned int>(soil_);
            break;
        case map_modes::sea:
            color_ = static_cast<unsigned int>(sea_);
            break;
    }
}

unsigned int province::base_color() const {
    return base_color_;
}

unsigned int province::color() const {
    return color_;
}

koppen_t province::koppen() const {
    return koppen_;
}

elevation_t province::elevation() const {
    return elevation_;
}

vegetation_t province::vegetation() const {
    return vegetation_;
}

soil_t province::soil() const {
    return soil_;
}

sea_t province::sea() const {
    return sea_;
}

double province::distance(const province &other) const {
    if (distances_processed_ && neighbors_.contains(&const_cast<province &>(other))) {
        return neighbors_.at(&const_cast<province &>(other));
    }
    return sqrt((center_[0] - other.center_[0]) * (center_[0] - other.center_[0]) +
                (center_[1] - other.center_[1]) * (center_[1] - other.center_[1]));
}

std::vector<province *> province::path_to(const province &destination,
                                          bool (*accessible)(const province &start, const province &end, void *param),
                                          double (*cost_modifier)(const province &start, const province &end,
                                                                  void *param),
                                          void *param) {
    std::unordered_map<province *, double> distances;
    std::unordered_map<province *, province *> previous;
    std::priority_queue<std::pair<double, province *>, std::vector<std::pair<double,
                province *> >, std::greater<> >
            queue;

    distances[this] = 0;
    queue.emplace(0, this);

    auto reached = false;

    while (!queue.empty()) {
        auto [currentDistance, currentProvince] = queue.top();
        queue.pop();

        if (currentProvince == &const_cast<province &>(destination)) {
            reached = true;
            break;
        }

        for (const auto &[neighborProvince, neighborDistance]: currentProvince->neighbors()) {
            if (!accessible(*this, *neighborProvince, param) ||
                cost_modifier(*this, *neighborProvince, param) < 0) {
                continue;
            }

            if (double new_distance = currentDistance + cost_modifier(*this, *neighborProvince, param) *
                                      neighborDistance;
                !distances.contains(neighborProvince) || new_distance < distances[neighborProvince]) {
                distances[neighborProvince] = new_distance;
                previous[neighborProvince] = currentProvince;
                queue.emplace(new_distance, neighborProvince);
            }
        }
    }

    if (!reached) {
        return {};
    }

    std::vector<province *> path;
    for (province *at = &const_cast<province &>(destination); at != nullptr; at = previous[at]) {
        path.insert(path.begin(), at);
    }

    return path;
}

void province::tick() {
    if (has_owner()) {
        owner_->add_gold(1);
    }
}

const std::array<int, 4> &province::bounds() const {
    return bounds_;
}

const std::array<int, 2> &province::center() const {
    return center_;
}

const std::list<std::unique_ptr<pop> > &province::pops() const {
    return pops_;
}

const std::unordered_map<province *, double> &province::neighbors() const {
    return neighbors_;
}

const std::unordered_map<province *, int> &province::river_boundaries() const {
    return river_boundaries_;
}

const std::vector<river *> &province::rivers() const {
    return rivers_;
}
