#include "gtest/gtest.h"
#include "EventHandler.h"
using namespace meshviewer::events;

TEST(EventHandler, Singleton) {
    EventHandler& eh1 = EventHandler::getInstance(); 
    EventHandler& eh2 = EventHandler::getInstance(); 
    ASSERT_EQ(&eh1, &eh2) << "EventHandler::getInstance() is not returning singleton" << std::endl;
}

TEST(EventHandler, EventListener) {
    
}

TEST(EventHandler, CallbackRegistration) {

} 
