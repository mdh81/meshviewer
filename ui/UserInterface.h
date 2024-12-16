#pragma once
#include "Types.h"
#include "Texture.h"

class GLFWwindow;
struct ImVec2;

namespace mv::ui {

    enum class Position {
        Bottom,
        Right,
    };

    class UserInterface {

    public:
        static void beginDraw(GLFWwindow*);
        static void endDraw();
        static void stop();

    private:
        static void initialize(GLFWwindow*);
        static void setPosition(Position = Position::Bottom);
        static void drawPanel();
        static void drawButtons();
        static void loadButtonTextures(std::filesystem::path const&);
        static void identifyButtonTextures(std::filesystem::path const&);
        static void handleResize(GLFWwindow*);
        static bool wasResized(GLFWwindow*);
        static void setNextPosition(unsigned char buttonIndex);
        static void drawModelControls();
        static void drawColorControls();
        static void drawCameraControls();
        static void drawSettingsControls();
        static void drawButtonLabel(std::string const& text);
        static void calculateHoverLabelWidth();

    private:
        static constexpr unsigned char NumButtons{4};
        static constexpr unsigned char ButtonSize{64};
        static constexpr unsigned char ButtonSpacing{10};
        static constexpr unsigned char ButtonMargin{5};
        static constexpr unsigned char WindowCornerRadius{20};
        static constexpr unsigned char LabelCornerRadius{3};
        static constexpr unsigned char PanelMargin{3};
        static constexpr float PanelOpacity{0.1f};
        static constexpr float TextPanelOpacity{0.45f};
        static constexpr float ButtonLabelSeparation{40};
        static constexpr float HoverDecorationTriangleBase{20};
        static constexpr float HoverDecorationTriangleHeight{10};
        static constexpr float HoverTriangleFudge{5};
        inline static common::Point2D origin{};
        inline static common::Point2D dimension{};
        inline static Position position{Position::Bottom};
        inline static bool initialized{};
        inline static common::Point2D windowSize{};
        inline static bool isVerticallyOriented{};
        inline static std::unordered_map<std::string, common::Texture> textures;
        inline static std::unordered_map<std::string, unsigned> openglTextures;
        struct Button {
            unsigned char position;
            std::string hoverLabel;
            std::string windowTitle;
            struct BoundingBox {
                common::Point2D upperLeftCorner;
                common::Point2D lowerRightCorner;
                bool isHovering(common::Point2D const& mouseLocation);
                void assign(ImVec2 const& widgetUpperLeft, ImVec2 const& widgetLowerRight);
            } bounds;
        };
        static std::unordered_map<std::string, Button> buttons;
        static inline float hoverLabelWidth{};
    };

}
