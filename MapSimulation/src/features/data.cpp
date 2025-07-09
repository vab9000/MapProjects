#include "data.hpp"

data data::instance_ = data();

const date &data::current_date() const {
    return current_date_;
}

const std::unordered_map<uint_fast32_t, province> &data::provinces() const {
    return provinces_;
}

std::unordered_map<uint_fast32_t, province> &data::provinces() {
    return provinces_;
}

void data::add_province(province &&p) {
    provinces_[p.base_color()] = std::move(p);
}

const std::unordered_map<uint_fast32_t, tag> &data::tags() const {
    return tags_;
}

std::unordered_map<uint_fast32_t, tag> &data::tags() {
    return tags_;
}

const std::unordered_map<uint_fast32_t, character> &data::characters() const {
    return characters_;
}

std::unordered_map<uint_fast32_t, character> &data::characters() {
    return characters_;
}

const std::vector<std::unique_ptr<river> > &data::rivers() const {
    return rivers_;
}

river &data::add_river() {
    return *rivers_.emplace_back(std::make_unique<river>());
}

data &data::instance() {
    return instance_;
}
