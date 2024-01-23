#pragma once
#include <vector>
#include <any>
#include <tuple>
#include <functional>
#include <memory>
#include "PointerTypes.h"

class BasicEventCallback {
public:
    virtual void call() const = 0;
    virtual ~BasicEventCallback() = default;
    using Pointer = mv::common::UniquePointer<BasicEventCallback>;
};

class DataEventCallback {
public:
    virtual void call(std::vector<std::any>&& eventData) const = 0;
    virtual ~DataEventCallback();
    using Pointer = std::unique_ptr<BasicEventCallback>;
};

template<typename InstanceType, typename FunctionPointerType>
class CallbackBase {
public:
    CallbackBase(InstanceType& instance, FunctionPointerType functionPointer)
            : instance(instance)
            , functionPointer(functionPointer) {
    }

protected:
    InstanceType& instance;
    FunctionPointerType functionPointer;
};


template<typename InstanceType, typename FunctionPointerType, typename... ArgumentTypes>
class BasicEventCallbackImpl : public CallbackBase<InstanceType, FunctionPointerType>, public BasicEventCallback {
public:
    BasicEventCallbackImpl(InstanceType& instance, FunctionPointerType functionPointer, ArgumentTypes&&... arguments)
    : BaseClass(instance, functionPointer)
    , arguments(arguments...) {

    }

    void call() const override {
        callImpl(std::make_integer_sequence<unsigned, sizeof...(ArgumentTypes)>{});
    }

private:
    template<unsigned... Indices>
    void callImpl(std::integer_sequence<unsigned, Indices...>&&) const {
        std::mem_fn(BaseClass::functionPointer)(BaseClass::instance, std::get<Indices>(arguments)...);
    }

    using BaseClass = CallbackBase<InstanceType, FunctionPointerType>;
    std::tuple<ArgumentTypes...> arguments;
};


template<typename InstanceType, typename FunctionPointerType>
class DataEventCallbackImpl : public CallbackBase<InstanceType, FunctionPointerType>, public DataEventCallback {
public:
    DataEventCallbackImpl(InstanceType& instance, FunctionPointerType functionPointer)
            : BaseClass(instance, functionPointer) {
    }

    void call(std::vector<std::any>&& eventData) const override {
        std::mem_fn(BaseClass::functionPointer)(BaseClass::instance, eventData);
    }

private:
    using BaseClass = CallbackBase<InstanceType, FunctionPointerType>;
};