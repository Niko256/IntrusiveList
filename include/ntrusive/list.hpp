#pragma once

#include "base_node.hpp"
#include "iterator.hpp"
#include "node.hpp"
#include <cassert>
#include <cstddef>
#include <type_traits>


/**
 *
 * @section PHILOSOPHY
 * Unlike standart containers (like std::list), an Intrusive List doesn't manage
 * memory lifetimes of objects it stores. Instead, the linkage metadata (like 'prev' and 'next' fields)
 * are embedded directly within the stored object itself.
 *
 * This approach has significant advantages:
 *  >> Zero Allocations on push/pop operations
 *  >> Memory Locality
 *
 */

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

/**
 * @brief Concept to verify T inherits from IntrusiveListNode .
 */
template <typename T>
concept DerivedFromNode = std::is_base_of_v<IntrusiveListNode, T>;

template <typename T>
class IntrusiveList {
  public:
    static_assert(DerivedFromNode<T>,
                  "T must inherit from IntrusiveListNode<...>");

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using node_type = IntrusiveListNode;
    using sentinel_type = IntrusiveListNode;

    using iterator = ListIterator<T>;
    using const_iterator = ListIterator<const T>;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

  public:
    IntrusiveList() noexcept;

    ~IntrusiveList();

    IntrusiveList(const IntrusiveList&) = delete;
    IntrusiveList& operator=(const IntrusiveList&) = delete;

    IntrusiveList(IntrusiveList&&) noexcept = delete;
    IntrusiveList& operator=(IntrusiveList&&) noexcept = delete;

  private:
    void init_sentinel() noexcept;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
  public:
    void push_back(reference element) noexcept;

    void push_front(reference element) noexcept;

    auto insert(const_iterator pos, reference element) noexcept -> iterator;

    void pop_front() noexcept;

    void pop_back() noexcept;

    auto erase(const_iterator pos) noexcept -> iterator;

    auto erase_range(const_iterator first, const_iterator last) noexcept -> iterator;

    void clear() noexcept;

    /**
     * @brief Transfer all elements from outer list before specific position.
     */
    void splice(const_iterator pos, IntrusiveList& other) noexcept;

    /**
     * @brief Transfer single element.
     *
     * Equivalent to splice_range() with single-element range.
     */
    void splice_cell(const_iterator pos, IntrusiveList& other, const_iterator element) noexcept;

    /**
     * @brief Transfer range of elements [first, last) from other to this list, inserting before specific position
     */
    void splice_range(const_iterator pos, IntrusiveList& other, const_iterator first, const_iterator last) noexcept;

    [[nodiscard]]
    auto try_pop_front() noexcept -> pointer;

    [[nodiscard]]
    auto try_pop_back() noexcept -> pointer;

    [[nodiscard]]
    auto extract_front(IntrusiveList& out, size_type max_cnt) noexcept -> size_type;

    /**
     * @brief Self removal of an element from ANY list.
     * This method enables objects to remove themselves without needing a ref to their containing list.
     *
     * @param Reference to the element to remove.
     */
    static void remove(reference element) noexcept;

  private:
    void insert_after(NodeBase* after, reference value) noexcept;

    void insert_before(NodeBase* before, reference value) noexcept;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

  public:
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]]
    auto size() const noexcept -> size_type;

    [[nodiscard]]
    auto front() noexcept -> reference;

    [[nodiscard]]
    auto front() const noexcept -> const_reference;

    [[nodiscard]]
    auto back() noexcept -> reference;

    [[nodiscard]]
    auto back() const noexcept -> const_reference;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

    [[nodiscard]]
    auto begin() noexcept -> iterator;

    [[nodiscard]]
    auto cbegin() const noexcept -> const_iterator;

    [[nodiscard]]
    auto end() noexcept -> iterator;

    [[nodiscard]]
    auto cend() const noexcept -> const_iterator;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

  private:
    NodeBase sentinel_;
};

/*---*---*---*---*---*---*---*---* IMPL *---*---*---*---*---*---*---*---*---*/

template <typename T>
IntrusiveList<T>::IntrusiveList() noexcept {
    init_sentinel();
}

