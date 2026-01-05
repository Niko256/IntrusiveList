#pragma once

#include "base_node.hpp"
#include "iterator.hpp"
#include "list.hpp"
#include "node.hpp"
#include "policies.hpp"


/* ------------------------------------------------------------------- */

template <typename Tag>
using NoLinkHook = IntrusiveListNode<Tag, NoTrackPolicy, NoOpHandler>;

template <typename Tag>
using AutoUnlinkHook = IntrusiveListNode<Tag, TrackingPolicy, AutoUnlinkHandler>;

/* ------------------------------------------------------------------- */
