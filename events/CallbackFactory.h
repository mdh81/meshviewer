#ifndef MESH_VIEWER_CALLBACK_FACTORY_H
#define MESH_VIEWER_CALLBACK_FACTORY_H

#include "Callback.h"
#include "NonMemberFunctionCallback.h"
#include "MemberFunctionCallback.h"

#include <memory>

namespace meshviewer { namespace events {

class CallbackFactory {

    public:
        static CallbackFactory& getInstance() {
            static CallbackFactory cb;
            return cb;
        }

        // TODO: Define a trigger that the callback call be associated with
        
        template<typename FunctionPtrT, typename... CallbackArgsT>
        void registerCallback(FunctionPtrT* functionPtr, const CallbackArgsT&... args) {
            m_callbackFunctions.push_back(
                    std::unique_ptr<Callback>(
                        new NonMemberFunctionCallback<FunctionPtrT, CallbackArgsT...>(functionPtr, args...))); 
        }
        
        template<typename InstanceT, typename FunctionPtrT, typename... CallbackArgsT>
        void registerCallback(InstanceT& instance, FunctionPtrT funcPtr, const CallbackArgsT&... args) {
            m_callbackFunctions.push_back(std::unique_ptr<Callback>(
                new MemberFunctionCallback<InstanceT, FunctionPtrT, CallbackArgsT...>(instance, funcPtr, args...)));
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
};

} }

#endif
