#include "river.hpp"

void river::add_province(province *prov) {
    provinces_.push_back(prov);
}

bool river::contains(const province *prov) const {
    return std::ranges::find(provinces_, prov) != provinces_.end();
}
