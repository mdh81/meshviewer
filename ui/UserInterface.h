#pragma once

class GLFWwindow;

namespace mv::ui {

    class UserInterface {

    public:
        virtual void beginDraw(GLFWwindow*) = 0;
        virtual void endDraw() = 0;
        virtual void stop() = 0;
        virtual ~UserInterface() = default;
    };
}
