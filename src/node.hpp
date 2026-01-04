#pragma once

#include "policies.hpp"
#include <cstdint>
#include <type_traits>

/**
 * @section DANGER_AND_SAFETY
 * The biggest risk in intrusive containers is the 'dangling pointer'
 * If a Node is destroyed while still being linked in a list
 * => the list becomes corrupted => undefined behavior
 *
 * To avoid this problem, this class provides different safety policies.
 */


/**
 * @brief Intrusive List Node Hook.
 * Inherit from this class to make your object linkable.
 *
 */
template <typename Tag = void, LinkPolicy Mode = Link, typename DestructionHandler = AssertHandler>
class IntrusiveListNode {
  public:
    static constexpr Mode link_policy;

    constexpr IntrusiveListNode() noexcept = default;

    ~IntrusiveListNode();

    /* non-copyable */
    IntrusiveListNode(const IntrusiveListNode&) = delete;
    IntrusiveListNode& operator=(const IntrusiveListNode&) = delete;

    /* non-moveble */
    IntrusiveListNode(IntrusiveListNode&&) noexcept = delete;
    IntrusiveListNode& operator=(IntrusiveListNode&&) noexcept = delete;

    /* ------------------------------------------------------------------- */

    [[nodiscard]]
    constexpr bool is_linked() const noexcept;

    void unlink() noexcept;

  private:
    constexpr void reset_links() noexcept;

    constexpr void set_linked() noexcept;

    constexpr void link_between(IntrusiveListNode* prev, IntrusiveListNode* next) noexcept;

    /* ------------------------------------------------------------------- */

    [[nodiscard]]
    constexpr IntrusiveListNode* next_node() const noexcept;

    [[nodiscard]]
    constexpr IntrusiveListNode* prev_node() const noexcept;

    constexpr void set_next() noexcept;

    constexpr void set_prev() noexcept;

    /* ------------------------------------------------------------------- */

  private:
    IntrusiveListNode* prev_{nullptr};
    IntrusiveListNode* next_{nullptr};

    [[no_unique_address]]
    std::conditional_t<Mode == LinkMode::Raw, std::false_type, bool> is_linked_{};

    template <typename, typename>
    friend class IntrusiveList;

    template <typename, typename>
    friend class ListIterator;
};

/* ------------------------------------------------------------------- */

template <typename Tag, LinkPolicy Mode, typename DestructionHandler>
IntrusiveListNode<Tag, Mode, DestructionHandler>::~IntrusiveListNode() {
}
