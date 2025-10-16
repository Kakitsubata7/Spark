#pragma once

#include "error.hpp"

namespace Spark {

template <typename T>
class Result {
private:
    bool _hasError;
    union {
        T _value;
        Error _error;
    };

public:
    explicit Result(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : _hasError(false), _value(std::move(value)) { }
    explicit Result(Error error) noexcept
        : _hasError(true), _error(std::move(error)) { }

    ~Result() noexcept {
        if (_hasError) {
            _error.~Error();
        } else {
            _value.~T();
        }
    }

    Result(const Result& other) : _hasError(other._hasError) {
        if (_hasError) {
            new (&_error) Error(other._error);
        } else {
            new (&_value) T(other._value);
        }
    }

    Result& operator=(const Result& other) {
        if (this == &other) {
            return *this;
        }

        this->~Result();
        new (this) Result(other);
        return *this;
    }

    Result(Result&& other) noexcept(std::is_nothrow_move_constructible_v<T>) : _hasError(other._hasError){
        if (_hasError) {
            new (&_error) Error(std::move(other._error));
        } else {
            new (&_value) T(std::move(other._value));
        }
    }

    Result& operator=(Result&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (this == &other) {
            return *this;
        }

        this->~Result();
        new (this) Result(std::move(other));
        return *this;
    }

    [[nodiscard]]
    constexpr bool hasError() const noexcept { return _hasError; }

    bool tryGetValue(T& out) const noexcept {
        if (_hasError) {
            return false;
        }
        out = _value;
        return true;
    }

    bool tryGetError(Error& out) const noexcept {
        if (!_hasError) {
            return false;
        }
        out = _error;
        return true;
    }
};

} // Spark
