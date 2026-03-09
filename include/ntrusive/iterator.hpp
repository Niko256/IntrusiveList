#pragma once

#include "base_node.hpp"
#include "node.hpp"
#include <cstddef>
#include <iterator>

/**
 * @brief Bidirectional Iterator for IntrusiveList<...>
 */
template <typename T>
class ListIterator {
  public:
    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = std::bidirectional_iterator_tag;

    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    using node_type = IntrusiveListNode;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

    constexpr ListIterator() noexcept = default;

    /* @param hook Pointer to the node (may be sentinel) */
    constexpr explicit ListIterator(NodeBase* hook) noexcept;

    [[nodiscard]]
    constexpr auto operator*() const noexcept -> reference;

    [[nodiscard]]
    constexpr auto operator->() const noexcept -> pointer;

    constexpr auto operator++() noexcept -> ListIterator&;

    constexpr auto operator++(int) noexcept -> ListIterator;

    constexpr auto operator--() noexcept -> ListIterator&;

    constexpr auto operator--(int) noexcept -> ListIterator;

    constexpr auto operator==(const ListIterator& other) const noexcept -> bool;

    constexpr auto operator!=(const ListIterator& other) const noexcept -> bool;

    explicit operator ListIterator<const value_type>() const noexcept;

    [[nodiscard]] constexpr auto base() const noexcept -> NodeBase*;

  private:
    NodeBase* current_{nullptr};
};

template <typename T>
using ConstListIterator = ListIterator<const T>;

/*---*---*---*---*---*---*---*---IMPL---*---*---*---*---*---*---*---*---*/

template <typename T>
constexpr ListIterator<T>::ListIterator(NodeBase* hook) noexcept
    : current_(hook) {}

template <typename T>
constexpr auto ListIterator<T>::operator*() const noexcept -> reference {
    /**
     *
     * NodeBase* --> node_type* --> T*
     *
     */

    return *static_cast<pointer>(static_cast<node_type*>(current_));
}

template <typename T>
constexpr auto ListIterator<T>::operator->() const noexcept -> pointer {
    return static_cast<pointer>(static_cast<node_type*>(current_));
}

template <typename T>
constexpr auto ListIterator<T>::operator++() noexcept -> ListIterator<T>& {
    current_ = current_->next_node();
    return *this;
}

template <typename T>
constexpr auto ListIterator<T>::operator--() noexcept -> ListIterator<T>& {
    current_ = current_->prev_node();
    return *this;
}

template <typename T>
constexpr auto ListIterator<T>::operator++(int) noexcept -> ListIterator<T> {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template <typename T>
constexpr auto ListIterator<T>::operator--(int) noexcept -> ListIterator<T> {
    auto tmp = *this;
    --(*this);
    return tmp;
}

template <typename T>
constexpr auto ListIterator<T>::operator==(const ListIterator& other) const noexcept -> bool {
    return current_ == other.current_;
}

template <typename T>
constexpr auto ListIterator<T>::operator!=(const ListIterator& other) const noexcept -> bool {
    return current_ != other.current_;
}

template <typename T>
ListIterator<T>::operator ListIterator<const value_type>() const noexcept {
    return ListIterator<const value_type>{current_};
}

template <typename T>
constexpr auto ListIterator<T>::base() const noexcept -> NodeBase* {
    return current_;
}

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
