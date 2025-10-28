#ifndef UTILS_NUMBERED
#define UTILS_NUMBERED
#include <limits>
#include <vector>

namespace utils {
    /// A class that gives each object a unique number.
    /// @tparam T The type of the class that inherits from numbered.
    /// @note @code T@endcode must inherit from @code numbered<T>@endcode.
    /// @note Supports objects upto @code size_t@endcode max - 1.
    template<typename T>
    class numbered {
        size_t id_;

        inline static size_t next_number_ = 0;
        inline static std::vector<size_t> free_numbers_{};

        static auto next_number() -> size_t {
            if (free_numbers_.empty()) { return next_number_++; }
            const auto number = free_numbers_.back();
            free_numbers_.pop_back();
            return number;
        }

    public:
        numbered() : id_(next_number()) {
            static_assert(std::is_base_of_v<numbered, T>,
                "T must inherit from numbered<T>");
        }

        numbered(const numbered &other) = delete;

        numbered(numbered &&other) noexcept : id_(other.id_) { other.id_ = std::numeric_limits<size_t>::max(); }

        ~numbered() noexcept(false) { if (id_ != std::numeric_limits<size_t>::max()) { free_numbers_.push_back(id_); } }

        auto operator=(const numbered &other) -> numbered & = delete;

        auto operator=(numbered &&other) noexcept -> numbered & {
            id_ = other.id_;
            other.id_ = std::numeric_limits<size_t>::max();
            return *this;
        }

        /// Get the id of the object.
        /// @return the id of the object.
        [[nodiscard]] auto id() const -> size_t { return id_; }

        /// Get the id of the next created object.
        /// @return the id.
        static auto next_id() -> size_t {
            if (free_numbers_.empty()) { return next_number_; }
            const auto number = free_numbers_.back();
            return number;
        }
    };
}
#endif
