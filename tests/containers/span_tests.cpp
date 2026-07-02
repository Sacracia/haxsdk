#include "../pch.h"

using namespace Hax;

TEST(SpanTest, Construction)
{
    Span<int> empty;
    EXPECT_TRUE(empty.Empty());
    EXPECT_EQ(empty.Size(), 0);

    int arr[] = { 1, 2, 3, 4, 5 };
    Span<int> fromArr(arr);
    EXPECT_EQ(fromArr.Size(), 5);
    EXPECT_EQ(fromArr.Data(), arr);

    Span<int> manual(arr, 3);
    EXPECT_EQ(manual.Size(), 3);
    EXPECT_EQ(manual[2], 3);
}

TEST(SpanTest, Access)
{
    int arr[] = { 10, 20, 30 };
    Span<int> s(arr);

    EXPECT_EQ(s[0], 10);
    EXPECT_EQ(s[1], 20);

    s[1] = 25;
    EXPECT_EQ(arr[1], 25);
}

TEST(SpanTest, Iteration)
{
    int arr[] = { 1, 2, 3 };
    Span<int> s(arr);

    int sum = 0;
    for (int val : s)
        sum += val;
    EXPECT_EQ(sum, 6);

    EXPECT_EQ(s.begin(), arr);
    EXPECT_EQ(s.end(), arr + 3);
}

TEST(SpanTest, ContainerInterop)
{
    struct MockCont
    {
        int* Data() { return Arr; }
        size_t Size() const { return 3; }
        int Arr[3] = { 1, 2, 3 };
    } cont;

    Span<int> s = cont;
    EXPECT_EQ(s.Size(), 3);
    EXPECT_EQ(s[0], 1);
}