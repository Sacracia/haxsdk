#include "../pch.h"

using namespace Hax;

TEST(OptionalTest, Construction)
{
    Optional<int> empty;
    EXPECT_FALSE(empty.HasValue());
    EXPECT_FALSE(static_cast<bool>(empty));

    Optional<int> full = 42;
    EXPECT_TRUE(full.HasValue());
    EXPECT_EQ(full.Value(), 42);
}

TEST(OptionalTest, Assignment)
{
    Optional<int> opt;
    opt = 10;
    EXPECT_TRUE(opt.HasValue());
    EXPECT_EQ(*opt, 10);

    opt.Reset();
    EXPECT_FALSE(opt.HasValue());
}

TEST(OptionalTest, ValueOr)
{
    Optional<int> empty;
    Optional<int> full = 5;

    EXPECT_EQ(empty.ValueOr(100), 100);
    EXPECT_EQ(full.ValueOr(100), 5);
}

TEST(OptionalTest, Equality)
{
    Optional<int> a = 10;
    Optional<int> b = 10;
    Optional<int> c = 20;
    Optional<int> d;

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
    EXPECT_TRUE(a == 10);
    EXPECT_FALSE(a == 20);
}

TEST(OptionalTest, PointerAccess)
{
    struct Dummy { int Field = 5; };
    Optional<Dummy> opt = Dummy{};

    EXPECT_EQ(opt->Field, 5);
    EXPECT_EQ((*opt).Field, 5);
}