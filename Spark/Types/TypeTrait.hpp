#pragma once

#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Pointer.hpp"
#include "String.hpp"
#include "Type.hpp"

namespace Spark {

    template <typename T>
    inline constexpr bool isSparkType = false;

    template <>
    inline constexpr bool isSparkType<Type> = true;

    template <>
    inline constexpr bool isSparkType<Integer> = true;

    template <>
    inline constexpr bool isSparkType<Float> = true;

    template <>
    inline constexpr bool isSparkType<Boolean> = true;

    template <>
    inline constexpr bool isSparkType<Pointer> = true;

    template <>
    inline constexpr bool isSparkType<String> = true;



    template <typename T>
    inline constexpr bool isSparkReferenceType = false;

    template <>
    inline constexpr bool isSparkReferenceType<String> = true;

} // Spark
