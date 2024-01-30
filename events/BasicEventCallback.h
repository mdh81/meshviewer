#pragma once

#include "Callback.h"
#include "CallbackBase.h"

namespace mv {

    // Derive the non-template base from the root non-template class, this is critical to get a type erased Callback
    // instance that can be stored in containers to manage lifecycle of callbacks
    class BasicEventCallback : public Callback {
    public:
        virtual void call() const = 0;
        virtual ~BasicEventCallback() = default;
        using ObservingPointer = std::weak_ptr<BasicEventCallback>;
    };

    template<typename InstanceType, typename FunctionPointerType, typename... ArgumentTypes>
    class BasicEventCallbackImpl : public CallbackBase<InstanceType, FunctionPointerType>, public BasicEventCallback {
    public:
        BasicEventCallbackImpl(InstanceType &instance, FunctionPointerType functionPointer,
                               ArgumentTypes &&... arguments)
        : BaseClass(instance, functionPointer), arguments(arguments...) {

        }

        void call() const override {
            callImpl(std::make_integer_sequence<unsigned, sizeof...(ArgumentTypes)>{});
        }

    private:
        template<unsigned... Indices>
        void callImpl(std::integer_sequence<unsigned, Indices...> &&) const {
            std::mem_fn(BaseClass::functionPointer)(BaseClass::instance, std::get<Indices>(arguments)...);
        }

        using BaseClass = CallbackBase<InstanceType, FunctionPointerType>;
        std::tuple<ArgumentTypes...> arguments;
    };

}
