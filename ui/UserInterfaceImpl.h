#pragma once
#include "Types.h"
#include "Texture.h"
#include "UserInterface.h"
#include "Panel.h"

class GLFWwindow;
struct ImVec2;

namespace mv::ui {

    enum class Position {
        Bottom,
        Right,
    };

    // NOLINTBEGIN(readability-convert-member-functions-to-static)
    class UserInterfaceImpl : public UserInterface {

    public:
        void beginDraw(GLFWwindow*) override;
        void endDraw() override;
        void stop() override;
        [[nodiscard]] bool requiresRedraw() const override;

    private:
        void initialize(GLFWwindow*);
        void setPosition(Position = Position::Bottom);
        void drawPanel();
        void drawButtons();
        void loadButtonTextures(std::filesystem::path const&);
        void identifyButtonTextures(std::filesystem::path const&);
        void handleResize(GLFWwindow*);
        bool wasResized(GLFWwindow*);
        void setNextPosition(unsigned char buttonIndex);
        void drawModelControls();
        void drawColorControls();
        void drawCameraControls();
        void drawSettingsControls();
        void drawButtonLabel(std::string const& text);
        void calculateHoverLabelWidth();
        void calculateScaleFactor(GLFWwindow*);

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
        static Position position;
        inline static bool initialized{};
        inline static common::Point2D displaySize{};
        inline static bool isVerticallyOriented{};
        inline static std::unordered_map<std::string, common::Texture> textures;
        inline static std::unordered_map<std::string, unsigned> openglTextures;
        struct Button {
            unsigned char position;
            std::string hoverLabel;
            struct BoundingBox {
                common::Point2D upperLeftCorner;
                common::Point2D lowerRightCorner;
                void assign(ImVec2 const& widgetUpperLeft, ImVec2 const& widgetLowerRight);
            } bounds;
        };
        struct ControlPanel {
            Button button;
            std::unique_ptr<Panel> panel;
            ControlPanel(unsigned char position, std::string hoverLabel, std::unique_ptr<Panel>&& panel);
            ControlPanel() = default; // dummy to satisfy the map's subscript operator, which expects the object to
                                      // default constructible
            ControlPanel(ControlPanel const&) = delete;
            ControlPanel& operator=(ControlPanel const&) = delete;
            ControlPanel(ControlPanel&&) = default;
            ControlPanel& operator=(ControlPanel&&) = default;
        };
        static inline std::unordered_map<std::string, ControlPanel> controlPanels{};
        static inline float hoverLabelWidth{};
        static inline float scaleFactor{1.f};
    };
    // NOLINTEND(readability-convert-member-functions-to-static)

}
