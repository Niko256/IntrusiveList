#include <algorithm>
#include <gtest/gtest.h>
#include <ntrusive/intrusive.hpp>
#include <vector>

struct Item : IntrusiveListNode {
    int value;

    explicit Item(int v) : value(v) {}
};

using ItemList = IntrusiveList<Item>;

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

static void check_integrity(ItemList& list, std::vector<int> expected) {
    std::vector<int> forward;

    for (auto& item : list) {
        forward.push_back(item.value);
    }
    EXPECT_EQ(forward, expected) << "forward traversal mismatch";

    // Reverse pass
    std::vector<int> reverse;

    auto it = list.end();

    while (it != list.begin()) {
        --it;

        reverse.push_back(it->value);
    }

    std::reverse(reverse.begin(), reverse.end());

    EXPECT_EQ(reverse, expected) << "reverse traversal mismatch (prev pointers broken)";

    EXPECT_EQ(list.size(), expected.size());
}

/* Fixture */
class ListTest : public testing::Test {
  protected:
    Item a{1}, b{2}, c{3}, d{4}, e{5};
    ItemList list;
};

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

TEST_F(ListTest, EmptyListState) {
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
    EXPECT_EQ(list.begin(), list.end());

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, EmptyAfterClear) {
    list.push_back(a);
    list.push_back(b);

    list.clear();

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, EmptyAfterPopAll) {
    list.push_back(a);
    list.pop_front();

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, SinglePushBack) {
    list.push_back(a);

    // ---

    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &a);
    EXPECT_TRUE(a.is_linked());

    // ---

    check_integrity(list, {1});
}

TEST_F(ListTest, SinglePushFront) {
    list.push_front(a);

    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &a);

    // ---

    check_integrity(list, {1});
}

TEST_F(ListTest, SinglePopFront) {
    list.push_back(a);
    list.pop_front();

    EXPECT_FALSE(a.is_linked());

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, SinglePopBack) {
    list.push_back(a);
    list.pop_back();

    EXPECT_FALSE(a.is_linked());

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, SingleUnlink) {
    list.push_back(a);
    a.unlink();

    EXPECT_FALSE(a.is_linked());

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, TwoElementsPushBack) {
    list.push_back(a);
    list.push_back(b);

    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &b);

    // ---
    //
    check_integrity(list, {1, 2});
}

TEST_F(ListTest, TwoElementsPopFront) {
    list.push_back(a);
    list.push_back(b);
    list.pop_front();

    EXPECT_EQ(&list.front(), &b);
    EXPECT_EQ(&list.back(), &b);

    // ---

    check_integrity(list, {2});
}

TEST_F(ListTest, TwoElementsPopBack) {
    list.push_back(a);
    list.push_back(b);
    list.pop_back();

    // ---

    check_integrity(list, {1});
}

TEST_F(ListTest, TwoElementsUnlinkFirst) {
    list.push_back(a);
    list.push_back(b);

    a.unlink();

    // ---

    check_integrity(list, {2});
}

TEST_F(ListTest, TwoElementsUnlinkSecond) {
    list.push_back(a);
    list.push_back(b);

    b.unlink();

    // ---

    check_integrity(list, {1});
}

TEST_F(ListTest, PushBackOrder) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    check_integrity(list, {1, 2, 3});
}

TEST_F(ListTest, PushFrontOrder) {
    list.push_front(a);
    list.push_front(b);
    list.push_front(c);

    check_integrity(list, {3, 2, 1});
}

TEST_F(ListTest, MixedPush) {
    list.push_back(b);
    list.push_front(a);

    list.push_back(c);

    check_integrity(list, {1, 2, 3});
}

TEST_F(ListTest, UnlinkFirst) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    a.unlink();

    check_integrity(list, {2, 3});
}

TEST_F(ListTest, UnlinkMiddle) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    b.unlink();

    check_integrity(list, {1, 3});
}

TEST_F(ListTest, UnlinkLast) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    c.unlink();

    check_integrity(list, {1, 2});
}

