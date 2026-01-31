#ifndef MECHANICS_BUILDING
#define MECHANICS_BUILDING
#include <reference.hpp>

namespace mechanics {
    class province;
    class pop;
    class tag;
    class character;

    class building {
    public:
        enum class owner_type_t : unsigned char {
            pop, tag, character,
        };

        struct employee {
            utils::ref<pop> pop;
            unsigned int amount;
            unsigned int wage;
        };

        template<owner_type_t OwnerType>
        using type_from_owner = std::conditional_t<OwnerType == owner_type_t::pop, pop &,
            std::conditional_t<OwnerType == owner_type_t::tag, tag &,
                std::conditional_t<OwnerType == owner_type_t::character, character &, void>>>;

    private:
        utils::ref<province> location_;
        std::vector<employee> workers_;

        union owner_t {
            utils::ref<pop> pop;
            utils::ref<tag> tag;
            utils::ref<character> character;
        } owner_;

        owner_type_t owner_type_;

    protected:
        building(utils::ref<province> location, pop &owner);

        building(utils::ref<province> location, tag &owner);

        building(utils::ref<province> location, character &owner);

    public:
        [[nodiscard]] auto location() const -> province &;

        [[nodiscard]] auto workers() const -> const std::vector<employee> &;

        [[nodiscard]] auto owner_type() const -> owner_type_t;

        template<owner_type_t OwnerType>
        [[nodiscard]] auto owner() const -> type_from_owner<OwnerType>;
    };

    template<building::owner_type_t OwnerType>
    auto building::owner() const -> type_from_owner<OwnerType> {
        if (OwnerType != owner_type_) { throw std::bad_variant_access(); }
        if constexpr (OwnerType == owner_type_t::pop) { return owner_.pop; }
        else if constexpr (OwnerType == owner_type_t::tag) { return owner_.tag; }
        else if constexpr (OwnerType == owner_type_t::character) { return owner_.character; }
        return;
    }
}
#endif
