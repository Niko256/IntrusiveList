#pragma once

struct NodeBase {
    /*---*---*---*---*---*---*/

    NodeBase* prev_{nullptr};
    NodeBase* next_{nullptr};

    /*---*---*---*---*---*---*/

    NodeBase() noexcept = default;

    NodeBase(NodeBase&& other) noexcept;
    NodeBase& operator=(NodeBase&& other) noexcept;

    NodeBase(const NodeBase&) = delete;
    NodeBase& operator=(const NodeBase&) = delete;

    [[nodiscard]]
    auto next_node() noexcept -> NodeBase*;

    [[nodiscard]]
    auto next_node() const noexcept -> const NodeBase*;

    [[nodiscard]]
    auto prev_node() noexcept -> NodeBase*;

    [[nodiscard]]
    auto prev_node() const noexcept -> const NodeBase*;

    void set_next(NodeBase* n) noexcept;
    void set_prev(NodeBase* p) noexcept;

    /**
     * @brief Links this node between prev and next.
     *
     * Before: prev <-> next
     * After:  prev <-> this <-> next
     */
    void link_between_base(NodeBase* prev, NodeBase* next) noexcept;

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
    auto is_linked_base() const noexcept -> bool;

    /**
     * @brief Initializes a node as an empty list sentinel.
     *
     * Empty sentinel state: prev and next point to itself.
     *
     * */
    void init_sentinel(NodeBase* sentinel) noexcept;

    /**
     * @brief Checks if sentinel represents an empty list.
     */
    [[nodiscard]]
    auto is_empty_sentinel(const NodeBase* sentinel) noexcept -> bool;

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
    static void transfer_range(NodeBase* pos, NodeBase* first, NodeBase* last) noexcept;
};

/*---*---*---*---*---*---*---*---* IMPL *---*---*---*---*---*---*---*---*/

inline auto NodeBase::next_node() noexcept -> NodeBase* {
    ///
    return next_;
    ///
}

inline auto NodeBase::next_node() const noexcept -> const NodeBase* {
    ///
    return next_;
    ///
}

inline auto NodeBase::prev_node() noexcept -> NodeBase* {
    ///
    return prev_;
    ///
}

inline auto NodeBase::prev_node() const noexcept -> const NodeBase* {
    ///
    return prev_;
    ///
}

inline void NodeBase::set_next(NodeBase* n) noexcept {
    ///
    next_ = n;
    ///
}

inline void NodeBase::set_prev(NodeBase* p) noexcept {
    ///
    prev_ = p;
    ///
}

NodeBase::NodeBase(NodeBase&&) noexcept
    : prev_(nullptr),
      next_(nullptr) {}

NodeBase& NodeBase::operator=(NodeBase&&) noexcept {
    prev_ = nullptr;
    next_ = nullptr;

    return *this;
}

inline void NodeBase::link_between_base(NodeBase* prev, NodeBase* next) noexcept {
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

inline auto NodeBase::is_linked_base() const noexcept -> bool {
    ///
    return next_ != nullptr;
    ///
}

inline void NodeBase::init_sentinel(NodeBase* sentinel) noexcept {
    ///
    sentinel->prev_ = sentinel;
    sentinel->next_ = sentinel;
    ///
}

inline auto NodeBase::is_empty_sentinel(const NodeBase* sentinel) noexcept -> bool {
    ///
    return sentinel->next_ == sentinel;
    ///
}

inline void NodeBase::transfer_range(NodeBase* pos, NodeBase* first, NodeBase* last) noexcept {
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

/*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*/
