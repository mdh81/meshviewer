#pragma once

#include "ReaderFactory.h"
#include "MockMeshFactory.h"

namespace mv {

    struct MockReader : readers::Reader {
        MockReader(std::string = "") : readers::Reader("", MockMeshFactory{}) {};
        MOCK_METHOD(Mesh::MeshPointer, getOutput, (Mesh::MeshPointer));
    };

    struct MockReaderFactory : readers::IReaderFactory {
        MOCK_METHOD(readers::ReaderPointer, getReader, (std::string const&), (const, override));
        MOCK_METHOD(bool, isFileTypeSupported, (std::filesystem::path const &), (const, override));

        IMeshFactory const& getMeshFactory() const override {
            return mockMeshFactory;
        }

        MockReaderFactory() {
            ON_CALL(*this, getReader).WillByDefault([this](std::string const& name) {
                auto itr = readers.find(name);
                readers::ReaderPointer result;
                if (itr != readers.end()) {
                    result = std::move(itr->second);
                    readers.erase(itr);
                }
                return result;
            });
        }
        MockMeshFactory mockMeshFactory;
        std::unordered_map<std::string, std::unique_ptr<MockReader>> readers;
    };

}