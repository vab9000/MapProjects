#include "Army.hpp"
#include <queue>
#include <unordered_map>
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
	if (location == nullptr || destination == nullptr) {
		return;
	}

	std::unordered_map<Province *, double> distances;
	std::unordered_map<Province *, Province *> previous;
	std::priority_queue<std::pair<double, Province *>, std::vector<std::pair<double, Province *>>, std::greater<>>
			queue;

	distances[location] = 0;
	queue.emplace(0, location);

	auto reached = false;

	while (!queue.empty()) {
		auto [currentDistance, currentProvince] = queue.top();
		queue.pop();

		if (currentProvince == destination) {
			reached = true;
			break;
		}

		const auto outline = currentProvince->getOutline();
		for (unsigned int i = 0; i < currentProvince->numOutline; ++i) {
			auto neighborProvince = outline[i].first;
			if (neighborProvince == nullptr || !army->tag->hasArmyAccess(*neighborProvince)) {
				continue;
			}

			if (double newDistance = currentDistance + currentProvince->distance(*neighborProvince);
			    !distances.contains(neighborProvince) || newDistance < distances[neighborProvince]) {
				distances[neighborProvince] = newDistance;
				previous[neighborProvince] = currentProvince;
				queue.emplace(newDistance, neighborProvince);
			}
		}
	}

	if (!reached) {
		return;
	}
	path->clear();
	for (Province *at = destination; at != nullptr; at = previous[at]) {
		path->insert(path->begin(), at);
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
