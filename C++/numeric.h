// numeric standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _NUMERIC_
#define _NUMERIC_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <xutility>

#if _HAS_CXX17
#include <limits>
#endif // _HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
_STD_BEGIN
template <class _InIt, class _Ty, class _Fn>
_NODISCARD _CONSTEXPR20 _Ty accumulate(const _InIt _First, const _InIt _Last, _Ty _Val, _Fn _Reduce_op) {
    // return noncommutative and nonassociative reduction of _Val and all in [_First, _Last), using _Reduce_op
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
#if _HAS_CXX20
        _Val = _Reduce_op(_STD move(_Val), *_UFirst);
#else // ^^^ _HAS_CXX20 ^^^ // vvv !_HAS_CXX20 vvv
        _Val = _Reduce_op(_Val, *_UFirst);
#endif // _HAS_CXX20
    }
    return _Val;
}

template <class _InIt, class _Ty>
_NODISCARD _CONSTEXPR20 _Ty accumulate(const _InIt _First, const _InIt _Last, _Ty _Val) {
    // return noncommutative and nonassociative reduction of _Val and all in [_First, _Last)
    return _STD accumulate(_First, _Last, _Val, plus<>{});
}

#if _HAS_CXX17
#if _STD_VECTORIZE_WITH_FLOAT_CONTROL
template <class _InIt, class _Ty, class _BinOp>
inline constexpr bool _Plus_on_arithmetic_ranges_reduction_v =
    conjunction_v<is_arithmetic<_Ty>, is_arithmetic<remove_pointer_t<_InIt>>, is_same<plus<>, _BinOp>>;

#pragma float_control(precise, off, push)
template <class _InIt, class _Ty>
_Ty _Reduce_plus_arithmetic_ranges(_InIt _First, const _InIt _Last, _Ty _Val) {
    // return reduction, plus arithmetic on contiguous ranges case
#pragma loop(ivdep)
    for (; _First != _Last; ++_First) {
        _Val += *_First;
    }

    return _Val;
}
#pragma float_control(pop)

#else // ^^^ _STD_VECTORIZE_WITH_FLOAT_CONTROL ^^^ // vvv !_STD_VECTORIZE_WITH_FLOAT_CONTROL vvv
template <class _InIt, class _Ty, class _BinOp>
inline constexpr bool _Plus_on_arithmetic_ranges_reduction_v = false;
#endif // _STD_VECTORIZE_WITH_FLOAT_CONTROL

template <class _InIt, class _Ty, class _BinOp>
_NODISCARD _CONSTEXPR20 _Ty reduce(const _InIt _First, const _InIt _Last, _Ty _Val, _BinOp _Reduce_op) {
    // return commutative and associative reduction of _Val and [_First, _Last), using _Reduce_op
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (_Plus_on_arithmetic_ranges_reduction_v<_Unwrapped_t<const _InIt&>, _Ty, _BinOp>) {
#ifdef __cpp_lib_is_constant_evaluated
        if (!_STD is_constant_evaluated())
#endif // __cpp_lib_is_constant_evaluated
        {
            return _Reduce_plus_arithmetic_ranges(_UFirst, _ULast, _Val);
        }
    }

    for (; _UFirst != _ULast; ++_UFirst) {
        _Val = _Reduce_op(_STD move(_Val), *_UFirst); // Requirement missing from N4713
    }

    return _Val;
}

template <class _InIt, class _Ty>
_NODISCARD _CONSTEXPR20 _Ty reduce(const _InIt _First, const _InIt _Last, _Ty _Val) {
    // return commutative and associative reduction of _Val and [_First, _Last)
    return _STD reduce(_First, _Last, _STD move(_Val), plus{});
}

template <class _InIt>
_NODISCARD _CONSTEXPR20 _Iter_value_t<_InIt> reduce(const _InIt _First, const _InIt _Last) {
    // return commutative and associative reduction of
    // iterator_traits<_InIt>::value_type{} and [_First, _Last)
    return _STD reduce(_First, _Last, _Iter_value_t<_InIt>{}, plus{});
}

