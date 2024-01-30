#pragma once

namespace mv {

    template<typename InstanceType, typename FunctionPointerType>
    class CallbackBase {
    public:
        CallbackBase(InstanceType &instance, FunctionPointerType functionPointer)
                : instance(instance), functionPointer(functionPointer) {
        }

    protected:
        InstanceType &instance;
        FunctionPointerType functionPointer;
    };

}
