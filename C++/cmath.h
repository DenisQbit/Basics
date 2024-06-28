// cmath standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _CMATH_
#define _CMATH_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <cstdlib>
#include <xtr1common>

#if !defined(_M_CEE) && !defined(__clang__) && !defined(__CUDACC__) && !defined(__INTEL_COMPILER)
#define _HAS_CMATH_INTRINSICS 1
#else // ^^^ intrinsics available ^^^ / vvv intrinsics unavailable vvv
#define _HAS_CMATH_INTRINSICS 0
#endif // ^^^ intrinsics unavailable ^^^

#if _HAS_CMATH_INTRINSICS
#include <intrin0.h>
#endif // _HAS_CMATH_INTRINSICS

#if _HAS_CXX20
#include <xutility>
#endif // _HAS_CXX20

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_NODISCARD _Check_return_ inline float acos(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD acosf(_Xx);
}

_NODISCARD _Check_return_ inline float acosh(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD acoshf(_Xx);
}

_NODISCARD _Check_return_ inline float asin(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD asinf(_Xx);
}

_NODISCARD _Check_return_ inline float asinh(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD asinhf(_Xx);
}

_NODISCARD _Check_return_ inline float atan(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD atanf(_Xx);
}

_NODISCARD _Check_return_ inline float atanh(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD atanhf(_Xx);
}

_NODISCARD _Check_return_ inline float atan2(_In_ float _Yx, _In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD atan2f(_Yx, _Xx);
}

_NODISCARD _Check_return_ inline float cbrt(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD cbrtf(_Xx);
}

