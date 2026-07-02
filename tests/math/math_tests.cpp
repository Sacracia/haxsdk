#include "../pch.h"

using namespace Hax;

constexpr float kEps = 0.0001f;

TEST(MathUtils, Basic)
{
    EXPECT_EQ(Clamp(5, 0, 10), 5);
    EXPECT_EQ(Clamp(-5, 0, 10), 0);
    EXPECT_EQ(Clamp(15, 0, 10), 10);

    EXPECT_EQ(Abs(-10), 10);
    EXPECT_EQ(Abs(10), 10);
    EXPECT_EQ(Abs(0), 0);

    EXPECT_NEAR(Lerp(0.f, 10.f, 0.5f), 5.f, kEps);
    EXPECT_NEAR(Lerp(0.f, 10.f, 1.f), 10.f, kEps);
}

TEST(MathUtils, Rounding) 
{
    EXPECT_FLOAT_EQ(Floor(2.9f), 2.f);
    EXPECT_FLOAT_EQ(Floor(-2.1f), -3.f);

    EXPECT_FLOAT_EQ(Ceil(2.1f), 3.f);
    EXPECT_FLOAT_EQ(Ceil(-2.9f), -2.f);

    EXPECT_FLOAT_EQ(Round(2.4f), 2.f);
    EXPECT_FLOAT_EQ(Round(2.6f), 3.f);

    EXPECT_FLOAT_EQ(Trunc(2.9f), 2.f);
    EXPECT_FLOAT_EQ(Trunc(-2.9f), -2.f);
}

TEST(MathUtils, Trigonometry) 
{
    EXPECT_NEAR(RadToDeg(3.14159f), 180.f, 0.1f);
    EXPECT_NEAR(DegToRad(180.f), 3.14159f, 0.001f);

    EXPECT_NEAR(Sin(0.f), 0.f, kEps);
    EXPECT_NEAR(Cos(0.f), 1.f, kEps);
}

TEST(MathUtils, Complex) 
{
    EXPECT_NEAR(Sqrt(9.f), 3.f, kEps);
    EXPECT_NEAR(Sqrt(0.f), 0.f, kEps);

    EXPECT_FLOAT_EQ(Mod(5.5f, 2.f), 1.5f);
    EXPECT_FLOAT_EQ(Mod(-5.5f, 2.f), -1.5f);
    EXPECT_FLOAT_EQ(Mod(5.5f, 0.f), 0.f);
}
