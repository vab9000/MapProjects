#ifndef MECHANICS_BUILDING
#define MECHANICS_BUILDING
#include <reference.hpp>

namespace mechanics {
    class province;
    class pop;
    class tag;
    class character;

    enum class goods_t {};

    enum class building_t {};

    enum class owner_type : unsigned char {
        pop, tag, character,
    };

    struct employee {
        utils::ref<pop> pop;
        unsigned int amount;
        unsigned int wage;
    };

    class building {
        utils::ref<province> location_;
        std::vector<employee> workers_;
        building_t building_type_;

        union owner_t {
            utils::ref<pop> pop;
            utils::ref<tag> tag;
            utils::ref<character> character;
        } owner_;

        owner_type owner_type_;

    public:
        building(utils::ref<province> location, building_t type, pop &owner);

        building(utils::ref<province> location, building_t type, tag &owner);

        building(utils::ref<province> location, building_t type, character &owner);
    };
}

#endif
