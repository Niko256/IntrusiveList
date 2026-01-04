#pragma once

#include "iterator.hpp"
#include "node.hpp"
#include <cassert>
#include <cstddef>
#include <type_traits>


/**
 *
 * @section PHILOSOPHY
 * Unlike standart containers (like std::list), an Intrusive List doesn't manage
 * memory lifetimes of the objects it stores. Instead, the linkage metadata (like 'prev' and 'next' fields)
 * are embedded directly within the stored object itself.
 *
 * This approach has significant advantages:
 *  |> Zero Allocations on push/pop operations
 *  |> Memory Locality
 *
 */

/* ------------------------------------------------------------------- */

template <typename T, typename Tag>
concept HasListHook = std::is_base_of_v<IntrusiveListNode<Tag>, T>;

template <typename T, typename Tag>
class IntrusiveList {
  public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = ListIterator<T, IntrusiveListNode<Tag>>;
    using const_iterator = ConstListIterator<T, IntrusiveListNode<Tag>>;

    using IntrusiveListNode = IntrusiveListNode<Tag>;

    /* ------------------------------------------------------------------- */

  public:
    IntrusiveList() noexcept;

    ~IntrusiveList();

    IntrusiveList(const IntrusiveList&) = delete;
    IntrusiveList& operator=(const IntrusiveList&) = delete;

    IntrusiveList(IntrusiveList&&) noexcept = delete;
    IntrusiveList& operator=(IntrusiveList&&) noexcept = delete;

    /* ------------------------------------------------------------------- */

    void push_back(reference element) noexcept;

    void push_front(reference element) noexcept;

    iterator insert(const_iterator pos, reference element) noexcept;

    void pop_front() noexcept;

    void pop_back() noexcept;

    iterator erase(const_iterator pos) noexcept;

    iterator erase_range(const_iterator first, const_iterator last) noexcept;

    void clear() noexcept;

    void splice(const_iterator pos, IntrusiveList& other) noexcept;

    void splice_cell(const_iterator pos, IntrusiveList& other, const_iterator element) noexcept;

    void splice_range(const_iterator pos, IntrusiveList& other, const_iterator first, const_iterator last) noexcept;

    [[nodiscard]]
    pointer try_pop_front() noexcept;

    [[nodiscard]]
    pointer try_pop_back() noexcept;

    [[nodiscard]]
    size_type extract_front(IntrusiveList& out, size_type max_cnt) noexcept;

    static void remove(reference element) noexcept;

    /* ------------------------------------------------------------------- */

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]]
    size_type size() const noexcept;

    [[nodiscard]]
    reference front() noexcept;

    [[nodiscard]]
    const_reference front() const noexcept;

    [[nodiscard]]
    reference back() noexcept;

    [[nodiscard]]
    const_reference back() const noexcept;

    /* ------------------------------------------------------------------- */

    [[nodiscard]]
    iterator begin() noexcept;

    [[nodiscard]]
    const_iterator begin() const noexcept;

    [[nodiscard]]
    const_iterator cbegin() const noexcept;

    [[nodiscard]]
    iterator end() noexcept;

    [[nodiscard]]
    const_iterator end() const noexcept;

    [[nodiscard]]
    const_iterator cend() const noexcept;

    /* ------------------------------------------------------------------- */

  private:
    mutable IntrusiveListNode sentinel_;
};

/* ------------------------------------------------------------------- */
