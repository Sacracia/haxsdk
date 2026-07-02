#include "../pch.h"

using namespace Hax;

struct TrivialStruct
{
    int A = 3, B = 4;
};

struct TrivialPaddedStruct
{
    int A = 3;
    size_t B = 4;
};

TEST(HashTest, TypesId)
{
    static_assert(GetTypeId<int>() != GetTypeId<float>());
    static_assert(GetTypeId<int>() == GetTypeId<int>());
    static_assert(GetTypeId<TrivialStruct> != GetTypeId<TrivialPaddedStruct>);
}

TEST(HashTest, Determinism)
{
    constexpr const wchar_t* str1 = L"Hello world!";
    constexpr const wchar_t* str1Copy = L"Hello world!\0";
    constexpr const wchar_t* otherStr = L"Goodbye world!";
    constexpr WStringView sv = L"Hello world!";
    String str2 = L"Hello world!";

    static_assert(Hash(str1) == Hash(sv));
    static_assert(Hash(str1) == Hash(str1Copy));
    EXPECT_EQ(Hash(str1), Hash(str2));
    static_assert(Hash(otherStr) != Hash(str1));
}

TEST(HashTest, Structures)
{
    TrivialStruct ts1 = {3, 4};
    TrivialStruct ts2 = {3, 4};
    TrivialStruct ts3 = {4, 4};
    EXPECT_EQ(Hash(ts1), Hash(ts1));
    EXPECT_EQ(Hash(ts1), Hash(ts2));
    EXPECT_NE(Hash(ts2), Hash(ts3));

    TrivialPaddedStruct tps1; memset(&tps1, 8, sizeof(tps1)); tps1.A = 3; tps1.B = 4;
    TrivialPaddedStruct tps2; memset(&tps2, 9, sizeof(tps2)); tps2.A = 3; tps2.B = 4;
    EXPECT_NE(memcmp(&tps1, &tps2, sizeof(TrivialPaddedStruct)), 0);
    EXPECT_EQ(Hash(tps1), Hash(tps2));
}