TEST_F(ListTest, UnlinkAllForwardOrder) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    // ---

    a.unlink();
    b.unlink();
    c.unlink();

    // ---

    check_integrity(list, {});
}

TEST_F(ListTest, UnlinkAllReverseOrder) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    c.unlink();
    b.unlink();
    a.unlink();

    check_integrity(list, {});
}

TEST_F(ListTest, ReinsertAfterUnlink) {
    list.push_back(a);
    list.push_back(b);

    a.unlink();

    EXPECT_FALSE(a.is_linked());

    list.push_back(a);

    check_integrity(list, {2, 1});
}

TEST_F(ListTest, ReinsertAtFront) {
    list.push_back(a);
    list.push_back(b);

    b.unlink();

    list.push_front(b);

    check_integrity(list, {2, 1});
}

TEST_F(ListTest, ReinsertCycle) {

    for (int i = 0; i < 10; ++i) {
        list.push_back(a);

        EXPECT_TRUE(a.is_linked());

        a.unlink();

        EXPECT_FALSE(a.is_linked());
    }

    check_integrity(list, {});
}

TEST_F(ListTest, ReinsertAfterPop) {
    list.push_back(a);
    list.push_back(b);

    list.pop_front();
    list.push_back(a);

    check_integrity(list, {2, 1});
}

TEST_F(ListTest, InsertBeforeBegin) {
    list.push_back(b);

    list.insert(list.begin(), a);

    check_integrity(list, {1, 2});
}

TEST_F(ListTest, InsertBeforeEnd) {
    list.push_back(a);

    list.insert(list.end(), b);

    check_integrity(list, {1, 2});
}

TEST_F(ListTest, InsertInMiddle) {
    list.push_back(a);
    list.push_back(c);

    auto it = list.begin();

    ++it;

    list.insert(it, b);

    check_integrity(list, {1, 2, 3});
}

TEST_F(ListTest, InsertIntoEmpty) {
    list.insert(list.end(), a);

    check_integrity(list, {1});
}

TEST_F(ListTest, InsertReturnsCorrectIterator) {
    list.push_back(a);
    list.push_back(c);

    auto it = list.begin();

    ++it;

    auto inserted = list.insert(it, b);

    EXPECT_EQ(&(*inserted), &b);

    EXPECT_EQ(inserted->value, 2);
}

TEST_F(ListTest, EraseFirst) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    auto next = list.erase(list.begin());

    EXPECT_EQ(next->value, 2);

    EXPECT_FALSE(a.is_linked());

    check_integrity(list, {2, 3});
}

TEST_F(ListTest, EraseMiddle) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    auto it = list.begin();

    ++it;

    auto next = list.erase(it);

    EXPECT_EQ(next->value, 3);

    check_integrity(list, {1, 3});
}

TEST_F(ListTest, EraseLast) {
    list.push_back(a);
    list.push_back(b);

    auto it = list.begin();

    ++it;

    auto next = list.erase(it);

    EXPECT_EQ(next, list.end());

    check_integrity(list, {1});
}

TEST_F(ListTest, EraseOnly) {
    list.push_back(a);

    auto next = list.erase(list.begin());

    EXPECT_EQ(next, list.end());

    check_integrity(list, {});
}

TEST_F(ListTest, EraseWhileIterating) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.push_back(d);

    auto it = list.begin();


    while (it != list.end()) {
        if (it->value % 2 == 0) {
            it = list.erase(it);
        } else {
            ++it;
        }
    }

    check_integrity(list, {1, 3});
}

TEST_F(ListTest, EraseRangeAll) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    list.erase_range(list.begin(), list.end());

    check_integrity(list, {});
}

TEST_F(ListTest, EraseRangeMiddle) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.push_back(d);

    auto first = list.begin();

    ++first;
    auto last = first;


    ++last;
    ++last;

    list.erase_range(first, last);

    check_integrity(list, {1, 4});
}

