#pragma once

#include "Types.h"
#include <memory>
#include <filesystem>
#include "external/stb_image.h"
#include "../config/ConfigurationReader.h"

namespace mv::common {

    struct Texture {

        explicit Texture(std::filesystem::path const& path) : data {nullptr, nullptr} {
            if (!exists(path)) {
                throw std::runtime_error(std::format("Unable to load texture {}", path.c_str()));
            }
            int nrChannels, w, h;
            data = std::unique_ptr<pixel, decltype(&stbi_image_free)>(
                    stbi_load(path.c_str(),&w, &h,
                              &nrChannels,
                              STBI_rgb_alpha), stbi_image_free);
            width = static_cast<unsigned>(w);
            height = static_cast<unsigned>(h);
        }

        explicit Texture(const char* fileName)
        : Texture(std::filesystem::path(mv::config::ConfigurationReader::getInstance().getValue("texturePath")) /
                  fileName) {
        }

        explicit Texture(std::string const& fileName)
        : Texture(fileName.c_str()) {
        }

        Texture(Texture const&) = delete;

        Texture& operator=(Texture&& another) noexcept {
            if (this != &another) {
                width = another.width;
                height = another.height;
                data = std::move(another.data);
            }
            return *this;
        }

        Texture(Texture&& another) noexcept
        : width (another.width)
        , height(another.height)
        , data(std::move(another.data)) {

        }

        std::unique_ptr<pixel, decltype(&stbi_image_free)> data;
        unsigned width;
        unsigned height;
    };

}