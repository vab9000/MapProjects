#ifndef COUNTRY_HPP
#define COUNTRY_HPP

#include <string>
#include <vector>
#include "../Warfare/Army.hpp"
#include "Tag.hpp"

class Province;

class Country final : public Tag {
	std::vector<Province *> provinces;

public:
	Country() {
		provinces = std::vector<Province *>();
	}

	explicit Country(const std::string &name, const unsigned int color) : Tag(name, color) {
		provinces = std::vector<Province *>();
	}

	~Country() override = default;

	void addProvince(Province *province) { provinces.emplace_back(province); }

	void removeProvince(Province *province) { std::erase(provinces, province); }

	bool hasProvince(const Province *province) const {
		return std::ranges::find(provinces, province) != provinces.end();
	}

	[[nodiscard]] std::vector<Province *> getProvinces() const { return provinces; }

	[[nodiscard]] bool hasArmyAccess(const Province &province) const override {
		return true;
	}
};


#endif // COUNTRY_HPP
