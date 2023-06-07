#pragma once

#include "Camera.h"
#include "Types.h"
#include "gmock/gmock.h"

namespace mv {

    class MockCamera : public Camera {
        MOCK_METHOD(void, zoom, (common::Direction));
        MOCK_METHOD(void, pan, (common::Direction));
    };

}
