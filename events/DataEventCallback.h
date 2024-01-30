#pragma once

#include "Callback.h"
#include "CallbackBase.h"

namespace mv {

    // Derive the non-template base from the root non-template class, this is critical to get a type erased Callback
    // instance that can be stored in containers to manage lifecycle of callbacks
    class DataEventCallback : public Callback {
    public:
        virtual void call(std::vector<std::any> &&eventData) const = 0;
        virtual ~DataEventCallback() = default;
        using ObservingPointer = std::weak_ptr<DataEventCallback>;
    };

    template<typename InstanceType, typename FunctionPointerType>
    class DataEventCallbackImpl : public CallbackBase<InstanceType, FunctionPointerType>, public DataEventCallback {
    public:
        DataEventCallbackImpl(InstanceType &instance, FunctionPointerType functionPointer)
        : BaseClass(instance, functionPointer) {
        }

        void call(std::vector<std::any> &&eventData) const override {
            std::mem_fn(BaseClass::functionPointer)
            (BaseClass::instance, std::forward<std::remove_reference_t<decltype(eventData)>>(eventData));
        }

    private:
        using BaseClass = CallbackBase<InstanceType, FunctionPointerType>;
    };

}