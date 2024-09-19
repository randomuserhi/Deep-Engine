#include "../pch.h"
#include "Deep.h"
#include "Deep/Memory/FixedSizeFreeList.h"

struct Item {
    int32 a;

    Item(int32 a) : a(a) {

    }

    ~Item() {
        std::cout << "Destructed - " << a << std::endl;
    }
};

// TODO(randomuserhi): Split up - also test asserts
TEST(FixedSizedFreeList, Construct) {
    Deep::FixedSizeFreeList<Item> list{ 4, 2 };
    uint32 i0 = list.ConstructItem(1);
    uint32 i1 = list.ConstructItem(2);
    uint32 i2 = list.ConstructItem(3);
    uint32 i3 = list.ConstructItem(4);
    EXPECT_EQ(list[i0].a, 1);
    EXPECT_EQ(list[i1].a, 2);
    EXPECT_EQ(list[i2].a, 3);
    EXPECT_EQ(list[i3].a, 4);

    uint32 i4 = list.ConstructItem(5);
    EXPECT_EQ(i4, Deep::FixedSizeFreeList<Item>::invalidItemIndex);

    list.FreeItem(i0);
    list.FreeItem(&list[i1]);

    i0 = list.ConstructItem(6);
    i1 = list.ConstructItem(7);
    EXPECT_EQ(list[i0].a, 6);
    EXPECT_EQ(list[i1].a, 7);
    EXPECT_EQ(list[i2].a, 3);
    EXPECT_EQ(list[i3].a, 4);

    Deep::FixedSizeFreeList<Item>::Batch batch;
    list.AddItemToBatch(batch, i0);
    list.AddItemToBatch(batch, i1);
    list.AddItemToBatch(batch, i2);
    list.AddItemToBatch(batch, i3);
    list.FreeBatch(batch);
}