template <typename T>
IntrusiveList<T>::~IntrusiveList() {
    /**
     * assert(empty() && "Destroying non-empty IntrusiveList. Unlink elements first!!");
     */
    clear();
}

template <typename T>
void IntrusiveList<T>::init_sentinel() noexcept {
    sentinel_.set_next(&sentinel_);
    sentinel_.set_prev(&sentinel_);
}

/*---*---*---*---*---*---*---* Capacity *---*---*---*---*---*---*---*/

template <typename T>
bool IntrusiveList<T>::empty() const noexcept {
    return sentinel_.next_node() == &sentinel_;
}

/* O(n) */
template <typename T>
auto IntrusiveList<T>::size() const noexcept -> size_type {
    size_type cnt = 0;

    for (auto it = cbegin(); it != cend(); ++it) {
        ++cnt;
    }

    return cnt;
}

/*---*---*---*---*---*---*---* Iterators *---*---*---*---*---*---*---*/

template <typename T>
auto IntrusiveList<T>::front() noexcept -> reference {
    assert(!empty() && "front() called on empty list...");
    return *begin();
}

template <typename T>
auto IntrusiveList<T>::front() const noexcept -> const_reference {
    assert(!empty() && "front() called on empty list...");
    return *cbegin();
}

template <typename T>
auto IntrusiveList<T>::back() noexcept -> reference {
    assert(!empty() && "back() called on empty list...");
    /**
     * end() points to -> sentinel => sentinel.prev_ is the last element
     */
    return *(--end());
}

template <typename T>
auto IntrusiveList<T>::back() const noexcept -> const_reference {
    assert(!empty() && "back() called on empty list...");
    return *(--cend());
}

template <typename T>
auto IntrusiveList<T>::begin() noexcept -> iterator {
    /**
     * First element is sentinel.next
     * If list is empty, sentinel.next == &sentinel, so begin() == end()
     */
    return iterator(sentinel_.next_node());
}

template <typename T>
auto IntrusiveList<T>::cbegin() const noexcept -> const_iterator {
    return const_iterator(const_cast<NodeBase*>(sentinel_.next_node()));
}

template <typename T>
auto IntrusiveList<T>::end() noexcept -> iterator {
    /**
     * end() points to the sentinel
     */
    return iterator(&sentinel_);
}

template <typename T>
auto IntrusiveList<T>::cend() const noexcept -> const_iterator {
    return const_iterator(const_cast<NodeBase*>(&sentinel_));
}

/*---*---*---*---*---*---*---* Modifiers *---*---*---*---*---*---*---*/

template <typename T>
void IntrusiveList<T>::insert_after(NodeBase* after, reference value) noexcept {
    auto& node = value;

    assert(!node.is_linked() && "Element already in a list!!");

    /**
     * Before : after <-> next
     * After : afetr <-> node <-> next
     */
    node.link_between(after, after->next_node());
}

template <typename T>
void IntrusiveList<T>::insert_before(NodeBase* before,
                                     reference value) noexcept {
    auto& node = value;

    assert(!node.is_linked() && "Element already in a list!!");

    /**
     * Before : prev <-> before
     * After : prev <-> node <-> before
     */

    node.link_between(before->prev_node(), before);
}

template <typename T>
void IntrusiveList<T>::push_back(reference element) noexcept {
    /**
     * Before : ... <-> last <-> sentinel
     * After : ... <-> last' <-> element <-> sentinel
     */
    insert_before(&sentinel_, element);
}

template <typename T>
void IntrusiveList<T>::push_front(reference element) noexcept {
    /**
     * Before : sentinel <-> first <-> ...
     * After : sentinel <-> element <-> first' <-> ...
     */
    insert_after(&sentinel_, element);
}

template <typename T>
auto IntrusiveList<T>::insert(
    const_iterator pos, reference element) noexcept -> iterator {
    /**
     * insert before pos (because inserted node position will become new pos)
     */
    insert_before(pos.base(), element);

    return iterator(static_cast<node_type*>(&static_cast<node_type&>(element)));
}

