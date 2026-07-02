#include "../pch.h"

using namespace Hax;

TEST(ArrayTest, Construction)
{
    Array<int, 3> a1 = {1, 2, 3};
    EXPECT_EQ(a1[0], 1);
    EXPECT_EQ(a1[2], 3);

    Array<int, 3> a2(10);
    EXPECT_EQ(a2[0], 10);
    EXPECT_EQ(a2[2], 10);

    static_assert(Array<int, 2>{1, 2}[1] == 2);
    static_assert(Array<int, 3>(7)[2] == 7);
}

TEST(ArrayTest, AccessAndIteration)
{
    Array<float, 3> arr = { 1.f, 2.f, 3.f };

    arr[1] = 10.f;
    EXPECT_FLOAT_EQ(arr[1], 10.f);

    float sum = 0.f;
    for (float v : arr) sum += v;
    EXPECT_FLOAT_EQ(sum, 14.f);

    EXPECT_EQ(arr.Size(), 3);
}

TEST(ArrayTest, ComparisonAndCopy)
{
    Array<int, 3> a1 = {1, 2, 3};
    Array<int, 3> a2 = a1;
    Array<int, 3> a3 = {1, 2, 4};

    EXPECT_EQ(a1, a2);
    EXPECT_NE(a1, a3);
}

TEST(ArrayTest, ComplexTypes)
{
    Array<Vector2, 2> vecs = { { 1.f, 2.f }, { 3.f, 4.f } };

    EXPECT_EQ(vecs[0], Vector2({ 1.f, 2.f }));
    EXPECT_EQ(vecs[1].Y, 4.f);

    vecs[0].X = 0.f;
    EXPECT_FLOAT_EQ(vecs[0].X, 0.f);
}