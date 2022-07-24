#ifndef MESH_VIEWER_MFUNC_CALLBACK_H
#define MESH_VIEWER_MFUNC_CALLBACK_H

#include "Callback.h"
#include <tuple>
#include <memory>

namespace meshviewer { namespace events {

template<typename InstanceT, typename MemberFunctionPtrT, typename... FunctionArgsT>
class MemberFunctionCallback : public Callback {
    public:
        // TODO: MemberFunctionCallback has no control over life-cycle of instance. "m_instance" could become a
        // dangling reference leading to a callback to reference invalid memory
        MemberFunctionCallback(InstanceT& instance, const MemberFunctionPtrT funcPtr, const FunctionArgsT&... args)
            : m_instance (instance)
            , m_functionPointer (funcPtr)
            , m_functionArgs(args...) {

        };

        void call() override {
            memberFunctionCallImpl(typename IndexGenerator<sizeof...(FunctionArgsT)>::type());
        }

        template<unsigned... Indices>
        void memberFunctionCallImpl(IndexSequence<Indices...>) {
            std::mem_fn(m_functionPointer)(m_instance, std::get<Indices>(m_functionArgs) ...);
        }

    private:
        InstanceT& m_instance;
        // Note: MemberFunctionPtrT is not a plain pointer. It is a pointer to a member function and 
        // is not declared like a regular pointer with a *
        MemberFunctionPtrT const m_functionPointer;
        std::tuple<FunctionArgsT...> m_functionArgs;
};

} }

#endif
