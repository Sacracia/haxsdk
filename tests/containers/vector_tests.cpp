#include "../pch.h"

using namespace Hax;

struct POD 
{
    int x;
    float y;
};

TEST(VectorTest, DefaultConstructor)
{
    Vector<int> v;
    EXPECT_EQ(v.Size(), 0);
    EXPECT_EQ(v.Capacity(), 0);
    EXPECT_TRUE(v.Empty());
}

TEST(VectorTest, PushBack)
{
    Vector<int> v;
    v.PushBack(10);
    v.PushBack(20);

    EXPECT_EQ(v.Size(), 2);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
}

TEST(VectorTest, PushFront)
{
    Vector<int> v;
    v.PushBack(2);
    v.PushBack(3);
    v.PushFront(1);

    EXPECT_EQ(v.Size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(VectorTest, InsertMiddle)
{
    Vector<int> v;
    v.PushBack(1);
    v.PushBack(3);
    v.Insert(v.begin() + 1, 2);

    EXPECT_EQ(v.Size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(VectorTest, ResizeGrow)
{
    Vector<int> v;
    v.Resize(5, 7);

    EXPECT_EQ(v.Size(), 5);
    for (int i = 0; i < 5; i++)
        EXPECT_EQ(v[i], 7);
}

TEST(VectorTest, ResizeShrink)
{
    Vector<int> v;
    v.Resize(5, 1);
    v.Resize(2);

    EXPECT_EQ(v.Size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 1);
}

TEST(VectorTest, ReserveIncreasesCapacity)
{
    Vector<int> v;
    v.Reserve(50);

    EXPECT_GE(v.Capacity(), 50);
    EXPECT_EQ(v.Size(), 0);
}

TEST(VectorTest, EnsureCapacityWorks)
{
    Vector<int> v;
    v.EnsureCapacity(20);

    EXPECT_GE(v.Capacity(), 20);
}

TEST(VectorTest, EraseSingle)
{
    Vector<int> v;
    v.PushBack(1);
    v.PushBack(2);
    v.PushBack(3);

    v.Erase(v.begin() + 1);

    EXPECT_EQ(v.Size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 3);
}

TEST(VectorTest, EraseRange)
{
    Vector<int> v;
    for (int i = 1; i <= 5; i++)
        v.PushBack(i);

    v.Erase(v.begin() + 1, v.begin() + 4);

    EXPECT_EQ(v.Size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 5);
}

TEST(VectorTest, Clear)
{
    Vector<int> v;
    v.PushBack(10);
    v.PushBack(20);

    v.Clear();

    EXPECT_EQ(v.Size(), 0);
    EXPECT_GT(v.Capacity(), 0);
}

TEST(VectorTest, ClearFree)
{
    Vector<int> v;
    v.PushBack(10);
    v.PushBack(20);

    v.ClearFree();

    EXPECT_EQ(v.Size(), 0);
    EXPECT_EQ(v.Capacity(), 0);
    EXPECT_EQ(v.begin(), nullptr);
}

TEST(VectorTest, ContainsAndFind)
{
    Vector<int> v;
    v.PushBack(5);
    v.PushBack(10);
    v.PushBack(15);

    EXPECT_TRUE(v.Contains(10));
    EXPECT_FALSE(v.Contains(99));

    EXPECT_EQ(*v.Find(15), 15);
    EXPECT_EQ(v.Find(99), v.end());
}

TEST(VectorTest, FindIndex)
{
    Vector<int> v;
    v.PushBack(3);
    v.PushBack(6);
    v.PushBack(9);

    EXPECT_EQ(v.FindIndex(6), 1);
    EXPECT_EQ(v.FindIndex(99), (size_t)-1);
}

TEST(VectorTest, FindErase)
{
    Vector<int> v;
    v.PushBack(1);
    v.PushBack(2);
    v.PushBack(3);

    EXPECT_TRUE(v.FindErase(2));
    EXPECT_EQ(v.Size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 3);

    EXPECT_FALSE(v.FindErase(99));
}

TEST(VectorTest, CopyAssignment)
{
    Vector<int> a;
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);

    Vector<int> b;
    b = a;

    EXPECT_EQ(b.Size(), 3);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
}

TEST(VectorTest, PODStructSupport)
{
    Vector<POD> v;
    POD p{42, 3.14f};

    v.PushBack(p);
    v.PushBack({7, 2.0f});

    EXPECT_EQ(v.Size(), 2);
    EXPECT_EQ(v[0].x, 42);
    EXPECT_FLOAT_EQ(v[0].y, 3.14f);
    EXPECT_EQ(v[1].x, 7);
    EXPECT_FLOAT_EQ(v[1].y, 2.0f);
}
