#ifndef MESH_VIEWER_NMFUNC_CALLBACK_H
#define MESH_VIEWER_NMFUNC_CALLBACK_H

#include "Callback.h"
#include <tuple>

namespace mv { namespace events {

template<typename FunctionPtrT, typename... FunctionArgsT>
class NonMemberFunctionCallback : public Callback {
    public:
        NonMemberFunctionCallback(FunctionPtrT* funcPtr, const FunctionArgsT&... args) 
            : m_functionPointer(funcPtr) {
            m_functionArgs = std::make_tuple(args...);
        };

    public:
        void call() const override {
            callImpl(typename IndexGenerator<sizeof...(FunctionArgsT)>::type());
        } 

        template<unsigned... Indices>
        void callImpl(IndexSequence<Indices...>) const {
            m_functionPointer(std::get<Indices>(m_functionArgs) ...); 
        } 
        
    protected:
        FunctionPtrT *const m_functionPointer; 
        std::tuple<FunctionArgsT...> m_functionArgs;
};

} }

#endif
