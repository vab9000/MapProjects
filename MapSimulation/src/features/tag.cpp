#include "tag.hpp"
#include <algorithm>
#include "province.hpp"
#include "army.hpp"

id_generator tag::id_gen_;

tag::tag() : tag(0) {
}

tag::tag(const uint_fast32_t color) : color_(color), id(id_gen_.next_id()) {
}

tag::~tag() {
    id_gen_.return_id(id);
}

uint_fast32_t tag::color() const {
    return color_;
}

void tag::set_color(const uint_fast32_t color) {
    color_ = color;
}

int_fast32_t tag::gold() const {
    return gold_;
}

void tag::add_gold(const int_fast32_t amount) {
    gold_ += amount;
}

void tag::remove_gold(const int_fast32_t amount) {
    gold_ -= amount;
}

[[nodiscard]] army &tag::new_army() {
    armies_.emplace_back(this);
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

[[nodiscard]] const std::list<province *> &tag::provinces() const {
    return provinces_;
}

std::list<province *> & tag::provinces() {
    return provinces_;
}

[[nodiscard]] bool tag::has_army_access(const province &access_province) const {
    return &access_province.owner() == this;
}

void tag::tick() {
}
