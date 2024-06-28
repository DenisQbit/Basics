// cuchar standard header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _CUCHAR_
#define _CUCHAR_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <uchar.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
using _CSTD mbstate_t;
using _CSTD size_t;
using _CSTD mbrtoc16;
using _CSTD c16rtomb;
using _CSTD mbrtoc32;
using _CSTD c32rtomb;
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CUCHAR_
