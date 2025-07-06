#include "province.hpp"
#include <algorithm>
#include <array>
#include <ranges>
#include <cmath>
#include <vector>
#include "tag.hpp"

province::province() : koppen_(koppen_t::none), elevation_(elevation_t::none), vegetation_(vegetation_t::none),
                       soil_(soil_t::none), sea_(sea_t::none), base_color_(0), color_(0) {
}

province::province(const uint_fast32_t color, const koppen_t koppen,
                   const elevation_t elevation,
                   const vegetation_t vegetation, const soil_t soil, const sea_t sea) : koppen_(koppen),
    elevation_(elevation),
    vegetation_(vegetation),
    soil_(soil), sea_(sea), base_color_(color), color_(color) {
}

void province::finalize(const std::vector<std::array<uint_fast32_t, 2> > &pixels) {
    size_ = pixels.size();

    auto x = std::vector<uint_fast32_t>(size_);
    auto y = std::vector<uint_fast32_t>(size_);

    for (const auto &pixel: pixels) {
        x.push_back(pixel[0]);
        y.push_back(pixel[1]);
    }

    std::ranges::sort(x);
    std::ranges::sort(y);

    std::array<uint_fast32_t, 2> test_center = {0, 0};

    if (size_ % 2 == 0) {
        test_center[0] = (x[size_ / 2] + x[size_ / 2 - 1]) / 2;
        test_center[1] = (y[size_ / 2] + y[size_ / 2 - 1]) / 2;
    } else {
        test_center[0] = x[size_ / 2];
        test_center[1] = y[size_ / 2];
    }

    for (uint_fast32_t i = 0; i < size_; ++i) {
        if (pixels[i][0] == test_center[0] && pixels[i][1] == test_center[1]) {
            center_[0] = test_center[0];
            center_[1] = test_center[1];
            break;
        }
    }

    auto distance = [](const std::array<uint_fast32_t, 2> &a, const std::array<uint_fast32_t, 2> &b) {
        return sqrt(
            pow(static_cast<int_fast32_t>(a[0]) - static_cast<int_fast32_t>(b[0]), 2) + pow(
                static_cast<int_fast32_t>(a[1]) - static_cast<int_fast32_t>(b[1]), 2));
    };

    if (center_[0] == -1) {
        double min_distance = (std::numeric_limits<double>::max)();
        for (uint_fast32_t i = 0; i < size_; ++i) {
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

void province::set_owner(tag *new_owner) {
    if (has_owner() && owner_->has_province(*this)) {
        owner_->remove_province(*this);
    }
    owner_ = std::move(new_owner);
    if (has_owner()) {
        owner_->add_province(*this);
    }
}

void province::remove_owner() {
    if (has_owner()) {
        owner_->remove_province(*this);
    }
    owner_ = nullptr;
}

const tag &province::owner() const {
    return *owner_;
}

bool province::has_owner() const {
    return owner_ != nullptr;
}

void province::add_pop(pop new_pop) {
    pops_.emplace_back(std::move(new_pop));
}

void province::remove_pop(pop *p) {
    std::erase_if(pops_, [p](const pop &elem) {
        return &elem == p;
    });
}

void province::add_river_boundary(province *neighbor, const uint_fast8_t size) {
    river_boundaries_[neighbor] = size;
}

void province::add_river(river *r) {
    rivers_.emplace_back(r);
}

void province::add_neighbor(province *neighbor) {
    neighbors_.emplace(neighbor, 0.0);
}

void province::expand_bounds(const uint_fast32_t x, const uint_fast32_t y) {
    if (size_ == 0) {
        bounds_[0] = x;
        bounds_[1] = y;
        bounds_[2] = x;
        bounds_[3] = y;
        size_ += 1;
        return;
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

void province::recolor(const map_mode_t mode) {
    if (mode == map_mode_t::provinces) {
        color_ = base_color_;
    } else if (mode == map_mode_t::owner) {
        if (has_owner()) {
            color_ = owner_->color();
        } else {
            color_ = 0xFFFFFFFF;
        }
    }
    switch (mode) {
        case map_mode_t::provinces:
            color_ = base_color_;
            break;
        case map_mode_t::owner:
            if (has_owner()) {
                color_ = owner_->color();
            } else {
                color_ = 0xFFFFFFFF;
            }
            break;
        case map_mode_t::koppen:
            color_ = static_cast<uint_fast32_t>(koppen_);
            break;
        case map_mode_t::elevation:
            color_ = static_cast<uint_fast32_t>(elevation_);
            break;
        case map_mode_t::vegetation:
            color_ = static_cast<uint_fast32_t>(vegetation_);
            break;
        case map_mode_t::soil:
            color_ = static_cast<uint_fast32_t>(soil_);
            break;
        case map_mode_t::sea:
            color_ = static_cast<uint_fast32_t>(sea_);
            break;
    }
}

uint_fast32_t province::base_color() const {
    return base_color_;
}

uint_fast32_t province::color() const {
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
    return sqrt(pow(static_cast<int_fast32_t>(center_[0]) - static_cast<int_fast32_t>(other.center_[0]), 2) +
                pow(static_cast<int_fast32_t>(center_[1]) - static_cast<int_fast32_t>(other.center_[1]), 2));
}

void province::tick() {
    if (has_owner()) {
        owner_->add_gold(1);
    }
}

const std::array<uint_fast32_t, 4> &province::bounds() const {
    return bounds_;
}

const std::array<uint_fast32_t, 2> &province::center() const {
    return center_;
}

const pop_container &province::pops() const {
    return pops_;
}

pop_container &province::pops() {
    return pops_;
}

const std::map<province *, double_t> &province::neighbors() const {
    return neighbors_;
}

const std::map<province *, uint_fast8_t> &province::river_boundaries() const {
    return river_boundaries_;
}

const std::vector<river *> &province::rivers() const {
    return rivers_;
}
