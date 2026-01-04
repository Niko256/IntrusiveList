#pragma once

#include "algorithms.hpp"
#include "iterator.hpp"
#include "list.hpp"
#include "node.hpp"


template <LinkMode Mode = LinkMode::Link>
using DefaultListHook = IntrusiveListNode<void, Mode>;


template <typename Tag = void>
using AutoUnlinkListHook = IntrusiveListNode<Tag, LinkMode::AutoUnlink>;


template <typename Tag = void>
using UnsafeListHook = IntrusiveListNode<Tag, LinkMode::Raw>;
