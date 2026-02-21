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
  public:
    constexpr IntrusiveListNode() noexcept = default;

    ~IntrusiveListNode();

    /* non-copyable */
    IntrusiveListNode(const IntrusiveListNode&) = delete;
    IntrusiveListNode& operator=(const IntrusiveListNode&) = delete;

    /* non-moveble */
    IntrusiveListNode(IntrusiveListNode&&) noexcept = delete;
    IntrusiveListNode& operator=(IntrusiveListNode&&) noexcept = delete;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

    /**
     * @brief Is this node currently in a list?
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

  private:
    /**
     * @brief Link this node this with two others
     */
    void link_between(NodeBase* prev, NodeBase* next) noexcept;

    /*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/

  private:
    bool is_linked_{false};

    template <typename>
    friend class IntrusiveList;

    template <typename>
    friend class ListIterator;
};

/*---*---*---*---*---*---*---*---* IMPL *---*---*---*---*---*---*---*---*---*/

IntrusiveListNode::~IntrusiveListNode() {
    /* Warn!!! */
    if (is_linked_) {
        #ifndef NDEBUG
        fprintf(stderr,
                        "[ntrusive] : WARNING : destroying node still in list.. auto-unlinking..\n");
        #endif

        unlink();
    }
}

constexpr bool IntrusiveListNode::is_linked() const noexcept {
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

void IntrusiveListNode::link_between(NodeBase* prev,
                                     NodeBase* next) noexcept {

    /* ................... */

    link_between_base(prev, next);
    set_linked();

    /* ................... */
}

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
