#pragma once

#include "Reader.h"
#include <filesystem>
#include <unordered_set>
#include <memory>

namespace mv::readers {
using ReaderPointer = std::unique_ptr<Reader>;

// TODO: Drop the ugly I prefix
struct IReaderFactory {
    virtual bool isFileTypeSupported(std::filesystem::path const&) const = 0;
    virtual ReaderPointer getReader(std::string const& fileName) const = 0;
    [[nodiscard]] virtual IMeshFactory const& getMeshFactory() const = 0;
};

class ReaderFactory : public IReaderFactory, public MeshViewerObject {
    public:
        ReaderFactory(IMeshFactory const& = MeshFactory{});
        ReaderPointer getReader(std::string const &fileName) const override;
        IMeshFactory const& getMeshFactory() const override;
        bool isFileTypeSupported(std::filesystem::path const&) const override;
    private:
        IMeshFactory const& meshFactory;
        static std::unordered_set<std::string> supportedExtensions;
};

}
