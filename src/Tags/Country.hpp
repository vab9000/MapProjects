//
// Created by varun on 2/1/2025.
//

#ifndef COUNTRY_HPP
#define COUNTRY_HPP

#include <vector>
#include <string>
#include "../Base/Province.hpp"

class Country {
	std::vector<Province *> *provinces;

public:
	std::string name;
	unsigned int color;

	Country() {
		name = "";
		color = 0;
		provinces = new std::vector<Province *>();
	}

	Country(const std::string &name) {
		this->name = name;
		color = 0;
		provinces = new std::vector<Province *>();
	}

	~Country() {
		delete provinces;
	}

	void addProvince(Province *province) {
		provinces->emplace_back(province);
	}

	void removeProvince(Province *province) {
		provinces->erase(std::remove(provinces->begin(), provinces->end(), province), provinces->end());
	}

	bool hasProvince(Province *province) {
		return std::find(provinces->begin(), provinces->end(), province) != provinces->end();
	}

	[[nodiscard]] std::vector<Province *> *getProvinces() const {
		return provinces;
	}
};



#endif //COUNTRY_HPP
