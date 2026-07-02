#include "../pch.h"

using namespace Hax;

TEST(RectTest, FactoryMethods)
{
    Rect r1 = Rect::FromFloats(0.f, 0.f, 10.f, 10.f);
    Rect r2 = Rect::FromMinMax({ 0.f, 0.f }, { 10.f, 10.f });
    Rect r3 = Rect::FromPosSize({ 0.f, 0.f }, { 10.f, 10.f });

    EXPECT_EQ(r1, r2);
    EXPECT_EQ(r1, r3);
}

TEST(RectTest, Dimensions)
{
    Rect r = Rect::FromFloats(0.f, 0.f, 10.f, 20.f);

    EXPECT_EQ(r.GetSize(), Vector2({ 10.f, 20.f }));
    EXPECT_EQ(r.GetCenter(), Vector2({ 5.f, 10.f }));
    EXPECT_FLOAT_EQ(r.GetHeight(), 20.f);
}

TEST(RectTest, Corners)
{
    Rect r = Rect::FromFloats(0.f, 10.f, 100.f, 50.f);

    EXPECT_EQ(r.GetTL(), Vector2({ 0.f, 10.f }));
    EXPECT_EQ(r.GetTR(), Vector2({ 100.f, 10.f }));
    EXPECT_EQ(r.GetBL(), Vector2({ 0.f, 50.f }));
    EXPECT_EQ(r.GetBR(), Vector2({ 100.f, 50.f }));
}

TEST(RectTest, Containment)
{
    Rect r = Rect::FromFloats(0.f, 0.f, 10.f, 10.f);

    EXPECT_TRUE(r.Contains(Vector2({ 5.f, 5.f })));
    EXPECT_TRUE(r.Contains(Vector2({ 0.f, 0.f })));
    EXPECT_FALSE(r.Contains(Vector2({ 10.f, 10.f })));
    EXPECT_FALSE(r.Contains(Vector2({ -1.f, 5.f })));

    Rect inner = Rect::FromFloats(1.f, 1.f, 5.f, 5.f);
    EXPECT_TRUE(r.Contains(inner));
}

TEST(RectTest, Intersection)
{
    Rect r1 = Rect::FromFloats(0.f, 0.f, 10.f, 10.f);
    Rect r2 = Rect::FromFloats(5.f, 5.f, 15.f, 15.f);
    Rect r3 = Rect::FromFloats(20.f, 20.f, 30.f, 30.f);

    EXPECT_TRUE(r1.Intersects(r2));
    EXPECT_FALSE(r1.Intersects(r3));

    Rect intersection = r1.Intersect(r2);
    EXPECT_EQ(intersection, Rect::FromFloats(5.f, 5.f, 10.f, 10.f));
}

TEST(RectTest, Clipping)
{
    Rect r = Rect::FromFloats(0.f, 0.f, 10.f, 10.f);
    r.ClipWith(Rect::FromFloats(5.f, -5.f, 15.f, 5.f));

    EXPECT_EQ(r, Rect::FromFloats(5.f, 0.f, 10.f, 5.f));
}

TEST(RectTest, Modification)
{
    Rect r = Rect::FromFloats(5.f, 5.f, 6.f, 6.f);

    r.Add(Vector2({ 0.f, 10.f }));
    EXPECT_EQ(r, Rect::FromFloats(0.f, 5.f, 6.f, 10.f));

    r.Add(Rect::FromFloats(-1.f, -1.f, 20.f, 20.f));
    EXPECT_EQ(r, Rect::FromFloats(-1.f, -1.f, 20.f, 20.f));
}

TEST(RectTest, Inversion)
{
    Rect valid = Rect::FromFloats(0.f, 0.f, 10.f, 10.f);
    Rect inverted = Rect::FromFloats(10.f, 10.f, 0.f, 0.f);

    EXPECT_FALSE(valid.IsInverted());
    EXPECT_TRUE(inverted.IsInverted());
}