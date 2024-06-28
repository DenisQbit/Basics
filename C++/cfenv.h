// cfenv standard header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _CFENV_
#define _CFENV_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <fenv.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
using _CSTD fegetround;
using _CSTD fesetround;

#if !defined(_M_CEE) || defined(_CRTBLD)
using _CSTD feclearexcept;
using _CSTD fegetenv;
using _CSTD fegetexceptflag;
using _CSTD feholdexcept;
using _CSTD fenv_t;
using _CSTD fesetenv;
using _CSTD fesetexceptflag;
using _CSTD fetestexcept;
using _CSTD fexcept_t;
#endif // !defined(_M_CEE) || defined(_CRTBLD)

#if !defined(_M_CEE) && !defined(_CRTBLD)
using _CSTD feraiseexcept;
using _CSTD feupdateenv;
#endif // !defined(_M_CEE) && !defined(_CRTBLD)
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CFENV_
