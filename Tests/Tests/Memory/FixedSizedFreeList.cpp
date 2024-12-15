#include "../pch.h"
#include <Deep.h>
#include <Deep/Memory/FixedSizeFreeList.h>

struct TestItem {
    static int32 destructorCalls;

    TestItem(int32 value) :
        value(value) {}

    ~TestItem() {
        ++destructorCalls;
    }

    int32 value;
};

int32 TestItem::destructorCalls = 0;

TEST(FixedSizedFreeList, ConstructAndFree) {
    TestItem::destructorCalls = 0; // Reset destructor calls

    // Constructing items
    Deep::FixedSizeFreeList<TestItem> list{ 4, 2 };
    uint32 i0 = list.ConstructItem(1);
    uint32 i1 = list.ConstructItem(2);
    uint32 i2 = list.ConstructItem(3);
    uint32 i3 = list.ConstructItem(4);
    EXPECT_EQ(list[i0].value, 1);
    EXPECT_EQ(list[i1].value, 2);
    EXPECT_EQ(list[i2].value, 3);
    EXPECT_EQ(list[i3].value, 4);

    // Fail to construct more than max items
    uint32 i4 = list.ConstructItem(5);
    EXPECT_EQ(i4, Deep::FixedSizeFreeList<TestItem>::invalidItemIndex);

    // Free single item
    list.FreeItem(i0);

    // Allocate item now that we have a free slot
    uint32 _i0 = list.ConstructItem(5);
    EXPECT_EQ(i0, _i0); // Slot should be the same as old one since its the only available item in the pool

    // Free Batch - prevent assertion
    Deep::FixedSizeFreeList<TestItem>::Batch batch;
    list.AddItemToBatch(batch, _i0);
    list.AddItemToBatch(batch, i1);
    list.AddItemToBatch(batch, i2);
    list.AddItemToBatch(batch, i3);
    list.FreeBatch(batch);

    EXPECT_EQ(TestItem::destructorCalls, 5); // Verify number of destructor calls is correct
}