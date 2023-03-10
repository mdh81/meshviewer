#ifndef MESH_VIEWER_CALLBACK_FACTORY_H
#define MESH_VIEWER_CALLBACK_FACTORY_H

#include "Callback.h"
#include "NonMemberFunctionCallback.h"
#include "MemberFunctionCallback.h"

#include <memory>

namespace mv { namespace events {

class CallbackFactory {

    public:
        static CallbackFactory& getInstance() {
            static CallbackFactory cb;
            return cb;
        }

        template<typename FunctionPtrT, typename... CallbackArgsT>
        Callback& registerCallback(FunctionPtrT* functionPtr, const CallbackArgsT&... args) {

            m_callbackFunctions.push_back(
                    std::unique_ptr<Callback>(
                        new NonMemberFunctionCallback<FunctionPtrT, CallbackArgsT...>(functionPtr, args...))); 

            return *m_callbackFunctions.back().get();
        }
        
        template<typename InstanceT, typename FunctionPtrT, typename... CallbackArgsT>
        Callback& registerCallback(InstanceT& instance, FunctionPtrT funcPtr, const CallbackArgsT&... args) {

            m_callbackFunctions.push_back(std::unique_ptr<Callback>(
                new MemberFunctionCallback<InstanceT, FunctionPtrT, CallbackArgsT...>(instance, funcPtr, args...)));
            return *m_callbackFunctions.back().get();
        }

    private:
        CallbackFactory() = default;
        ~CallbackFactory() = default;
        CallbackFactory(const CallbackFactory&) = delete;
        CallbackFactory(CallbackFactory&&) = delete;
        void operator=(const CallbackFactory&) = delete;
        void operator=(CallbackFactory&&) = delete;

    private:
        using CallbackPointerT = std::unique_ptr<Callback>; 
        std::vector<CallbackPointerT> m_callbackFunctions;

    friend class CallbackFactoryTester;

};

} }

#endif
