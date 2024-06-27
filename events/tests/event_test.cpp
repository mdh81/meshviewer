#include "gtest/gtest.h"
#include "Event.h"
#include <iostream>

using namespace mv::events;
using namespace std;

TEST(Event, ComparisonProducesRightResults) {
    Event eventA{100}, eventB{100};
    ASSERT_EQ(eventA, eventB) << "Two events with same id should compare as equal";

    Event eventA1{100, 101}, eventB1{100, 101};
    ASSERT_EQ(eventA1, eventB1) << "Two events with same id and modifier should compare as equal";

    Event eventA2{100}, eventB2{101};
    ASSERT_NE(eventA2, eventB2) << "Two events with different ids should NOT compare as equal";

    Event eventA3{100, 101}, eventB3{100, 102};
    ASSERT_NE(eventA3, eventB3) << "Two events with different modifiers should NOT compare as equal";
}
