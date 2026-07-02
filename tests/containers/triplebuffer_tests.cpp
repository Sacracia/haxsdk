#include "../pch.h"

using namespace Hax;

TEST(TripleBufferTest, StateTransitionLogic) 
{
    TripleBuffer<int> buffer(10);

    auto& back1 = buffer.GetBack();
    back1.PushBack(100);
    back1.PushBack(200);

    buffer.RefreshSpare(); 

    auto& back2 = buffer.GetBack();
    EXPECT_NE(&back1, &back2);
    EXPECT_TRUE(back2.Empty());

    auto& front1 = buffer.GetFront();
    EXPECT_EQ(front1.Size(), 2);
    EXPECT_EQ(front1[0], 100);
    EXPECT_EQ(front1[1], 200);
    EXPECT_EQ(&front1, &back1);

    auto& front2 = buffer.GetFront();
    EXPECT_EQ(&front1, &front2);

    auto& back3 = buffer.GetBack();
    back3.PushBack(300);
    buffer.RefreshSpare();

    auto& front3 = buffer.GetFront();
    EXPECT_EQ(front3[0], 300);
    EXPECT_NE(&front3, &front1);
}