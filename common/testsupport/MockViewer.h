#pragma once

#include "Viewer.h"
#include "3dmath/Matrix.h"
#include "gmock/gmock.h"

struct MockViewer : mv::viewer::Viewer {
    MOCK_METHOD(void, add, (mv::Drawable::DrawablePointer&&), (override));
    // NOTE: It's far easier to actually write a simple implementation than attempt to use gmock to resolve
    // calls to an overloaded mocked function
    unsigned numDrawables{};
    void add(mv::Drawable::DrawablePointers const& drawables) override {
        numDrawables += static_cast<unsigned>(drawables.size());
    }
    MOCK_METHOD(void, render, (), (override));
    //NOTE: MOCK_METHOD macro cannot handle multiple template types in the return type hence the alias
    using Matrix3x3 = math3d::Matrix<float, 3, 3>;
    MOCK_METHOD(Matrix3x3, getViewportToWindowTransform, (), (const, override));
    bool removeCalled{};
    void remove(mv::Drawable::DrawablePointer const&) override {
        removeCalled = true;
    }
    bool addSingleDrawableCalled{};
    void add(mv::Drawable::DrawablePointer const&) override {
        addSingleDrawableCalled = true;
    }



};