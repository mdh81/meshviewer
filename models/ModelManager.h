#pragma once

#include "MeshViewerObject.h"
#include "ReaderFactory.h"
#include "ViewerFactory.h"
#include "Drawable.h"
#include <filesystem>
#include <vector>
#include <string>
#include "Viewer.h"

namespace mv::models {

    class ModelManager : public  MeshViewerObject {

    public:

        enum class Mode {
            DisplaySingleModel,
            DisplayMultipleModels
        };

        ModelManager(Mode = Mode::DisplaySingleModel, readers::IReaderFactory const& = readers::ReaderFactory{},
                     viewer::Viewer& = viewer::ViewerFactory{}.getViewer());
        void setMode(Mode);
        void loadModelFiles(std::vector<std::string> const&);
        void loadModelFilesFromDirectory(std::filesystem::path const&);
        size_t getNumberOfModels() const;

        // Implementation logic
        void cycleThroughModels();

        // No copy or move semantics
        ModelManager(ModelManager const&) = delete;
        ModelManager(ModelManager&&) = delete;
        ModelManager& operator=(ModelManager const&) = delete;
        ModelManager& operator=(ModelManager&&) = delete;

    private:
        struct ModelDrawablePair {
            std::filesystem::path modelFile;
            Drawable::DrawablePointer modelDrawable;
            ModelDrawablePair(std::filesystem::path const& file, Drawable::DrawablePointer&& drawable);
        };
        std::vector<ModelDrawablePair> modelDrawables;
        std::vector<ModelDrawablePair>::size_type currentModel;
        Mode mode;
        readers::IReaderFactory const& readerFactory;
        bool loaded;
        mv::viewer::Viewer& viewer;
        std::mutex modelLoadMutex;
    };

}