#ifndef PROVINCE_HPP
#define PROVINCE_HPP

#include <string>
#include <vector>
#include <utility>
#include <array>

class Country;
enum class MapMode;
class Pop;

class Province {
	std::vector<std::array<int, 2>> *pixels;
	std::vector<std::pair<Province *, std::array<int, 2>>> *outline;
	std::array<int, 2> *lockedPixels{};
	std::pair<Province *, std::array<int, 2>> *lockedOutline{};
	std::vector<Pop *> *pops;
	bool locked{};
	Country *owner;

public:
	std::string name;
	unsigned int color;
	unsigned int baseColor;
	unsigned int numPixels;
	unsigned int numOutline;
	int bounds[4]{};
	int center[2]{};

	Province(const std::string &name, unsigned int color, int i, int j);

	~Province();

	void lock();

	void setOwner(Country *newOwner);

	[[nodiscard]] Country *getOwner() const;

	void addPixel(int x, int y);

	void addOutline(int x, int y, Province *other);

	[[nodiscard]] std::array<int, 2> *getPixels() const;

	[[nodiscard]] std::pair<Province *, std::array<int, 2>> *getOutline() const;

	void expandBounds(int x, int y);

	void recolor(MapMode mode);

	[[nodiscard]] unsigned int distance(const Province& other) const;
};

#endif // PROVINCE_HPP
