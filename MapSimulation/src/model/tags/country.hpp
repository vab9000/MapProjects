#pragma once

#include <string>
#include <vector>
#include "../warfare/army.hpp"
#include "tag.hpp"

class province;

class country final : public tag {
    std::vector<province *> provinces_;

public:
    country() {
        provinces_ = std::vector<province *>();
    }

    explicit country(const std::string &name, const unsigned int color) : tag(name, color) {
        provinces_ = std::vector<province *>();
    }

    ~country() override = default;

    void add_province(province *province) { provinces_.emplace_back(province); }

    void remove_province(province *province) { std::erase(provinces_, province); }

    bool has_province(const province *province) const {
        return std::ranges::find(provinces_, province) != provinces_.end();
    }

    [[nodiscard]] std::vector<province *> get_provinces() const { return provinces_; }

    [[nodiscard]] bool has_army_access(const province &province) const override {
        return true;
    }
};
