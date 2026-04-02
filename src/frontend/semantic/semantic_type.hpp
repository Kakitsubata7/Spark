#pragma once

#include <algorithm>
#include <vector>

#include "semantic_func.hpp"

namespace Spark::FrontEnd {

class SemanticFunc;

/**
 * Represents a type during semantic passes.
 */
class SemanticType {
private:
    std::string _name;

protected:
    explicit SemanticType(std::string name) noexcept : _name(std::move(name)) { }

public:
    virtual ~SemanticType() = default;

    [[nodiscard]]
    std::string_view name() const noexcept { return _name; }

    /**
     * Checks whether two types are the same one.
     * @param other Other type to check.
     * @return `true` if the two types are the same one, `false` otherwise.
     */
    [[nodiscard]]
    bool is(SemanticType* other);

    [[nodiscard]]
    virtual SemanticFunc* getCallable(const std::vector<SemanticFunc*>& paramTypes) const = 0;
};

/**
 * Represents a record type's field during semantic passes.
 */
class RecordField {
private:
    std::string _name;
    SemanticType* _type;

public:
    RecordField(std::string name, SemanticType* type) noexcept
        : _name(std::move(name)), _type(type) { }

    [[nodiscard]]
    std::string_view name() const noexcept { return _name; }

    [[nodiscard]]
    SemanticType* type() const noexcept { return _type; }
};

class RecordMethod {
private:
    std::string _name;
    SemanticFunc* _func;

public:
    RecordMethod(std::string name, SemanticFunc* func) noexcept
        : _name(std::move(name)), _func(func) { }

    [[nodiscard]]
    std::string_view name() const noexcept { return _name; }

    [[nodiscard]]
    SemanticFunc* func() const noexcept { return _func; }
};

class RecordType : public SemanticType {
private:
    std::vector<SemanticType*> _traits;

    std::vector<RecordField> _fields;
    std::vector<RecordMethod> _methods;

protected:
    explicit RecordType(std::string name,
                        std::vector<SemanticType*> traits = {},
                        std::vector<RecordField> fields = {},
                        std::vector<RecordMethod> methods = {}) noexcept
        : SemanticType(std::move(name)), _traits(std::move(traits)), _fields(std::move(fields)),
          _methods(std::move(methods)) { }

public:
    [[nodiscard]]
    const std::vector<SemanticType*>& traits() const noexcept { return _traits; }

    [[nodiscard]]
    const std::vector<RecordField>& fields() const noexcept { return _fields; }

    [[nodiscard]]
    const std::vector<RecordMethod>& methods() const noexcept { return _methods; }

    [[nodiscard]]
    const RecordField* getField(std::string_view name) const noexcept {
        auto it = std::find_if(_fields.begin(), _fields.end(),
            [name](const RecordField& field) {
                return field.name() == name;
            }
        );
        return it == _fields.end() ? nullptr : &*it;
    }

    [[nodiscard]]
    const RecordMethod* getMethod(std::string_view name) const noexcept {
        auto it = std::find_if(_methods.begin(), _methods.end(),
            [name](const RecordMethod& method) {
                return method.name() == name;
            }
        );
        return it == _methods.end() ? nullptr : &*it;
    }

    [[nodiscard]]
    SemanticFunc* getCallable(const std::vector<SemanticFunc*>& paramTypes) const override {
        if (const RecordMethod* method = getMethod("operator()"); method != nullptr) {
            if (method->func()->isCallableWith()) {

            }
        }
        return nullptr;
    }
};

class StructType final : public RecordType {
public:
    explicit StructType(std::string name, std::vector<SemanticType*> traits = {}) noexcept
        : RecordType(std::move(name), std::move(traits)) { }
};

class ClassType final : public RecordType {
private:
    SemanticType* _base;

public:
    explicit ClassType(std::string name, SemanticType* base = nullptr, std::vector<SemanticType*> traits = {}) noexcept
        : RecordType(std::move(name), std::move(traits)), _base(base) { }

    [[nodiscard]]
    SemanticType* base() const noexcept { return _base; }
};

class FuncType final : public SemanticType {
private:
    SemanticFunc* _func;

public:
    FuncType(std::string name, SemanticFunc* func) noexcept
        : SemanticType(std::move(name)), _func(func) { }

    [[nodiscard]]
    SemanticFunc* func() const noexcept { return _func; }
};

class OverloadedFuncType final : public SemanticType {
private:
    std::vector<FuncType*> _funcTypes;

public:
    OverloadedFuncType(std::string name, std::vector<FuncType*> funcTypes) noexcept
        : SemanticType(std::move(name)), _funcTypes(std::move(funcTypes)) { }

    [[nodiscard]]
    const std::vector<FuncType*>& funcTypes() const noexcept { return _funcTypes; }
};

class TypeType final : public SemanticType {
private:
    SemanticType* _declaredType;

public:
    TypeType(std::string name, SemanticType* declaredType) noexcept
        : SemanticType(std::move(name)), _declaredType(declaredType) { }

    [[nodiscard]]
    SemanticType* declaredType() const noexcept { return _declaredType; }
};

} // Spark::FrontEnd
