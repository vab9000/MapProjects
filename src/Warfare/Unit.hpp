//
// Created by varun on 2/1/2025.
//

#ifndef UNIT_HPP
#define UNIT_HPP

#include <vector>

class Province;
class Army;

class Unit {
public:
	unsigned int size;
	Province *location;
	Army *army;
	std::vector<Province *> *path;
	int travelProgress;

	Unit() {
        size = 0;
		location = nullptr;
		army = nullptr;
		path = new std::vector<Province *>();
		travelProgress = 0;
    }

	~Unit() {
        delete path;
    }
};



#endif //UNIT_HPP
