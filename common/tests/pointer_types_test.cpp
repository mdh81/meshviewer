#include "gtest/gtest.h"
#include "PointerTypes.h"
#include <unordered_set>
using namespace std;
using namespace mv::common;

struct Foo {
    Foo(int a, float b, char c) : a(a), b(b), c(c) {}
    int a;
    float b;
    char c;
};

TEST(PointerTypes, MoveConstruction) {
    UniquePointer<Foo> f = std::make_unique<Foo>(10, 2.5f, 'x');
    ASSERT_EQ(f->a, 10);
    ASSERT_FLOAT_EQ(f->b, 2.5f);
    ASSERT_EQ(f->c, 'x');
}

TEST(PointerTypes, ExplicitConstruction) {
    UniquePointer<Foo> f = new Foo(10, 100.35f, 'c');
    ASSERT_EQ(f->a, 10);
    ASSERT_FLOAT_EQ(f->b, 100.35f);
    ASSERT_EQ(f->c, 'c');
}

TEST(PointerTypes, SharedPointerIntegrity) {
    SharedPointer<Foo> f1 = std::make_shared<Foo>(10, 11.11f, 'x');
    {
        SharedPointer<Foo> f2 = f1;
        ASSERT_EQ(f2.use_count(), 2);
    }
    ASSERT_EQ(f1.use_count(), 1);
}

TEST(PointerTypes, Hashing) {
    UniquePointer<Foo> f = new Foo(10, 100.35f, 'c');
    ASSERT_EQ(SmartPointerHasher{}(f), reinterpret_cast<size_t>(f.get()));
}

TEST(PointerTypes, Comparision) {
    SharedPointer<Foo> f1 = std::make_shared<Foo>(10, 11.11f, 'x');
    SharedPointer<Foo> f2 = f1;
    ASSERT_TRUE(SmartPointerComparator{}(f1, f2));
    SharedPointer<Foo> f3 = std::make_shared<Foo>(10, 11.11f, 'x');
    ASSERT_FALSE(SmartPointerComparator{}(f1, f3));
}

TEST(PointerTypes, Lookup) {
   unordered_set<SharedPointer<Foo>, SmartPointerHasher, SmartPointerComparator> fooSet;
   SharedPointer<Foo> p1 = std::make_shared<Foo>(1, 1.f, 'x');
   SharedPointer<Foo> p2 = p1;
   fooSet.insert(p1);
   ASSERT_TRUE(fooSet.find(p2) != fooSet.end());
   SharedPointer<Foo> p3 = std::make_shared<Foo>(1, 1.f, 'x');
   ASSERT_TRUE(fooSet.find(p3) == fooSet.end());
   fooSet.erase(fooSet.find(p2));
   ASSERT_TRUE(fooSet.find(p1) == fooSet.end());
}