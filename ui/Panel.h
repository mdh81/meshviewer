#pragma once

#include "Types.h"
#include <chrono>
#include <string>
using namespace std::chrono_literals;

namespace mv::ui {

    class Panel {
    public:
        explicit Panel(std::string title);
        virtual void draw(common::Point2D const& position) = 0;
        virtual ~Panel() = default;
        [[nodiscard]] virtual bool isActive() const;
        [[nodiscard]] virtual bool requiresRedraw() const;

    protected:
        void beginDraw(common::Point2D const &position);
        void endDraw();
        [[nodiscard]] float getCurrentOpacity() const;

    private:
        std::string const title;
        enum class DisplayType {
            FadeIn,
            FadeOut
        };
        class AnimatedDisplay {
        public:
            explicit AnimatedDisplay(DisplayType type, std::chrono::milliseconds const& animationDuration = 100ms,
                float minOpacity = 0.f, float maxOpacity = 0.45f);
            float getOpacity();
            void start();
            void stop();
        private:
            DisplayType type;
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
            float const minOpacity;
            float const maxOpacity;
            float opacity;
            float opacityIncrement;
            bool animating;
            friend class Panel;
        };

        std::unique_ptr<AnimatedDisplay> fadeInDisplay;
        bool active;
        int const displayFlags;
    };

}
