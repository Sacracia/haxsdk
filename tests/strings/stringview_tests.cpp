#include "../pch.h"

using namespace Hax;

TEST(StringViewTest, Construction)
{
    static_assert(StringView().Empty());
    static_assert(StringView("abc").Length() == 3);
    static_assert(StringView("abc", 2).Length() == 2);

    StringView s = "test";
    EXPECT_EQ(s.Length(), 4);
    EXPECT_STREQ(s.Data(), "test");
}

TEST(StringViewTest, Access)
{
    static_assert(StringView("abc").First() == 'a');
    static_assert(StringView("abc").Last() == 'c');
    static_assert(StringView("abc")[1] == 'b');

    StringView s = "view";
    EXPECT_EQ(s[0], 'v');
}

TEST(StringViewTest, Comparison)
{
    static_assert(StringView("abc") == StringView("abc"));
    static_assert(StringView("abc") != StringView("abd"));
    static_assert(StringView("a") < StringView("b"));

    StringView s1 = "apple";
    StringView s2 = "banana";
    EXPECT_TRUE(s1.Compare(s2) < 0);
    EXPECT_TRUE(s2.StartsWith("ban"));
    EXPECT_TRUE(s1.EndsWith("ple"));
}

TEST(StringViewTest, SubstringAndMutation)
{
    constexpr auto check = []()
    {
        StringView v = "hello world";
        v.RemovePrefix(6);
        return v == StringView("world");
    };
    static_assert(check());

    StringView s = "hello world";
    EXPECT_EQ(s.Substr(0, 5), StringView("hello"));

    s.RemoveSuffix(6);
    EXPECT_EQ(s, StringView("hello"));
}

TEST(StringViewTest, Iteration)
{
    StringView s = "abc";
    size_t count = 0;
    for (char c : s)
        count++;

    EXPECT_EQ(count, 3);
    EXPECT_EQ(s.begin(), s.Data());
    EXPECT_EQ(s.end(), s.Data() + 3);
}

TEST(StringViewTest, WideString)
{
    static_assert(WStringView(L"abc").Length() == 3);
    WStringView ws = L"wide";
    EXPECT_EQ(ws.Length(), 4);
}