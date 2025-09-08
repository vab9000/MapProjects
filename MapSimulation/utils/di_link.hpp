#pragma once
#include "reference.hpp"

namespace utils {
    /// A bidirectional link between two objects of different types.
    /// When one object is destroyed, the link in the other object is set to null.
    /// The link can be released or relinked to another object.
    /// The link is designed to be stored in its owner object,
    /// doing otherwise is not recommended.
    /// @tparam T The type of the owner object.
    /// @tparam U The type of the linked object.
    template<typename T, typename U>
    class di_link {
        ref<T> value_;
        di_link<U, T> *link_{nullptr};

        friend class di_link<U, T>;

        auto self() const -> T & { return value_; }

    public:
        /// Create a new link without linking it to another object.
        /// @param value The owner object.
        explicit di_link(T &value) : value_(value) {}

        /// Create a new link and link it to another object.
        /// @param value The owner object.
        /// @param other The link stored in the other object to connect to.
        di_link(T &value, di_link<U, T> &other) : value_(value), link_(&other) {
            other.link_ = this;
        }

        di_link(di_link &&other) noexcept : value_(other.value_) {
            auto link = other.link_;
            other.link_ = nullptr;
            if (link != nullptr) {
                link->link_ = this;
                link_ = link;
            }
        }

        auto operator=(di_link &&other) noexcept -> di_link & {
            release();
            value_ = other.value_;
            auto link = other.link_;
            other.link_ = nullptr;
            if (link != nullptr) {
                link->link_ = this;
            }
            link_ = link;
            return *this;
        }

        ~di_link() {
            if (link_ != nullptr) {
                link_->link_ = nullptr;
            }
        }

        di_link(const di_link &) = delete;

        auto operator=(const di_link &) = delete;

        /// Get the linked object.
        /// @returns A reference to the linked object.
        /// @warning Undefined behavior if the link is not set.
        auto get() const -> U & { return link_->self(); }

        /// Check if the link is set.
        /// @returns True if the link is set, false otherwise.
        [[nodiscard]] auto has_value() const -> bool { return link_ != nullptr; }

        /// Release the link to the other object. The link in the other object is also released.
        auto release() -> void {
            if (link_ != nullptr) {
                link_->link_ = nullptr;
                link_ = nullptr;
            }
        }

        /// Relink to another object. The link in the previous object is released.
        /// @param other The link stored in the other object to connect to.
        auto relink(di_link<U, T> &other) -> void {
            release();
            link_ = &other;
            other.link_ = this;
        }
    };
}