TEST_F(ListTest, EraseRangeEmptyRange) {
    list.push_back(a);

    list.erase_range(list.begin(), list.begin());

    check_integrity(list, {1});
}

TEST_F(ListTest, ClearUnlinksAll) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    list.clear();

    EXPECT_FALSE(a.is_linked());
    EXPECT_FALSE(b.is_linked());
    EXPECT_FALSE(c.is_linked());

    check_integrity(list, {});
}

TEST_F(ListTest, ClearThenReuse) {
    list.push_back(a);
    list.push_back(b);

    list.clear();

    list.push_back(c);
    list.push_back(a);

    check_integrity(list, {3, 1});
}

TEST_F(ListTest, TryPopFrontNonEmpty) {
    list.push_back(a);
    list.push_back(b);

    auto* ptr = list.try_pop_front();

    ASSERT_NE(ptr, nullptr);

    EXPECT_EQ(ptr, &a);

    EXPECT_FALSE(a.is_linked());

    check_integrity(list, {2});
}

TEST_F(ListTest, TryPopFrontEmpty) {
    EXPECT_EQ(list.try_pop_front(), nullptr);
}

TEST_F(ListTest, TryPopBackNonEmpty) {
    list.push_back(a);
    list.push_back(b);

    auto* ptr = list.try_pop_back();

    ASSERT_NE(ptr, nullptr);

    EXPECT_EQ(ptr, &b);

    check_integrity(list, {1});
}

TEST_F(ListTest, TryPopBackEmpty) {
    EXPECT_EQ(list.try_pop_back(), nullptr);
}

TEST_F(ListTest, DrainViaTryPop) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    std::vector<int> drained;

    while (auto* ptr = list.try_pop_front()) {
        drained.push_back(ptr->value);
    }

    EXPECT_EQ(drained, (std::vector{1, 2, 3}));

    check_integrity(list, {});
}

TEST_F(ListTest, StaticRemoveLinked) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    ItemList::remove(b);

    EXPECT_FALSE(b.is_linked());

    check_integrity(list, {1, 3});
}

TEST_F(ListTest, StaticRemoveNotLinkedIsNoop) {
    ItemList::remove(a);

    EXPECT_FALSE(a.is_linked());
}

/// !!
TEST_F(ListTest, AutoUnlinkOnlyElement) {
    {
        Item temp{99};
        list.push_back(temp);
    }

    check_integrity(list, {});
}

TEST_F(ListTest, AutoUnlinkFirst) {
    {
        Item temp{99};
        list.push_back(temp);
        list.push_back(a);
    }

    check_integrity(list, {1});
}

TEST_F(ListTest, AutoUnlinkMiddle) {
    {
        Item temp{99};
        list.push_back(a);
        list.push_back(temp);
        list.push_back(b);
    }

    check_integrity(list, {1, 2});
}

TEST_F(ListTest, AutoUnlinkLast) {
    {
        Item temp{99};
        list.push_back(a);
        list.push_back(temp);
    }

    check_integrity(list, {1});
}

TEST_F(ListTest, AutoUnlinkMultipleInReverseDestructionOrder) {
    {
        Item t1{10}, t2{20};
        list.push_back(a);
        list.push_back(t1);
        list.push_back(t2);
        list.push_back(b);
    }

    check_integrity(list, {1, 2});
}

TEST_F(ListTest, AutoUnlinkNotLinkedIsHarmless) {
    {
        Item temp{99};
    }

    check_integrity(list, {});
}

TEST_F(ListTest, SpliceAllToEnd) {
    ItemList other;
    list.push_back(a);
    list.push_back(b);
    other.push_back(c);
    other.push_back(d);


    list.splice(list.end(), other);

    check_integrity(list, {1, 2, 3, 4});
    check_integrity(other, {});
}

TEST_F(ListTest, SpliceAllToBegin) {
    ItemList other;
    list.push_back(c);
    list.push_back(d);
    other.push_back(a);
    other.push_back(b);

    list.splice(list.begin(), other);


    check_integrity(list, {1, 2, 3, 4});
    check_integrity(other, {});
}

