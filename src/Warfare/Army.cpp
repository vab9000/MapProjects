#include "Army.hpp"
#include <vector>
#include "../Base/AI.hpp"
#include "../Base/Province.hpp"
#include "../Populations/Character.hpp"
#include "../Tags/Tag.hpp"

Unit::Unit(Army *army, Province *location) {
	size = 0;
	this->location = location;
	this->army = army;
	army->units->emplace_back(this);
	path = new std::vector<Province *>();
	travelProgress = 0;
	general = nullptr;
	retreating = false;
}

Unit::~Unit() { delete path; }

void Unit::setDestination(Province *destination) {
	const auto generatedPath = location->getPathTo(
	        destination,
	        [](const Province &province, void *param) {
		        const auto unit = static_cast<Unit *>(param);
		        return unit->army->tag->hasArmyAccess(province);
	        },
	        [](const Province &, void *) { return 1.0; }, this);
	for (const auto &province: generatedPath) {
		path->emplace_back(province);
	}
}

Army::Army(Tag *tag) {
	this->tag = tag;
	units = new std::vector<Unit *>();
	ai = new AI();
	directive = {.type = ArmyDirectiveType::ATTACK, .target = nullptr};
	tag->armies->emplace_back(this);
}

Army::~Army() {
	for (const auto &unit: *units) {
		delete unit;
	}

	delete units;
	delete ai;
}
