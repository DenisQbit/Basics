// vector experimental header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _EXPERIMENTAL_VECTOR_
#define _EXPERIMENTAL_VECTOR_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <vector>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
namespace experimental {
    inline namespace fundamentals_v2 {

        template <class _Ty, class _Alloc, class _Pr>
        _DEPRECATE_EXPERIMENTAL_ERASE void erase_if(vector<_Ty, _Alloc>& _Cont, _Pr _Pred) {
            _Erase_remove_if(_Cont, _Pass_fn(_Pred));
        }

        template <class _Ty, class _Alloc, class _Uty>
        _DEPRECATE_EXPERIMENTAL_ERASE void erase(vector<_Ty, _Alloc>& _Cont, const _Uty& _Val) {
            _Erase_remove(_Cont, _Val);
        }

    } // namespace fundamentals_v2
} // namespace experimental
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _EXPERIMENTAL_VECTOR_
