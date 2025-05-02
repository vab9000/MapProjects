#include "tag.hpp"

#include <utility>
#include "../base/province.hpp"
#include "../warfare/army.hpp"

tag::tag() : tag("", 0) {
}

tag::tag(std::string name, const unsigned int color) : name_(std::move(name)), color_(color) {
}

tag::~tag() = default;

unsigned int tag::get_color() const {
    return color_;
}

void tag::set_color(const unsigned int color) {
    color_ = color;
}

int tag::get_gold() const {
    return gold_;
}

void tag::add_gold(const int amount) {
    gold_ += amount;
}

void tag::remove_gold(const int amount) {
    gold_ -= amount;
}

std::string_view tag::get_name() const {
    return name_;
}

void tag::set_name(const std::string &name) {
    name_ = name;
}

[[nodiscard]] army &tag::new_army() {
    armies_.emplace_back(army(*this));
    return armies_.back();
}

void tag::add_province(province &added_province) {
    provinces_.emplace_back(&added_province);
}

void tag::remove_province(const province &removed_province) {
    std::erase(provinces_, &removed_province);
}

bool tag::has_province(const province &found_province) const {
    return std::ranges::find(provinces_, &found_province) != provinces_.end();
}

[[nodiscard]] const std::vector<province *> &tag::get_provinces() const {
    return provinces_;
}

[[nodiscard]] bool tag::has_army_access(const province &access_province) const {
    return &access_province.get_owner() == this;
}

void tag::tick() {
}