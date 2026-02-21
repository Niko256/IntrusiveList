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
    constexpr reference operator*() const noexcept;

    [[nodiscard]]
    constexpr pointer operator->() const noexcept;

    constexpr ListIterator& operator++() noexcept;

    constexpr ListIterator operator++(int) noexcept;

    constexpr ListIterator& operator--() noexcept;

    constexpr ListIterator operator--(int) noexcept;

    constexpr bool operator==(const ListIterator& other) const noexcept;

    constexpr bool operator!=(const ListIterator& other) const noexcept;

    operator ListIterator<const value_type>() const noexcept;

    [[nodiscard]] constexpr NodeBase* base() const noexcept;

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
constexpr typename ListIterator<T>::reference
ListIterator<T>::operator*() const noexcept {

    /**
     *
     * NodeBase* --> node_type* --> T*
     *
     */

    return *static_cast<pointer>(static_cast<node_type*>(current_));
}

template <typename T>
constexpr typename ListIterator<T>::pointer
ListIterator<T>::operator->() const noexcept {
    return static_cast<pointer>(static_cast<node_type*>(current_));
}

template <typename T>
constexpr ListIterator<T>& ListIterator<T>::operator++() noexcept {
    current_ = current_->next_node();
    return *this;
}

template <typename T>
constexpr ListIterator<T>& ListIterator<T>::operator--() noexcept {
    current_ = current_->prev_node();
    return *this;
}

template <typename T>
constexpr ListIterator<T> ListIterator<T>::operator++(int) noexcept {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template <typename T>
constexpr ListIterator<T> ListIterator<T>::operator--(int) noexcept {
    auto tmp = *this;
    --(*this);
    return tmp;
}

template <typename T>
constexpr bool ListIterator<T>::operator==(const ListIterator& other) const noexcept {
    return current_ == other.current_;
}

template <typename T>
constexpr bool ListIterator<T>::operator!=(const ListIterator& other) const noexcept {
    return current_ != other.current_;
}

template <typename T>
ListIterator<T>::operator ListIterator<const value_type>() const noexcept {
    return ListIterator<const value_type>{current_};
}

template <typename T>
constexpr NodeBase* ListIterator<T>::base() const noexcept {
    return current_;
}

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
