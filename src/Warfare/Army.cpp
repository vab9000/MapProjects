#include "Army.hpp"
#include <vector>
#include "../Base/AI.hpp"
#include "../Base/Province.hpp"
#include "../Populations/Character.hpp"
#include "../Tags/Tag.hpp"

Unit::Unit(const Army &army, const Province &location) {
	size = 0;
	this->location = &const_cast<Province &>(location);
	this->army = &const_cast<Army &>(army);
	path = std::vector<Province *>();
	travelProgress = 0;
	general = nullptr;
	retreating = false;
	speed = 1.0;
}

void Unit::setDestination(const Province &destination) {
	const auto generatedPath = location->getPathTo(
	        destination,
	        [](const Province &province, void *param) {
		        const auto unit = static_cast<Unit *>(param);
		        return unit->army->tag->hasArmyAccess(province);
	        },
	        [](const Province &, void *) { return 1.0; }, this);
	for (const auto &province: generatedPath) {
		path.emplace_back(province);
	}
}

void Unit::move() {
	travelProgress += speed;
	if (travelProgress >= 100) {
		location = path.at(0);
		path.erase(path.begin());
		travelProgress = 0;
	}
}

Army::Army(const Tag &tag) {
	this->tag = &const_cast<Tag &>(tag);
	units = std::vector<Unit *>();
	ai = AI();
	directive = {.type = ArmyDirectiveType::ATTACK, .target = nullptr};
}

Army::~Army() {
	for (const auto &unit: units) {
		delete unit;
	}
}

[[nodiscard]] Unit *Army::newUnit(const Province &location) {
	auto unit = new Unit(*this, location);
	units.emplace_back(unit);
	return unit;
}
