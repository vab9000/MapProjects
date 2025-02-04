//
// Created by varun on 2/1/2025.
//

#ifndef ARMY_HPP
#define ARMY_HPP

#include <vector>
#include "../Characters/Character.hpp"
#include "../Base/AI.hpp"

class Tag;
class Province;
class Army;

enum class ArmyDirectiveType {
	ATTACK
};

struct ArmyDirective {
    ArmyDirectiveType type;
    void *target;
};

class Unit {
public:
	unsigned int size;
	Province *location;
	Army *army;
	std::vector<Province *> *path;
	unsigned int travelProgress;
	Character *general;

	Unit() {
		size = 0;
		location = nullptr;
		army = nullptr;
		path = new std::vector<Province *>();
		travelProgress = 0;
		general = nullptr;
	}

	~Unit() {
		delete path;
	}
};

class Army {
	Tag *tag;
	std::vector<Unit *> *units;
	AI *ai;
	ArmyDirective directive{};

public:
	explicit Army(Tag *tag) {
		this->tag = tag;
		units = new std::vector<Unit *>();
		ai = new AI();
		directive = {
            .type = ArmyDirectiveType::ATTACK,
            .target = nullptr
        };
	}

	~Army() {
		for (const auto &unit: *units) {
            delete unit;
        }

		delete units;
		delete ai;
	}
};


#endif // ARMY_HPP
