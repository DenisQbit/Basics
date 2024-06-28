// numbers standard header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _NUMBERS_
#define _NUMBERS_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#if !_HAS_CXX20
#pragma message("The contents of <numbers> are available only with C++20 or later.")
#else // ^^^ !_HAS_CXX20 / _HAS_CXX20 vvv
#ifdef __cpp_lib_concepts
#include <concepts>
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
#include <xstddef>
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
namespace numbers {
    template <class _Ty>
    struct _Invalid {
        static_assert(_Always_false<_Ty>, "A program that instantiates a primary template of a mathematical constant "
                                          "variable template is ill-formed. (N4835 [math.constants]/3)");
    };

    template <class _Ty>
    inline constexpr _Ty e_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty log2e_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty log10e_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty pi_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty inv_pi_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty inv_sqrtpi_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty ln2_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty ln10_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty sqrt2_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty sqrt3_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty inv_sqrt3_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty egamma_v = _Invalid<_Ty>{};
    template <class _Ty>
    inline constexpr _Ty phi_v = _Invalid<_Ty>{};

#ifdef __cpp_lib_concepts
    template <floating_point _Floating>
    inline constexpr _Floating e_v<_Floating> = static_cast<_Floating>(2.718281828459045);
    template <floating_point _Floating>
    inline constexpr _Floating log2e_v<_Floating> = static_cast<_Floating>(1.4426950408889634);
    template <floating_point _Floating>
    inline constexpr _Floating log10e_v<_Floating> = static_cast<_Floating>(0.4342944819032518);
    template <floating_point _Floating>
    inline constexpr _Floating pi_v<_Floating> = static_cast<_Floating>(3.141592653589793);
    template <floating_point _Floating>
    inline constexpr _Floating inv_pi_v<_Floating> = static_cast<_Floating>(0.3183098861837907);
    template <floating_point _Floating>
    inline constexpr _Floating inv_sqrtpi_v<_Floating> = static_cast<_Floating>(0.5641895835477563);
    template <floating_point _Floating>
    inline constexpr _Floating ln2_v<_Floating> = static_cast<_Floating>(0.6931471805599453);
    template <floating_point _Floating>
    inline constexpr _Floating ln10_v<_Floating> = static_cast<_Floating>(2.302585092994046);
    template <floating_point _Floating>
    inline constexpr _Floating sqrt2_v<_Floating> = static_cast<_Floating>(1.4142135623730951);
    template <floating_point _Floating>
    inline constexpr _Floating sqrt3_v<_Floating> = static_cast<_Floating>(1.7320508075688772);
    template <floating_point _Floating>
    inline constexpr _Floating inv_sqrt3_v<_Floating> = static_cast<_Floating>(0.5773502691896257);
    template <floating_point _Floating>
    inline constexpr _Floating egamma_v<_Floating> = static_cast<_Floating>(0.5772156649015329);
    template <floating_point _Floating>
    inline constexpr _Floating phi_v<_Floating> = static_cast<_Floating>(1.618033988749895);
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
    template <>
    inline constexpr double e_v<double> = 2.718281828459045;
    template <>
    inline constexpr double log2e_v<double> = 1.4426950408889634;
    template <>
    inline constexpr double log10e_v<double> = 0.4342944819032518;
    template <>
    inline constexpr double pi_v<double> = 3.141592653589793;
    template <>
    inline constexpr double inv_pi_v<double> = 0.3183098861837907;
    template <>
    inline constexpr double inv_sqrtpi_v<double> = 0.5641895835477563;
    template <>
    inline constexpr double ln2_v<double> = 0.6931471805599453;
    template <>
    inline constexpr double ln10_v<double> = 2.302585092994046;
    template <>
    inline constexpr double sqrt2_v<double> = 1.4142135623730951;
    template <>
    inline constexpr double sqrt3_v<double> = 1.7320508075688772;
    template <>
    inline constexpr double inv_sqrt3_v<double> = 0.5773502691896257;
    template <>
    inline constexpr double egamma_v<double> = 0.5772156649015329;
    template <>
    inline constexpr double phi_v<double> = 1.618033988749895;

    template <>
    inline constexpr float e_v<float> = static_cast<float>(e_v<double>);
    template <>
    inline constexpr float log2e_v<float> = static_cast<float>(log2e_v<double>);
    template <>
    inline constexpr float log10e_v<float> = static_cast<float>(log10e_v<double>);
    template <>
    inline constexpr float pi_v<float> = static_cast<float>(pi_v<double>);
    template <>
    inline constexpr float inv_pi_v<float> = static_cast<float>(inv_pi_v<double>);
    template <>
    inline constexpr float inv_sqrtpi_v<float> = static_cast<float>(inv_sqrtpi_v<double>);
    template <>
    inline constexpr float ln2_v<float> = static_cast<float>(ln2_v<double>);
    template <>
    inline constexpr float ln10_v<float> = static_cast<float>(ln10_v<double>);
    template <>
    inline constexpr float sqrt2_v<float> = static_cast<float>(sqrt2_v<double>);
    template <>
    inline constexpr float sqrt3_v<float> = static_cast<float>(sqrt3_v<double>);
    template <>
    inline constexpr float inv_sqrt3_v<float> = static_cast<float>(inv_sqrt3_v<double>);
    template <>
    inline constexpr float egamma_v<float> = static_cast<float>(egamma_v<double>);
    template <>
    inline constexpr float phi_v<float> = static_cast<float>(phi_v<double>);

    template <>
    inline constexpr long double e_v<long double> = e_v<double>;
    template <>
    inline constexpr long double log2e_v<long double> = log2e_v<double>;
    template <>
    inline constexpr long double log10e_v<long double> = log10e_v<double>;
    template <>
    inline constexpr long double pi_v<long double> = pi_v<double>;
    template <>
    inline constexpr long double inv_pi_v<long double> = inv_pi_v<double>;
    template <>
    inline constexpr long double inv_sqrtpi_v<long double> = inv_sqrtpi_v<double>;
    template <>
    inline constexpr long double ln2_v<long double> = ln2_v<double>;
    template <>
    inline constexpr long double ln10_v<long double> = ln10_v<double>;
    template <>
    inline constexpr long double sqrt2_v<long double> = sqrt2_v<double>;
    template <>
    inline constexpr long double sqrt3_v<long double> = sqrt3_v<double>;
    template <>
    inline constexpr long double inv_sqrt3_v<long double> = inv_sqrt3_v<double>;
    template <>
    inline constexpr long double egamma_v<long double> = egamma_v<double>;
    template <>
    inline constexpr long double phi_v<long double> = phi_v<double>;
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^

    inline constexpr double e          = e_v<double>;
    inline constexpr double log2e      = log2e_v<double>;
    inline constexpr double log10e     = log10e_v<double>;
    inline constexpr double pi         = pi_v<double>;
    inline constexpr double inv_pi     = inv_pi_v<double>;
    inline constexpr double inv_sqrtpi = inv_sqrtpi_v<double>;
    inline constexpr double ln2        = ln2_v<double>;
    inline constexpr double ln10       = ln10_v<double>;
    inline constexpr double sqrt2      = sqrt2_v<double>;
    inline constexpr double sqrt3      = sqrt3_v<double>;
    inline constexpr double inv_sqrt3  = inv_sqrt3_v<double>;
    inline constexpr double egamma     = egamma_v<double>;
    inline constexpr double phi        = phi_v<double>;
} // namespace numbers
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _HAS_CXX20
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _NUMBERS_
