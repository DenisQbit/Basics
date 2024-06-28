// xfacet internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XFACET_
#define _XFACET_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

_STD_BEGIN
class _CRTIMP2_PURE_IMPORT _Facet_base { // code for reference counting a facet
public:
    virtual __CLR_OR_THIS_CALL ~_Facet_base() noexcept {} // ensure that derived classes can be destroyed properly

    // increment use count
    virtual void __CLR_OR_THIS_CALL _Incref() noexcept = 0;

    // decrement use count
    virtual _Facet_base* __CLR_OR_THIS_CALL _Decref() noexcept = 0;
};

#if defined(_M_CEE)
void __CLRCALL_OR_CDECL _Facet_Register_m(_Facet_base*);

#else // defined(_M_CEE)
void __CLRCALL_OR_CDECL _Facet_Register(_Facet_base*);
#endif // defined(_M_CEE)
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XFACET_