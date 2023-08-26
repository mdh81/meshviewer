#pragma once

#include "Reader.h"
#include "STLReader.h"
#include "PLYReader.h"
#include <filesystem>
#include <exception>
#include <memory>
#include <algorithm>

namespace mv::readers {
using ReaderPointer = std::unique_ptr<Reader>;

class ReaderFactory {
public:
    static ReaderPointer getReader(std::string const &fileName) {
        std::filesystem::path file = fileName;
        if (!file.has_extension()) {
            throw std::runtime_error(fileName + " has no extension. Unable to find type");
        }
        if (!std::filesystem::exists(file)) {
            throw std::runtime_error(fileName + " does not exist");
        }

        // Check if the input file's extension is a case-insensitive version of a known extension
        // NOTE: Both the file's extension and the known extension against which it is checked
        // cannot be mixed case
        auto isExtension =
                [](std::filesystem::path const& file, std::string const& extension) {
            std::string oppositeCaseExtension;
            oppositeCaseExtension.resize(extension.size());
            auto isLowerCase = extension.at(0) >= 'a' && extension.at(0) < 'z';
            std::transform(extension.begin(), extension.end(),
                           oppositeCaseExtension.begin(), [isLowerCase](char const c) {
                return isLowerCase ? c - ('a' - 'A') : c + ('a' - 'A');
            });
            return file.extension().compare('.' + extension) == 0 ||
                   file.extension().compare('.' + oppositeCaseExtension) == 0;
        };

        if (isExtension(file, "stl")) {
            return std::unique_ptr<Reader>(new STLReader(fileName));
        } else if (isExtension(file, "ply")) {
            return std::unique_ptr<Reader>(new PLYReader(fileName));
        }
        throw std::runtime_error(fileName + " has no extension. Unable to find type");
    }

private:
    ReaderFactory() = default;
    ~ReaderFactory() = default;

public:
    ReaderFactory(ReaderFactory const&) = delete;
    ReaderFactory(ReaderFactory&&) = delete;
    ReaderFactory& operator=(ReaderFactory const&) = delete;
    ReaderFactory& operator=(ReaderFactory&&) = delete;
};

}
