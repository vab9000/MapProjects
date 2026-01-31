#ifndef MECHANICS_DATA
#define MECHANICS_DATA
#include <unordered_map>
#include "date.hpp"
#include "event.hpp"
#include "life/character.hpp"
#include "province/province.hpp"
#include "society/culture.hpp"
#include "society/tag.hpp"

namespace mechanics {
    class data {
        inline static std::vector<province> provinces_{};
        inline static std::unordered_map<unsigned int, utils::ref<province>> provinces_map_{};
        inline static std::vector<std::unique_ptr<tag>> tags_{};
        inline static std::vector<std::unique_ptr<character>> characters_{};
        inline static std::vector<std::unique_ptr<culture>> cultures_{};
        inline static std::priority_queue<std::unique_ptr<event>, std::vector<std::unique_ptr<event>>, std::greater<>> events_{};
        inline static date current_date_{};

    public:
        // Get the current date
        [[nodiscard]] static auto current_date() -> const date &;

        static auto provinces() -> std::vector<province> &;

        // Add a province
        template<typename... Args>
        static auto emplace_province(Args &&... args) -> province & {
            auto &val = provinces_.emplace_back(std::forward<Args>(args)...);
            return val;
        }

        static auto lock_provinces() -> void;

        [[nodiscard]] static auto province_at(unsigned int color) -> province &;

        static auto add_event(event &&e) -> void;

        // Advance the simulation
        static auto tick() -> void;
    };
}
#endif
