#pragma once

#include "base_node.hpp"
#include "policies.hpp"
#include <cassert>
#include <concepts>

/**
 * @brief Intrusive List Node.
 * Inherit from this class to make your object linkable.
 *
 */
template <typename Tag = void, LinkPolicy Policy = TrackingPolicy, typename Handler = AutoUnlinkHandler>
class IntrusiveListNode : public NodeBase {
  public:
    using tag_type = Tag;
    using policy_type = Policy;
    using destruction_handler = Handler;
    using state_type = typename Policy::State;

    constexpr IntrusiveListNode() noexcept = default;

    ~IntrusiveListNode();

    /* non-copyable */
    IntrusiveListNode(const IntrusiveListNode&) = delete;
    IntrusiveListNode& operator=(const IntrusiveListNode&) = delete;

    /* non-moveble */
    IntrusiveListNode(IntrusiveListNode&&) noexcept = delete;
    IntrusiveListNode& operator=(IntrusiveListNode&&) noexcept = delete;

    /* ------------------------------------------------------------------- */

    /**
     * @brief Checks if this node is currently in a list.
     */
    [[nodiscard]]
    constexpr bool is_linked() const noexcept;

    /**
     * @brief Removes this node from its current list.
     */
    void unlink() noexcept;

  private:
    /**
     * @brief marks node as linked.
     */
    constexpr void set_linked() noexcept;

    /**
     * @brief Link this node this with two others
     */
    void link_between(IntrusiveListNode* prev, IntrusiveListNode* next) noexcept;

    /* ------------------------------------------------------------------- */

  private:
    [[no_unique_address]]
    state_type state_;

    template <typename, typename>
    friend class IntrusiveList;

    template <typename, typename>
    friend class ListIterator;
};

/* ------------------------------------------------------------------- */

template <typename Tag, LinkPolicy Policy, typename Handler>
IntrusiveListNode<Tag, Policy, Handler>::~IntrusiveListNode() {
    Handler::on_destroy(*this);
}

template <typename Tag, LinkPolicy Policy, typename Handler>
constexpr bool IntrusiveListNode<Tag, Policy, Handler>::is_linked() const noexcept {
    return Policy::is_linked(state_);
}

template <typename Tag, LinkPolicy Policy, typename Handler>
void IntrusiveListNode<Tag, Policy, Handler>::unlink() noexcept {

    if constexpr (std::same_as<Policy, TrackingPolicy>) {
        assert(is_linked() && "Attempting to unlink element not in list!!");
    }

    /* ................... */

    unlink_base();
    Policy::on_unlink(state_);

    /* ................... */
}

template <typename Tag, LinkPolicy Policy, typename Handler>
constexpr void IntrusiveListNode<Tag, Policy, Handler>::set_linked() noexcept {
    Policy::on_link(state_);
}

template <typename Tag, LinkPolicy Policy, typename Handler>
void IntrusiveListNode<Tag, Policy, Handler>::link_between(
    IntrusiveListNode* prev, IntrusiveListNode* next) noexcept {

    /* ................... */

    link_between(prev, next);
    set_linked();

    /* ................... */
}
