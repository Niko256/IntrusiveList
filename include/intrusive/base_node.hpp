#pragma once

struct NodeBase {
    NodeBase* prev_;
    NodeBase* next_;

    /* ------------------------------------------------------------------- */

    [[nodiscard]]
    NodeBase* next_node() noexcept;

    [[nodiscard]]
    const NodeBase* next_node() const noexcept;

    [[nodiscard]]
    NodeBase* prev_node() noexcept;

    [[nodiscard]]
    const NodeBase* prev_node() const noexcept;

    void set_next(NodeBase* n) noexcept;
    void set_prev(NodeBase* p) noexcept;

    /**
     * @brief Links this node between prev and next.
     *
     * Before: prev <-> next
     * After:  prev <-> this <-> next
     */
    void link_between(NodeBase* prev, NodeBase* next) noexcept;

    /**
     * @brief Unlinks this node from its neighbors.
     *
     * Before: prev <-> this <-> next
     * After:  prev <-> next
     *
     * and now 'this' is isolated
     */
    void unlink_base() noexcept;

    /**
     * @brief Checks if node appears to be linked (has neighbors).
     *
     * Note: This is a basic check. For proper tracking, use
     * IntrusiveListNode with TrackingPolicy.
     */
    [[nodiscard]]
    bool is_linked_base() const noexcept;
};

/**
 * @brief Initializes a node as an empty list sentinel.
 *
 * Empty sentinel state: prev and next point to itself.
 *
 * */
inline void init_sentinel(NodeBase* sentinel) noexcept;

/**
 * @brief Checks if sentinel represents an empty list.
 */
[[nodiscard]]
inline bool is_empty_sentinel(const NodeBase* sentinel) noexcept;

/**
 * @brief Transfers range [first, last) before pos.
 *
 * @param pos Position to insert before
 * @param first First node of range to transfer (inclusive)
 * @param last End of range (exclusive)
 *
 *
 * Before:
 *   Source: ... <-> before_first <-> [first...actual_last] <-> last <-> ...
 *   Dest:   ... <-> before_pos <-> pos <-> ...
 *
 * After:
 *   Source: ... <-> before_first <-> last <-> ...
 *   Dest:   ... <-> before_pos <-> [first...actual_last] <-> pos <-> ...
 */
inline void transfer_range(NodeBase* pos, NodeBase* first, NodeBase* last) noexcept;

inline NodeBase* NodeBase::next_node() noexcept {
    return next_;
}

inline const NodeBase* NodeBase::next_node() const noexcept {
    return next_;
}

inline NodeBase* NodeBase::prev_node() noexcept {
    return prev_;
}

inline const NodeBase* NodeBase::prev_node() const noexcept {
    return prev_;
}

inline void NodeBase::set_next(NodeBase* n) noexcept {
    next_ = n;
}

inline void NodeBase::set_prev(NodeBase* p) noexcept {
    prev_ = p;
}

inline void NodeBase::link_between(NodeBase* prev, NodeBase* next) noexcept {
    prev_ = prev;
    next_ = next;
    prev->next_ = this;
    next->prev_ = this;
}

inline void NodeBase::unlink_base() noexcept {
    prev_->next_ = next_;
    next_->prev_ = prev_;
    prev_ = nullptr;
    next_ = nullptr;
}

inline bool NodeBase::is_linked_base() const noexcept {
    return next_ != nullptr;
}

inline void init_sentinel(NodeBase* sentinel) noexcept {
    sentinel->prev_ = sentinel;
    sentinel->next_ = sentinel;
}

inline bool is_empty_sentinel(const NodeBase* sentinel) noexcept {
    return sentinel->next_ == sentinel;
}

inline void transfer_range(NodeBase* pos, NodeBase* first, NodeBase* last) noexcept {
    if (first == last) {
        return; /* Empty range */
    }

    /* actual_last is the last node IN the range */
    NodeBase* actual_last = last->prev_;
    NodeBase* before_first = first->prev_;

    /* detach range from source */
    before_first->next_ = last;
    last->prev_ = before_first;

    /* insert range before pos */
    NodeBase* before_pos = pos->prev_;

    before_pos->next_ = first;
    first->prev_ = before_pos;

    actual_last->next_ = pos;
    pos->prev_ = actual_last;
}