_NODISCARD _Check_return_ inline float ceil(_In_ float _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __ceilf(_Xx);
#elif defined(__clang__)
    return __builtin_ceilf(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD ceilf(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline float copysign(_In_ float _Number, _In_ float _Sign) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __copysignf(_Number, _Sign);
#elif defined(__clang__)
    return __builtin_copysignf(_Number, _Sign);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD copysignf(_Number, _Sign);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline float cos(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD cosf(_Xx);
}

_NODISCARD _Check_return_ inline float cosh(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD coshf(_Xx);
}

_NODISCARD _Check_return_ inline float erf(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD erff(_Xx);
}

_NODISCARD _Check_return_ inline float erfc(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD erfcf(_Xx);
}

_NODISCARD _Check_return_ inline float exp(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD expf(_Xx);
}

_NODISCARD _Check_return_ inline float exp2(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD exp2f(_Xx);
}

_NODISCARD _Check_return_ inline float expm1(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD expm1f(_Xx);
}

_NODISCARD _Check_return_ inline float fabs(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD fabsf(_Xx);
}

_NODISCARD _Check_return_ inline float fdim(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD fdimf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float floor(_In_ float _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __floorf(_Xx);
#elif defined(__clang__)
    return __builtin_floorf(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD floorf(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline float fma(_In_ float _Xx, _In_ float _Yx, _In_ float _Zx) noexcept /* strengthened */ {
    return _CSTD fmaf(_Xx, _Yx, _Zx);
}

_NODISCARD _Check_return_ inline float fmax(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD fmaxf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float fmin(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD fminf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float fmod(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD fmodf(_Xx, _Yx);
}

inline float frexp(_In_ float _Xx, _Out_ int* _Yx) noexcept /* strengthened */ {
    return _CSTD frexpf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float hypot(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD hypotf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline int ilogb(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD ilogbf(_Xx);
}

_NODISCARD _Check_return_ inline float ldexp(_In_ float _Xx, _In_ int _Yx) noexcept /* strengthened */ {
    return _CSTD ldexpf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float lgamma(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD lgammaf(_Xx);
}

_NODISCARD _Check_return_ inline long long llrint(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD llrintf(_Xx);
}

_NODISCARD _Check_return_ inline long long llround(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD llroundf(_Xx);
}

_NODISCARD _Check_return_ inline float log(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD logf(_Xx);
}

_NODISCARD _Check_return_ inline float log10(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD log10f(_Xx);
}

_NODISCARD _Check_return_ inline float log1p(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD log1pf(_Xx);
}

_NODISCARD _Check_return_ inline float log2(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD log2f(_Xx);
}

_NODISCARD _Check_return_ inline float logb(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD logbf(_Xx);
}

_NODISCARD _Check_return_ inline long lrint(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD lrintf(_Xx);
}

_NODISCARD _Check_return_ inline long lround(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD lroundf(_Xx);
}

inline float modf(_In_ float _Xx, _Out_ float* _Yx) noexcept /* strengthened */ {
    return _CSTD modff(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float nearbyint(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD nearbyintf(_Xx);
}

_NODISCARD _Check_return_ inline float nextafter(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD nextafterf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float nexttoward(_In_ float _Xx, _In_ long double _Yx) noexcept /* strengthened */ {
    return _CSTD nexttowardf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float pow(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD powf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float remainder(_In_ float _Xx, _In_ float _Yx) noexcept /* strengthened */ {
    return _CSTD remainderf(_Xx, _Yx);
}

inline float remquo(_In_ float _Xx, _In_ float _Yx, _Out_ int* _Zx) noexcept /* strengthened */ {
    return _CSTD remquof(_Xx, _Yx, _Zx);
}

_NODISCARD _Check_return_ inline float rint(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD rintf(_Xx);
}

_NODISCARD _Check_return_ inline float round(_In_ float _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __roundf(_Xx);
#elif defined(__clang__)
    return __builtin_roundf(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD roundf(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline float scalbln(_In_ float _Xx, _In_ long _Yx) noexcept /* strengthened */ {
    return _CSTD scalblnf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float scalbn(_In_ float _Xx, _In_ int _Yx) noexcept /* strengthened */ {
    return _CSTD scalbnf(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline float sin(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD sinf(_Xx);
}

_NODISCARD _Check_return_ inline float sinh(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD sinhf(_Xx);
}

_NODISCARD _Check_return_ inline float sqrt(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD sqrtf(_Xx);
}

_NODISCARD _Check_return_ inline float tan(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD tanf(_Xx);
}

_NODISCARD _Check_return_ inline float tanh(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD tanhf(_Xx);
}

_NODISCARD _Check_return_ inline float tgamma(_In_ float _Xx) noexcept /* strengthened */ {
    return _CSTD tgammaf(_Xx);
}

_NODISCARD _Check_return_ inline float trunc(_In_ float _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __truncf(_Xx);
#elif defined(__clang__)
    return __builtin_truncf(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD truncf(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline long double acos(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD acosl(_Xx);
}

_NODISCARD _Check_return_ inline long double acosh(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD acoshl(_Xx);
}

_NODISCARD _Check_return_ inline long double asin(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD asinl(_Xx);
}

_NODISCARD _Check_return_ inline long double asinh(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD asinhl(_Xx);
}

_NODISCARD _Check_return_ inline long double atan(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD atanl(_Xx);
}

_NODISCARD _Check_return_ inline long double atanh(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD atanhl(_Xx);
}

_NODISCARD _Check_return_ inline long double atan2(_In_ long double _Yx, _In_ long double _Xx) noexcept
/* strengthened */ {
    return _CSTD atan2l(_Yx, _Xx);
}

_NODISCARD _Check_return_ inline long double cbrt(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD cbrtl(_Xx);
}

_NODISCARD _Check_return_ inline long double ceil(_In_ long double _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __ceil(static_cast<double>(_Xx));
#elif defined(__clang__)
    return __builtin_ceill(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD ceill(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline long double copysign(_In_ long double _Number, _In_ long double _Sign) noexcept
/* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __copysign(static_cast<double>(_Number), static_cast<double>(_Sign));
#elif defined(__clang__)
    return __builtin_copysignl(_Number, _Sign);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD copysignl(_Number, _Sign);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline long double cos(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD cosl(_Xx);
}

_NODISCARD _Check_return_ inline long double cosh(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD coshl(_Xx);
}

_NODISCARD _Check_return_ inline long double erf(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD erfl(_Xx);
}

_NODISCARD _Check_return_ inline long double erfc(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD erfcl(_Xx);
}

_NODISCARD _Check_return_ inline long double exp(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD expl(_Xx);
}

_NODISCARD _Check_return_ inline long double exp2(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD exp2l(_Xx);
}

_NODISCARD _Check_return_ inline long double expm1(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD expm1l(_Xx);
}

_NODISCARD _Check_return_ inline long double fabs(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD fabsl(_Xx);
}

_NODISCARD _Check_return_ inline long double fdim(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD fdiml(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double floor(_In_ long double _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __floor(static_cast<double>(_Xx));
#elif defined(__clang__)
    return __builtin_floorl(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD floorl(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline long double fma(
    _In_ long double _Xx, _In_ long double _Yx, _In_ long double _Zx) noexcept /* strengthened */ {
    return _CSTD fmal(_Xx, _Yx, _Zx);
}

_NODISCARD _Check_return_ inline long double fmax(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD fmaxl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double fmin(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD fminl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double fmod(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD fmodl(_Xx, _Yx);
}

inline long double frexp(_In_ long double _Xx, _Out_ int* _Yx) noexcept /* strengthened */ {
    return _CSTD frexpl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double hypot(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD hypotl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline int ilogb(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD ilogbl(_Xx);
}

_NODISCARD _Check_return_ inline long double ldexp(_In_ long double _Xx, _In_ int _Yx) noexcept /* strengthened */ {
    return _CSTD ldexpl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double lgamma(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD lgammal(_Xx);
}

_NODISCARD _Check_return_ inline long long llrint(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD llrintl(_Xx);
}

_NODISCARD _Check_return_ inline long long llround(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD llroundl(_Xx);
}

_NODISCARD _Check_return_ inline long double log(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD logl(_Xx);
}

_NODISCARD _Check_return_ inline long double log10(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD log10l(_Xx);
}

_NODISCARD _Check_return_ inline long double log1p(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD log1pl(_Xx);
}

_NODISCARD _Check_return_ inline long double log2(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD log2l(_Xx);
}

_NODISCARD _Check_return_ inline long double logb(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD logbl(_Xx);
}

_NODISCARD _Check_return_ inline long lrint(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD lrintl(_Xx);
}

_NODISCARD _Check_return_ inline long lround(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD lroundl(_Xx);
}

inline long double modf(_In_ long double _Xx, _Out_ long double* _Yx) noexcept /* strengthened */ {
    return _CSTD modfl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double nearbyint(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD nearbyintl(_Xx);
}

_NODISCARD _Check_return_ inline long double nextafter(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD nextafterl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double nexttoward(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD nexttowardl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double pow(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD powl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double remainder(_In_ long double _Xx, _In_ long double _Yx) noexcept
/* strengthened */ {
    return _CSTD remainderl(_Xx, _Yx);
}

inline long double remquo(_In_ long double _Xx, _In_ long double _Yx, _Out_ int* _Zx) noexcept /* strengthened */ {
    return _CSTD remquol(_Xx, _Yx, _Zx);
}

_NODISCARD _Check_return_ inline long double rint(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD rintl(_Xx);
}

_NODISCARD _Check_return_ inline long double round(_In_ long double _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __round(static_cast<double>(_Xx));
#elif defined(__clang__)
    return __builtin_roundl(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD roundl(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}

_NODISCARD _Check_return_ inline long double scalbln(_In_ long double _Xx, _In_ long _Yx) noexcept /* strengthened */ {
    return _CSTD scalblnl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double scalbn(_In_ long double _Xx, _In_ int _Yx) noexcept /* strengthened */ {
    return _CSTD scalbnl(_Xx, _Yx);
}

_NODISCARD _Check_return_ inline long double sin(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD sinl(_Xx);
}

_NODISCARD _Check_return_ inline long double sinh(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD sinhl(_Xx);
}

_NODISCARD _Check_return_ inline long double sqrt(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD sqrtl(_Xx);
}

_NODISCARD _Check_return_ inline long double tan(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD tanl(_Xx);
}

_NODISCARD _Check_return_ inline long double tanh(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD tanhl(_Xx);
}

_NODISCARD _Check_return_ inline long double tgamma(_In_ long double _Xx) noexcept /* strengthened */ {
    return _CSTD tgammal(_Xx);
}

_NODISCARD _Check_return_ inline long double trunc(_In_ long double _Xx) noexcept /* strengthened */ {
#if _HAS_CMATH_INTRINSICS
    return __trunc(static_cast<double>(_Xx));
#elif defined(__clang__)
    return __builtin_truncl(_Xx);
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
    return _CSTD truncl(_Xx);
#endif // ^^^ intrinsics unavailable ^^^
}


_STD_BEGIN
template <class _Ty1, class _Ty2>
using _Common_float_type_t = conditional_t<is_same_v<_Ty1, long double> || is_same_v<_Ty2, long double>, long double,
    conditional_t<is_same_v<_Ty1, float> && is_same_v<_Ty2, float>, float,
        double>>; // find type for two-argument math function
_STD_END

template <class _Ty, _STD enable_if_t<_STD is_integral_v<_Ty>, int> = 0>
double frexp(_Ty _Value, _Out_ int* const _Exp) noexcept /* strengthened */ {
    return _CSTD frexp(static_cast<double>(_Value), _Exp);
}

template <class _Ty1, class _Ty2, class _Ty3,
    _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2> && _STD is_arithmetic_v<_Ty3>, int> = 0>
_NODISCARD _STD _Common_float_type_t<_Ty1, _STD _Common_float_type_t<_Ty2, _Ty3>> fma(
    _Ty1 _Left, _Ty2 _Middle, _Ty3 _Right) noexcept /* strengthened */ {
    using _Common = _STD _Common_float_type_t<_Ty1, _STD _Common_float_type_t<_Ty2, _Ty3>>;
    if constexpr (_STD is_same_v<_Common, float>) {
        return _CSTD fmaf(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
    } else if constexpr (_STD is_same_v<_Common, double>) {
        return _CSTD fma(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
    } else {
        return _CSTD fmal(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
    }
}

template <class _Ty1, class _Ty2, _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2>, int> = 0>
_STD _Common_float_type_t<_Ty1, _Ty2> remquo(_Ty1 _Left, _Ty2 _Right, int* _Pquo) noexcept /* strengthened */ {
    using _Common = _STD _Common_float_type_t<_Ty1, _Ty2>;
    if constexpr (_STD is_same_v<_Common, float>) {
        return _CSTD remquof(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
    } else if constexpr (_STD is_same_v<_Common, double>) {
        return _CSTD remquo(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
    } else {
        return _CSTD remquol(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
    }
}

#define _GENERIC_MATH1_BASE(NAME, RET, FUN)                                  \
    template <class _Ty, _STD enable_if_t<_STD is_integral_v<_Ty>, int> = 0> \
    _NODISCARD RET NAME(_Ty _Left) noexcept /* strengthened */ {             \
        return FUN(static_cast<double>(_Left));                              \
    }

#define _GENERIC_MATH1R(FUN, RET) _GENERIC_MATH1_BASE(FUN, RET, _CSTD FUN)
#define _GENERIC_MATH1(FUN)       _GENERIC_MATH1R(FUN, double)

#if _HAS_CMATH_INTRINSICS
#define _GENERIC_MATH1I(FUN, CLANG_INTRIN, MSVC_INTRIN) _GENERIC_MATH1_BASE(FUN, double, MSVC_INTRIN)
#elif defined(__clang__)
#define _GENERIC_MATH1I(FUN, CLANG_INTRIN, MSVC_INTRIN) _GENERIC_MATH1_BASE(FUN, double, CLANG_INTRIN)
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
#define _GENERIC_MATH1I(FUN, CLANG_INTRIN, MSVC_INTRIN) _GENERIC_MATH1_BASE(FUN, double, _CSTD FUN)
#endif // ^^^ intrinsics unavailable ^^^

#define _GENERIC_MATH1X(FUN, ARG2)                                             \
    template <class _Ty, _STD enable_if_t<_STD is_integral_v<_Ty>, int> = 0>   \
    _NODISCARD double FUN(_Ty _Left, ARG2 _Arg2) noexcept /* strengthened */ { \
        return _CSTD FUN(static_cast<double>(_Left), _Arg2);                   \
    }

#define _GENERIC_MATH2_BASE(NAME, FUN)                                                                           \
    template <class _Ty1, class _Ty2,                                                                            \
        _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2>, int> = 0>                     \
    _NODISCARD _STD _Common_float_type_t<_Ty1, _Ty2> NAME(_Ty1 _Left, _Ty2 _Right) noexcept /* strengthened */ { \
        using _Common = _STD _Common_float_type_t<_Ty1, _Ty2>;                                                   \
        return FUN(static_cast<_Common>(_Left), static_cast<_Common>(_Right));                                   \
    }

#define _GENERIC_MATH2(FUN) _GENERIC_MATH2_BASE(FUN, _CSTD FUN)

#if _HAS_CMATH_INTRINSICS
#define _GENERIC_MATH2I(FUN, CLANG_INTRIN, MSVC_INTRIN) _GENERIC_MATH2_BASE(FUN, MSVC_INTRIN)
#elif defined(__clang__)
#define _GENERIC_MATH2I(FUN, CLANG_INTRIN, MSVC_INTRIN) _GENERIC_MATH2_BASE(FUN, CLANG_INTRIN)
#else // ^^^ defined(__clang__) ^^^ / vvv intrinsics unavailable vvv
#define _GENERIC_MATH2I(FUN, CLANG_INTRIN, MSVC_INTRIN) _GENERIC_MATH2_BASE(FUN, _CSTD FUN)
#endif // ^^^ intrinsics unavailable ^^^

// The following order matches N4820 26.8.1 [cmath.syn].
_GENERIC_MATH1(acos)
_GENERIC_MATH1(asin)
_GENERIC_MATH1(atan)
_GENERIC_MATH2(atan2)
_GENERIC_MATH1(cos)
_GENERIC_MATH1(sin)
_GENERIC_MATH1(tan)
_GENERIC_MATH1(acosh)
_GENERIC_MATH1(asinh)
_GENERIC_MATH1(atanh)
_GENERIC_MATH1(cosh)
_GENERIC_MATH1(sinh)
_GENERIC_MATH1(tanh)
_GENERIC_MATH1(exp)
_GENERIC_MATH1(exp2)
_GENERIC_MATH1(expm1)
// frexp() is hand-crafted
_GENERIC_MATH1R(ilogb, int)
_GENERIC_MATH1X(ldexp, int)
_GENERIC_MATH1(log)
_GENERIC_MATH1(log10)
_GENERIC_MATH1(log1p)
_GENERIC_MATH1(log2)
_GENERIC_MATH1(logb)
// No modf(), types must match
_GENERIC_MATH1X(scalbn, int)
_GENERIC_MATH1X(scalbln, long)
_GENERIC_MATH1(cbrt)
// abs() has integer overloads
_GENERIC_MATH1(fabs)
_GENERIC_MATH2(hypot)
// 3-arg hypot() is hand-crafted
_GENERIC_MATH2(pow)
_GENERIC_MATH1(sqrt)
_GENERIC_MATH1(erf)
_GENERIC_MATH1(erfc)
_GENERIC_MATH1(lgamma)
_GENERIC_MATH1(tgamma)
_GENERIC_MATH1I(ceil, __builtin_ceil, __ceil)
_GENERIC_MATH1I(floor, __builtin_floor, __floor)
_GENERIC_MATH1(nearbyint)
_GENERIC_MATH1(rint)
_GENERIC_MATH1R(lrint, long)
_GENERIC_MATH1R(llrint, long long)
_GENERIC_MATH1I(round, __builtin_round, __round)
_GENERIC_MATH1R(lround, long)
_GENERIC_MATH1R(llround, long long)
_GENERIC_MATH1I(trunc, __builtin_trunc, __trunc)
_GENERIC_MATH2(fmod)
_GENERIC_MATH2(remainder)
// remquo() is hand-crafted
_GENERIC_MATH2I(copysign, __builtin_copysign, __copysign)
// nan(const char*) is exempt
_GENERIC_MATH2(nextafter)
_GENERIC_MATH1X(nexttoward, long double)
_GENERIC_MATH2(fdim)
_GENERIC_MATH2(fmax)
_GENERIC_MATH2(fmin)
// fma() is hand-crafted
// lerp() should be exempt, LWG-3223
// The "classification/comparison functions" (fpclassify(), etc.) are exempt, LWG-1327
// TRANSITION, VSO-945789, Special Math shouldn't be exempt

#undef _GENERIC_MATH1_BASE
#undef _GENERIC_MATH1R
#undef _GENERIC_MATH1
#undef _GENERIC_MATH1I
#undef _GENERIC_MATH1X
#undef _GENERIC_MATH2_BASE
#undef _GENERIC_MATH2
#undef _GENERIC_MATH2I
#undef _HAS_CMATH_INTRINSICS


_STD_BEGIN
using _CSTD abs;
using _CSTD acos;
using _CSTD asin;
using _CSTD atan;
using _CSTD atan2;
using _CSTD ceil;
using _CSTD cos;
using _CSTD cosh;
using _CSTD exp;
using _CSTD fabs;
using _CSTD floor;
using _CSTD fmod;
using _CSTD frexp;
using _CSTD ldexp;
using _CSTD log;
using _CSTD log10;
using _CSTD modf;
using _CSTD pow;
using _CSTD sin;
using _CSTD sinh;
using _CSTD sqrt;
using _CSTD tan;
using _CSTD tanh;

using _CSTD acosf;
using _CSTD asinf;
using _CSTD atanf;
using _CSTD atan2f;
using _CSTD ceilf;
using _CSTD cosf;
using _CSTD coshf;
using _CSTD expf;
using _CSTD fabsf;
using _CSTD floorf;
using _CSTD fmodf;
using _CSTD frexpf;
using _CSTD ldexpf;
using _CSTD logf;
using _CSTD log10f;
using _CSTD modff;
using _CSTD powf;
using _CSTD sinf;
using _CSTD sinhf;
using _CSTD sqrtf;
using _CSTD tanf;
using _CSTD tanhf;

using _CSTD acosl;
using _CSTD asinl;
using _CSTD atanl;
using _CSTD atan2l;
using _CSTD ceill;
using _CSTD cosl;
using _CSTD coshl;
using _CSTD expl;
using _CSTD fabsl;
using _CSTD floorl;
using _CSTD fmodl;
using _CSTD frexpl;
using _CSTD ldexpl;
using _CSTD logl;
using _CSTD log10l;
using _CSTD modfl;
using _CSTD powl;
using _CSTD sinl;
using _CSTD sinhl;
using _CSTD sqrtl;
using _CSTD tanl;
using _CSTD tanhl;

using _CSTD float_t;
using _CSTD double_t;

using _CSTD acosh;
using _CSTD asinh;
using _CSTD atanh;
using _CSTD cbrt;
using _CSTD erf;
using _CSTD erfc;
using _CSTD expm1;
using _CSTD exp2;
using _CSTD hypot;
using _CSTD ilogb;
using _CSTD lgamma;
using _CSTD log1p;
using _CSTD log2;
using _CSTD logb;
using _CSTD llrint;
using _CSTD lrint;
using _CSTD nearbyint;
using _CSTD rint;
using _CSTD llround;
using _CSTD lround;
using _CSTD fdim;
using _CSTD fma;
using _CSTD fmax;
using _CSTD fmin;
using _CSTD round;
using _CSTD trunc;
using _CSTD remainder;
using _CSTD remquo;
using _CSTD copysign;
using _CSTD nan;
using _CSTD nextafter;
using _CSTD scalbn;
using _CSTD scalbln;
using _CSTD nexttoward;
using _CSTD tgamma;

using _CSTD acoshf;
using _CSTD asinhf;
using _CSTD atanhf;
using _CSTD cbrtf;
using _CSTD erff;
using _CSTD erfcf;
using _CSTD expm1f;
using _CSTD exp2f;
using _CSTD hypotf;
using _CSTD ilogbf;
using _CSTD lgammaf;
using _CSTD log1pf;
using _CSTD log2f;
using _CSTD logbf;
using _CSTD llrintf;
using _CSTD lrintf;
using _CSTD nearbyintf;
using _CSTD rintf;
using _CSTD llroundf;
using _CSTD lroundf;
using _CSTD fdimf;
using _CSTD fmaf;
using _CSTD fmaxf;
using _CSTD fminf;
using _CSTD roundf;
using _CSTD truncf;
using _CSTD remainderf;
using _CSTD remquof;
using _CSTD copysignf;
using _CSTD nanf;
using _CSTD nextafterf;
using _CSTD scalbnf;
using _CSTD scalblnf;
using _CSTD nexttowardf;
using _CSTD tgammaf;

using _CSTD acoshl;
using _CSTD asinhl;
using _CSTD atanhl;
using _CSTD cbrtl;
using _CSTD erfl;
using _CSTD erfcl;
using _CSTD expm1l;
using _CSTD exp2l;
using _CSTD hypotl;
using _CSTD ilogbl;
using _CSTD lgammal;
using _CSTD log1pl;
using _CSTD log2l;
using _CSTD logbl;
using _CSTD llrintl;
using _CSTD lrintl;
using _CSTD nearbyintl;
using _CSTD rintl;
using _CSTD llroundl;
using _CSTD lroundl;
using _CSTD fdiml;
using _CSTD fmal;
using _CSTD fmaxl;
using _CSTD fminl;
using _CSTD roundl;
using _CSTD truncl;
using _CSTD remainderl;
using _CSTD remquol;
using _CSTD copysignl;
using _CSTD nanl;
using _CSTD nextafterl;
using _CSTD scalbnl;
using _CSTD scalblnl;
using _CSTD nexttowardl;
using _CSTD tgammal;

using _CSTD fpclassify;
using _CSTD signbit;
using _CSTD isfinite;
using _CSTD isinf;
using _CSTD isnan;
using _CSTD isnormal;
using _CSTD isgreater;
using _CSTD isgreaterequal;
using _CSTD isless;
using _CSTD islessequal;
using _CSTD islessgreater;
using _CSTD isunordered;
_STD_END

#if _HAS_CXX17
_EXTERN_C
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_assoc_laguerre(unsigned int, unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_assoc_laguerref(unsigned int, unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_assoc_legendre(unsigned int, unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_assoc_legendref(unsigned int, unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_beta(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_betaf(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_1(double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_1f(float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_2(double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_2f(float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_3(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_3f(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_bessel_i(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_bessel_if(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_bessel_j(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_bessel_jf(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_bessel_k(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_bessel_kf(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_neumann(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_neumannf(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_ellint_1(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_ellint_1f(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_ellint_2(double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_ellint_2f(float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_ellint_3(double, double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_ellint_3f(float, float, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_expint(double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_expintf(float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_hermite(unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_hermitef(unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_laguerre(unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_laguerref(unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_legendre(unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_legendref(unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_riemann_zeta(double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_riemann_zetaf(float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_sph_bessel(unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_sph_besself(unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_sph_legendre(unsigned int, unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_sph_legendref(unsigned int, unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_sph_neumann(unsigned int, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_sph_neumannf(unsigned int, float) noexcept;
_NODISCARD _CRT_SATELLITE_2 double __stdcall __std_smf_hypot3(double, double, double) noexcept;
_NODISCARD _CRT_SATELLITE_2 float __stdcall __std_smf_hypot3f(float, float, float) noexcept;
_END_EXTERN_C

_STD_BEGIN
_NODISCARD inline double assoc_laguerre(const unsigned int _Degree, const unsigned int _Order, const double _Value) {
    return __std_smf_assoc_laguerre(_Degree, _Order, _Value);
}

_NODISCARD inline float assoc_laguerref(const unsigned int _Degree, const unsigned int _Order, const float _Value) {
    return __std_smf_assoc_laguerref(_Degree, _Order, _Value);
}

_NODISCARD inline long double assoc_laguerrel(
    const unsigned int _Degree, const unsigned int _Order, const long double _Value) {
    return __std_smf_assoc_laguerre(_Degree, _Order, static_cast<double>(_Value));
}

_NODISCARD inline double assoc_legendre(const unsigned int _Degree, const unsigned int _Order, const double _Value) {
    return __std_smf_assoc_legendre(_Degree, _Order, _Value);
}

_NODISCARD inline float assoc_legendref(const unsigned int _Degree, const unsigned int _Order, const float _Value) {
    return __std_smf_assoc_legendref(_Degree, _Order, _Value);
}

_NODISCARD inline long double assoc_legendrel(
    const unsigned int _Degree, const unsigned int _Order, const long double _Value) {
    return __std_smf_assoc_legendre(_Degree, _Order, static_cast<double>(_Value));
}

_NODISCARD inline double beta(const double _Arg1, const double _Arg2) {
    return __std_smf_beta(_Arg1, _Arg2);
}

_NODISCARD inline float betaf(const float _Arg1, const float _Arg2) {
    return __std_smf_betaf(_Arg1, _Arg2);
}

_NODISCARD inline long double betal(const long double _Arg1, const long double _Arg2) {
    return __std_smf_beta(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double comp_ellint_1(const double _Arg) {
    return __std_smf_comp_ellint_1(_Arg);
}

_NODISCARD inline float comp_ellint_1f(const float _Arg) {
    return __std_smf_comp_ellint_1f(_Arg);
}

_NODISCARD inline long double comp_ellint_1l(const long double _Arg) {
    return __std_smf_comp_ellint_1(static_cast<double>(_Arg));
}

_NODISCARD inline double comp_ellint_2(const double _Arg) {
    return __std_smf_comp_ellint_2(_Arg);
}

_NODISCARD inline float comp_ellint_2f(const float _Arg) {
    return __std_smf_comp_ellint_2f(_Arg);
}

_NODISCARD inline long double comp_ellint_2l(const long double _Arg) {
    return __std_smf_comp_ellint_2(static_cast<double>(_Arg));
}

_NODISCARD inline double comp_ellint_3(const double _Arg1, const double _Arg2) {
    return __std_smf_comp_ellint_3(_Arg1, _Arg2);
}

_NODISCARD inline float comp_ellint_3f(const float _Arg1, const float _Arg2) {
    return __std_smf_comp_ellint_3f(_Arg1, _Arg2);
}

_NODISCARD inline long double comp_ellint_3l(const long double _Arg1, const long double _Arg2) {
    return __std_smf_comp_ellint_3(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double cyl_bessel_i(const double _Arg1, const double _Arg2) {
    return __std_smf_cyl_bessel_i(_Arg1, _Arg2);
}

_NODISCARD inline float cyl_bessel_if(const float _Arg1, const float _Arg2) {
    return __std_smf_cyl_bessel_if(_Arg1, _Arg2);
}

_NODISCARD inline long double cyl_bessel_il(const long double _Arg1, const long double _Arg2) {
    return __std_smf_cyl_bessel_i(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double cyl_bessel_j(const double _Arg1, const double _Arg2) {
    return __std_smf_cyl_bessel_j(_Arg1, _Arg2);
}

_NODISCARD inline float cyl_bessel_jf(const float _Arg1, const float _Arg2) {
    return __std_smf_cyl_bessel_jf(_Arg1, _Arg2);
}

_NODISCARD inline long double cyl_bessel_jl(const long double _Arg1, const long double _Arg2) {
    return __std_smf_cyl_bessel_j(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double cyl_bessel_k(const double _Arg1, const double _Arg2) {
    return __std_smf_cyl_bessel_k(_Arg1, _Arg2);
}

_NODISCARD inline float cyl_bessel_kf(const float _Arg1, const float _Arg2) {
    return __std_smf_cyl_bessel_kf(_Arg1, _Arg2);
}

_NODISCARD inline long double cyl_bessel_kl(const long double _Arg1, const long double _Arg2) {
    return __std_smf_cyl_bessel_k(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double cyl_neumann(const double _Arg1, const double _Arg2) {
    return __std_smf_cyl_neumann(_Arg1, _Arg2);
}

_NODISCARD inline float cyl_neumannf(const float _Arg1, const float _Arg2) {
    return __std_smf_cyl_neumannf(_Arg1, _Arg2);
}

_NODISCARD inline long double cyl_neumannl(const long double _Arg1, const long double _Arg2) {
    return __std_smf_cyl_neumann(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double ellint_1(const double _Arg1, const double _Arg2) {
    return __std_smf_ellint_1(_Arg1, _Arg2);
}

_NODISCARD inline float ellint_1f(const float _Arg1, const float _Arg2) {
    return __std_smf_ellint_1f(_Arg1, _Arg2);
}

_NODISCARD inline long double ellint_1l(const long double _Arg1, const long double _Arg2) {
    return __std_smf_ellint_1(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double ellint_2(const double _Arg1, const double _Arg2) {
    return __std_smf_ellint_2(_Arg1, _Arg2);
}

_NODISCARD inline float ellint_2f(const float _Arg1, const float _Arg2) {
    return __std_smf_ellint_2f(_Arg1, _Arg2);
}

_NODISCARD inline long double ellint_2l(const long double _Arg1, const long double _Arg2) {
    return __std_smf_ellint_2(static_cast<double>(_Arg1), static_cast<double>(_Arg2));
}

_NODISCARD inline double ellint_3(const double _Arg1, const double _Arg2, const double _Arg3) {
    return __std_smf_ellint_3(_Arg1, _Arg2, _Arg3);
}

_NODISCARD inline float ellint_3f(const float _Arg1, const float _Arg2, const float _Arg3) {
    return __std_smf_ellint_3f(_Arg1, _Arg2, _Arg3);
}

_NODISCARD inline long double ellint_3l(const long double _Arg1, const long double _Arg2, const long double _Arg3) {
    return __std_smf_ellint_3(static_cast<double>(_Arg1), static_cast<double>(_Arg2), static_cast<double>(_Arg3));
}

_NODISCARD inline double expint(const double _Arg) {
    return __std_smf_expint(_Arg);
}

_NODISCARD inline float expintf(const float _Arg) {
    return __std_smf_expintf(_Arg);
}

_NODISCARD inline long double expintl(const long double _Arg) {
    return __std_smf_expint(static_cast<double>(_Arg));
}

_NODISCARD inline double hermite(const unsigned int _Arg1, const double _Arg2) {
    return __std_smf_hermite(_Arg1, _Arg2);
}

_NODISCARD inline float hermitef(const unsigned int _Arg1, const float _Arg2) {
    return __std_smf_hermitef(_Arg1, _Arg2);
}

_NODISCARD inline long double hermitel(const unsigned int _Arg1, const long double _Arg2) {
    return __std_smf_hermite(_Arg1, static_cast<double>(_Arg2));
}

_NODISCARD inline double laguerre(const unsigned int _Arg1, const double _Arg2) {
    return __std_smf_laguerre(_Arg1, _Arg2);
}

_NODISCARD inline float laguerref(const unsigned int _Arg1, const float _Arg2) {
    return __std_smf_laguerref(_Arg1, _Arg2);
}

_NODISCARD inline long double laguerrel(const unsigned int _Arg1, const long double _Arg2) {
    return __std_smf_laguerre(_Arg1, static_cast<double>(_Arg2));
}

_NODISCARD inline double legendre(const unsigned int _Degree, const double _Value) {
    return __std_smf_legendre(_Degree, _Value);
}

_NODISCARD inline float legendref(const unsigned int _Degree, const float _Value) {
    return __std_smf_legendref(_Degree, _Value);
}

_NODISCARD inline long double legendrel(const unsigned int _Degree, const long double _Value) {
    return __std_smf_legendre(_Degree, static_cast<double>(_Value));
}

_NODISCARD inline double riemann_zeta(const double _Arg) {
    return __std_smf_riemann_zeta(_Arg);
}

_NODISCARD inline float riemann_zetaf(const float _Arg) {
    return __std_smf_riemann_zetaf(_Arg);
}

_NODISCARD inline long double riemann_zetal(const long double _Arg) {
    return __std_smf_riemann_zeta(static_cast<double>(_Arg));
}

_NODISCARD inline double sph_bessel(const unsigned int _Arg1, const double _Arg2) {
    return __std_smf_sph_bessel(_Arg1, _Arg2);
}

_NODISCARD inline float sph_besself(const unsigned int _Arg1, const float _Arg2) {
    return __std_smf_sph_besself(_Arg1, _Arg2);
}

_NODISCARD inline long double sph_bessell(const unsigned int _Arg1, const long double _Arg2) {
    return __std_smf_sph_bessel(_Arg1, static_cast<double>(_Arg2));
}

_NODISCARD inline double sph_legendre(const unsigned int _Arg1, const unsigned int _Arg2, const double _Theta) {
    return __std_smf_sph_legendre(_Arg1, _Arg2, _Theta);
}

_NODISCARD inline float sph_legendref(const unsigned int _Arg1, const unsigned int _Arg2, const float _Theta) {
    return __std_smf_sph_legendref(_Arg1, _Arg2, _Theta);
}

_NODISCARD inline long double sph_legendrel(
    const unsigned int _Arg1, const unsigned int _Arg2, const long double _Theta) {
    return __std_smf_sph_legendre(_Arg1, _Arg2, static_cast<double>(_Theta));
}

_NODISCARD inline double sph_neumann(const unsigned int _Arg1, const double _Arg2) {
    return __std_smf_sph_neumann(_Arg1, _Arg2);
}

_NODISCARD inline float sph_neumannf(const unsigned int _Arg1, const float _Arg2) {
    return __std_smf_sph_neumannf(_Arg1, _Arg2);
}

_NODISCARD inline long double sph_neumannl(const unsigned int _Arg1, const long double _Arg2) {
    return __std_smf_sph_neumann(_Arg1, static_cast<double>(_Arg2));
}

_NODISCARD inline double hypot(const double _Dx, const double _Dy, const double _Dz) {
    return __std_smf_hypot3(_Dx, _Dy, _Dz);
}

_NODISCARD inline float hypot(const float _Dx, const float _Dy, const float _Dz) {
    return __std_smf_hypot3f(_Dx, _Dy, _Dz);
}

_NODISCARD inline long double hypot(const long double _Dx, const long double _Dy, const long double _Dz) {
    return __std_smf_hypot3(static_cast<double>(_Dx), static_cast<double>(_Dy), static_cast<double>(_Dz));
}

template <class _Ty1, class _Ty2, class _Ty3,
    enable_if_t<is_arithmetic_v<_Ty1> && is_arithmetic_v<_Ty2> && is_arithmetic_v<_Ty3>, int> = 0>
_NODISCARD auto hypot(const _Ty1 _Dx, const _Ty2 _Dy, const _Ty3 _Dz) {
    // N4727 [cmath.syn]/2 "Sufficient additional overloads"
    // Note that this template is selected by overload resolution only when at least one
    // argument is double/long double/integral but not all three are double or long double.
    using _Common      = _Common_float_type_t<_Ty1, _Common_float_type_t<_Ty2, _Ty3>>; // TRANSITION, fold expressions
    const auto _Result = __std_smf_hypot3(static_cast<double>(_Dx), static_cast<double>(_Dy), static_cast<double>(_Dz));
    return static_cast<_Common>(_Result);
}

#if _HAS_CXX20
template <class _Ty>
_NODISCARD constexpr _Ty _Common_lerp(const _Ty _ArgA, const _Ty _ArgB, const _Ty _ArgT) noexcept {
    // on a line intersecting {(0.0, _ArgA), (1.0, _ArgB)}, return the Y value for X == _ArgT

    const bool _T_is_finite = _Is_finite(_ArgT);
    if (_T_is_finite && _Is_finite(_ArgA) && _Is_finite(_ArgB)) {
        // 99% case, put it first; this block comes from P0811R3
        if ((_ArgA <= 0 && _ArgB >= 0) || (_ArgA >= 0 && _ArgB <= 0)) {
            // exact, monotonic, bounded, determinate, and (for _ArgA == _ArgB == 0) consistent:
            return _ArgT * _ArgB + (1 - _ArgT) * _ArgA;
        }

        if (_ArgT == 1) {
            // exact
            return _ArgB;
        }

        // exact at _ArgT == 0, monotonic except near _ArgT == 1, bounded, determinate, and consistent:
        const auto _Candidate = _ArgA + _ArgT * (_ArgB - _ArgA);
        // monotonic near _ArgT == 1:
        if ((_ArgT > 1) == (_ArgB > _ArgA)) {
            if (_ArgB > _Candidate) {
                return _ArgB;
            }
        } else {
            if (_Candidate > _ArgB) {
                return _ArgB;
            }
        }

        return _Candidate;
    }

    if (_STD is_constant_evaluated()) {
        if (_Is_nan(_ArgA)) {
            return _ArgA;
        }

        if (_Is_nan(_ArgB)) {
            return _ArgB;
        }

        if (_Is_nan(_ArgT)) {
            return _ArgT;
        }
    } else {
        // raise FE_INVALID if at least one of _ArgA, _ArgB, and _ArgT is signaling NaN
        if (_Is_nan(_ArgA) || _Is_nan(_ArgB)) {
            return (_ArgA + _ArgB) + _ArgT;
        }

        if (_Is_nan(_ArgT)) {
            return _ArgT + _ArgT;
        }
    }

    if (_T_is_finite) {
        // _ArgT is finite, _ArgA and/or _ArgB is infinity
        if (_ArgT < 0) {
            // if _ArgT < 0:     return infinity in the "direction" of _ArgA if that exists, NaN otherwise
            return _ArgA - _ArgB;
        } else if (_ArgT <= 1) {
            // if _ArgT == 0:    return _ArgA (infinity) if _ArgB is finite, NaN otherwise
            // if 0 < _ArgT < 1: return infinity "between" _ArgA and _ArgB if that exists, NaN otherwise
            // if _ArgT == 1:    return _ArgB (infinity) if _ArgA is finite, NaN otherwise
            return _ArgT * _ArgB + (1 - _ArgT) * _ArgA;
        } else {
            // if _ArgT > 1:     return infinity in the "direction" of _ArgB if that exists, NaN otherwise
            return _ArgB - _ArgA;
        }
    } else {
        // _ArgT is an infinity; return infinity in the "direction" of _ArgA and _ArgB if that exists, NaN otherwise
        return _ArgT * (_ArgB - _ArgA);
    }
}

// As of 2019-06-17 it is unclear whether the "sufficient additional overloads" clause is intended to target lerp;
// LWG-3223 is pending.

_NODISCARD constexpr inline float lerp(const float _ArgA, const float _ArgB, const float _ArgT) noexcept {
    return _Common_lerp(_ArgA, _ArgB, _ArgT);
}

_NODISCARD constexpr inline double lerp(const double _ArgA, const double _ArgB, const double _ArgT) noexcept {
    return _Common_lerp(_ArgA, _ArgB, _ArgT);
}

_NODISCARD constexpr inline long double lerp(
    const long double _ArgA, const long double _ArgB, const long double _ArgT) noexcept {
    return _Common_lerp(_ArgA, _ArgB, _ArgT);
}
#endif // _HAS_CXX20
_STD_END
#endif // _HAS_CXX17

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CMATH_
