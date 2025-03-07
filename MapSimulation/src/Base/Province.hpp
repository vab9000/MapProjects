#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Utils.hpp"


class Country;
enum class MapMode;
class Pop;

class Province {
	std::vector<std::array<int, 2>> pixels;
	std::vector<std::pair<Province *, std::array<int, 2>>> outline;
	std::vector<Pop *> pops;
	bool distancesProcessed{};
	Country *owner;
	std::unordered_map<Province *, double> neighbors;
	unsigned int numPixels;
	unsigned int numOutline;
	int bounds[4]{};
	int center[2]{};

public:
	unsigned int color;
	unsigned int baseColor;
	std::string name;

	Province(const std::string &name, unsigned int color, int i, int j);

	void finalize();

	void processDistances();

	void setOwner(const Country &newOwner);

	[[nodiscard]] Country *getOwner() const;

	void addPixel(int x, int y);

	void addOutline(int x, int y, const Province &other);

	[[nodiscard]] const std::array<int, 2> *getPixels() const;

	[[nodiscard]] const std::pair<Province *, std::array<int, 2>> *getOutline() const;

	void expandBounds(int x, int y);

	void recolor(MapMode mode);

	[[nodiscard]] double distance(const Province &other) const;

	[[nodiscard]] std::vector<Province *> getPathTo(const Province &destination, bool (*accessible)(const Province &, void *),
	                                                double (*costModifier)(const Province &, void *), void *param);

	[[nodiscard]] unsigned int getNumPixels() const;

	[[nodiscard]] unsigned int getNumOutline() const;

	[[nodiscard]] const int *getBounds() const;

    [[nodiscard]] const int *getCenter() const;

    [[nodiscard]] const std::vector<Pop *> &getPops() const;

    [[nodiscard]] const std::unordered_map<Province *, double> &getNeighbors() const;

	void tick();
};