template <class _ExPo, class _FwdIt, class _Ty, class _BinOp, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Ty reduce(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Ty _Val, _BinOp _Reduce_op) noexcept; // terminates

template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Ty reduce(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, _Ty _Val) noexcept /* terminates */ {
    // return commutative and associative reduction of _Val and [_First, _Last)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    return _STD reduce(_STD forward<_ExPo>(_Exec), _First, _Last, _STD move(_Val), plus{});
}

template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Iter_value_t<_FwdIt> reduce(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last) noexcept
/* terminates */ {
    // return commutative and associative reduction of
    // iterator_traits<_FwdIt>::value_type{} and [_First, _Last)
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    return _STD reduce(_STD forward<_ExPo>(_Exec), _First, _Last, _Iter_value_t<_FwdIt>{}, plus{});
}
#endif // _HAS_CXX17

template <class _InIt1, class _InIt2, class _Ty, class _BinOp1, class _BinOp2>
_NODISCARD _CONSTEXPR20 _Ty inner_product(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Ty _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) {
    // return noncommutative and nonassociative transform-reduction of sequences, using
    // _Reduce_op and _Transform_op
    _Adl_verify_range(_First1, _Last1);
    auto _UFirst1      = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2      = _Get_unwrapped_n(_First2, _Idl_distance<_InIt1>(_UFirst1, _ULast1));
    for (; _UFirst1 != _ULast1; ++_UFirst1, (void) ++_UFirst2) {
#if _HAS_CXX20
        _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_UFirst1, *_UFirst2)); // Requirement missing from N4713
#else // ^^^ _HAS_CXX20 ^^^ // vvv !_HAS_CXX20 vvv
        _Val = _Reduce_op(_Val, _Transform_op(*_UFirst1, *_UFirst2)); // Requirement missing from N4713
#endif // _HAS_CXX20
    }

    return _Val;
}

template <class _InIt1, class _InIt2, class _Ty>
_NODISCARD _CONSTEXPR20 _Ty inner_product(const _InIt1 _First1, const _InIt1 _Last1, const _InIt2 _First2, _Ty _Val) {
    // return noncommutative and nonassociative transform-reduction of sequences
    return _STD inner_product(_First1, _Last1, _First2, _STD move(_Val), plus<>{}, multiplies<>{});
}

#if _HAS_CXX17
#if _STD_VECTORIZE_WITH_FLOAT_CONTROL
template <class _InIt1, class _InIt2, class _Ty, class _BinOp1, class _BinOp2>
inline constexpr bool _Default_ops_transform_reduce_v =
    conjunction_v<is_arithmetic<_Ty>, is_arithmetic<remove_pointer_t<_InIt1>>, is_arithmetic<remove_pointer_t<_InIt2>>,
        is_same<plus<>, _BinOp1>, is_same<multiplies<>, _BinOp2>>;

#pragma float_control(precise, off, push)
template <class _InIt1, class _InIt2, class _Ty>
_Ty _Transform_reduce_arithmetic_defaults(_InIt1 _First1, const _InIt1 _Last1, _InIt2 _First2, _Ty _Val) {
    // return transform-reduction, default ops on contiguous arithmetic ranges case
#pragma loop(ivdep)
    for (; _First1 != _Last1; ++_First1, (void) ++_First2) {
        _Val += *_First1 * *_First2;
    }

    return _Val;
}
#pragma float_control(pop)
#else // ^^^ _STD_VECTORIZE_WITH_FLOAT_CONTROL ^^^ // vvv !_STD_VECTORIZE_WITH_FLOAT_CONTROL vvv
template <class _InIt1, class _InIt2, class _Ty, class _BinOp1, class _BinOp2>
inline constexpr bool _Default_ops_transform_reduce_v = false;
#endif // _STD_VECTORIZE_WITH_FLOAT_CONTROL

