#include "ModelManager.h"
#include "EventHandler.h"
#include "ReaderFactory.h"
#include "Types.h"
#include <format>
#include <memory>
#include <mutex>

namespace mv::models {

    ModelManager::ModelDrawablePair::ModelDrawablePair(std::filesystem::path const& file,
                                                       Drawable::DrawablePointer&& drawable)
    : modelFile(file)
    , modelDrawable(std::move(drawable)) {}

    ModelManager::ModelManager(Mode mode, std::unique_ptr<readers::IReaderFactory const>&& readerFactory,
                               viewer::Viewer& viewer)
    : currentModel{}
    , mode(mode)
    , loaded(false)
    , viewer(viewer)
    , readerFactory(std::move(readerFactory)) {
        events::EventHandler{}.registerBasicEventCallback(
                events::Event{GLFW_KEY_M, GLFW_MOD_SHIFT},
                *this,
                &ModelManager::cycleThroughModels);
    }

    void ModelManager::setMode(Mode m) {
        std::lock_guard lock{modelLoadMutex};
        if (!loaded) {
             mode = m;
        } else {
            throw std::runtime_error(std::format("Error in {}. Display mode must be set prior to loading the models.",
                                                 __PRETTY_FUNCTION__));
        }
    }

    void ModelManager::loadModelFiles(std::vector<std::string> const& modelFiles) {
        std::lock_guard lock{modelLoadMutex};
        if (loaded) {
            throw std::runtime_error(std::format("Error in {}. "
                                                 "At this time, {} does not support loading models "
                                                 "more than once", __PRETTY_FUNCTION__, common::AppName));
        }

        for (decltype(currentModel) i = 0; i < modelFiles.size(); ++i) {
            if (mode == Mode::DisplayMultipleModels) {
                modelDrawables.emplace_back(modelFiles[i], readerFactory->getReader(modelFiles[i])->getOutput());
            } else {
                modelDrawables.emplace_back(modelFiles[i],
                                            !i ? readerFactory->getReader(modelFiles[i])->getOutput() : nullptr);
            }
        }

        // render loaded models
        std::vector<Drawable::DrawablePointer> drawablesToRender;
        std::transform(modelDrawables.begin(), modelDrawables.end(), std::back_inserter(drawablesToRender),
                       [](auto &drawableEntry) { return drawableEntry.modelDrawable; });
        drawablesToRender.erase(std::remove(drawablesToRender.begin(), drawablesToRender.end(), nullptr),
                                drawablesToRender.end());
        viewer.add(drawablesToRender);

        loaded = true;
    }

    void ModelManager::loadModelFilesFromDirectory(std::filesystem::path const& modelFilesDirectory) {
        std::vector<std::string> modelsToLoad;
        for (auto const& dirEntry : std::filesystem::directory_iterator{modelFilesDirectory}) {
            auto const& path = dirEntry.path();
            if (readerFactory->isFileTypeSupported(path)) {
                modelsToLoad.push_back(path);
            } else if (isDebugOn()) {
                std::puts(std::format("Skipping file {} from the model directory\n", path.c_str()).c_str());
            }
        }
        loadModelFiles(modelsToLoad);
    }

    void ModelManager::cycleThroughModels() {
        if (mode != Mode::DisplaySingleModel) {
            throw std::runtime_error("Cannot cycle through models when there all the loaded models are displayed");
        }
        if (modelDrawables.size() > 1) {
            viewer.remove(modelDrawables[currentModel].modelDrawable);
            ++currentModel;
            if (currentModel == modelDrawables.size()) {
                currentModel = 0;
            }
            if (!modelDrawables[currentModel].modelDrawable) {
                modelDrawables[currentModel].modelDrawable =
                        std::move(readerFactory->getReader(modelDrawables[currentModel].modelFile)->getOutput());
            }
            viewer.add(modelDrawables[currentModel].modelDrawable);
        }
    }

    size_t ModelManager::getNumberOfModels() const {
        return modelDrawables.size();
    }

}
