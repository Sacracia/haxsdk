#include "../pch.h"

using namespace Hax;

TEST(AllocatorTest, BasicAllocFree)
{
    Allocator testAlloc{"Test"};
    size_t initialSize = testAlloc.TotalAllocated;

    void* ptr = Alloc(testAlloc, 100);
    ASSERT_NE(ptr, nullptr);
    EXPECT_GE(testAlloc.TotalAllocated, initialSize + 100);
    EXPECT_GE(testAlloc.MaxAllocated, testAlloc.TotalAllocated);

    Free(testAlloc, ptr);
}

TEST(AllocatorTest, NewDeleteLifecycle)
{
    struct MockObj
    {
        MockObj(int v, bool* destroyed) : Val(v), Destroyed(destroyed) {}
        ~MockObj() { *Destroyed = true; }
        int Val;
        bool* Destroyed;
    };

    Allocator testAlloc{"Lifecycle"};
    bool isDestroyed = false;

    MockObj* obj = New<MockObj>(testAlloc, 42, &isDestroyed);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->Val, 42);
    EXPECT_FALSE(isDestroyed);

    Delete(testAlloc, obj);
    EXPECT_TRUE(isDestroyed);
}

TEST(AllocatorTest, GlobalAlloc)
{
    void* ptr = Alloc(16);
    EXPECT_NE(ptr, nullptr);
    Free(ptr);
}

TEST(AllocatorTest, Alignment)
{
    Allocator testAlloc{"Alignment"};

    void* ptr = Alloc(testAlloc, 1);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(std::max_align_t), 0);

    Free(testAlloc, ptr);
}

TEST(AllocatorTest, ZeroAlloc)
{
    Allocator testAlloc{"Zero"};
    void* ptr = Alloc(testAlloc, 0);
    Free(testAlloc, ptr);
}