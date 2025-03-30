#include "Province.hpp"
#include <algorithm>
#include <array>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../Populations/Pop.hpp"
#include "../Tags/Country.hpp"
#include "Utils.hpp"

province::province(const std::string &name, const unsigned int color, const int i, const int j) {
    distances_processed_ = false;
    this->name = name;
    this->color = color;
    base_color = color;
    bounds_[0] = i;
    bounds_[1] = j;
    bounds_[2] = i;
    bounds_[3] = j;
    num_pixels_ = 0;
    num_outline_ = 0;
    pixels_ = std::vector<std::array<int, 2> >();
    outline_ = std::vector<std::pair<province *, std::array<int, 2> > >();
    center_[0] = -1;
    center_[1] = -1;
    owner_ = nullptr;
    pops_ = std::vector<pop *>();
    neighbors_ = std::unordered_map<province *, double>();
}

void province::finalize() {
    const auto total_pixels = num_pixels_ + num_outline_;

    auto x = std::vector<int>(total_pixels);
    auto y = std::vector<int>(total_pixels);

    std::ranges::sort(x);
    std::ranges::sort(y);

    int test_center[2] = {0, 0};

    if (total_pixels % 2 == 0) {
        test_center[0] = (x[total_pixels / 2] + x[total_pixels / 2 - 1]) / 2;
        test_center[1] = (y[total_pixels / 2] + y[total_pixels / 2 - 1]) / 2;
    } else {
        test_center[0] = x[total_pixels / 2];
        test_center[1] = y[total_pixels / 2];
    }

    for (int i = 0; i < num_pixels_; ++i) {
        if (pixels_[i][0] == test_center[0] && pixels_[i][1] == test_center[1]) {
            center_[0] = test_center[0];
            center_[1] = test_center[1];
            break;
        }
    }

    for (int i = 0; i < num_outline_; ++i) {
        if (outline_[i].second[0] == test_center[0] && outline_[i].second[1] == test_center[1]) {
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
        for (int i = 0; i < num_pixels_; ++i) {
            if (const auto dist = distance(pixels_[i], {test_center[0], test_center[1]}); dist < min_distance) {
                min_distance = dist;
                center_[0] = pixels_[i][0];
                center_[1] = pixels_[i][1];
            }
        }
        for (int i = 0; i < num_outline_; ++i) {
            if (const auto dist = distance(outline_[i].second, {test_center[0], test_center[1]}); dist < min_distance) {
                min_distance = dist;
                center_[0] = outline_[i].second[0];
                center_[1] = outline_[i].second[1];
            }
        }
    }
}

void province::process_distances() {
    for (const auto neighbor: neighbors_ | std::views::keys) {
        neighbors_[neighbor] = this->distance(*neighbor);
    }
    distances_processed_ = true;
}

void province::set_owner(const country &new_owner) {
    if (owner_ != nullptr && owner_->has_province(this)) {
        owner_->remove_province(this);
    }
    owner_ = &const_cast<country &>(new_owner);
}

[[nodiscard]] country *province::get_owner() const { return owner_; }

void province::add_pixel(const int x, const int y) {
    pixels_.push_back({x, y});
    num_pixels_ += 1;
}

void province::add_outline(const int x, const int y, const province &other) {
    outline_.emplace_back(&const_cast<province &>(other), std::array{x, y});
    num_outline_ += 1;
    neighbors_.emplace(&const_cast<province &>(other), 0.0);
}

[[nodiscard]] const std::array<int, 2> *province::get_pixels() const { return pixels_.data(); }

[[nodiscard]] const std::pair<province *, std::array<int, 2> > *province::get_outline() const { return outline_.data(); }

void province::expand_bounds(const int x, const int y) {
    if (x < bounds_[0]) {
        bounds_[0] = x;
    } else if (x > bounds_[2]) {
        bounds_[2] = x;
    } else if (y < bounds_[1]) {
        bounds_[1] = y;
    }
    if (y > bounds_[3]) {
        bounds_[3] = y;
    }
}

void province::recolor(const map_modes mode) {
    if (mode == map_modes::provinces) {
        color = base_color;
    } else if (mode == map_modes::owner) {
        if (owner_ != nullptr) {
            color = owner_->color;
        } else {
            color = 0xFFFFFFFF;
        }
    }
}

[[nodiscard]] double province::distance(const province &other) const {
    if (distances_processed_ && neighbors_.contains(&const_cast<province &>(other))) {
        return neighbors_.at(&const_cast<province &>(other));
    }
    return sqrt((center_[0] - other.center_[0]) * (center_[0] - other.center_[0]) +
                (center_[1] - other.center_[1]) * (center_[1] - other.center_[1]));
}

[[nodiscard]] std::vector<province *> province::get_path_to(const province &destination,
                                                          bool (*accessible)(const province &, void *),
                                                          double (*cost_modifier)(const province &, void *),
                                                          void *param) {
    std::unordered_map<province *, double> distances;
    std::unordered_map<province *, province *> previous;
    std::priority_queue<std::pair<double, province *>, std::vector<std::pair<double, province *> >, std::greater<> >
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

        for (const auto &[neighborProvince, neighborDistance]: currentProvince->neighbors_) {
            if (neighborProvince == nullptr || !accessible(*neighborProvince, param) ||
                cost_modifier(*neighborProvince, param) < 0) {
                continue;
            }

            if (double new_distance = currentDistance + cost_modifier(*neighborProvince, param) * neighborDistance;
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
    if (owner_ != nullptr) {
        owner_->gold += 1;
    }
}

[[nodiscard]] unsigned int province::get_num_pixels() const { return num_pixels_; }

[[nodiscard]] unsigned int province::get_num_outline() const { return num_outline_; }

[[nodiscard]] const int *province::get_bounds() const { return bounds_; }

[[nodiscard]] const int *province::get_center() const { return center_; }

[[nodiscard]] const std::vector<pop *> &province::get_pops() const { return pops_; }

[[nodiscard]] const std::unordered_map<province *, double> &province::get_neighbors() const { return neighbors_; }
