#ifndef COUNTRY_HPP
#define COUNTRY_HPP

#include <string>
#include <vector>
#include "../Base/AI.hpp"
#include "../Warfare/Army.hpp"
#include "Tag.hpp"

class Province;

class Country : public Tag {
	std::vector<Province *> *provinces;
	std::vector<Army *> *armies;
	AI *ai;

public:
	Country() {
		provinces = new std::vector<Province *>();
		armies = new std::vector<Army *>();
		ai = new AI();
	}

	explicit Country(const std::string &name, const unsigned int color) : Tag(name, color) {
		provinces = new std::vector<Province *>();
		armies = new std::vector<Army *>();
		ai = new AI();
	}

	~Country() {
		for (const auto &army: *armies) {
			delete army;
		}

		delete provinces;
		delete armies;
		delete ai;
	}

	void addProvince(Province *province) const { provinces->emplace_back(province); }

	void removeProvince(Province *province) const { std::erase(*provinces, province); }

	bool hasProvince(const Province *province) const {
		return std::ranges::find(*provinces, province) != provinces->end();
	}

	[[nodiscard]] std::vector<Province *> *getProvinces() const { return provinces; }
};


#endif // COUNTRY_HPP
