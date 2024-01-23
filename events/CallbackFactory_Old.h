#ifndef MESH_VIEWER_CALLBACK_FACTORY_H
#define MESH_VIEWER_CALLBACK_FACTORY_H

#include "Callback_Old.h"
#include "NonMemberFunctionCallback.h"
#include "MemberFunctionCallback.h"

#include <memory>

namespace mv { namespace events {

class CallbackFactory_Old {

    public:
        static CallbackFactory_Old& getInstance() {
            static CallbackFactory_Old cb;
            return cb;
        }

        template<typename FunctionPtrT, typename... CallbackArgsT>
        Callback_Old& registerCallback(FunctionPtrT* functionPtr, const CallbackArgsT&... args) {

            m_callbackFunctions.push_back(
                    std::unique_ptr<Callback_Old>(
                        new NonMemberFunctionCallback<FunctionPtrT, CallbackArgsT...>(functionPtr, args...))); 

            return *m_callbackFunctions.back().get();
        }
        
        template<typename InstanceT, typename FunctionPtrT, typename... CallbackArgsT>
        Callback_Old& registerCallback(InstanceT& instance, FunctionPtrT funcPtr, const CallbackArgsT&... args) {

            m_callbackFunctions.push_back(std::unique_ptr<Callback_Old>(
                new MemberFunctionCallback<InstanceT, FunctionPtrT, CallbackArgsT...>(instance, funcPtr, args...)));
            return *m_callbackFunctions.back().get();
        }

    private:
        CallbackFactory_Old() = default;
        ~CallbackFactory_Old() = default;
        CallbackFactory_Old(const CallbackFactory_Old&) = delete;
        CallbackFactory_Old(CallbackFactory_Old&&) = delete;
        void operator=(const CallbackFactory_Old&) = delete;
        void operator=(CallbackFactory_Old&&) = delete;

    private:
        using CallbackPointerT = std::unique_ptr<Callback_Old>;
        std::vector<CallbackPointerT> m_callbackFunctions;

    friend class CallbackFactoryTester;

};

} }

#endif
