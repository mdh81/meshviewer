#pragma once

#include "Types.h"
#include <memory>
#include <filesystem>
#include "external/stb_image.h"
#include "ConfigurationReader.h"

namespace mv::common {

    struct Texture {
        Texture(std::string const& fileName) : data(nullptr, nullptr) { // NOLINT: Allow implicit conversion of char* to path
            auto path =
                std::filesystem::path(mv::config::ConfigurationReader::getInstance().getValue("texturePath")) /
                fileName;
            if (!exists(path)) {
                throw std::runtime_error("Unable to load texture" + path.string());
            }
            int nrChannels, w, h;
            data = std::unique_ptr<pixel, decltype(&stbi_image_free)>(
                stbi_load(path.c_str(),&w, &h,
                          &nrChannels,
                          STBI_rgb_alpha), stbi_image_free);
            width = static_cast<unsigned>(w);
            height = static_cast<unsigned>(h);
        }

        Texture(Texture const&) = delete;

        Texture& operator=(Texture&& another) {
            if (this != &another) {
                width = another.width;
                height = another.height;
                data = std::move(another.data);
            }
            return *this;
        }

        Texture(Texture&& another)
        : width (another.width)
        , height(another.height)
        , data(std::move(another.data)) {

        }

        std::unique_ptr<pixel, decltype(&stbi_image_free)> data;
        unsigned width;
        unsigned height;
    };

}