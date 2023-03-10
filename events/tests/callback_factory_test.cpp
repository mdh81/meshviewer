#include "gtest/gtest.h"
#include "CallbackFactory.h"
#include <memory>
using namespace mv::events;
using namespace std;

// Friend class implementation. This class is used to test the internal state of
// the CallbackFactory object as callback functions are registered
namespace mv { namespace events {
class CallbackFactoryTester {
    public:
        CallbackFactoryTester(CallbackFactory& cbf) : m_cbf(cbf) { }
        size_t getNumberOfCallbacks() const { return m_cbf.m_callbackFunctions.size(); } 
        void callFunction() { m_cbf.m_callbackFunctions.front()->call(); }
    private:
        CallbackFactory& m_cbf;
};
} }

void freeFunction(int a, float b, char c) {
    cout << "freeFunction called with parameters " << a << ' ' << b << ' ' << c; 
}

class TestClass {
    public:
        void memberFunction(int a, float b, char c) {
            cout << "TestClass::memberFunction called with parameters " << a << ' ' << b << ' ' << c;
        }
        ~TestClass() { cout << "Destructor" << endl; }
};


TEST(CallbackFactory, Singleton) {
    CallbackFactory& cb1 = CallbackFactory::getInstance(); 
    CallbackFactory& cb2 = CallbackFactory::getInstance(); 
    ASSERT_EQ(&cb1, &cb2) << "CallbackFactory::getInstance() is not returning a singleton" << std::endl;
}

TEST(CallbackFactory, RegisterFreeFunction) {
    CallbackFactory::getInstance().registerCallback(&freeFunction, 5, 2.3f, 'x'); 
    ASSERT_TRUE(CallbackFactoryTester(CallbackFactory::getInstance()).getNumberOfCallbacks() == 1);
    testing::internal::CaptureStdout();
    CallbackFactoryTester(CallbackFactory::getInstance()).callFunction();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), 
              "freeFunction called with parameters 5 2.3 x") << "Free function not called as expected" << endl;
}

TEST(CallbackFactory, RegisterMemberFunction) {
    TestClass t;
    CallbackFactory::getInstance().registerCallback(t, &TestClass::memberFunction, 15, 12.3f, 'y');
    testing::internal::CaptureStdout();
    CallbackFactoryTester(CallbackFactory::getInstance()).callFunction();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), 
              "TestClass::memberFunction called with parameters 15 12.3 y") << "Member function not called as expected" << endl;
}

TEST(CallbackFactory, InstanceLifeCycleManagement) {
    
    TestClass t; 
    CallbackFactory::getInstance().registerCallback(t, &TestClass::memberFunction, 25, 22.3f, 'z');
    testing::internal::CaptureStdout();
    CallbackFactoryTester(CallbackFactory::getInstance()).callFunction();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), 
              "TestClass::memberFunction called with parameters 25 22.3 z") << "Member function not called as expected" << endl;
}
