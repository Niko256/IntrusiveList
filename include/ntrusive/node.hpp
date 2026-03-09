#pragma once

#include "base_node.hpp"
#include <cassert>
#include <cstdio>

/**
 * @brief Intrusive List Node.
 * Inherit from this class to make your object linkable.
 *
 * >> SAFETY : Automatically unlinks on destruction.
 *
 * >> USAGE :
 *  struct Task : IntrusiveListNode {
 *    void whatever(...);
 *  };
 *
 *
 */
class IntrusiveListNode : public NodeBase {
  private:
    /*---*---*---*---*---*---*---*/

    bool is_linked_{false};

    /*---*---*---*---*---*---*---*/

  public:
    constexpr IntrusiveListNode() noexcept = default;

    ~IntrusiveListNode();

    /* non-copyable */
    IntrusiveListNode(const IntrusiveListNode&) = delete;
    auto operator=(const IntrusiveListNode&) -> IntrusiveListNode& = delete;

    /* non-moveble */
    IntrusiveListNode(IntrusiveListNode&&) noexcept = delete;
    auto operator=(IntrusiveListNode&&) noexcept -> IntrusiveListNode& = delete;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

    /**
     * @brief Is this node currently in a list?
     */
    [[nodiscard]]
    constexpr auto is_linked() const noexcept -> bool;

    /**
     * @brief Removes this node from its current list.
     */
    void unlink() noexcept;

  private:
    /**
     * @brief marks node as linked.
     */
    constexpr void set_linked() noexcept;

  private:
    /**
     * @brief Link this node this with two others
     */
    void link_between(NodeBase* prev, NodeBase* next) noexcept;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

    template <typename>
    friend class IntrusiveList;

    template <typename>
    friend class ListIterator;
};

/*---*---*---*---*---*---*---*---* IMPL *---*---*---*---*---*---*---*---*/

IntrusiveListNode::~IntrusiveListNode() {
    /* Warn!!! */
    if (is_linked_) {
#ifndef NDEBUG
        std::fprintf(
            stderr,
            "[ntrusive] : WARNING : destroying node still in list.. "
            "auto-unlinking.."
        );
#endif

        unlink();
    }
}

constexpr auto IntrusiveListNode::is_linked() const noexcept -> bool {
    return is_linked_;
}

void IntrusiveListNode::unlink() noexcept {
    assert(is_linked_ && "attempting to unlink node not in a list...");

    /* ................... */

    unlink_base();
    is_linked_ = false;

    /* ................... */
}

constexpr void IntrusiveListNode::set_linked() noexcept {
    is_linked_ = true;
}

void IntrusiveListNode::link_between(NodeBase* prev, NodeBase* next) noexcept {
    /* ................... */

    link_between_base(prev, next);
    set_linked();

    /* ................... */
}

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