template <class _InIt1, class _InIt2, class _Ty, class _BinOp1, class _BinOp2>
_NODISCARD _CONSTEXPR20 _Ty transform_reduce(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Ty _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) {
    // return commutative and associative transform-reduction of sequences, using
    // _Reduce_op and _Transform_op
    _Adl_verify_range(_First1, _Last1);
    auto _UFirst1      = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2      = _Get_unwrapped_n(_First2, _Idl_distance<_InIt1>(_UFirst1, _ULast1));
    if constexpr (_Default_ops_transform_reduce_v<_Unwrapped_t<const _InIt1&>, _Unwrapped_t<const _InIt2&>, _Ty,
                      _BinOp1, _BinOp2>) {
#ifdef __cpp_lib_is_constant_evaluated
        // TRANSITION, DevCom-878972
        if (!_STD is_constant_evaluated())
#endif // __cpp_lib_is_constant_evaluated
        {
            return _Transform_reduce_arithmetic_defaults(_UFirst1, _ULast1, _UFirst2, _STD move(_Val));
        }
    }

    for (; _UFirst1 != _ULast1; ++_UFirst1, (void) ++_UFirst2) {
        _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_UFirst1, *_UFirst2)); // Requirement missing from N4713
    }
    return _Val;
}

template <class _InIt1, class _InIt2, class _Ty>
_NODISCARD _CONSTEXPR20 _Ty transform_reduce(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _Ty _Val) {
    // return commutative and associative transform-reduction of sequences
    return _STD transform_reduce(_First1, _Last1, _First2, _STD move(_Val), plus{}, multiplies{});
}

template <class _InIt, class _Ty, class _BinOp, class _UnaryOp>
_NODISCARD _CONSTEXPR20 _Ty transform_reduce(
    const _InIt _First, const _InIt _Last, _Ty _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op) {
    // return commutative and associative reduction of transformed sequence, using
    // _Reduce_op and _Transform_op
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_UFirst)); // Requirement missing from N4713
    }

    return _Val;
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp1, class _BinOp2,
    _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Ty transform_reduce(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Ty _Val,
    _BinOp1 _Reduce_op, _BinOp2 _Transform_op) noexcept; // terminates

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Ty transform_reduce(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Ty _Val) noexcept
/* terminates */ {
    // return commutative and associative transform-reduction of sequences
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    return _STD transform_reduce(
        _STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _STD move(_Val), plus{}, multiplies{});
}

template <class _ExPo, class _FwdIt, class _Ty, class _BinOp, class _UnaryOp, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Ty transform_reduce(_ExPo&& _Exec, const _FwdIt _First1, const _FwdIt _Last1, _Ty _Val, _BinOp _Reduce_op,
    _UnaryOp _Transform_op) noexcept; // terminates
#endif // _HAS_CXX17

