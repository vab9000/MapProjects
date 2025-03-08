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
	units = std::vector<std::unique_ptr<Unit>>();
	directive = {.type = ArmyDirectiveType::ATTACK, .target = nullptr};
	commander = nullptr;
}

void setUnitDestination(void* sParam, void* oParam) {
	const auto unit = static_cast<Unit *>(sParam);
	const auto province = static_cast<Province*>(oParam);
    unit->setDestination(*province);
}

int getSetUnitDestinationWeight(void* sParam, void* oParam) {
    // const auto unit = static_cast<Unit *>(sParam);
    // const auto province = static_cast<Province*>(oParam);
    return 100;
}

[[nodiscard]] Unit *Army::newUnit(const Province &location) {
	auto unit = std::make_unique<Unit>(*this, location);
	units.push_back(std::move(unit));
	return units.back().get();
}