template <typename T>
void IntrusiveList<T>::pop_front() noexcept {
    assert(!empty() && "pop_front() on empty list!!");

    /**
     * unlink the first element
     */
    static_cast<node_type*>(sentinel_.next_node())->unlink();
}

template <typename T>
void IntrusiveList<T>::pop_back() noexcept {
    assert(!empty() && "pop_front() on empty list!!");

    /**
     * unlink the last element
     */
    static_cast<node_type*>(sentinel_.prev_node())->unlink();
}

template <typename T>
auto IntrusiveList<T>::erase(const_iterator pos) noexcept -> iterator {
    assert(pos != end() && "Cannot erase sentinel...");

    NodeBase* node = pos.base();
    NodeBase* next = node->next_node();

    static_cast<node_type*>(node)->unlink();

    /* return the iterator to the element that followed the erased one */
    return iterator(next);
}

template <typename T>
auto IntrusiveList<T>::erase_range(const_iterator first,
                                   const_iterator last) noexcept -> iterator {

    auto it = iterator(first.base());
    auto stop = last.base();

    while (it.base() != stop) {
        it = erase(it);
    }

    return iterator(stop);
}

template <typename T>
void IntrusiveList<T>::clear() noexcept {
    /*
     * remove all elements one by one
     */
    while (!empty()) {
        pop_front();
    }
}

template <typename T>
auto IntrusiveList<T>::extract_front(IntrusiveList& out,
                                     size_type max_cnt) noexcept -> size_type {
    /*
     * Extract up max_cnt elements from the front to the end of outer list
     */

    size_type count = 0;
    auto split_point = begin();

    /* find where to split */
    while (split_point != end() && count < max_cnt) {
        ++split_point;
        ++count;
    }

    /* transfer [begin, split_point) to the outer list */
    if (count > 0) {
        splice_range(out.end(), *this, begin(), split_point);
    }

    return count;
}

template <typename T>
void IntrusiveList<T>::remove(reference element) noexcept {
    /*
     * derived-to-base conversion : T& -> IntrusiveListNode<...>&
     * [T must inherit from IntrusiveListNode<>]
     *
     * Magic : this conversion unlocks self-removal because
     * any T can access its embedded list node without knowing specific list containing it.
     * => IntrusiveList<Task>::remove(*this);
     */
    node_type& node = element;

    if (node.is_linked()) {
        node.unlink();
    }
}

template <typename T>
void IntrusiveList<T>::splice_range(
    const_iterator position, IntrusiveList& other,
    const_iterator first, const_iterator last) noexcept {

    /*
     * Before :
     *    other list : ... <-> before <-> [first <-> ... <-> last] <-> after <-> ...
     *
     *    this list : ... <-> before' <-> position <-> after' <-> ...
     *
     * After :
     *    other list : ... <-> before <-> after <-> ...
     *
     *    this list : ... <-> before' <-> [first <-> ... <-> last] <-> position <-> after' <-> ...
     */

    NodeBase::transfer_range(position.base(), first.base(), last.base());
}

template <typename T>
void IntrusiveList<T>::splice(const_iterator position,
                              IntrusiveList& other) noexcept {
    if (other.empty()) {
        return;
    }

    if (this == &other) {
        return;
    }

    splice_range(position, other, other.begin(), other.end());
}

template <typename T>
void IntrusiveList<T>::splice_cell(const_iterator position,
                                   IntrusiveList& other,
                                   const_iterator element) noexcept {

    if (element == other.end()) {
        return;
    }

    auto next = element;
    ++next;

    splice_range(position, other, element, next);
}

/*---*---*---*---*---*---*---*---* TRY *---*---*---*---*---*---*---*---*/

template <typename T>
auto IntrusiveList<T>::try_pop_front() noexcept -> pointer {
    if (empty()) {
        return nullptr;
    }

    pointer result = &(front());
    pop_front();

    return result;
}

template <typename T>
auto IntrusiveList<T>::try_pop_back() noexcept -> pointer {
    if (empty()) {
        return nullptr;
    }

    pointer res = &(back());
    pop_back();

    return res;
}

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