template <class _InIt, class _OutIt, class _BinOp>
_CONSTEXPR20 _OutIt partial_sum(const _InIt _First, const _InIt _Last, _OutIt _Dest, _BinOp _Reduce_op) {
    // compute partial noncommutative and nonassociative reductions into _Dest, using _Reduce_op
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));

    if (_UFirst != _ULast) {
        _Iter_value_t<_InIt> _Val(*_UFirst);
        for (;;) {
            *_UDest = _Val;
            ++_UDest;
            ++_UFirst;
            if (_UFirst == _ULast) {
                break;
            }
#if _HAS_CXX20
            _Val = _Reduce_op(_STD move(_Val), *_UFirst);
#else // ^^^ _HAS_CXX20 ^^^ // vvv !_HAS_CXX20 vvv
            _Val = _Reduce_op(_Val, *_UFirst);
#endif // _HAS_CXX20
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _InIt, class _OutIt>
_CONSTEXPR20 _OutIt partial_sum(_InIt _First, _InIt _Last, _OutIt _Dest) {
    // compute partial noncommutative and nonassociative reductions into _Dest
    return _STD partial_sum(_First, _Last, _Dest, plus<>{});
}

#if _HAS_CXX17
template <class _InIt, class _OutIt, class _Ty, class _BinOp>
_CONSTEXPR20 _OutIt exclusive_scan(const _InIt _First, const _InIt _Last, _OutIt _Dest, _Ty _Val, _BinOp _Reduce_op) {
    // set each value in [_Dest, _Dest + (_Last - _First)) to the associative reduction of predecessors and _Val
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    if (_UFirst != _ULast) {
        for (;;) {
            _Ty _Tmp(_Reduce_op(_Val, *_UFirst)); // temp to enable _First == _Dest, also requirement missing
            *_UDest = _Val;
            ++_UDest;
            ++_UFirst;
            if (_UFirst == _ULast) {
                break;
            }

            _Val = _STD move(_Tmp); // Requirement missing from N4713
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _InIt, class _OutIt, class _Ty>
_CONSTEXPR20 _OutIt exclusive_scan(const _InIt _First, const _InIt _Last, const _OutIt _Dest, _Ty _Val) {
    // set each value in [_Dest, _Dest + (_Last - _First)) to the associative reduction of predecessors and _Val
    return _STD exclusive_scan(_First, _Last, _Dest, _STD move(_Val), plus{});
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 exclusive_scan(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _Ty _Val,
    _BinOp _Reduce_op) noexcept; // terminates

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 exclusive_scan(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, const _FwdIt2 _Dest, _Ty _Val) noexcept
/* terminates */ {
    // set each value in [_Dest, _Dest + (_Last - _First)) to the associative reduction of predecessors and _Val
    return _STD exclusive_scan(_STD forward<_ExPo>(_Exec), _First, _Last, _Dest, _STD move(_Val), plus{});
}

template <class _InIt, class _OutIt, class _Ty, class _BinOp>
_CONSTEXPR20 _OutIt inclusive_scan(const _InIt _First, const _InIt _Last, _OutIt _Dest, _BinOp _Reduce_op, _Ty _Val) {
    // compute partial noncommutative and associative reductions including _Val into _Dest, using _Reduce_op
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    for (; _UFirst != _ULast; ++_UFirst) {
        _Val    = _Reduce_op(_STD move(_Val), *_UFirst); // Requirement missing from N4713
        *_UDest = _Val;
        ++_UDest;
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _InIt, class _OutIt, class _BinOp>
_CONSTEXPR20 _OutIt inclusive_scan(const _InIt _First, const _InIt _Last, _OutIt _Dest, _BinOp _Reduce_op) {
    // compute partial noncommutative and associative reductions into _Dest, using _Reduce_op
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    if (_UFirst != _ULast) {
        _Iter_value_t<_InIt> _Val = *_UFirst; // Requirement missing from N4713
        for (;;) {
            *_UDest = _Val;
            ++_UDest;
            ++_UFirst;
            if (_UFirst == _ULast) {
                break;
            }

            _Val = _Reduce_op(_STD move(_Val), *_UFirst); // Requirement missing from N4713
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _InIt, class _OutIt>
_CONSTEXPR20 _OutIt inclusive_scan(const _InIt _First, const _InIt _Last, const _OutIt _Dest) {
    // compute partial noncommutative and associative reductions into _Dest
    return _STD inclusive_scan(_First, _Last, _Dest, plus{});
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 inclusive_scan(
    _ExPo&& _Exec, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op, _Ty _Val) noexcept; // terminates

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 inclusive_scan(
    _ExPo&& _Exec, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Reduce_op) noexcept; // terminates

template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 inclusive_scan(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, const _FwdIt2 _Dest) noexcept
/* terminates */ {
    // compute partial noncommutative and associative reductions into _Dest
    return _STD inclusive_scan(_STD forward<_ExPo>(_Exec), _First, _Last, _Dest, plus{});
}

template <class _InIt, class _OutIt, class _Ty, class _BinOp, class _UnaryOp>
_CONSTEXPR20 _OutIt transform_exclusive_scan(
    const _InIt _First, const _InIt _Last, _OutIt _Dest, _Ty _Val, _BinOp _Reduce_op, _UnaryOp _Transform_op) {
    // set each value in [_Dest, _Dest + (_Last - _First)) to the associative reduction of transformed predecessors
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    if (_UFirst != _ULast) {
        for (;;) {
            _Ty _Tmp(_Reduce_op(_Val, _Transform_op(*_UFirst))); // temp to enable _First == _Dest
            *_UDest = _Val;
            ++_UDest;
            ++_UFirst;
            if (_UFirst == _ULast) {
                break;
            }

            _Val = _STD move(_Tmp); // Requirement missing from N4713
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _ExPo, class _FwdIt1, class _OutIt, class _Ty, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> = 0>
_OutIt transform_exclusive_scan(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, _OutIt _Dest, _Ty _Val,
    _BinOp _Reduce_op, _UnaryOp _Transform_op) noexcept; // terminates

template <class _InIt, class _OutIt, class _Ty, class _BinOp, class _UnaryOp>
_CONSTEXPR20 _OutIt transform_inclusive_scan(
    const _InIt _First, const _InIt _Last, _OutIt _Dest, _BinOp _Reduce_op, _UnaryOp _Transform_op, _Ty _Val) {
    // compute partial noncommutative and associative transformed reductions including _Val into _Dest
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    for (; _UFirst != _ULast; ++_UFirst) {
        _Val    = _Reduce_op(_STD move(_Val), _Transform_op(*_UFirst)); // Requirement missing from N4713
        *_UDest = _Val;
        ++_UDest;
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _InIt, class _OutIt, class _BinOp, class _UnaryOp>
_CONSTEXPR20 _OutIt transform_inclusive_scan(
    const _InIt _First, const _InIt _Last, _OutIt _Dest, _BinOp _Reduce_op, _UnaryOp _Transform_op) {
    // compute partial noncommutative and associative transformed reductions into _Dest
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    if (_UFirst != _ULast) {
        auto _Val = _Transform_op(*_UFirst); // Requirement missing from N4713, also type to use unclear
        for (;;) {
            *_UDest = _Val;
            ++_UDest;
            ++_UFirst;
            if (_UFirst == _ULast) {
                break;
            }

            _Val = _Reduce_op(_STD move(_Val), _Transform_op(*_UFirst)); // Requirement missing from N4713
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 transform_inclusive_scan(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest,
    _BinOp _Reduce_op, _UnaryOp _Transform_op, _Ty _Val) noexcept; // terminates

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, class _UnaryOp,
    _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 transform_inclusive_scan(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest,
    _BinOp _Reduce_op, _UnaryOp _Transform_op) noexcept; // terminates
#endif // _HAS_CXX17

template <class _InIt, class _OutIt, class _BinOp>
_CONSTEXPR20 _OutIt adjacent_difference(const _InIt _First, const _InIt _Last, _OutIt _Dest, _BinOp _Func) {
    // compute adjacent differences into _Dest
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest       = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    if (_UFirst != _ULast) {
        _Iter_value_t<_InIt> _Val = *_UFirst;
        *_UDest                   = _Val;
        while (++_UFirst != _ULast) { // compute another difference
            _Iter_value_t<_InIt> _Tmp = *_UFirst;
#if _HAS_CXX20
            *++_UDest = _Func(_Tmp, _STD move(_Val));
#else // ^^^ _HAS_CXX20 ^^^ // vvv !_HAS_CXX20 vvv
            *++_UDest = _Func(_Tmp, _Val);
#endif // _HAS_CXX20
            _Val = _STD move(_Tmp);
        }

        ++_UDest;
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

template <class _InIt, class _OutIt>
_CONSTEXPR20 _OutIt adjacent_difference(const _InIt _First, const _InIt _Last, const _OutIt _Dest) {
    // compute adjacent differences into _Dest
    return _STD adjacent_difference(_First, _Last, _Dest, minus<>{});
}

#if _HAS_CXX17
template <class _ExPo, class _FwdIt1, class _FwdIt2, class _BinOp, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 adjacent_difference(
    _ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _BinOp _Diff_op) noexcept; // terminates

template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 adjacent_difference(_ExPo&& _Exec, const _FwdIt1 _First, const _FwdIt1 _Last, const _FwdIt2 _Dest) noexcept
/* terminates */ {
    // compute adjacent differences into _Dest
    return _STD adjacent_difference(_STD forward<_ExPo>(_Exec), _First, _Last, _Dest, minus{});
}
#endif // _HAS_CXX17

template <class _FwdIt, class _Ty>
_CONSTEXPR20 void iota(_FwdIt _First, _FwdIt _Last, _Ty _Val) {
    // compute increasing sequence into [_First, _Last)
    _Adl_verify_range(_First, _Last);
    auto _UFirst      = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst, (void) ++_Val) {
        *_UFirst = _Val;
    }
}

#if _HAS_CXX17
template <class _Integral>
_NODISCARD constexpr auto _Abs_u(const _Integral _Val) noexcept {
    // computes absolute value of _Val (converting to an unsigned integer type if necessary to avoid overflow
    // representing the negation of the minimum value)
    static_assert(is_integral_v<_Integral>);

    if constexpr (is_signed_v<_Integral>) {
        using _Unsigned = make_unsigned_t<_Integral>;
        if (_Val < 0) {
            // note static_cast to _Unsigned such that _Integral == short returns unsigned short rather than int
            return static_cast<_Unsigned>(_Unsigned{0} - static_cast<_Unsigned>(_Val));
        }

        return static_cast<_Unsigned>(_Val);
    } else {
        return _Val;
    }
}

template <class _Mt, class _Nt>
_NODISCARD constexpr common_type_t<_Mt, _Nt> gcd(const _Mt _Mx, const _Nt _Nx) noexcept /* strengthened */ {
    // calculate greatest common divisor
    static_assert(_Is_nonbool_integral<_Mt> && _Is_nonbool_integral<_Nt>, "GCD requires nonbool integral types");

    using _Common                  = common_type_t<_Mt, _Nt>;
    using _Common_unsigned         = make_unsigned_t<_Common>;
    _Common_unsigned _Mx_magnitude = _Abs_u(_Mx);
    _Common_unsigned _Nx_magnitude = _Abs_u(_Nx);
    if (_Mx_magnitude == 0U) {
        return static_cast<_Common>(_Nx_magnitude);
    }

    if (_Nx_magnitude == 0U) {
        return static_cast<_Common>(_Mx_magnitude);
    }

    const auto _Mx_trailing_zeroes = static_cast<unsigned long>(_Countr_zero(_Mx_magnitude));
    const auto _Common_factors_of_2 =
        (_STD min)(_Mx_trailing_zeroes, static_cast<unsigned long>(_Countr_zero(_Nx_magnitude)));
    _Nx_magnitude >>= _Common_factors_of_2;
    _Mx_magnitude >>= _Mx_trailing_zeroes;
    do {
        _Nx_magnitude >>= static_cast<unsigned long>(_Countr_zero(_Nx_magnitude));
        if (_Mx_magnitude > _Nx_magnitude) {
            _Common_unsigned _Temp = _Mx_magnitude;
            _Mx_magnitude          = _Nx_magnitude;
            _Nx_magnitude          = _Temp;
        }

        _Nx_magnitude -= _Mx_magnitude;
    } while (_Nx_magnitude != 0U);

    return static_cast<_Common>(_Mx_magnitude << _Common_factors_of_2);
}

template <class _Mt, class _Nt>
_NODISCARD constexpr common_type_t<_Mt, _Nt> lcm(const _Mt _Mx, const _Nt _Nx) noexcept /* strengthened */ {
    // calculate least common multiple
    static_assert(_Is_nonbool_integral<_Mt> && _Is_nonbool_integral<_Nt>, "LCM requires nonbool integral types");
    using _Common                        = common_type_t<_Mt, _Nt>;
    using _Common_unsigned               = make_unsigned_t<_Common>;
    const _Common_unsigned _Mx_magnitude = _Abs_u(_Mx);
    const _Common_unsigned _Nx_magnitude = _Abs_u(_Nx);
    if (_Mx_magnitude == 0 || _Nx_magnitude == 0) {
        return 0;
    }

    return static_cast<_Common>((_Mx_magnitude / _STD gcd(_Mx_magnitude, _Nx_magnitude)) * _Nx_magnitude);
}
#endif // _HAS_CXX17

#if _HAS_CXX20
template <class _Ty, enable_if_t<is_arithmetic_v<_Ty> && !is_same_v<remove_cv_t<_Ty>, bool>, int> = 0>
_NODISCARD constexpr _Ty midpoint(const _Ty _Val1, const _Ty _Val2) noexcept {
    if constexpr (is_floating_point_v<_Ty>) {
        if (_STD is_constant_evaluated()) {
            if (_Is_nan(_Val1)) {
                return _Val1;
            }

            if (_Is_nan(_Val2)) {
                return _Val2;
            }
        } else {
            if (_Is_nan(_Val1) || _Is_nan(_Val2)) {
                // raise FE_INVALID if at least one of _Val1 and _Val2 is signaling NaN
                return _Val1 + _Val2;
            }
        }

        constexpr _Ty _High_limit = (numeric_limits<_Ty>::max)() / 2;
        const auto _Val1_a        = _Float_abs(_Val1);
        const auto _Val2_a        = _Float_abs(_Val2);
        if (_Val1_a <= _High_limit && _Val2_a <= _High_limit) {
            // _Val1 and _Val2 are small enough that _Val1 + _Val2 won't overflow

            // For the division to be inexact, the result of the addition must produce a value with the smallest
            // effective exponent and the low order bit in the mantissa set. For an addition to be inexact in this
            // condition, the difference between the inputs would have to be smaller than one ULP, but that is
            // impossible.
            //
            // For example, with doubles, the sum/difference of the inputs would have to be finer than 2^-1074, for it
            // to round (via whatever mode) to the value with a least significant 1 bit and p-1022, but the inputs can't
            // be finer than 2^-1074 and addition/subtraction can't create smaller steps.

            return (_Val1 + _Val2) / 2;
        }

        // Here at least one of {_Val1, _Val2} has large magnitude.
        // Therefore, if one of the values is too small to divide by 2 exactly, the small magnitude is much less than
        // one ULP of the result, so we can add it directly without the potentially inexact division by 2.

        // In the default rounding mode this less than one ULP difference will always be rounded away, so under
        // /fp:fast we could avoid these tests if we had some means of detecting it in the caller.
        constexpr _Ty _Low_limit = (numeric_limits<_Ty>::min)() * 2;
        if (_Val1_a < _Low_limit) {
            return _Val1 + _Val2 / 2;
        }

        if (_Val2_a < _Low_limit) {
            // division of _Val2 by 2 would be inexact, etc.
            return _Val1 / 2 + _Val2;
        }

        return _Val1 / 2 + _Val2 / 2;
    } else {
        using _Unsigned    = make_unsigned_t<_Ty>;
        const auto _Val1_u = static_cast<_Unsigned>(_Val1);
        const auto _Val2_u = static_cast<_Unsigned>(_Val2);
        if (_Val1 > _Val2) {
            return static_cast<_Ty>(_Val1 - static_cast<_Ty>(static_cast<_Unsigned>(_Val1_u - _Val2_u) / 2));
        } else {
            return static_cast<_Ty>(_Val1 + static_cast<_Ty>(static_cast<_Unsigned>(_Val2_u - _Val1_u) / 2));
        }
    }
}

template <class _Ty, enable_if_t<is_object_v<_Ty>, int> = 0>
_NODISCARD constexpr _Ty* midpoint(_Ty* const _Val1, _Ty* const _Val2) noexcept /* strengthened */ {
    if (_Val1 > _Val2) {
        return _Val1 - ((_Val1 - _Val2) >> 1); // shift for codegen
    } else {
        return _Val1 + ((_Val2 - _Val1) >> 1); // shift for codegen
    }
}
#endif // _HAS_CXX20
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _NUMERIC_
