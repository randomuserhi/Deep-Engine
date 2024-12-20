#include "../pch.h"
#include <Deep.h>
#include <Deep/Memory/PooledFixedSizeFreeList.h>

static int32 destructorCalls;
static int32 constructorCalls;

TEST(PooledFixedSizedFreeList, AcquireAndRelease) {
    struct TestItem {
        TestItem() :
            value(5) {
            mem = new int[value];
            ++constructorCalls;
        }

        ~TestItem() {
            delete[] mem;
            ++destructorCalls;
        }

        int32 value;
        void* mem;
    };

    destructorCalls = 0;  // Reset destructor calls
    constructorCalls = 0; // Reset constructor calls

    // Constructing items
    Deep::PooledFixedSizeFreeList<TestItem> list{ 4, 2 };
    uint32 i0 = list.AcquireItem();
    EXPECT_EQ(constructorCalls, 2); // After page allocation, there should have been 2 constructor calls

    uint32 i1 = list.AcquireItem();
    uint32 i2 = list.AcquireItem();
    EXPECT_EQ(constructorCalls, 4); // After new page allocation, there should have been 4 constructor calls

    uint32 i3 = list.AcquireItem();
    EXPECT_EQ(list[i0].value, 5);
    EXPECT_EQ(list[i1].value, 5);
    EXPECT_EQ(list[i2].value, 5);
    EXPECT_EQ(list[i3].value, 5);

    // Fail to construct more than max items
    uint32 i4 = list.AcquireItem();
    EXPECT_EQ(i4, Deep::PooledFixedSizeFreeList<TestItem>::invalidItemIndex);

    // Alter value
    list[i0].value = 2;

    // Free single item
    list.ReleaseItem(i0);

    // Allocate item now that we have a free slot
    uint32 _i0 = list.AcquireItem();
    EXPECT_EQ(i0, _i0);            // Slot should be the same as old one since its the only available item in the pool
    EXPECT_EQ(list[_i0].value, 2); // Changed value should persist as its an object pool

    // Free Batch - prevent assertion
    Deep::PooledFixedSizeFreeList<TestItem>::Batch batch;
    list.AddItemToBatch(batch, _i0);
    list.AddItemToBatch(batch, i1);
    list.AddItemToBatch(batch, i2);
    list.AddItemToBatch(batch, i3);
    list.ReleaseBatch(batch);

    EXPECT_EQ(destructorCalls, 0); // No destructor calls until object is destructed
    list.~PooledFixedSizeFreeList();
    EXPECT_EQ(destructorCalls, 4); // All objects should now be destructed
}