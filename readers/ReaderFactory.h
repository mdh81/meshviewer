#pragma once

#include "Reader.h"
#include "STLReader.h"
#include <filesystem>
#include <exception>
#include <memory>

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
        if (file.extension().compare(".STL") == 0 ||
            file.extension().compare(".stl") == 0) {
            return std::unique_ptr<Reader>(new STLReader(fileName));
        } else {
            throw std::runtime_error(fileName + " has no extension. Unable to find type");
        }
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
