#include "../pch.h"

using namespace Hax;

TEST(Vector2Test, Equality) 
{
    Vector2 v1 = {1.f, 2.f};
    Vector2 v2 = {1.f, 2.f};
    Vector2 v3 = {0.f, 0.f};

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
}

TEST(Vector2Test, Arithmetic) 
{
    Vector2 a = {10.f, 20.f};
    Vector2 b = {2.f, 5.f};

    EXPECT_EQ(a + b,    Vector2(12.0f, 25.0f));
    EXPECT_EQ(a - b,    Vector2(8.0f, 15.0f));
    EXPECT_EQ(a * 2.0f, Vector2(20.0f, 40.0f));
    EXPECT_EQ(a * b,    Vector2(20.0f, 100.0f));
    EXPECT_EQ(a / 2.0f, Vector2(5.0f, 10.0f));
    EXPECT_EQ(a / b,    Vector2(5.0f, 4.0f));
}

TEST(Vector2Test, AssignmentArithmetic) 
{
    Vector2 v = {1.f, 2.f};

    v += Vector2{2.0f, 3.0f};
    EXPECT_EQ(v, Vector2(3.f, 5.f));

    v *= 2.0f;
    EXPECT_EQ(v, Vector2(6.f, 10.f));

    v /= Vector2{2.0f, 5.0f};
    EXPECT_EQ(v, Vector2(3.0f, 2.0f));
}

TEST(Vector2Test, MathFunctions) 
{
    Vector2 v = {3.f, 4.f};
    EXPECT_FLOAT_EQ(v.Length(), 5.f);

    Vector2 v2 = {1.7f, -2.3f};
    EXPECT_EQ(Round(v2), Vector2(2.0f, -2.0f));
    EXPECT_EQ(Floor(v2), Vector2(1.0f, -3.0f));
    EXPECT_EQ(Trunc(v2), Vector2(1.0f, -2.0f));
}

TEST(Vector2Test, MinMax) 
{
    Vector2 a = {1.f, 10.f};
    Vector2 b = {5.f, 2.f};

    EXPECT_EQ(Min(a, b), Vector2(1.f, 2.f));
    EXPECT_EQ(Max(a, b), Vector2(5.f, 10.f));
}