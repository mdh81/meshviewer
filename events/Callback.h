#pragma once
#include <vector>
#include <any>
#include <tuple>
#include <functional>
#include <memory>
#include "PointerTypes.h"
#include "MeshViewerObject.h"

namespace mv {

    class Callback {
    public:
        using Pointer = common::SharedPointer<Callback>;
        using ObservingPointer = std::weak_ptr<Callback>;
        // Make it polymorphic to allow pointer cast operations to work correctly
        virtual ~Callback() = default;
    };

    class CallbackCreationObserver {
    public:
        virtual void notifyCallbackCreated(MeshViewerObject const&,
                                           Callback::ObservingPointer) = 0;
    };
}