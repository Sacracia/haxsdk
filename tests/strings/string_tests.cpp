#include "../pch.h"

using namespace Hax;

class StringTest : public ::testing::Test 
{
protected:
    size_t initialMemory = 0;

    void SetUp() override 
    {
        initialMemory = g_GlobalAlloc.TotalAllocated;
    }

    void TearDown() override 
    {
        EXPECT_EQ(g_GlobalAlloc.TotalAllocated, initialMemory) 
            << "Memory leak detected! Usage before: " << initialMemory 
            << ", after: " << g_GlobalAlloc.TotalAllocated;
    }
};

TEST_F(StringTest, DefaultConstructorDoesNotAllocate) 
{
    String s;
    EXPECT_EQ(s.Length(), 0);
    EXPECT_TRUE(s.Empty());
    EXPECT_STREQ(s.CStr(), L"");
}

TEST_F(StringTest, AssignmentAllocatesCorrectAmount) 
{
    const wchar_t* testStr = L"Hello";
    String s(testStr);

    EXPECT_EQ(s.Length(), 5);
    EXPECT_STREQ(s.CStr(), testStr);

    size_t expectedSize = (5 + 1) * sizeof(wchar_t);
    size_t totalUsage = g_GlobalAlloc.TotalAllocated;
    size_t init = initialMemory;
    EXPECT_GE(totalUsage - init, expectedSize);
}

TEST_F(StringTest, AppendConcatenatesCorrectly) 
{
    String s(L"Visual");
    s.Append(L"Studio");

    EXPECT_STREQ(s.CStr(), L"VisualStudio");
    EXPECT_EQ(s.Length(), 12);
}

TEST_F(StringTest, ClearFreeResetsMemoryUsage) 
{
    {
        String s(L"Temporary Data");
        EXPECT_GT(g_GlobalAlloc.TotalAllocated, initialMemory);
        s.ClearFree();
        EXPECT_EQ(g_GlobalAlloc.TotalAllocated, initialMemory);
        EXPECT_TRUE(s.Empty());
        EXPECT_STREQ(s.CStr(), L"");
    }
}

TEST_F(StringTest, EqualityOperators) 
{
    String s1(L"Test");
    String s2(L"Test");
    String s3(L"Other");

    EXPECT_TRUE(s1 == s2);
    EXPECT_TRUE(s1 == L"Test");
    EXPECT_FALSE(s1 == s3);
    EXPECT_FALSE(s1 == nullptr);
}

//TEST_F(StringTest, HandleNullAndEmptyInputs) 
//{
//    String s{};
//    EXPECT_TRUE(s.Empty());
//    EXPECT_STREQ(s.CStr(), L"");
//
//    s = L"Data";
//    s = nullptr;
//    EXPECT_TRUE(s.Empty());
//    EXPECT_EQ(g_GlobalAlloc.TotalUsage(), initialMemory);
//}

TEST_F(StringTest, CopyConstructorIsDeep) 
{
    String original(L"Original");
    {
        String copy = original;
        EXPECT_STREQ(copy.CStr(), original.CStr());
        EXPECT_NE(copy.m_Data.Data(), original.m_Data.Data());
    } 
    EXPECT_STREQ(original.CStr(), L"Original");
}