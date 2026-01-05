#pragma once

#include <concepts>

/**
 * ...[CONTRACT]...
 */
template <typename P>
concept LinkPolicy = requires(typename P::State s, const typename P::State cs) {
    { P::on_link(s) } noexcept;
    { P::on_unlink(s) } noexcept;
    { P::is_linked(cs) } noexcept -> std::same_as<bool>;
};

/* ------------------------------------------------------------------- */

/**
 * @section DANGER_AND_SAFETY
 * The biggest risk in intrusive containers is the 'dangling pointer'
 * If a Node is destroyed while still being linked in a list
 * => the list becomes corrupted => undefined behavior
 *
 * To avoid this problem, this class provides different safety policies.
 */

struct NoTrackPolicy {
    struct State {};

    static constexpr void on_link(State&) noexcept {}

    static constexpr void on_unlink(State&) noexcept {}

    static constexpr bool is_linked(const State&) noexcept { return false; }

    static constexpr const char* name() noexcept { return "NoLink"; }
};

struct TrackingPolicy {
    struct State {
        bool is_linked_ = false;
    };

    static constexpr void on_link(State& s) noexcept {
        s.is_linked_ = true;
    }

    static constexpr void on_unlink(State& s) noexcept {
        s.is_linked_ = false;
    }

    static constexpr bool is_linked(const State& s) noexcept { return s.is_linked_; }

    static constexpr const char* name() noexcept { return "Link"; }
};

/*
 * ...
 */

/* ------------------------------------------------------------------- */

struct NoOpHandler {
    template <typename Hook>
    static void on_destroy(Hook&) noexcept {}
};

struct AutoUnlinkHandler {
    template <typename Hook>
    static void on_destroy(Hook& h) noexcept {
        if (h.is_linked()) {
            h.unlink();
        }
    }
};

struct AssertNotLinkedHandler {
    template <typename Hook>
    static void on_destroy(Hook& h) noexcept {
        assert(!h.is_linked() && "Destroying object still in list!! Remove explicitly.");
    }
};
