//
// Created by varun on 2/1/2025.
//

#ifndef ARMY_HPP
#define ARMY_HPP

#include <vector>
#include "../Base/AI.hpp"
#include "Unit.hpp"

class Tag;

enum class ArmyDirectiveType {
	ATTACK
};

struct ArmyDirective {
    ArmyDirectiveType type;
    void *target;
};

class Army {
	Tag *tag;
	std::vector<Unit *> *units;
	AI *ai;
	ArmyDirective directive;

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
