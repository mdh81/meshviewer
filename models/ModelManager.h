#pragma once

#include "MeshViewerObject.h"
#include "ReaderFactory.h"
#include "ViewerFactory.h"
#include "Drawable.h"
#include <filesystem>
#include <vector>
#include <string>

namespace mv::viewer {
    class Viewer;
}

namespace mv::models {

    class ModelManager : public  MeshViewerObject {

    public:

        enum class Mode {
            DisplaySingleModel,
            DisplayMultipleModels
        };
        explicit ModelManager(Mode = Mode::DisplaySingleModel,
                     std::unique_ptr<readers::IReaderFactory const>&& = std::make_unique<readers::ReaderFactory>(),
                     viewer::Viewer& = viewer::ViewerFactory::getViewer());
        void setMode(Mode);
        void loadModelFiles(std::vector<std::string> const&);
        void loadModelFilesFromDirectory(std::filesystem::path const&);
        [[nodiscard]] size_t getNumberOfModels() const;

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
        std::unique_ptr<readers::IReaderFactory const> readerFactory;
        bool loaded;
        viewer::Viewer& viewer;
        std::mutex modelLoadMutex;
    };

}