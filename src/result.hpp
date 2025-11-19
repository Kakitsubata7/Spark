#pragma once

#include <variant>

namespace Spark {

template <typename T, typename E>
class Result {
private:
    std::variant<T, E> _data;

    template <typename... Args>
    explicit Result(std::in_place_index_t<0>, Args&&... args)
        : _data(std::in_place_index<0>, std::forward<Args>(args)...) {}

    template <typename... Args>
    explicit Result(std::in_place_index_t<1>, Args&&... args)
        : _data(std::in_place_index<1>, std::forward<Args>(args)...) {}

public:
    static Result ok(T v) {
        return Result(std::in_place_index<0>, std::move(v));
    }

    static Result err(E e) {
        return Result(std::in_place_index<1>, std::move(e));
    }

    template <typename... Args>
    static Result ok(Args&&... args) noexcept {
        return Result(std::in_place_index<0>, T(std::forward<Args>(args)...));
    }

    template <typename... Args>
    static Result err(Args&&... args) noexcept {
        return Result(std::in_place_index<1>, E(std::forward<Args>(args)...));
    }

    bool hasValue() const noexcept {
        return std::holds_alternative<T>(_data);
    }

    const T& value() const {
        return std::get<T>(_data);
    }

    const E& error() const {
        return std::get<E>(_data);
    }
};

} // Spark
