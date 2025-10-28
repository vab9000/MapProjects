#ifndef MECHANICS_POP
#define MECHANICS_POP
#include <di_link.hpp>
#include <enums.hpp>
#include "../date.hpp"

namespace mechanics {
    class culture;
    class province;

    enum class lifestyle_t : unsigned char {};
    enum class profession_t : unsigned char {};

    struct pop_properties {
        lifestyle_t lifestyle : utils::enum_bits_required_v<lifestyle_t>;
        profession_t profession : utils::enum_bits_required_v<profession_t>;
    };

    class pop {
        utils::ref<province> location_;
        utils::di_link<pop, culture> culture_;
        unsigned int size_ = 0U;
        pop_properties properties_;

    public:
        explicit pop(province &prov, culture &cul);

        pop(pop &&other) noexcept = default;

        [[nodiscard]] auto size() const -> unsigned int;

        auto tick(tick_t tick_type) -> void;

        [[nodiscard]] auto location() const -> province &;

        auto set_location(province &prov) -> void;
    };
}
#endif
