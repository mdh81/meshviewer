#include "gtest/gtest.h"

#include "MockReaderFactory.h"
#include "EventHandler.h"
#include "MockViewer.h"
#include "ModelManager.h"
#include "gmock/gmock.h"
using namespace mv::models;
using namespace testing;

namespace mv {

    TEST(ModelManager, LoadSpecifiedModels) {
        MockReaderFactory mrf;
        MockViewer mockViewer;

        // A reader should be instantiated to read each model
        EXPECT_CALL(mrf, getReader("a")).Times(Exactly(1));
        EXPECT_CALL(mrf, getReader("b")).Times(Exactly(1));
        EXPECT_CALL(mrf, getReader("c")).Times(Exactly(1));
        // Reader's output should be invoked for each model
        std::unique_ptr<MockReader> mockReaderA = std::make_unique<MockReader>();
        std::unique_ptr<MockReader> mockReaderB = std::make_unique<MockReader>();
        std::unique_ptr<MockReader> mockReaderC = std::make_unique<MockReader>();
        EXPECT_CALL(*mockReaderA, getOutput(_)).Times(Exactly(1));
        EXPECT_CALL(*mockReaderB, getOutput(_)).Times(Exactly(1));
        EXPECT_CALL(*mockReaderC, getOutput(_)).Times(Exactly(1));
        mrf.readers.emplace("a", std::move(mockReaderA));
        mrf.readers.emplace("b", std::move(mockReaderB));
        mrf.readers.emplace("c", std::move(mockReaderC));

        ModelManager modelManager {ModelManager::Mode::DisplayMultipleModels, mrf, mockViewer};
        modelManager.loadModelFiles({"a" , "b", "c"});
        ASSERT_EQ(modelManager.getNumberOfModels(), 3) << "Wrong number of models";
        ASSERT_EQ(mockViewer.numDrawables, 3) << "Wrong number of drawables";
    }

    TEST(ModelManager, LoadTheFirstModel) {
        MockReaderFactory mrf;
        MockViewer mockViewer;
        EXPECT_CALL(mrf, getReader("a")).Times(Exactly(0));
        EXPECT_CALL(mrf, getReader("b")).Times(Exactly(0));
        EXPECT_CALL(mrf, getReader("c")).Times(Exactly(1));
        std::unique_ptr<MockReader> mockReaderA = std::make_unique<MockReader>();
        std::unique_ptr<MockReader> mockReaderB = std::make_unique<MockReader>();
        std::unique_ptr<MockReader> mockReaderC = std::make_unique<MockReader>();
        EXPECT_CALL(*mockReaderA, getOutput(_)).Times(Exactly(0));
        EXPECT_CALL(*mockReaderB, getOutput(_)).Times(Exactly(0));
        EXPECT_CALL(*mockReaderC, getOutput(_)).Times(Exactly(1));
        mrf.readers.emplace("a", std::move(mockReaderA));
        mrf.readers.emplace("b", std::move(mockReaderB));
        mrf.readers.emplace("c", std::move(mockReaderC));

        ModelManager modelManager {ModelManager::Mode::DisplaySingleModel, mrf, mockViewer};
        modelManager.loadModelFiles({"a" , "b", "c"});
        ASSERT_EQ(mockViewer.numDrawables, 1) << "Wrong number of drawables";
        ASSERT_EQ(modelManager.getNumberOfModels(), 3) << "Wrong number of models";
    }

    TEST(ModelManager, LoadModelFilesFromDirectory) {
        MockReaderFactory mrf;
        MockViewer mockViewer;
        std::filesystem::create_directory("test_dir");
        std::filesystem::path validFile1{"test_dir/a.stl"};
        std::filesystem::path validFile2{"test_dir/b.ply"};
        std::filesystem::path invalidFile{"test_dir/c.obj"};
        std::filesystem::path subDirectory{"test_dir/sub_dir"};
        std::ofstream {validFile1};
        std::ofstream {validFile2};
        std::ofstream {invalidFile};
        std::filesystem::create_directory(subDirectory);
        // Expectations
        // NOTE: EXPECT_CALL macro expansion doesn't lend itself to creating filesystem::path objects from c-style
        // strings in the same way a normal function call to ReaderFactory::isFileTypeSupporter("c-string") does.
        // Hence, the temporary named filesystem::path objects
        EXPECT_CALL(mrf, isFileTypeSupported(validFile1)).WillOnce(Return(true));
        EXPECT_CALL(mrf, isFileTypeSupported(validFile2)).WillOnce(Return(true));
        EXPECT_CALL(mrf, isFileTypeSupported(invalidFile)).WillOnce(Return(false));
        EXPECT_CALL(mrf, isFileTypeSupported(subDirectory)).WillOnce(Return(false));
        EXPECT_CALL(mrf, getReader(validFile1.c_str())).Times(Exactly(1));
        EXPECT_CALL(mrf, getReader(validFile2.c_str())).Times(Exactly(1));
        EXPECT_CALL(mrf, getReader(invalidFile.c_str())).Times(Exactly(0));
        // Use mock reader instance, otherwise, gmock will return a nullptr from the mocked getReader() method, which
        // will cause a crash in the function-under-test, loadModelFilesFromDirectory(), when it calls getOutput() on
        // the reader to get the drawable for valid models
        std::unique_ptr<MockReader> mockReaderA = std::make_unique<MockReader>();
        std::unique_ptr<MockReader> mockReaderB = std::make_unique<MockReader>();
        mrf.readers.emplace(validFile1, std::move(mockReaderA));
        mrf.readers.emplace(validFile2, std::move(mockReaderB));

        // Call
        // NOTE: Using DisplayMultipleModels since this test is to ensure all valid models are loaded
        ModelManager modelManager {ModelManager::Mode::DisplayMultipleModels, mrf, mockViewer};
        modelManager.loadModelFilesFromDirectory("test_dir");
        ASSERT_EQ(mockViewer.numDrawables, 2) << std::format("Expected drawables for {} and {}",
                                                             validFile1.c_str(),
                                                             validFile2.c_str());
    }

    TEST(ModelManager, CycleThroughModels) {
        MockReaderFactory mrf;
        MockViewer mockViewer;
        std::unique_ptr<MockReader>
            mockReaderA = std::make_unique<MockReader>(),
            mockReaderB = std::make_unique<MockReader>(),
            mockReaderC = std::make_unique<MockReader>();
        mrf.readers.emplace("a", std::move(mockReaderA));
        mrf.readers.emplace("b", std::move(mockReaderB));
        mrf.readers.emplace("c", std::move(mockReaderC));
        ModelManager modelManager {models::ModelManager::Mode::DisplaySingleModel, mrf, mockViewer};
        modelManager.loadModelFiles({"a" , "b", "c"});
        // During a cycle models event, the current drawable should be removed and a new one should be added
        events::EventHandler eventHandler;
        constexpr unsigned M = 77, SHIFT = 1;
        eventHandler.raiseEvent(events::Event{M, SHIFT});
        ASSERT_TRUE(mockViewer.removeCalled) << "Drawable not removed during model cycle through event";
        ASSERT_TRUE(mockViewer.addSingleDrawableCalled) << "Drawable not added during model cycle through event";
    }
}