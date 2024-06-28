// string_view standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _STRING_VIEW_
#define _STRING_VIEW_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX17
#pragma message("The contents of <string_view> are available only with C++17 or later.")
#else // ^^^ !_HAS_CXX17 / _HAS_CXX17 vvv
#include <xstring>
#endif // _HAS_CXX17
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _STRING_VIEW_