TEST_F(ListTest, SpliceToMiddle) {
    ItemList other;
    list.push_back(a);
    list.push_back(d);
    other.push_back(b);
    other.push_back(c);

    auto it = list.begin();

    ++it;

    list.splice(it, other);

    check_integrity(list, {1, 2, 3, 4});
    check_integrity(other, {});
}

TEST_F(ListTest, SpliceEmptySource) {
    ItemList other;
    list.push_back(a);

    list.splice(list.end(), other);

    check_integrity(list, {1});
}

TEST_F(ListTest, SpliceIntoEmpty) {
    ItemList other;
    other.push_back(a);
    other.push_back(b);

    list.splice(list.end(), other);

    check_integrity(list, {1, 2});
    check_integrity(other, {});
}

TEST_F(ListTest, SpliceRangePartial) {
    ItemList other;
    list.push_back(a);
    other.push_back(b);
    other.push_back(c);
    other.push_back(d);

    auto first = other.begin();
    auto last = first;

    ++last;
    ++last;

    list.splice_range(list.end(), other, first, last);

    check_integrity(list, {1, 2, 3});
    check_integrity(other, {4});
}

TEST_F(ListTest, SpliceCellSingle) {
    ItemList other;
    list.push_back(a);
    list.push_back(c);
    other.push_back(b);

    auto it = list.begin();

    ++it;

    list.splice_cell(it, other, other.begin());


    check_integrity(list, {1, 2, 3});
    check_integrity(other, {});
}

TEST_F(ListTest, ExtractFrontPartial) {
    ItemList out;
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.push_back(d);

    auto count = list.extract_front(out, 2);

    EXPECT_EQ(count, 2u);


    check_integrity(out, {1, 2});
    check_integrity(list, {3, 4});
}

TEST_F(ListTest, ExtractFrontAll) {
    ItemList out;
    list.push_back(a);
    list.push_back(b);

    auto count = list.extract_front(out, 100);

    EXPECT_EQ(count, 2u);

    check_integrity(out, {1, 2});
    check_integrity(list, {});
}

TEST_F(ListTest, ExtractFrontZero) {
    ItemList out;
    list.push_back(a);

    auto count = list.extract_front(out, 0);

    EXPECT_EQ(count, 0u);

    check_integrity(list, {1});

    check_integrity(out, {});
}

TEST_F(ListTest, ExtractFrontFromEmpty) {
    ItemList out;
    auto count = list.extract_front(out, 5);

    EXPECT_EQ(count, 0u);
}

TEST_F(ListTest, IteratorStableAfterUnlinkOther) {
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    auto it = list.begin();

    ++it;

    ++it;

    b.unlink();


    EXPECT_EQ(it->value, 3);
}

TEST_F(ListTest, IteratorStableAfterPushBack) {
    list.push_back(a);

    auto it = list.begin();


    list.push_back(b);

    EXPECT_EQ(it->value, 1);
}

TEST_F(ListTest, IteratorStableAfterPushFront) {
    list.push_back(b);

    auto it = list.begin();

    list.push_front(a);

    EXPECT_EQ(it->value, 2);
}

TEST_F(ListTest, DereferenceReturnsOriginalObject) {
    list.push_back(a);
    list.push_back(b);

    auto it = list.begin();

    EXPECT_EQ(&(*it), &a);

    ++it;

    EXPECT_EQ(&(*it), &b);
}

TEST_F(ListTest, ArrowReturnsOriginalObject) {
    list.push_back(a);

    EXPECT_EQ(&(list.begin()->value), &a.value);
}

TEST_F(ListTest, FrontBackReturnOriginalObjects) {
    list.push_back(a);
    list.push_back(b);

    // ---

    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &b);
}

TEST_F(ListTest, MutateThroughIterator) {
    list.push_back(a);
    list.push_back(b);

    for (auto& item : list) {
        item.value *= 10;
    }

    EXPECT_EQ(a.value, 10);
    EXPECT_EQ(b.value, 20);
}
