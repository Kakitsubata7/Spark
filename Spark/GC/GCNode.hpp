#pragma once

namespace Spark {

template <typename T>
class GCNode {

    /* ===== Data ===== */

private:
    T _data;

public:
    [[nodiscard]]
    constexpr T& data() { return _data; }

    [[nodiscard]]
    constexpr const T& data() const { return _data; }



    /* ===== Constructors ===== */

public:
    explicit GCNode(const T& value = T()) : _data(value) { }

};

} // Spark
