// ccomplex standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _CCOMPLEX_
#define _CCOMPLEX_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <complex>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_CXX17_DEPRECATE_C_HEADER typedef int _Header_ccomplex;
using _Hdr_ccomplex = _Header_ccomplex;

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CCOMPLEX_
