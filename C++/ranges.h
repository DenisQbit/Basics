// ranges standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _RANGES_
#define _RANGES_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#ifndef __cpp_lib_ranges
#pragma message("The contents of <ranges> are available only with C++20 or later.")
#else // ^^^ !defined(__cpp_lib_ranges) / defined(__cpp_lib_ranges) vvv
#include <__msvc_int128.hpp>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <span>
#include <string_view>
#include <tuple>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
namespace ranges {
    // MUCH machinery defined in <xutility>

    template <class _Ty>
    inline constexpr bool _Is_initializer_list = _Is_specialization_v<remove_cvref_t<_Ty>, initializer_list>;

    // clang-format off
    template <class _Rng>
    concept viewable_range = range<_Rng>
        && ((view<remove_cvref_t<_Rng>> && constructible_from<remove_cvref_t<_Rng>, _Rng>)
            || (!view<remove_cvref_t<_Rng>>
                && (is_lvalue_reference_v<_Rng>
                    || (movable<remove_reference_t<_Rng>> && !_Is_initializer_list<_Rng>))));

    template <class _Rng>
    concept _Simple_view = view<_Rng> && range<const _Rng>
        && same_as<iterator_t<_Rng>, iterator_t<const _Rng>>
        && same_as<sentinel_t<_Rng>, sentinel_t<const _Rng>>;

    template <class _Ty>
    concept _Copy_constructible_object = copy_constructible<_Ty> && _Destructible_object<_Ty>;

    template <class _It>
    concept _Has_arrow = input_iterator<_It>
        && (is_pointer_v<_It> || _Has_member_arrow<_It&>);
    // clang-format on

    template <bool _IsConst, class _Ty>
    using _Maybe_const = conditional_t<_IsConst, const _Ty, _Ty>;

    template <bool _IsWrapped, class _Ty>
    using _Maybe_wrapped = conditional_t<_IsWrapped, _Ty, _Unwrapped_t<_Ty>>;

    namespace _Pipe {
        // clang-format off
        template <class _Left, class _Right>
        concept _Can_pipe = requires(_Left&& __l, _Right&& __r) {
            static_cast<_Right&&>(__r)(static_cast<_Left&&>(__l));
        };

        template <class _Left, class _Right>
        concept _Can_compose = constructible_from<remove_cvref_t<_Left>, _Left>
            && constructible_from<remove_cvref_t<_Right>, _Right>;
        // clang-format on

        template <class, class>
        struct _Pipeline;

        template <class _Derived>
        struct _Base {
            // clang-format off
            template <class _Other>
                requires _Can_compose<_Derived, _Other>
            constexpr auto operator|(_Base<_Other>&& __r) && noexcept(
                noexcept(_Pipeline{static_cast<_Derived&&>(*this), static_cast<_Other&&>(__r)})) {
                // clang-format on
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Derived, _Base<_Derived>>);
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Other, _Base<_Other>>);
                return _Pipeline{static_cast<_Derived&&>(*this), static_cast<_Other&&>(__r)};
            }

            // clang-format off
            template <class _Other>
                requires _Can_compose<_Derived, const _Other&>
            constexpr auto operator|(const _Base<_Other>& __r) && noexcept(
                noexcept(_Pipeline{static_cast<_Derived&&>(*this), static_cast<const _Other&>(__r)})) {
                // clang-format on
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Derived, _Base<_Derived>>);
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Other, _Base<_Other>>);
                return _Pipeline{static_cast<_Derived&&>(*this), static_cast<const _Other&>(__r)};
            }

            // clang-format off
            template <class _Other>
                requires _Can_compose<const _Derived&, _Other>
            constexpr auto operator|(_Base<_Other>&& __r) const& noexcept(
                noexcept(_Pipeline{static_cast<const _Derived&>(*this), static_cast<_Other&&>(__r)})) {
                // clang-format on
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Derived, _Base<_Derived>>);
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Other, _Base<_Other>>);
                return _Pipeline{static_cast<const _Derived&>(*this), static_cast<_Other&&>(__r)};
            }

            // clang-format off
            template <class _Other>
                requires _Can_compose<const _Derived&, const _Other&>
            constexpr auto operator|(const _Base<_Other>& __r) const& noexcept(
                noexcept(_Pipeline{static_cast<const _Derived&>(*this), static_cast<const _Other&>(__r)})) {
                // clang-format on
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Derived, _Base<_Derived>>);
                _STL_INTERNAL_STATIC_ASSERT(derived_from<_Other, _Base<_Other>>);
                return _Pipeline{static_cast<const _Derived&>(*this), static_cast<const _Other&>(__r)};
            }

            template <_Can_pipe<const _Derived&> _Left>
            friend constexpr auto operator|(_Left&& __l, const _Base& __r)
#ifdef __EDG__ // TRANSITION, VSO-1222776
                noexcept(noexcept(_STD declval<const _Derived&>()(_STD forward<_Left>(__l))))
#else // ^^^ workaround / no workaround vvv
                noexcept(noexcept(static_cast<const _Derived&>(__r)(_STD forward<_Left>(__l))))
#endif // TRANSITION, VSO-1222776
            {
                return static_cast<const _Derived&>(__r)(_STD forward<_Left>(__l));
            }

            template <_Can_pipe<_Derived> _Left>
            friend constexpr auto operator|(_Left&& __l, _Base&& __r)
#ifdef __EDG__ // TRANSITION, VSO-1222776
                noexcept(noexcept(_STD declval<_Derived>()(_STD forward<_Left>(__l))))
#else // ^^^ workaround / no workaround vvv
                noexcept(noexcept(static_cast<_Derived&&>(__r)(_STD forward<_Left>(__l))))
#endif // TRANSITION, VSO-1222776
            {
                return static_cast<_Derived&&>(__r)(_STD forward<_Left>(__l));
            }
        };

        template <class _Left, class _Right>
        struct _Pipeline : _Base<_Pipeline<_Left, _Right>> {
            /* [[no_unique_address]] */ _Left __l;
            /* [[no_unique_address]] */ _Right __r;

            template <class _Ty1, class _Ty2>
            constexpr explicit _Pipeline(_Ty1&& _Val1, _Ty2&& _Val2) noexcept(
                is_nothrow_convertible_v<_Ty1, _Left>&& is_nothrow_convertible_v<_Ty2, _Right>)
                : __l(_STD forward<_Ty1>(_Val1)), __r(_STD forward<_Ty2>(_Val2)) {}

            template <class _Ty>
            _NODISCARD constexpr auto operator()(_Ty&& _Val) noexcept(
                noexcept(__r(__l(_STD forward<_Ty>(_Val))))) requires requires {
                __r(__l(static_cast<_Ty&&>(_Val)));
            }
            { return __r(__l(_STD forward<_Ty>(_Val))); }

            template <class _Ty>
            _NODISCARD constexpr auto operator()(_Ty&& _Val) const
                noexcept(noexcept(__r(__l(_STD forward<_Ty>(_Val))))) requires requires {
                __r(__l(static_cast<_Ty&&>(_Val)));
            }
            { return __r(__l(_STD forward<_Ty>(_Val))); }
        };

        template <class _Ty1, class _Ty2>
        _Pipeline(_Ty1, _Ty2) -> _Pipeline<_Ty1, _Ty2>;
    } // namespace _Pipe

    template <range _Rng, class _Derived>
    class _Cached_position : public view_interface<_Derived> {
        static_assert(_Always_false<_Rng>, "A range must be at least forward for position caching to be worthwhile.");
    };

    template <forward_range _Rng, class _Derived>
    class _Cached_position<_Rng, _Derived> : public view_interface<_Derived> {
    private:
        using _It = iterator_t<_Rng>;

        /* [[no_unique_address]] */ _It _Pos{};
        bool _Cached = false;

    protected:
        _Cached_position()  = default;
        ~_Cached_position() = default;

        // a copied iterator doesn't point into a copied range, so cache values must not propagate via copy
        constexpr _Cached_position(const _Cached_position&) noexcept(is_nothrow_default_constructible_v<_It>) {}
        constexpr _Cached_position& operator=(const _Cached_position&) noexcept(noexcept(_Pos = _It{})) {
            _Pos    = _It{};
            _Cached = false;
            return *this;
        }

        // a moved iterator doesn't point into a moved range, so cache values must not propagate via move;
        // similarly, a cache value might not be valid for a moved-from view so clear move sources
        constexpr _Cached_position(_Cached_position&& _Other) noexcept(noexcept(_Pos = _It{})) {
            _Other._Pos    = _It{};
            _Other._Cached = false;
        }
        constexpr _Cached_position& operator=(_Cached_position&& _Other) noexcept(noexcept(_Pos = _It{})) {
            _Pos           = _It{};
            _Cached        = false;
            _Other._Pos    = _It{};
            _Other._Cached = false;
            return *this;
        }

        _NODISCARD constexpr bool _Has_cache() const noexcept { // Is there a cached position?
            return _Cached;
        }

        _NODISCARD constexpr _It _Get_cache(_Rng&) const noexcept(is_nothrow_copy_constructible_v<_It>) {
            _STL_INTERNAL_CHECK(_Cached);
            return _Pos;
        }

        constexpr void _Set_cache(_Rng&, _It _Iter) noexcept(is_nothrow_move_assignable_v<_It>) {
            _Pos    = _STD move(_Iter);
            _Cached = true;
        }
    };

    template <random_access_range _Rng, class _Derived>
    class _Cached_position<_Rng, _Derived> : public view_interface<_Derived> {
    private:
        using _It = iterator_t<_Rng>;

        range_difference_t<_Rng> _Off = -1;

    protected:
        _Cached_position()  = default;
        ~_Cached_position() = default;

        // Offsets are oblivious to copying, so cache values _do_ propagate via copying.
        _Cached_position(const _Cached_position&) = default;
        _Cached_position& operator=(const _Cached_position&) = default;

        // Offsets are potentially invalidated by move, so source caches are invalidated after move
        constexpr _Cached_position(_Cached_position&& _Other) noexcept
            : _Off(_STD exchange(_Other._Off, range_difference_t<_Rng>{-1})) {}
        constexpr _Cached_position& operator=(_Cached_position&& _Other) noexcept {
            _Off = _STD exchange(_Other._Off, range_difference_t<_Rng>{-1});
            return *this;
        }

        _NODISCARD constexpr bool _Has_cache() const noexcept { // Is there a cached position?
            return _Off >= range_difference_t<_Rng>{0};
        }

        _NODISCARD constexpr _It _Get_cache(_Rng& _Range) const noexcept(noexcept(_RANGES begin(_Range) + _Off)) {
            _STL_INTERNAL_CHECK(_Has_cache());
            return _RANGES begin(_Range) + _Off;
        }

        constexpr void _Set_cache(_Rng& _Range, const _It& _Iter) noexcept(
            noexcept(_Off = _Iter - _RANGES begin(_Range))) {
            _Off = _Iter - _RANGES begin(_Range);
        }
    };

    template <bool _Enable, class _Rng, class _Derived>
    using _Cached_position_t = conditional_t<_Enable, _Cached_position<_Rng, _Derived>, view_interface<_Derived>>;

    template <_Copy_constructible_object _Ty>
    class _Copyable_box { // a simplified optional that augments copy_constructible types with full copyability
    public:
        constexpr _Copyable_box() noexcept(is_nothrow_default_constructible_v<_Ty>) requires default_initializable<_Ty>
            : _Val(), _Engaged{true} {}

        template <class... _Types>
        constexpr _Copyable_box(in_place_t, _Types&&... _Args) noexcept(
            is_nothrow_constructible_v<_Ty, _Types...>) // strengthened
            : _Val(_STD forward<_Types>(_Args)...), _Engaged{true} {}

        _CONSTEXPR20_DYNALLOC ~_Copyable_box() {
            if (_Engaged) {
                _Val.~_Ty();
            }
        }

        // TRANSITION, LLVM-46269, destructor order is significant
        // clang-format off
        ~_Copyable_box() requires is_trivially_destructible_v<_Ty> = default;

        _Copyable_box(const _Copyable_box&) requires is_trivially_copy_constructible_v<_Ty> = default;
        _CONSTEXPR20_DYNALLOC _Copyable_box(const _Copyable_box& _That) : _Engaged{_That._Engaged} {
            if (_That._Engaged) {
                _Construct_in_place(_Val, _That._Val);
            }
        }

        _Copyable_box(_Copyable_box&&) requires is_trivially_move_constructible_v<_Ty> = default;
        _CONSTEXPR20_DYNALLOC _Copyable_box(_Copyable_box&& _That) : _Engaged{_That._Engaged} {
            if (_That._Engaged) {
                _Construct_in_place(_Val, _STD move(_That._Val));
            }
        }

        // clang-format off
        _Copyable_box& operator=(const _Copyable_box&) noexcept
            requires copyable<_Ty> && is_trivially_copy_assignable_v<_Ty> = default;
        // clang-format on

        _CONSTEXPR20_DYNALLOC _Copyable_box& operator=(const _Copyable_box& _That) noexcept(
            is_nothrow_copy_constructible_v<_Ty>&& is_nothrow_copy_assignable_v<_Ty>) /* strengthened */ requires
            copyable<_Ty> {
            if (_Engaged) {
                if (_That._Engaged) {
                    _Val = _That._Val;
                } else {
                    _Val.~_Ty();
                    _Engaged = false;
                }
            } else {
                if (_That._Engaged) {
                    _Construct_in_place(_Val, _That._Val);
                    _Engaged = true;
                } else {
                    // nothing to do
                }
            }

            return *this;
        }

        _CONSTEXPR20_DYNALLOC _Copyable_box& operator=(const _Copyable_box& _That) noexcept(
            is_nothrow_copy_constructible_v<_Ty>) {
            if (_STD addressof(_That) == this) {
                return *this;
            }

            if (_Engaged) {
                _Val.~_Ty();
                _Engaged = false;
            }

            if (_That._Engaged) {
                _Construct_in_place(_Val, _That._Val);
                _Engaged = true;
            }

            return *this;
        }

        // clang-format off
        _Copyable_box& operator=(_Copyable_box&&) noexcept
            requires movable<_Ty> && is_trivially_move_assignable_v<_Ty> = default;
        // clang-format on

        _CONSTEXPR20_DYNALLOC _Copyable_box& operator=(_Copyable_box&& _That) noexcept(
            is_nothrow_move_constructible_v<_Ty>&& is_nothrow_move_assignable_v<_Ty>) /* strengthened */ requires
            movable<_Ty> {
            if (_Engaged) {
                if (_That._Engaged) {
                    _Val = _STD move(_That._Val);
                } else {
                    _Val.~_Ty();
                    _Engaged = false;
                }
            } else {
                if (_That._Engaged) {
                    _Construct_in_place(_Val, _STD move(_That._Val));
                    _Engaged = true;
                } else {
                    // nothing to do
                }
            }

            return *this;
        }

        _CONSTEXPR20_DYNALLOC _Copyable_box& operator=(_Copyable_box&& _That) noexcept(
            is_nothrow_move_constructible_v<_Ty>) {
            if (_STD addressof(_That) == this) {
                return *this;
            }

            if (_Engaged) {
                _Val.~_Ty();
                _Engaged = false;
            }

            if (_That._Engaged) {
                _Construct_in_place(_Val, _STD move(_That._Val));
                _Engaged = true;
            }

            return *this;
        }

        constexpr explicit operator bool() const noexcept {
            return _Engaged;
        }

        _NODISCARD constexpr _Ty& operator*() noexcept {
            _STL_INTERNAL_CHECK(_Engaged);
            return _Val;
        }
        _NODISCARD constexpr const _Ty& operator*() const noexcept {
            _STL_INTERNAL_CHECK(_Engaged);
            return _Val;
        }

    private:
        union {
            _Ty _Val;
        };
        bool _Engaged;
    };

    // clang-format off
    template <_Copy_constructible_object _Ty>
        requires copyable<_Ty>
            || (is_nothrow_copy_constructible_v<_Ty>
                && (movable<_Ty> || is_nothrow_move_constructible_v<_Ty>))
    class _Copyable_box<_Ty> { // provide the same API more efficiently when we can avoid the disengaged state
        // clang-format on
    public:
        _Copyable_box() requires default_initializable<_Ty>
        = default;

        template <class... _Types>
        constexpr _Copyable_box(in_place_t, _Types&&... _Args) noexcept(
            is_nothrow_constructible_v<_Ty, _Types...>) // strengthened
            : _Val(_STD forward<_Types>(_Args)...) {}

        _Copyable_box(const _Copyable_box&) = default;
        _Copyable_box(_Copyable_box&&)      = default;
        _Copyable_box& operator             =(const _Copyable_box&) requires copyable<_Ty>
        = default;
        _Copyable_box& operator=(_Copyable_box&&) requires movable<_Ty>
        = default;

        _CONSTEXPR20_DYNALLOC _Copyable_box& operator=(const _Copyable_box& _That) noexcept {
            if (_STD addressof(_That) != this) {
                _Val.~_Ty();
                _Construct_in_place(_Val, _That._Val);
            }

            return *this;
        }

        _CONSTEXPR20_DYNALLOC _Copyable_box& operator=(_Copyable_box&& _That) noexcept {
            if (_STD addressof(_That) != this) {
                _Val.~_Ty();
                _Construct_in_place(_Val, _STD move(_That._Val));
            }

            return *this;
        }

        constexpr explicit operator bool() const noexcept {
            return true;
        }

        _NODISCARD constexpr _Ty& operator*() noexcept {
            return _Val;
        }
        _NODISCARD constexpr const _Ty& operator*() const noexcept {
            return _Val;
        }

    private:
        /* [[no_unique_address]] */ _Ty _Val{};
    };

    template <movable _Ty>
    class _Defaultabox { // a simplified optional that augments movable types with default-constructibility
    public:
        constexpr _Defaultabox() noexcept {}

        _CONSTEXPR20_DYNALLOC ~_Defaultabox() {
            if (_Engaged) {
                _Val.~_Ty();
            }
        }

        // TRANSITION, LLVM-46269, destructor order is significant
        // clang-format off
        ~_Defaultabox() requires is_trivially_destructible_v<_Ty> = default;

        _Defaultabox(const _Defaultabox&)
            requires copy_constructible<_Ty> && is_trivially_copy_constructible_v<_Ty> = default;
        // clang-format on

        _CONSTEXPR20_DYNALLOC _Defaultabox(const _Defaultabox& _That) requires copy_constructible<_Ty>
            : _Engaged{_That._Engaged} {
            if (_That._Engaged) {
                _Construct_in_place(_Val, _That._Val);
            }
        }

        _Defaultabox(_Defaultabox&&) requires is_trivially_move_constructible_v<_Ty>
        = default;

        _CONSTEXPR20_DYNALLOC _Defaultabox(_Defaultabox&& _That) : _Engaged{_That._Engaged} {
            if (_That._Engaged) {
                _Construct_in_place(_Val, _STD move(_That._Val));
            }
        }

        template <_Not_same_as<_Ty> _Uty>
        requires convertible_to<const _Uty&, _Ty>
        constexpr _Defaultabox(const _Defaultabox<_Uty>& _That) : _Engaged{_That} {
            if (_That) {
                _Construct_in_place(_Val, *_That);
            }
        }

        template <_Not_same_as<_Ty> _Uty>
        requires convertible_to<_Uty, _Ty>
        constexpr _Defaultabox(_Defaultabox<_Uty>&& _That) : _Engaged{_That} {
            if (_That) {
                _Construct_in_place(_Val, _STD move(*_That));
            }
        }

        // clang-format off
        _Defaultabox& operator=(const _Defaultabox&) noexcept
            requires copyable<_Ty> && is_trivially_copy_assignable_v<_Ty> = default;
        // clang-format on

        _CONSTEXPR20_DYNALLOC _Defaultabox& operator=(const _Defaultabox& _That) noexcept(
            is_nothrow_copy_constructible_v<_Ty>&& is_nothrow_copy_assignable_v<_Ty>) /* strengthened */ requires
            copyable<_Ty> {
            if (_Engaged) {
                if (_That._Engaged) {
                    _Val = _That._Val;
                } else {
                    _Val.~_Ty();
                    _Engaged = false;
                }
            } else {
                if (_That._Engaged) {
                    _Construct_in_place(_Val, _That._Val);
                    _Engaged = true;
                } else {
                    // nothing to do
                }
            }

            return *this;
        }

        _Defaultabox& operator=(_Defaultabox&&) noexcept requires is_trivially_move_assignable_v<_Ty>
        = default;

        _CONSTEXPR20_DYNALLOC _Defaultabox& operator=(_Defaultabox&& _That) noexcept(
            is_nothrow_move_constructible_v<_Ty>&& is_nothrow_move_assignable_v<_Ty>) /* strengthened */ {
            if (_Engaged) {
                if (_That._Engaged) {
                    _Val = _STD move(_That._Val);
                } else {
                    _Val.~_Ty();
                    _Engaged = false;
                }
            } else {
                if (_That._Engaged) {
                    _Construct_in_place(_Val, _STD move(_That._Val));
                    _Engaged = true;
                } else {
                    // nothing to do
                }
            }

            return *this;
        }

        _CONSTEXPR20_DYNALLOC _Defaultabox& operator=(_Ty&& _That) noexcept(
            is_nothrow_move_constructible_v<_Ty>&& is_nothrow_move_assignable_v<_Ty>) {
            if (_Engaged) {
                _Val = _STD move(_That);
            } else {
                _Construct_in_place(_Val, _STD move(_That));
                _Engaged = true;
            }

            return *this;
        }

        _CONSTEXPR20_DYNALLOC _Defaultabox& operator=(const _Ty& _That) noexcept(
            is_nothrow_copy_constructible_v<_Ty>&& is_nothrow_copy_assignable_v<_Ty>) requires copyable<_Ty> {
            if (_Engaged) {
                _Val = _That;
            } else {
                _Construct_in_place(_Val, _That);
                _Engaged = true;
            }

            return *this;
        }

        constexpr explicit operator bool() const noexcept {
            return _Engaged;
        }

        _NODISCARD constexpr _Ty& operator*() noexcept {
            _STL_INTERNAL_CHECK(_Engaged);
            return _Val;
        }
        _NODISCARD constexpr const _Ty& operator*() const noexcept {
            _STL_INTERNAL_CHECK(_Engaged);
            return _Val;
        }

        constexpr void _Reset() noexcept {
            if (_Engaged) {
                _Val.~_Ty();
                _Engaged = false;
            }
        }

        _NODISCARD constexpr bool operator==(const _Defaultabox& _That) const noexcept(noexcept(_Val == _That._Val)) {
            _STL_INTERNAL_STATIC_ASSERT(equality_comparable<_Ty>);
            return _Engaged == _That._Engaged && (!_Engaged || _Val == _That._Val);
        }

    private:
        union {
            _Ty _Val;
        };
        bool _Engaged = false;
    };

    // clang-format off
    template <movable _Ty>
        requires default_initializable<_Ty>
    class _Defaultabox<_Ty> { // provide the same API more efficiently for default-constructible types
        // clang-format on
    public:
        _Defaultabox() = default;

        template <_Not_same_as<_Ty> _Uty>
        requires convertible_to<const _Uty&, _Ty>
        constexpr _Defaultabox(const _Defaultabox<_Uty>& _That) {
            if (_That) {
                _Value = static_cast<_Ty>(*_That);
            }
        }

        template <_Not_same_as<_Ty> _Uty>
        requires convertible_to<_Uty, _Ty>
        constexpr _Defaultabox(_Defaultabox<_Uty>&& _That) {
            if (_That) {
                _Value = static_cast<_Ty>(_STD move(*_That));
            }
        }

        constexpr _Defaultabox& operator=(const _Ty& _Right) noexcept(
            is_nothrow_copy_assignable_v<_Ty>) requires copyable<_Ty> {
            _Value = _Right;
            return *this;
        }
        constexpr _Defaultabox& operator=(_Ty&& _Right) noexcept(is_nothrow_move_assignable_v<_Ty>) {
            _Value = _STD move(_Right);
            return *this;
        }

        constexpr explicit operator bool() const noexcept {
            return true;
        }

        _NODISCARD constexpr _Ty& operator*() noexcept {
            return _Value;
        }
        _NODISCARD constexpr const _Ty& operator*() const noexcept {
            return _Value;
        }

        constexpr void _Reset() noexcept(noexcept(_Value = _Ty{})) {
            _Value = _Ty{};
        }

        _NODISCARD bool operator==(const _Defaultabox&) const = default;

    private:
        /* [[no_unique_address]] */ _Ty _Value{};
    };

    template <_Destructible_object _Ty, bool _Needs_operator_bool = true>
    class _Non_propagating_cache { // a simplified optional that resets on copy / move
    public:
        constexpr _Non_propagating_cache() noexcept {}

        constexpr ~_Non_propagating_cache() {
            if (_Engaged) {
                _Val.~_Ty();
            }
        }

        // TRANSITION, LLVM-46269, destructor order is significant
        // clang-format off
        ~_Non_propagating_cache() requires is_trivially_destructible_v<_Ty> = default;
        // clang-format on

        constexpr _Non_propagating_cache(const _Non_propagating_cache&) noexcept {}

        constexpr _Non_propagating_cache(_Non_propagating_cache&& _Other) noexcept {
            if (_Other._Engaged) {
                _Other._Val.~_Ty();
                _Other._Engaged = false;
            }
        }

        constexpr _Non_propagating_cache& operator=(const _Non_propagating_cache& _Other) noexcept {
            if (_STD addressof(_Other) == this) {
                return *this;
            }

            if (_Engaged) {
                _Val.~_Ty();
                _Engaged = false;
            }

            return *this;
        }

        constexpr _Non_propagating_cache& operator=(_Non_propagating_cache&& _Other) noexcept {
            if (_Engaged) {
                _Val.~_Ty();
                _Engaged = false;
            }

            if (_Other._Engaged) {
                _Other._Val.~_Ty();
                _Other._Engaged = false;
            }

            return *this;
        }

        _NODISCARD constexpr explicit operator bool() const noexcept requires _Needs_operator_bool {
            return _Engaged;
        }

        _NODISCARD constexpr _Ty& operator*() noexcept {
            _STL_INTERNAL_CHECK(_Engaged);
            return _Val;
        }
        _NODISCARD constexpr const _Ty& operator*() const noexcept {
            _STL_INTERNAL_CHECK(_Engaged);
            return _Val;
        }

        template <class... _Types>
        constexpr _Ty& _Emplace(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty, _Types...>) {
            if (_Engaged) {
                _Val.~_Ty();
                _Engaged = false;
            }

            _Construct_in_place(_Val, _STD forward<_Types>(_Args)...);
            _Engaged = true;

            return _Val;
        }

    private:
        union {
            _Ty _Val;
        };
        bool _Engaged = false;
    };

    template <_Destructible_object _Ty>
    requires is_trivially_destructible_v<_Ty>
    class _Non_propagating_cache<_Ty, false> { // a specialization for trivially destructible types where checking if
                                               // the cache contains a value is not needed
    public:
        constexpr _Non_propagating_cache() noexcept {}

        ~_Non_propagating_cache() = default;

        constexpr _Non_propagating_cache(const _Non_propagating_cache&) noexcept {}

        constexpr _Non_propagating_cache(_Non_propagating_cache&&) noexcept {}

        constexpr _Non_propagating_cache& operator=(const _Non_propagating_cache&) noexcept {
            return *this;
        }

        constexpr _Non_propagating_cache& operator=(_Non_propagating_cache&&) noexcept {
            return *this;
        }

        _NODISCARD constexpr _Ty& operator*() noexcept {
            return _Val;
        }
        _NODISCARD constexpr const _Ty& operator*() const noexcept {
            return _Val;
        }

        template <class... _Types>
        constexpr _Ty& _Emplace(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty, _Types...>) {
            _Construct_in_place(_Val, _STD forward<_Types>(_Args)...);
            return _Val;
        }

    private:
        union {
            _Ty _Val;
        };
    };

    template <class _Fn, class... _Types>
    class _Range_closure : public _Pipe::_Base<_Range_closure<_Fn, _Types...>> {
    public:
        // We assume that _Fn is the type of a customization point object. That means
        // 1. The behavior of operator() is independent of cvref qualifiers, so we can use `invocable<_Fn, ` without
        //    loss of generality, and
        // 2. _Fn must be default-constructible and stateless, so we can create instances "on-the-fly" and avoid
        //    storing a copy.

        _STL_INTERNAL_STATIC_ASSERT((same_as<decay_t<_Types>, _Types> && ...));
        _STL_INTERNAL_STATIC_ASSERT(is_empty_v<_Fn>&& is_default_constructible_v<_Fn>);

        // clang-format off
        template <class... _UTypes>
            requires (same_as<decay_t<_UTypes>, _Types> && ...)
        constexpr explicit _Range_closure(_UTypes&&... _Args) noexcept(
            conjunction_v<is_nothrow_constructible<_Types, _UTypes>...>)
            : _Captures(_STD forward<_UTypes>(_Args)...) {}
        // clang-format on

        void operator()(auto&&) &       = delete;
        void operator()(auto&&) const&  = delete;
        void operator()(auto&&) &&      = delete;
        void operator()(auto&&) const&& = delete;

        using _Indices = index_sequence_for<_Types...>;

        template <class _Ty>
        requires invocable<_Fn, _Ty, _Types&...>
        constexpr decltype(auto) operator()(_Ty&& _Arg) & noexcept(
            noexcept(_Call(*this, _STD forward<_Ty>(_Arg), _Indices{}))) {
            return _Call(*this, _STD forward<_Ty>(_Arg), _Indices{});
        }

        template <class _Ty>
        requires invocable<_Fn, _Ty, const _Types&...>
        constexpr decltype(auto) operator()(_Ty&& _Arg) const& noexcept(
            noexcept(_Call(*this, _STD forward<_Ty>(_Arg), _Indices{}))) {
            return _Call(*this, _STD forward<_Ty>(_Arg), _Indices{});
        }

        template <class _Ty>
        requires invocable<_Fn, _Ty, _Types...>
        constexpr decltype(auto) operator()(_Ty&& _Arg) && noexcept(
            noexcept(_Call(_STD move(*this), _STD forward<_Ty>(_Arg), _Indices{}))) {
            return _Call(_STD move(*this), _STD forward<_Ty>(_Arg), _Indices{});
        }

        template <class _Ty>
        requires invocable<_Fn, _Ty, const _Types...>
        constexpr decltype(auto) operator()(_Ty&& _Arg) const&& noexcept(
            noexcept(_Call(_STD move(*this), _STD forward<_Ty>(_Arg), _Indices{}))) {
            return _Call(_STD move(*this), _STD forward<_Ty>(_Arg), _Indices{});
        }

    private:
        template <class _SelfTy, class _Ty, size_t... _Idx>
        static constexpr decltype(auto) _Call(_SelfTy&& _Self, _Ty&& _Arg, index_sequence<_Idx...>) noexcept(
            noexcept(_Fn{}(_STD forward<_Ty>(_Arg), _STD get<_Idx>(_STD forward<_SelfTy>(_Self)._Captures)...))) {
            _STL_INTERNAL_STATIC_ASSERT(same_as<index_sequence<_Idx...>, _Indices>);
            return _Fn{}(_STD forward<_Ty>(_Arg), _STD get<_Idx>(_STD forward<_SelfTy>(_Self)._Captures)...);
        }

        tuple<_Types...> _Captures;
    };

    // clang-format off
    template <class _Ty>
        requires is_object_v<_Ty>
    class empty_view : public view_interface<empty_view<_Ty>> {
        // clang-format on
    public:
        _NODISCARD static constexpr _Ty* begin() noexcept {
            return nullptr;
        }

        _NODISCARD static constexpr _Ty* end() noexcept {
            return nullptr;
        }

        _NODISCARD static constexpr _Ty* data() noexcept {
            return nullptr;
        }

        _NODISCARD static constexpr size_t size() noexcept {
            return 0;
        }

        _NODISCARD static constexpr bool empty() noexcept {
            return true;
        }
    };

    template <class _Ty>
    inline constexpr bool enable_borrowed_range<empty_view<_Ty>> = true;

    namespace views {
        template <class _Ty>
        inline constexpr empty_view<_Ty> empty;
    } // namespace views

    // clang-format off
    template <copy_constructible _Ty>
        requires is_object_v<_Ty>
    class single_view : public view_interface<single_view<_Ty>> {
        // clang-format on
    public:
        single_view() requires default_initializable<_Ty>
        = default;
        constexpr explicit single_view(const _Ty& _Val_) noexcept(is_nothrow_copy_constructible_v<_Ty>) // strengthened
            : _Val{in_place, _Val_} {}
        constexpr explicit single_view(_Ty&& _Val_) noexcept(is_nothrow_move_constructible_v<_Ty>) // strengthened
            : _Val{in_place, _STD move(_Val_)} {}

        // clang-format off
        template <class... _Types>
            requires constructible_from<_Ty, _Types...>
        constexpr explicit single_view(in_place_t, _Types&&... _Args) noexcept(
            is_nothrow_constructible_v<_Ty, _Types...>) // strengthened
            // clang-format on
            : _Val{in_place, _STD forward<_Types>(_Args)...} {}

        _NODISCARD constexpr _Ty* begin() noexcept {
            return data();
        }
        _NODISCARD constexpr const _Ty* begin() const noexcept {
            return data();
        }

        _NODISCARD constexpr _Ty* end() noexcept {
            return data() + 1;
        }
        _NODISCARD constexpr const _Ty* end() const noexcept {
            return data() + 1;
        }

        _NODISCARD static constexpr size_t size() noexcept {
            return 1;
        }

        _NODISCARD constexpr _Ty* data() noexcept {
            return _STD addressof(*_Val);
        }
        _NODISCARD constexpr const _Ty* data() const noexcept {
            return _STD addressof(*_Val);
        }

    private:
        /* [[no_unique_address]] */ _Copyable_box<_Ty> _Val{};
    };

    template <class _Ty>
    single_view(_Ty) -> single_view<_Ty>;

    namespace views {
        struct _Single_fn {
            // clang-format off
            template <class _Ty>
            _NODISCARD constexpr auto operator()(_Ty&& _Val) const noexcept(
                noexcept(single_view<decay_t<_Ty>>(_STD forward<_Ty>(_Val)))) requires requires {
                typename single_view<decay_t<_Ty>>;
                single_view<decay_t<_Ty>>(static_cast<_Ty&&>(_Val));
            } {
                return single_view<decay_t<_Ty>>(_STD forward<_Ty>(_Val));
            }
            // clang-format on
        };

        inline constexpr _Single_fn single;
    } // namespace views

    template <class _Ty>
    using _Iota_diff_t = conditional_t<is_integral_v<_Ty>,
        conditional_t<sizeof(_Ty) < sizeof(int), int,
            conditional_t<sizeof(_Ty) < sizeof(long long), long long, _Signed128>>,
        iter_difference_t<_Ty>>;

    // clang-format off
    template <class _Ty>
    concept _Decrementable = incrementable<_Ty> && requires(_Ty __t) {
        { --__t } -> same_as<_Ty&>;
        { __t-- } -> same_as<_Ty>;
    };

    template <class _Ty>
    concept _Advanceable = _Decrementable<_Ty> && totally_ordered<_Ty>
        && requires(_Ty __i, const _Ty __j, const _Iota_diff_t<_Ty> __n) {
            { __i += __n } -> same_as<_Ty&>;
            { __i -= __n } -> same_as<_Ty&>;
            _Ty(__j + __n);
            _Ty(__n + __j);
            _Ty(__j - __n);
            { __j - __j } -> convertible_to<_Iota_diff_t<_Ty>>;
        };

    template <class _Wi>
    struct _Ioterator_category_base {};

    template <incrementable _Wi>
        requires integral<_Iota_diff_t<_Wi>> // TRANSITION, LWG-3670
    struct _Ioterator_category_base<_Wi> {
        using iterator_category = input_iterator_tag;
    };

    template <weakly_incrementable _Wi>
        requires copyable<_Wi>
    struct _Ioterator : _Ioterator_category_base<_Wi> {
        /* [[no_unique_address]] */ _Wi _Current{};

        using iterator_concept = conditional_t<_Advanceable<_Wi>, random_access_iterator_tag,
            conditional_t<_Decrementable<_Wi>, bidirectional_iterator_tag,
                conditional_t<incrementable<_Wi>, forward_iterator_tag, input_iterator_tag>>>;
        using value_type       = _Wi;
        using difference_type  = _Iota_diff_t<_Wi>;

        // clang-format off
        _Ioterator() requires default_initializable<_Wi> = default;
        // clang-format on

        constexpr explicit _Ioterator(_Wi _Val) noexcept(is_nothrow_move_constructible_v<_Wi>) /* strengthened */
            : _Current(_STD move(_Val)) {}

        _NODISCARD constexpr _Wi operator*() const noexcept(is_nothrow_copy_constructible_v<_Wi>) {
            return _Current;
        }

        constexpr _Ioterator& operator++() noexcept(noexcept(++_Current)) /* strengthened */ {
            ++_Current;
            return *this;
        }

        constexpr auto operator++(int) noexcept(
            noexcept(++_Current) && (!incrementable<_Wi> || is_nothrow_copy_constructible_v<_Wi>) ) /* strengthened */ {
            if constexpr (incrementable<_Wi>) {
                auto _Tmp = *this;
                ++_Current;
                return _Tmp;
            } else {
                ++_Current;
            }
        }

        constexpr _Ioterator& operator--() noexcept(
            noexcept(--_Current)) /* strengthened */ requires _Decrementable<_Wi> {
            --_Current;
            return *this;
        }

        constexpr _Ioterator operator--(int) noexcept(is_nothrow_copy_constructible_v<_Wi>&& noexcept(
            --_Current)) /* strengthened */ requires _Decrementable<_Wi> {
            auto _Tmp = *this;
            --_Current;
            return _Tmp;
        }

#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, DevCom-1347136
    private : template <class _Left, class _Right>
              static constexpr bool _Nothrow_plus_equal =
                  noexcept(_STD declval<_Left&>() += _STD declval<const _Right&>());
        template <_Integer_like _Left, class _Right>
        static constexpr bool _Nothrow_plus_equal<_Left, _Right> = true;

        template <class _Left, class _Right>
        static constexpr bool _Nothrow_minus_equal = noexcept(_STD declval<_Left&>() -= _STD declval<const _Right&>());
        template <_Integer_like _Left, class _Right>
        static constexpr bool _Nothrow_minus_equal<_Left, _Right> = true;

    public:
#endif // TRANSITION, DevCom-1347136

        constexpr _Ioterator& operator+=(const difference_type _Off)
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1347136
            noexcept(noexcept(_Current += _Off)) /* strengthened */
#else // ^^^ no workaround / workaround vvv
            noexcept(_Nothrow_plus_equal<_Wi, difference_type>) /* strengthened */
#endif // TRANSITION, DevCom-1347136
            requires _Advanceable<_Wi> {
            if constexpr (_Integer_like<_Wi>) {
                if constexpr (_Signed_integer_like<_Wi>) {
                    _Current = static_cast<_Wi>(_Current + _Off);
                } else {
                    if (_Off >= difference_type{0}) {
                        _Current += static_cast<_Wi>(_Off);
                    } else {
                        _Current -= static_cast<_Wi>(-_Off);
                    }
                }
            } else {
                _Current += _Off;
            }
            return *this;
        }

        constexpr _Ioterator& operator-=(const difference_type _Off)
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1347136
            noexcept(noexcept(_Current -= _Off)) /* strengthened */
#else // ^^^ no workaround / workaround vvv
            noexcept(_Nothrow_minus_equal<_Wi, difference_type>) /* strengthened */
#endif // TRANSITION, DevCom-1347136
            requires _Advanceable<_Wi> {
            if constexpr (_Integer_like<_Wi>) {
                if constexpr (_Signed_integer_like<_Wi>) {
                    _Current = static_cast<_Wi>(_Current - _Off);
                } else {
                    if (_Off >= difference_type{0}) {
                        _Current -= static_cast<_Wi>(_Off);
                    } else {
                        _Current += static_cast<_Wi>(-_Off);
                    }
                }
            } else {
                _Current -= _Off;
            }
            return *this;
        }

        _NODISCARD constexpr _Wi operator[](const difference_type _Idx) const
            noexcept(noexcept(static_cast<_Wi>(_Current + _Idx))) /* strengthened */ requires _Advanceable<_Wi> {
            if constexpr (_Integer_like<_Wi>) {
                return static_cast<_Wi>(_Current + static_cast<_Wi>(_Idx));
            } else {
                return static_cast<_Wi>(_Current + _Idx);
            }
        }

        _NODISCARD_FRIEND constexpr bool operator==(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
            noexcept(_Left._Current == _Right._Current)) requires equality_comparable<_Wi> {
            return _Left._Current == _Right._Current;
        }

        _NODISCARD_FRIEND constexpr bool operator<(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
            noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires totally_ordered<_Wi> {
            return _Left._Current < _Right._Current;
        }
        _NODISCARD_FRIEND constexpr bool operator>(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
            noexcept(_Right._Current < _Left._Current)) /* strengthened */ requires totally_ordered<_Wi> {
            return _Right._Current < _Left._Current;
        }
        _NODISCARD_FRIEND constexpr bool operator<=(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
            noexcept(!(_Right._Current < _Left._Current))) /* strengthened */ requires totally_ordered<_Wi> {
            return !(_Right._Current < _Left._Current);
        }
        _NODISCARD_FRIEND constexpr bool operator>=(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
            noexcept(!(_Left._Current < _Right._Current))) /* strengthened */ requires totally_ordered<_Wi> {
            return !(_Left._Current < _Right._Current);
        }

        _NODISCARD_FRIEND constexpr auto operator<=>(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
            noexcept(_Left._Current <=> _Right._Current)) requires totally_ordered<_Wi> && three_way_comparable<_Wi> {
            return _Left._Current <=> _Right._Current;
        }

        _NODISCARD_FRIEND constexpr _Ioterator operator+(_Ioterator _It, const difference_type _Off) noexcept(
            is_nothrow_move_constructible_v<_Ioterator>&& noexcept(
                _It += _Off)) /* strengthened */ requires _Advanceable<_Wi> {
            _It += _Off;
            return _It;
        }
        _NODISCARD_FRIEND constexpr _Ioterator operator+(const difference_type _Off, _Ioterator _It) noexcept(
            is_nothrow_move_constructible_v<_Wi>&& noexcept(
                static_cast<_Wi>(_It._Current + _Off))) /* strengthened */ requires _Advanceable<_Wi> {
            return _Ioterator{static_cast<_Wi>(_It._Current + _Off)};
        }

        _NODISCARD_FRIEND constexpr _Ioterator operator-(_Ioterator _It, const difference_type _Off) noexcept(
            is_nothrow_move_constructible_v<_Ioterator>&& noexcept(
                _It -= _Off)) /* strengthened */ requires _Advanceable<_Wi> {
            _It -= _Off;
            return _It;
        }

        _NODISCARD_FRIEND constexpr difference_type
            operator-(const _Ioterator& _Left, const _Ioterator& _Right) noexcept(
                noexcept(_Left._Current - _Right._Current)) /* strengthened */ requires _Advanceable<_Wi> {
            if constexpr (_Integer_like<_Wi>) {
                if constexpr (_Signed_integer_like<_Wi>) {
                    return static_cast<difference_type>(
                        static_cast<difference_type>(_Left._Current) - static_cast<difference_type>(_Right._Current));
                } else if (_Right._Current > _Left._Current) {
                    return static_cast<difference_type>(
                        -static_cast<difference_type>(_Right._Current - _Left._Current));
                } else {
                    return static_cast<difference_type>(_Left._Current - _Right._Current);
                }
            } else {
                return static_cast<difference_type>(_Left._Current - _Right._Current);
            }
        }
    };

    // clang-format off
    template <weakly_incrementable _Wi, semiregular _Bo>
        requires _Weakly_equality_comparable_with<_Wi, _Bo> && copyable<_Wi>
    struct _Iotinel {
        // clang-format on
    private:
        using _It = _Ioterator<_Wi>;

        _NODISCARD constexpr bool _Equal(const _It& _That) const noexcept(noexcept(_That._Current == _Last)) {
            return _That._Current == _Last;
        }

        _NODISCARD constexpr iter_difference_t<_Wi> _Delta(const _It& _That) const
            noexcept(noexcept(_Last - _That._Current)) {
            _STL_INTERNAL_STATIC_ASSERT(sized_sentinel_for<_Bo, _Wi>);
            return _Last - _That._Current;
        }

    public:
        /* [[no_unique_address]] */ _Bo _Last{};

        _NODISCARD_FRIEND constexpr bool operator==(const _It& _Left, const _Iotinel& _Right) noexcept(
            noexcept(_Right._Equal(_Left))) /* strengthened */ {
            return _Right._Equal(_Left);
        }

        _NODISCARD_FRIEND constexpr iter_difference_t<_Wi> operator-(const _It& _Left, const _Iotinel& _Right) noexcept(
            noexcept(_Right._Delta(_Left))) /* strengthened */ requires sized_sentinel_for<_Bo, _Wi> {
            return -_Right._Delta(_Left);
        }

        _NODISCARD_FRIEND constexpr iter_difference_t<_Wi> operator-(const _Iotinel& _Left, const _It& _Right) noexcept(
            noexcept(_Left._Delta(_Right))) /* strengthened */ requires sized_sentinel_for<_Bo, _Wi> {
            return _Left._Delta(_Right);
        }
    };

    // clang-format off
    template <weakly_incrementable _Wi, semiregular _Bo = unreachable_sentinel_t>
        requires _Weakly_equality_comparable_with<_Wi, _Bo> && copyable<_Wi>
    class iota_view : public view_interface<iota_view<_Wi, _Bo>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Wi _Value{};
        /* [[no_unique_address]] */ _Bo _Bound{};

        using _It = _Ioterator<_Wi>;
        using _Se = conditional_t<same_as<_Wi, _Bo>, _It,
            conditional_t<same_as<_Bo, unreachable_sentinel_t>, _Bo, _Iotinel<_Wi, _Bo>>>;

        _NODISCARD static constexpr _Bo& _Bound_from(_Se& _Last) noexcept {
            if constexpr (same_as<_Wi, _Bo>) {
                return _Last._Current;
            } else if constexpr (same_as<_Bo, unreachable_sentinel_t>) {
                return _Last;
            } else {
                return _Last._Last;
            }
        }

    public:
        iota_view() requires default_initializable<_Wi>
        = default;

        constexpr explicit iota_view(_Wi _Value_) noexcept(
            is_nothrow_move_constructible_v<_Wi>&& is_nothrow_default_constructible_v<_Bo>) // strengthened
            : _Value(_STD move(_Value_)) {}

        constexpr iota_view(type_identity_t<_Wi> _Value_, type_identity_t<_Bo> _Bound_) noexcept(
            is_nothrow_move_constructible_v<_Wi>&& is_nothrow_move_constructible_v<_Bo>) // strengthened
            : _Value(_STD move(_Value_)), _Bound(_STD move(_Bound_)) {
            if constexpr (totally_ordered_with<_Wi, _Bo>) {
                _STL_ASSERT(_Value_ <= _Bound_, "Per N4878 [range.iota.view]/8, the first argument must precede the "
                                                "second when their types are totally ordered.");
            }
        }

        constexpr iota_view(_It _First, _Se _Last) noexcept(
            is_nothrow_move_constructible_v<_Wi>&& is_nothrow_move_constructible_v<_Bo>) // strengthened
            : _Value(_STD move(_First._Current)), _Bound(_STD move(_Bound_from(_Last))) {}

        _NODISCARD constexpr _It begin() const noexcept(is_nothrow_copy_constructible_v<_Wi>) /* strengthened */ {
            return _It{_Value};
        }

        _NODISCARD constexpr _Se end() const noexcept(is_nothrow_copy_constructible_v<_Bo>) /* strengthened */ {
            if constexpr (same_as<_Wi, _Bo>) {
                return _It{_Bound};
            } else if constexpr (same_as<_Bo, unreachable_sentinel_t>) {
                return unreachable_sentinel;
            } else {
                return _Se{_Bound};
            }
        }

        // clang-format off
        _NODISCARD constexpr auto size() const noexcept(noexcept(_Bound - _Value)) /* strengthened */
            requires (same_as<_Wi, _Bo> && _Advanceable<_Wi>)
                || (_Integer_like<_Wi> && _Integer_like<_Bo>)
                || sized_sentinel_for<_Bo, _Wi> {
            // clang-format on
            if constexpr (_Integer_like<_Wi> && _Integer_like<_Bo>) {
#pragma warning(suppress : 4146) // unary minus operator applied to unsigned type, result still unsigned
                return (_Value < 0) ? ((_Bound < 0) ? (_To_unsigned_like(-_Value) - _To_unsigned_like(-_Bound))
#pragma warning(suppress : 4146) // unary minus operator applied to unsigned type, result still unsigned
                                                    : (_To_unsigned_like(_Bound) + _To_unsigned_like(-_Value)))
                                    : (_To_unsigned_like(_Bound) - _To_unsigned_like(_Value));
            } else {
                return _To_unsigned_like(_Bound - _Value);
            }
        }
    };

    // clang-format off
    template <class _Wi, class _Bo>
        requires (!_Integer_like<_Wi> || !_Integer_like<_Bo>
            || (_Signed_integer_like<_Wi> == _Signed_integer_like<_Bo>))
    iota_view(_Wi, _Bo) -> iota_view<_Wi, _Bo>;
    // clang-format on

    template <class _Wi, class _Bo>
    inline constexpr bool enable_borrowed_range<iota_view<_Wi, _Bo>> = true;

    namespace views {
        struct _Iota_fn {
            template <class _Ty>
            _NODISCARD constexpr auto operator()(_Ty&& _Val) const
                noexcept(noexcept(iota_view(static_cast<_Ty&&>(_Val)))) requires requires {
                iota_view(static_cast<_Ty&&>(_Val));
            }
            { return iota_view(static_cast<_Ty&&>(_Val)); }

            template <class _Ty1, class _Ty2>
            _NODISCARD constexpr auto operator()(_Ty1&& _Val1, _Ty2&& _Val2) const noexcept(
                noexcept(iota_view(static_cast<_Ty1&&>(_Val1), static_cast<_Ty2&&>(_Val2)))) requires requires {
                iota_view(static_cast<_Ty1&&>(_Val1), static_cast<_Ty2&&>(_Val2));
            }
            { return iota_view(static_cast<_Ty1&&>(_Val1), static_cast<_Ty2&&>(_Val2)); }
        };

        inline constexpr _Iota_fn iota;
    } // namespace views

    template <class _Ty, class _Elem, class _Traits>
    concept _Stream_extractable = requires(basic_istream<_Elem, _Traits>& __is, _Ty& __t) {
        __is >> __t;
    };

    // clang-format off
    template <movable _Ty, class _Elem, class _Traits = char_traits<_Elem>>
        requires default_initializable<_Ty> && _Stream_extractable<_Ty, _Elem, _Traits>
    class basic_istream_view : public view_interface<basic_istream_view<_Ty, _Elem, _Traits>> {
        // clang-format on
    private:
        class _Iterator {
        private:
            basic_istream_view* _Parent;

        public:
            using iterator_concept = input_iterator_tag;
            using difference_type  = ptrdiff_t;
            using value_type       = _Ty;

            constexpr explicit _Iterator(basic_istream_view& _Parent_) noexcept : _Parent{_STD addressof(_Parent_)} {}

            _Iterator(const _Iterator&) = delete;
            _Iterator(_Iterator&&)      = default;

            _Iterator& operator=(const _Iterator&) = delete;
            _Iterator& operator=(_Iterator&&) = default;

            _Iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL != 0
                // Per LWG-3489
                _STL_VERIFY(
                    !_Parent->_Stream_at_end(), "cannot increment basic_istream_view iterator at end of stream");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                *_Parent->_Stream >> _Parent->_Val;
                return *this;
            }

            void operator++(int) {
                ++*this;
            }

            _NODISCARD _Ty& operator*() const noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
                // Per LWG-3489
                _STL_VERIFY(
                    !_Parent->_Stream_at_end(), "cannot dereference basic_istream_view iterator at end of stream");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Parent->_Val;
            }

            _NODISCARD_FRIEND bool operator==(const _Iterator& _Left, default_sentinel_t) noexcept /* strengthened */ {
                return _Left._Parent->_Stream_at_end();
            }
        };

        basic_istream<_Elem, _Traits>* _Stream;
        _Ty _Val = _Ty{}; // Per LWG issue submitted but unnumbered as of 2021-06-15

    public:
        constexpr explicit basic_istream_view(basic_istream<_Elem, _Traits>& _Stream_) noexcept(
            is_nothrow_default_constructible_v<_Ty>) // strengthened
            : _Stream{_STD addressof(_Stream_)} {}

        _NODISCARD constexpr auto begin() {
            *_Stream >> _Val;
            return _Iterator{*this};
        }

        _NODISCARD constexpr default_sentinel_t end() const noexcept {
            return default_sentinel;
        }

        _NODISCARD constexpr bool _Stream_at_end() const noexcept {
            return !*_Stream;
        }
    };

    template <class _Ty>
    using istream_view = basic_istream_view<_Ty, char>;
    template <class _Ty>
    using wistream_view = basic_istream_view<_Ty, wchar_t>;

    namespace views {
        template <class _Ty>
        struct _Istream_fn {
            // clang-format off
            template <class _StreamTy>
                requires derived_from<_StreamTy,
                    basic_istream<typename _StreamTy::char_type, typename _StreamTy::traits_type>>
            _NODISCARD constexpr auto operator()(_StreamTy& _Stream) const
                noexcept(is_nothrow_default_constructible_v<_Ty>) /* strengthened */ {
                // clang-format on
                return basic_istream_view<_Ty, typename _StreamTy::char_type, typename _StreamTy::traits_type>(_Stream);
            }
        };

        template <class _Ty>
        inline constexpr _Istream_fn<_Ty> istream;
    } // namespace views

    template <range _Rng>
    requires is_object_v<_Rng>
    class ref_view : public view_interface<ref_view<_Rng>> {
    private:
        _Rng* _Range;

        static void _Rvalue_poison(_Rng&);
        static void _Rvalue_poison(_Rng&&) = delete;

    public:
        // clang-format off
        template <_Not_same_as<ref_view> _OtherRng>
        constexpr ref_view(_OtherRng&& _Other) noexcept(
            noexcept(static_cast<_Rng&>(_STD forward<_OtherRng>(_Other)))) // strengthened
            requires convertible_to<_OtherRng, _Rng&> && requires {
                _Rvalue_poison(static_cast<_OtherRng&&>(_Other));
            } : _Range{_STD addressof(static_cast<_Rng&>(_STD forward<_OtherRng>(_Other)))} {}
        // clang-format on

        _NODISCARD constexpr _Rng& base() const noexcept /* strengthened */ {
            return *_Range;
        }

        _NODISCARD constexpr iterator_t<_Rng> begin() const
            noexcept(noexcept(_RANGES begin(*_Range))) /* strengthened */ {
            return _RANGES begin(*_Range);
        }

        _NODISCARD constexpr sentinel_t<_Rng> end() const noexcept(noexcept(_RANGES end(*_Range))) /* strengthened */ {
            return _RANGES end(*_Range);
        }

        _NODISCARD constexpr bool empty() const noexcept(noexcept(_RANGES empty(*_Range))) /* strengthened */
            requires _Can_empty<_Rng> {
            return _RANGES empty(*_Range);
        }

        _NODISCARD constexpr auto size() const noexcept(noexcept(_RANGES size(*_Range))) /* strengthened */
            requires sized_range<_Rng> {
            return _RANGES size(*_Range);
        }

        _NODISCARD constexpr auto data() const noexcept(noexcept(_RANGES data(*_Range))) /* strengthened */
            requires contiguous_range<_Rng> {
            return _RANGES data(*_Range);
        }
    };

    template <class _Rng>
    ref_view(_Rng&) -> ref_view<_Rng>;

    template <class _Rng>
    inline constexpr bool enable_borrowed_range<ref_view<_Rng>> = true;

    // clang-format off
    template <range _Rng>
        requires (movable<_Rng> && !_Is_initializer_list<_Rng>)
    class owning_view : public view_interface<owning_view<_Rng>> {
        // clang-format on
    private:
        _Rng _Range{};

    public:
        // clang-format off
        owning_view() requires default_initializable<_Rng> = default;
        // clang-format on

        constexpr owning_view(_Rng&& _Range_) noexcept(is_nothrow_move_constructible_v<_Rng>) // strengthened
            : _Range(_STD move(_Range_)) {}

        owning_view(owning_view&&) = default;
        owning_view& operator=(owning_view&&) = default;

        _NODISCARD constexpr _Rng& base() & noexcept {
            return _Range;
        }
        _NODISCARD constexpr const _Rng& base() const& noexcept {
            return _Range;
        }
        _NODISCARD constexpr _Rng&& base() && noexcept {
            return _STD move(_Range);
        }
        _NODISCARD constexpr const _Rng&& base() const&& noexcept {
            return _STD move(_Range);
        }

        _NODISCARD constexpr iterator_t<_Rng> begin() noexcept(noexcept(_RANGES begin(_Range))) /* strengthened */ {
            return _RANGES begin(_Range);
        }

        _NODISCARD constexpr sentinel_t<_Rng> end() noexcept(noexcept(_RANGES end(_Range))) /* strengthened */ {
            return _RANGES end(_Range);
        }

        _NODISCARD constexpr auto begin() const
            noexcept(noexcept(_RANGES begin(_Range))) /* strengthened */ requires range<const _Rng> {
            return _RANGES begin(_Range);
        }

        _NODISCARD constexpr auto end() const
            noexcept(noexcept(_RANGES end(_Range))) /* strengthened */ requires range<const _Rng> {
            return _RANGES end(_Range);
        }

        _NODISCARD constexpr bool empty() noexcept(noexcept(_RANGES empty(_Range))) /* strengthened */
            requires _Can_empty<_Rng> {
            return _RANGES empty(_Range);
        }
        _NODISCARD constexpr bool empty() const noexcept(noexcept(_RANGES empty(_Range))) /* strengthened */
            requires _Can_empty<const _Rng> {
            return _RANGES empty(_Range);
        }

        _NODISCARD constexpr auto size() noexcept(noexcept(_RANGES size(_Range))) /* strengthened */
            requires sized_range<_Rng> {
            return _RANGES size(_Range);
        }
        _NODISCARD constexpr auto size() const noexcept(noexcept(_RANGES size(_Range))) /* strengthened */
            requires sized_range<const _Rng> {
            return _RANGES size(_Range);
        }

        _NODISCARD constexpr auto data() noexcept(noexcept(_RANGES data(_Range))) /* strengthened */
            requires contiguous_range<_Rng> {
            return _RANGES data(_Range);
        }
        _NODISCARD constexpr auto data() const noexcept(noexcept(_RANGES data(_Range))) /* strengthened */
            requires contiguous_range<const _Rng> {
            return _RANGES data(_Range);
        }
    };

    template <class _Rng>
    inline constexpr bool enable_borrowed_range<owning_view<_Rng>> = enable_borrowed_range<_Rng>;

    namespace views {
        template <class _Rng>
        concept _Can_ref_view = requires(_Rng&& __r) {
            ref_view{static_cast<_Rng&&>(__r)};
        };

        template <class _Rng>
        concept _Ownable = requires(_Rng&& __r) {
            owning_view{static_cast<_Rng&&>(__r)};
        };

        class _All_fn : public _Pipe::_Base<_All_fn> {
        private:
            enum class _St { _None, _View, _Ref, _Own };

            template <class _Rng>
            _NODISCARD static _CONSTEVAL _Choice_t<_St> _Choose() noexcept {
                if constexpr (view<remove_cvref_t<_Rng>>) {
                    if constexpr (constructible_from<remove_cvref_t<_Rng>, _Rng>) {
                        return {_St::_View, is_nothrow_constructible_v<remove_cvref_t<_Rng>, _Rng>};
                    }
                } else if constexpr (_Can_ref_view<_Rng>) {
                    return {_St::_Ref, noexcept(ref_view{_STD declval<_Rng>()})};
                } else if constexpr (_Ownable<_Rng>) {
                    return {_St::_Own, noexcept(owning_view{_STD declval<_Rng>()})};
                }

                return {_St::_None};
            }

            template <class _Rng>
            static constexpr _Choice_t<_St> _Choice = _Choose<_Rng>();

        public:
            // clang-format off
            template <viewable_range _Rng>
                requires (_Choice<_Rng>._Strategy != _St::_None)
            _NODISCARD constexpr auto operator()(_Rng&& _Range) const noexcept(_Choice<_Rng>._No_throw) {
                // clang-format on
                constexpr _St _Strat = _Choice<_Rng>._Strategy;

                if constexpr (_Strat == _St::_View) {
                    return _STD forward<_Rng>(_Range);
                } else if constexpr (_Strat == _St::_Ref) {
                    return ref_view{_STD forward<_Rng>(_Range)};
                } else if constexpr (_Strat == _St::_Own) {
                    return owning_view{_STD forward<_Rng>(_Range)};
                } else {
                    static_assert(_Always_false<_Rng>, "Should be unreachable");
                }
            }
        };

        inline constexpr _All_fn all;

        template <viewable_range _Rng>
        using all_t = decltype(all(_STD declval<_Rng>()));
    } // namespace views

    // clang-format off
    template <input_range _Vw, indirect_unary_predicate<iterator_t<_Vw>> _Pr>
        requires view<_Vw> && is_object_v<_Pr>
    class filter_view : public _Cached_position_t<forward_range<_Vw>, _Vw, filter_view<_Vw, _Pr>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        /* [[no_unique_address]] */ _Copyable_box<_Pr> _Pred{};

        template <class _View>
        struct _Category_base {};
        // clang-format off
        template <forward_range _View>
        struct _Category_base<_View> {
            // clang-format on
            using iterator_category =
                conditional_t<derived_from<_Iter_cat_t<iterator_t<_View>>, bidirectional_iterator_tag>,
                    bidirectional_iterator_tag,
                    conditional_t<derived_from<_Iter_cat_t<iterator_t<_View>>, forward_iterator_tag>,
                        forward_iterator_tag, _Iter_cat_t<iterator_t<_View>>>>;
        };

        class _Iterator : public _Category_base<_Vw> {
        private:
            /* [[no_unique_address]] */ iterator_t<_Vw> _Current{};
            filter_view* _Parent{};

#if _ITERATOR_DEBUG_LEVEL != 0
            constexpr void _Check_dereference() const noexcept {
                _STL_VERIFY(_Parent != nullptr, "cannot dereference value-initialized filter_view iterator");
                _STL_VERIFY(_Current != _RANGES end(_Parent->_Range), "cannot dereference end filter_view iterator");
            }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        public:
            using iterator_concept = conditional_t<bidirectional_range<_Vw>, bidirectional_iterator_tag,
                conditional_t<forward_range<_Vw>, forward_iterator_tag, input_iterator_tag>>;
            using value_type       = range_value_t<_Vw>;
            using difference_type  = range_difference_t<_Vw>;

            _Iterator() requires default_initializable<iterator_t<_Vw>>
            = default;

            constexpr _Iterator(filter_view& _Parent_, iterator_t<_Vw> _Current_) noexcept(
                is_nothrow_move_constructible_v<iterator_t<_Vw>>) // strengthened
                : _Current(_STD move(_Current_)), _Parent{_STD addressof(_Parent_)} {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Adl_verify_range(_Current, _RANGES end(_Parent_._Range));
                if constexpr (forward_range<_Vw>) {
                    _Adl_verify_range(_RANGES begin(_Parent_._Range), _Current);
                }
#endif // _ITERATOR_DEBUG_LEVEL != 0
            }

            _NODISCARD constexpr const iterator_t<_Vw>& base() const& noexcept /* strengthened */ {
                return _Current;
            }
            _NODISCARD constexpr iterator_t<_Vw> base() && noexcept(
                is_nothrow_move_constructible_v<iterator_t<_Vw>>) /* strengthened */ {
                return _STD move(_Current);
            }

            _NODISCARD constexpr range_reference_t<_Vw> operator*() const
                noexcept(noexcept(*_Current)) /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return *_Current;
            }

            // clang-format off
            _NODISCARD constexpr iterator_t<_Vw> operator->() const
                noexcept(is_nothrow_copy_constructible_v<iterator_t<_Vw>>) /* strengthened */
                requires _Has_arrow<iterator_t<_Vw>> && copyable<iterator_t<_Vw>> {
                // clang-format on
#if _ITERATOR_DEBUG_LEVEL != 0
                _Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Current;
            }

            constexpr _Iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL != 0
                _STL_VERIFY(_Parent != nullptr, "cannot increment value-initialized filter_view iterator");
                _STL_VERIFY(_Current != _RANGES end(_Parent->_Range), "cannot increment filter_view iterator past end");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _Current =
                    _RANGES find_if(_STD move(++_Current), _RANGES end(_Parent->_Range), _STD ref(*_Parent->_Pred));
                return *this;
            }

            constexpr decltype(auto) operator++(int) {
                if constexpr (forward_range<_Vw>) {
                    auto _Tmp = *this;
                    ++*this;
                    return _Tmp;
                } else {
                    ++*this;
                }
            }

            constexpr _Iterator& operator--() requires bidirectional_range<_Vw> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _STL_VERIFY(_Parent != nullptr, "cannot decrement value-initialized filter_view iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                do {
#if _ITERATOR_DEBUG_LEVEL != 0
                    _STL_VERIFY(_Current != _RANGES begin(_Parent->_Range),
                        "cannot decrement filter_view iterator before begin");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                    --_Current;
                } while (!_STD invoke(*_Parent->_Pred, *_Current));
                return *this;
            }

            constexpr _Iterator operator--(int) requires bidirectional_range<_Vw> {
                auto _Tmp = *this;
                --*this;
                return _Tmp;
            }

            _NODISCARD_FRIEND constexpr bool operator==(
                const _Iterator& _Left, const _Iterator& _Right) requires equality_comparable<iterator_t<_Vw>> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _STL_VERIFY(
                    _Left._Parent == _Right._Parent, "cannot compare incompatible filter_view iterators for equality");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Left._Current == _Right._Current;
            }

            _NODISCARD_FRIEND constexpr range_rvalue_reference_t<_Vw> iter_move(const _Iterator& _It) noexcept(
                noexcept(_RANGES iter_move(_It._Current))) {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _RANGES iter_move(_It._Current);
            }

            friend constexpr void iter_swap(const _Iterator& _Left, const _Iterator& _Right) noexcept(noexcept(
                _RANGES iter_swap(_Left._Current, _Right._Current))) requires indirectly_swappable<iterator_t<_Vw>> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Check_dereference();
                _Right._Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _RANGES iter_swap(_Left._Current, _Right._Current);
            }

            _NODISCARD constexpr bool _Equal(const sentinel_t<_Vw>& _Last) const
                noexcept(noexcept(_Implicitly_convert_to<bool>(_Current == _Last))) {
                return _Current == _Last;
            }
        };

        class _Sentinel {
        private:
            /* [[no_unique_address]] */ sentinel_t<_Vw> _Last{};

        public:
            _Sentinel() = default;
            constexpr explicit _Sentinel(filter_view& _Parent) noexcept(
                noexcept(_RANGES end(_Parent._Range))
                && is_nothrow_move_constructible_v<sentinel_t<_Vw>>) // strengthened
                : _Last(_RANGES end(_Parent._Range)) {}

            _NODISCARD constexpr sentinel_t<_Vw> base() const
                noexcept(is_nothrow_copy_constructible_v<sentinel_t<_Vw>>) /* strengthened */ {
                return _Last;
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _It, const _Sentinel& _Se) noexcept(
                noexcept(_It._Equal(_Se._Last))) /* strengthened */ {
                return _It._Equal(_Se._Last);
            }
        };

    public:
        // clang-format off
        filter_view() requires default_initializable<_Vw> && default_initializable<_Pr> = default;
        // clang-format on

        constexpr filter_view(_Vw _Range_, _Pr _Pred_) noexcept(
            is_nothrow_move_constructible_v<_Vw>&& is_nothrow_move_constructible_v<_Pr>) // strengthened
            : _Range(_STD move(_Range_)), _Pred{in_place, _STD move(_Pred_)} {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr const _Pr& pred() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Pred, "filter_view has no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return *_Pred;
        }

        _NODISCARD constexpr _Iterator begin() {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(
                _Pred, "N4861 [range.filter.view]/3 forbids calling begin on a filter_view that holds no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            if constexpr (forward_range<_Vw>) {
                if (this->_Has_cache()) {
                    return _Iterator{*this, this->_Get_cache(_Range)};
                }
            }

            auto _First = _RANGES find_if(_Range, _STD ref(*_Pred));
            if constexpr (forward_range<_Vw>) {
                this->_Set_cache(_Range, _First);
            }

            return _Iterator{*this, _STD move(_First)};
        }

        _NODISCARD constexpr auto end() {
            if constexpr (common_range<_Vw>) {
                return _Iterator{*this, _RANGES end(_Range)};
            } else {
                return _Sentinel{*this};
            }
        }
    };

    template <class _Rng, class _Pr>
    filter_view(_Rng&&, _Pr) -> filter_view<views::all_t<_Rng>, _Pr>;

    namespace views {
        struct _Filter_fn {
            // clang-format off
            template <viewable_range _Rng, class _Pr>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, _Pr&& _Pred) const noexcept(noexcept(
                filter_view(_STD forward<_Rng>(_Range), _STD forward<_Pr>(_Pred)))) requires requires {
                filter_view(static_cast<_Rng&&>(_Range), _STD forward<_Pr>(_Pred));
            } {
                // clang-format on
                return filter_view(_STD forward<_Rng>(_Range), _STD forward<_Pr>(_Pred));
            }

            // clang-format off
            template <class _Pr>
                requires constructible_from<decay_t<_Pr>, _Pr>
            _NODISCARD constexpr auto operator()(_Pr&& _Pred) const
                noexcept(is_nothrow_constructible_v<decay_t<_Pr>, _Pr>) {
                // clang-format on
                return _Range_closure<_Filter_fn, decay_t<_Pr>>{_STD forward<_Pr>(_Pred)};
            }
        };

        inline constexpr _Filter_fn filter;
    } // namespace views

    // clang-format off
    template <class _Rng, class _Fn> // TRANSITION, LLVM-47414
    concept _Can_const_transform = range<const _Rng> && regular_invocable<const _Fn&, range_reference_t<const _Rng>>;

#if _ITERATOR_DEBUG_LEVEL == 0
#define _NOEXCEPT_IDL0(...) noexcept(__VA_ARGS__)
#else
#define _NOEXCEPT_IDL0(...)
#endif // _ITERATOR_DEBUG_LEVEL == 0

    template <input_range _Vw, copy_constructible _Fn>
        requires view<_Vw> && is_object_v<_Fn>
            && regular_invocable<_Fn&, range_reference_t<_Vw>>
            && _Can_reference<invoke_result_t<_Fn&, range_reference_t<_Vw>>>
    class transform_view : public view_interface<transform_view<_Vw, _Fn>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        /* [[no_unique_address]] */ _Copyable_box<_Fn> _Fun{};

        template <bool _Const>
        class _Sentinel;

        template <class _Base>
        struct _Category_base {};
        // clang-format off
        template <forward_range _Base>
        struct _Category_base<_Base> {
            // clang-format on
            using iterator_category =
                conditional_t<is_lvalue_reference_v<invoke_result_t<_Fn&, range_reference_t<_Base>>>,
                    conditional_t<derived_from<_Iter_cat_t<iterator_t<_Base>>, contiguous_iterator_tag>,
                        random_access_iterator_tag, _Iter_cat_t<iterator_t<_Base>>>,
                    input_iterator_tag>;
        };

        template <bool _Const>
        class _Iterator : public _Category_base<_Maybe_const<_Const, _Vw>> {
        private:
            template <bool>
            friend class _Iterator;
            template <bool>
            friend class _Sentinel;

            using _Parent_t = _Maybe_const<_Const, transform_view>;
            using _Base     = _Maybe_const<_Const, _Vw>;

            iterator_t<_Base> _Current{};
            _Parent_t* _Parent{};

#if _ITERATOR_DEBUG_LEVEL != 0
            constexpr void _Check_dereference() const noexcept {
                _STL_VERIFY(_Parent != nullptr, "cannot dereference value-initialized transform_view iterator");
                _STL_VERIFY(_Current != _RANGES end(_Parent->_Range), "cannot dereference end transform_view iterator");
            }
#endif // _ITERATOR_DEBUG_LEVEL != 0

#if _ITERATOR_DEBUG_LEVEL != 0
            constexpr void _Same_range(const _Iterator& _Right) const noexcept {
                _STL_VERIFY(_Parent == _Right._Parent, "cannot compare incompatible transform_view iterators");
            }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        public:
            using iterator_concept = conditional_t<random_access_range<_Base>, random_access_iterator_tag,
                conditional_t<bidirectional_range<_Base>, bidirectional_iterator_tag,
                    conditional_t<forward_range<_Base>, forward_iterator_tag, input_iterator_tag>>>;
            using value_type       = remove_cvref_t<invoke_result_t<_Fn&, range_reference_t<_Base>>>;
            using difference_type  = range_difference_t<_Base>;

            _Iterator() requires default_initializable<iterator_t<_Base>>
            = default;

            constexpr _Iterator(_Parent_t& _Parent_, iterator_t<_Base> _Current_) noexcept(
                is_nothrow_move_constructible_v<iterator_t<_Base>>) // strengthened
                : _Current{_STD move(_Current_)}, _Parent{_STD addressof(_Parent_)} {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Adl_verify_range(_Current, _RANGES end(_Parent_._Range));
                if constexpr (forward_range<_Base>) {
                    _Adl_verify_range(_RANGES begin(_Parent_._Range), _Current);
                }
#endif // _ITERATOR_DEBUG_LEVEL != 0
            }

            // clang-format off
            constexpr _Iterator(_Iterator<!_Const> _It)
                noexcept(is_nothrow_constructible_v<iterator_t<_Base>, iterator_t<_Vw>>) // strengthened
                requires _Const && convertible_to<iterator_t<_Vw>, iterator_t<_Base>>
                : _Current{_STD move(_It._Current)}, _Parent{_It._Parent} {}
            // clang-format on

            _NODISCARD constexpr const iterator_t<_Base>& base() const& noexcept /* strengthened */ {
                return _Current;
            }
            _NODISCARD constexpr iterator_t<_Base> base() && noexcept(
                is_nothrow_move_constructible_v<iterator_t<_Base>>) /* strengthened */ {
                return _STD move(_Current);
            }

            _NODISCARD constexpr decltype(auto) operator*() const
                noexcept(noexcept(_STD invoke(*_Parent->_Fun, *_Current))) {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Check_dereference();
                _STL_VERIFY(
                    _Parent->_Fun, "Cannot dereference iterator into transform_view with no transformation function");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _STD invoke(*_Parent->_Fun, *_Current);
            }

            constexpr _Iterator& operator++() noexcept(noexcept(++_Current)) /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
                _STL_VERIFY(_Parent != nullptr, "Cannot increment value-initialized transform_view iterator");
                _STL_VERIFY(
                    _Current != _RANGES end(_Parent->_Range), "Cannot increment transform_view iterator past end");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                ++_Current;
                return *this;
            }

            constexpr decltype(auto) operator++(int) noexcept(
                noexcept(++_Current)
                && (!forward_range<_Base> || is_nothrow_copy_constructible_v<iterator_t<_Base>>) ) /* strengthened */ {
                if constexpr (forward_range<_Base>) {
                    auto _Tmp = *this;
                    ++*this;
                    return _Tmp;
                } else {
                    ++*this;
                }
            }

            constexpr _Iterator& operator--() noexcept(
                noexcept(--_Current)) /* strengthened */ requires bidirectional_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _STL_VERIFY(_Parent != nullptr, "Cannot decrement value-initialized transform_view iterator");
                if constexpr (forward_range<_Vw>) {
                    _STL_VERIFY(_Current != _RANGES begin(_Parent->_Range),
                        "Cannot decrement transform_view iterator before begin");
                }
#endif // _ITERATOR_DEBUG_LEVEL != 0
                --_Current;
                return *this;
            }
            constexpr _Iterator operator--(int) noexcept(
                noexcept(--_Current) && is_nothrow_copy_constructible_v<iterator_t<_Base>>) /* strengthened */
                requires bidirectional_range<_Base> {
                auto _Tmp = *this;
                --*this;
                return _Tmp;
            }

            constexpr void _Verify_offset(const difference_type _Off) const requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL == 0
                (void) _Off;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 / _ITERATOR_DEBUG_LEVEL != 0 vvv
                _STL_VERIFY(_Off == 0 || _Parent, "cannot seek value-initialized transform_view iterator");

                if constexpr (_Offset_verifiable_v<iterator_t<_Base>>) {
                    _Current._Verify_offset(_Off);
                } else {
                    if (_Off < 0) {
                        if constexpr (sized_sentinel_for<iterator_t<_Base>, iterator_t<_Base>>) {
                            _STL_VERIFY(_Off >= _RANGES begin(_Parent->_Range) - _Current,
                                "cannot seek transform_view iterator before begin");
                        }
                    } else if (_Off > 0) {
                        if constexpr (sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>) {
                            _STL_VERIFY(_Off <= _RANGES end(_Parent->_Range) - _Current,
                                "cannot seek transform_view iterator after end");
                        } else if constexpr (sized_sentinel_for<iterator_t<_Base>,
                                                 iterator_t<_Base>> && sized_range<_Base>) {
                            const auto _Size = _RANGES distance(_Parent->_Range);
                            _STL_VERIFY(_Off <= _Size - (_Current - _RANGES begin(_Parent->_Range)),
                                "cannot seek transform_view iterator after end");
                        }
                    }
                }
#endif // _ITERATOR_DEBUG_LEVEL == 0
            }

            constexpr _Iterator& operator+=(const difference_type _Off)
                _NOEXCEPT_IDL0(noexcept(_Current += _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _Current += _Off;
                return *this;
            }
            constexpr _Iterator& operator-=(const difference_type _Off)
                _NOEXCEPT_IDL0(noexcept(_Current -= _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(-_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _Current -= _Off;
                return *this;
            }

            _NODISCARD constexpr decltype(auto) operator[](const difference_type _Idx) const
                _NOEXCEPT_IDL0(noexcept(_STD invoke(*_Parent->_Fun, _Current[_Idx]))) /* strengthened */
                requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(_Idx);
                _STL_VERIFY(
                    _Parent->_Fun, "Cannot dereference iterator into transform_view with no transformation function");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _STD invoke(*_Parent->_Fun, _Current[_Idx]);
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current
                         == _Right._Current)) /* strengthened */ requires equality_comparable<iterator_t<_Base>> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Same_range(_Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Left._Current == _Right._Current;
            }

            _NODISCARD_FRIEND constexpr bool operator<(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Same_range(_Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Left._Current < _Right._Current;
            }
            _NODISCARD_FRIEND constexpr bool operator>(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return _Right < _Left;
            }
            _NODISCARD_FRIEND constexpr bool operator<=(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return !(_Right < _Left);
            }
            _NODISCARD_FRIEND constexpr bool operator>=(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return !(_Left < _Right);
            }
            // clang-format off
            _NODISCARD_FRIEND constexpr auto operator<=>(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current <=> _Right._Current)) /* strengthened */
                requires random_access_range<_Base> && three_way_comparable<iterator_t<_Base>> {
                // clang-format on
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Same_range(_Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Left._Current <=> _Right._Current;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator+(_Iterator _It, difference_type _Off)
                _NOEXCEPT_IDL0(noexcept(_It._Current += _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Verify_offset(_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _It._Current += _Off;
                return _It;
            }
            _NODISCARD_FRIEND constexpr _Iterator operator+(difference_type _Off, _Iterator _It)
                _NOEXCEPT_IDL0(noexcept(_It._Current += _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Verify_offset(_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _It._Current += _Off;
                return _It;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator-(_Iterator _It, difference_type _Off)
                _NOEXCEPT_IDL0(noexcept(_It._Current -= _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Verify_offset(-_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _It._Current -= _Off;
                return _It;
            }

            _NODISCARD_FRIEND constexpr difference_type operator-(const _Iterator& _Left,
                const _Iterator& _Right) noexcept(noexcept(_Left._Current - _Right._Current)) /* strengthened */
                requires sized_sentinel_for<iterator_t<_Base>, iterator_t<_Base>> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Same_range(_Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Left._Current - _Right._Current;
            }
        };

        template <bool _Const>
        class _Sentinel {
        private:
            template <bool>
            friend class _Sentinel;

            using _Parent_t = _Maybe_const<_Const, transform_view>;
            using _Base     = _Maybe_const<_Const, _Vw>;
            template <bool _OtherConst>
            using _Maybe_const_iter = iterator_t<_Maybe_const<_OtherConst, _Vw>>;

            sentinel_t<_Base> _Last{};

            template <bool _OtherConst>
            _NODISCARD static constexpr const _Maybe_const_iter<_OtherConst>& _Get_current(
                const _Iterator<_OtherConst>& _It) noexcept {
#if _ITERATOR_DEBUG_LEVEL != 0
                _STL_VERIFY(
                    _It._Parent != nullptr, "cannot compare transform_view sentinel with value-initialized iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _It._Current;
            }

        public:
            _Sentinel() = default;
            constexpr explicit _Sentinel(sentinel_t<_Base> _Last_) noexcept(
                is_nothrow_move_constructible_v<sentinel_t<_Base>>) // strengthened
                : _Last(_STD move(_Last_)) {}

            // clang-format off
            constexpr _Sentinel(_Sentinel<!_Const> _Se)
                noexcept(is_nothrow_constructible_v<sentinel_t<_Base>, sentinel_t<_Vw>>) // strengthened
                requires _Const && convertible_to<sentinel_t<_Vw>, sentinel_t<_Base>>
                : _Last(_STD move(_Se._Last)) {}
            // clang-format on

            _NODISCARD constexpr sentinel_t<_Base> base() const
                noexcept(is_nothrow_copy_constructible_v<sentinel_t<_Base>>) /* strengthened */ {
                return _Last;
            }

            // clang-format off
            template <bool _OtherConst>
                requires sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator<_OtherConst>& _Left,
                const _Sentinel& _Right) noexcept(noexcept(_Get_current(_Left) == _Right._Last)) /* strengthened */ {
                return _Get_current(_Left) == _Right._Last;
            }

            template <bool _OtherConst>
                requires sized_sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr range_difference_t<_Maybe_const<_OtherConst, _Vw>>
                operator-(const _Iterator<_OtherConst>& _Left, const _Sentinel& _Right) noexcept(
                    noexcept(_Get_current(_Left) - _Right._Last)) /* strengthened */ {
                return _Get_current(_Left) - _Right._Last;
            }

            template <bool _OtherConst>
                requires sized_sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr range_difference_t<_Maybe_const<_OtherConst, _Vw>>
                operator-(const _Sentinel& _Left, const _Iterator<_OtherConst>& _Right) noexcept(
                    noexcept(_Left._Last - _Get_current(_Right))) /* strengthened */ {
                return _Left._Last - _Get_current(_Right);
            }
        };

    public:
        transform_view() requires default_initializable<_Vw> && default_initializable<_Fn> = default;
        // clang-format on

        constexpr transform_view(_Vw _Range_, _Fn _Fun_) noexcept(
            is_nothrow_move_constructible_v<_Vw>&& is_nothrow_move_constructible_v<_Fn>) // strengthened
            : _Range(_STD move(_Range_)), _Fun{in_place, _STD move(_Fun_)} {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr _Iterator<false> begin() noexcept(
            noexcept(_RANGES begin(_Range)) && is_nothrow_move_constructible_v<iterator_t<_Vw>>) /* strengthened */ {
            return _Iterator<false>{*this, _RANGES begin(_Range)};
        }

        // clang-format off
        _NODISCARD constexpr _Iterator<true> begin() const noexcept(noexcept(
            _RANGES begin(_Range)) && is_nothrow_move_constructible_v<iterator_t<_Vw>>) /* strengthened */
#ifdef __clang__ // TRANSITION, LLVM-47414
            requires _Can_const_transform<_Vw, _Fn>
#else // ^^^ workaround / no workaround vvv
            requires range<const _Vw> && regular_invocable<const _Fn&, range_reference_t<const _Vw>>
#endif // TRANSITION, LLVM-47414
        {
            // clang-format on
            return _Iterator<true>{*this, _RANGES begin(_Range)};
        }

        // clang-format off
        _NODISCARD constexpr auto end() noexcept(noexcept(
            _RANGES end(_Range)) && is_nothrow_move_constructible_v<decltype(_RANGES end(_Range))>) /* strengthened */ {
            // clang-format on
            if constexpr (common_range<_Vw>) {
                return _Iterator<false>{*this, _RANGES end(_Range)};
            } else {
                return _Sentinel<false>{_RANGES end(_Range)};
            }
        }

        // clang-format off
        _NODISCARD constexpr auto end() const noexcept(noexcept(
            _RANGES end(_Range)) && is_nothrow_move_constructible_v<decltype(_RANGES end(_Range))>) /* strengthened */
#ifdef __clang__ // TRANSITION, LLVM-47414
            requires _Can_const_transform<_Vw, _Fn>
#else // ^^^ workaround / no workaround vvv
            requires range<const _Vw> && regular_invocable<const _Fn&, range_reference_t<const _Vw>>
#endif // TRANSITION, LLVM-47414
        {
            // clang-format on
            if constexpr (common_range<_Vw>) {
                return _Iterator<true>{*this, _RANGES end(_Range)};
            } else {
                return _Sentinel<true>{_RANGES end(_Range)};
            }
        }

        _NODISCARD constexpr auto size() noexcept(
            noexcept(_RANGES size(_Range))) /* strengthened */ requires sized_range<_Vw> {
            return _RANGES size(_Range);
        }
        _NODISCARD constexpr auto size() const
            noexcept(noexcept(_RANGES size(_Range))) /* strengthened */ requires sized_range<const _Vw> {
            return _RANGES size(_Range);
        }
    };

#undef _NOEXCEPT_IDL0

    template <class _Rng, class _Fn>
    transform_view(_Rng&&, _Fn) -> transform_view<views::all_t<_Rng>, _Fn>;

    namespace views {
        struct _Transform_fn {
            // clang-format off
            template <viewable_range _Rng, class _Fn>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, _Fn _Fun) const noexcept(noexcept(
                transform_view(_STD forward<_Rng>(_Range), _STD move(_Fun)))) requires requires {
                transform_view(static_cast<_Rng&&>(_Range), _STD move(_Fun));
            } {
                // clang-format on
                return transform_view(_STD forward<_Rng>(_Range), _STD move(_Fun));
            }

            // clang-format off
            template <class _Fn>
                requires constructible_from<decay_t<_Fn>, _Fn>
            _NODISCARD constexpr auto operator()(_Fn&& _Fun) const
                noexcept(is_nothrow_constructible_v<decay_t<_Fn>, _Fn>) {
                // clang-format on
                return _Range_closure<_Transform_fn, decay_t<_Fn>>{_STD forward<_Fn>(_Fun)};
            }
        };

        inline constexpr _Transform_fn transform;
    } // namespace views

    template <view _Vw>
    class take_view : public view_interface<take_view<_Vw>> {
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        range_difference_t<_Vw> _Count = 0;

        template <bool _Const, bool _Wrapped = true>
        class _Sentinel {
        private:
            template <bool, bool>
            friend class _Sentinel;

            using _Base_t        = _Maybe_const<_Const, _Vw>;
            using _Base_sentinel = _Maybe_wrapped<_Wrapped, sentinel_t<_Base_t>>;
            template <bool _OtherConst>
            using _Base_iterator = _Maybe_wrapped<_Wrapped, iterator_t<_Maybe_const<_OtherConst, _Vw>>>;
            template <bool _OtherConst>
            using _Counted_iter = counted_iterator<_Base_iterator<_OtherConst>>;

            _Base_sentinel _Last{};

        public:
            _Sentinel() = default;

            constexpr explicit _Sentinel(_Base_sentinel _Last_) noexcept(
                is_nothrow_move_constructible_v<_Base_sentinel>) // strengthened
                : _Last(_STD move(_Last_)) {}

            // clang-format off
            constexpr _Sentinel(_Sentinel<!_Const, _Wrapped> _That) noexcept(
                is_nothrow_constructible_v<_Base_sentinel, _Maybe_wrapped<_Wrapped, sentinel_t<_Vw>>>) // strengthened
                requires _Const && convertible_to<_Maybe_wrapped<_Wrapped, sentinel_t<_Vw>>, _Base_sentinel>
                : _Last(_STD move(_That._Last)) {}
            // clang-format on

            _NODISCARD constexpr _Base_sentinel base() const
                noexcept(is_nothrow_copy_constructible_v<_Base_sentinel>) /* strengthened */ {
                return _Last;
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Counted_iter<_Const>& _Left, const _Sentinel& _Right) {
                return _Left.count() == 0 || _Left.base() == _Right._Last;
            }

            // clang-format off
            template <bool _OtherConst = !_Const>
                requires sentinel_for<_Base_sentinel, _Base_iterator<_OtherConst>>
            _NODISCARD_FRIEND constexpr bool operator==(
                const _Counted_iter<_OtherConst>& _Left, const _Sentinel& _Right) {
                // clang-format on
                return _Left.count() == 0 || _Left.base() == _Right._Last;
            }

            using _Prevent_inheriting_unwrap = _Sentinel;

            // clang-format off
            _NODISCARD constexpr auto _Unwrapped() const&
                requires _Wrapped && _Unwrappable_v<const iterator_t<_Base_t>&> {
                // clang-format on
                return _Sentinel<_Const, false>{_Get_unwrapped(_Last)};
            }
            // clang-format off
            _NODISCARD constexpr auto _Unwrapped() && requires _Wrapped && _Unwrappable_v<iterator_t<_Base_t>> {
                // clang-format on
                return _Sentinel<_Const, false>{_Get_unwrapped(_STD move(_Last))};
            }

            static constexpr bool _Unwrap_when_unverified = _Do_unwrap_when_unverified_v<iterator_t<_Base_t>>;

            constexpr void _Seek_to(const _Sentinel<_Const, false>& _That) requires _Wrapped {
                _Seek_wrapped(_Last, _That._Last);
            }
            constexpr void _Seek_to(_Sentinel<_Const, false>&& _That) requires _Wrapped {
                _Seek_wrapped(_Last, _STD move(_That._Last));
            }
        };

    public:
        take_view() requires default_initializable<_Vw>
        = default;

        constexpr take_view(_Vw _Range_, const range_difference_t<_Vw> _Count_) noexcept(
            is_nothrow_move_constructible_v<_Vw>) // strengthened
            : _Range(_STD move(_Range_)), _Count{_Count_} {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        // clang-format off
        _NODISCARD constexpr auto begin() requires (!_Simple_view<_Vw>) {
            // clang-format on
            if constexpr (sized_range<_Vw>) {
                if constexpr (random_access_range<_Vw>) {
                    return _RANGES begin(_Range);
                } else {
                    const auto _Size = static_cast<range_difference_t<_Vw>>(size());
                    return counted_iterator(_RANGES begin(_Range), _Size);
                }
            } else {
                return counted_iterator(_RANGES begin(_Range), _Count);
            }
        }

        _NODISCARD constexpr auto begin() const requires range<const _Vw> {
            if constexpr (sized_range<const _Vw>) {
                if constexpr (random_access_range<const _Vw>) {
                    return _RANGES begin(_Range);
                } else {
                    const auto _Size = static_cast<range_difference_t<_Vw>>(size());
                    return counted_iterator(_RANGES begin(_Range), _Size);
                }
            } else {
                return counted_iterator(_RANGES begin(_Range), _Count);
            }
        }

        // clang-format off
        _NODISCARD constexpr auto end() requires (!_Simple_view<_Vw>) {
            // clang-format on
            if constexpr (sized_range<_Vw>) {
                if constexpr (random_access_range<_Vw>) {
                    return _RANGES begin(_Range) + static_cast<range_difference_t<_Vw>>(size());
                } else {
                    return default_sentinel;
                }
            } else {
                return _Sentinel<false>{_RANGES end(_Range)};
            }
        }

        _NODISCARD constexpr auto end() const requires range<const _Vw> {
            if constexpr (sized_range<const _Vw>) {
                if constexpr (random_access_range<const _Vw>) {
                    return _RANGES begin(_Range) + static_cast<range_difference_t<_Vw>>(size());
                } else {
                    return default_sentinel;
                }
            } else {
                return _Sentinel<true>{_RANGES end(_Range)};
            }
        }

        _NODISCARD constexpr auto size() requires sized_range<_Vw> {
            const auto _Length = _RANGES size(_Range);
            return (_STD min)(_Length, static_cast<decltype(_Length)>(_Count));
        }

        _NODISCARD constexpr auto size() const requires sized_range<const _Vw> {
            const auto _Length = _RANGES size(_Range);
            return (_STD min)(_Length, static_cast<decltype(_Length)>(_Count));
        }
    };

    template <class _Rng>
    take_view(_Rng&&, range_difference_t<_Rng>) -> take_view<views::all_t<_Rng>>;

    template <class _Rng>
    inline constexpr bool enable_borrowed_range<take_view<_Rng>> = enable_borrowed_range<_Rng>;

    namespace views {
        template <class>
        inline constexpr bool _Is_subrange = false;
        template <class _It, class _Se, subrange_kind _Ki>
        inline constexpr bool _Is_subrange<subrange<_It, _Se, _Ki>> = true;

        // clang-format off
        template <class _Rng>
        concept _Random_sized_range = random_access_range<_Rng> && sized_range<_Rng>;
        // clang-format on

        class _Take_fn {
        private:
            enum class _St {
                _Empty,
                _Reconstruct_span,
                _Reconstruct_string_view,
                _Reconstruct_iota_view,
                _Reconstruct_subrange,
                _Take_view
            };

            template <class _Rng>
            _NODISCARD static _CONSTEVAL _Choice_t<_St> _Choose() noexcept {
                using _Ty = remove_cvref_t<_Rng>;

                if constexpr (_Is_specialization_v<_Ty, empty_view>) {
                    return {_St::_Empty, true};
                } else if constexpr (_Is_span_v<_Ty>) {
                    return {_St::_Reconstruct_span, true};
                } else if constexpr (_Is_specialization_v<_Ty, basic_string_view>) {
                    return {_St::_Reconstruct_string_view, true};
                } else if constexpr (_Random_sized_range<_Ty> && _Is_specialization_v<_Ty, iota_view>) {
                    return {_St::_Reconstruct_iota_view,
                        noexcept(_RANGES begin(_STD declval<_Rng&>()) + _RANGES distance(_STD declval<_Rng&>()))};
                } else if constexpr (_Random_sized_range<_Ty> && _Is_subrange<_Ty>) {
                    return {_St::_Reconstruct_subrange,
                        noexcept(subrange(_RANGES begin(_STD declval<_Rng&>()),
                            _RANGES begin(_STD declval<_Rng&>()) + _RANGES distance(_STD declval<_Rng&>())))};
                } else {
                    return {_St::_Take_view, noexcept(take_view(_STD declval<_Rng>(), range_difference_t<_Rng>{0}))};
                }
            }

            template <class _Rng>
            static constexpr _Choice_t<_St> _Choice = _Choose<_Rng>();

        public:
            // clang-format off
            template <viewable_range _Rng>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, range_difference_t<_Rng> _Count) const noexcept(
                _Choice<_Rng>._No_throw) {
                // clang-format on
                constexpr _St _Strat = _Choice<_Rng>._Strategy;

                if constexpr (_Strat == _St::_Empty) {
                    // it's an empty_view: return another empty view
                    return remove_cvref_t<_Rng>{};
                } else if constexpr (_Strat == _St::_Take_view) {
                    return take_view(_STD forward<_Rng>(_Range), _Count);
                } else {
                    // it's a "reconstructible range"; return the same kind of range with a restricted extent
                    _Count            = (_STD min)(_RANGES distance(_Range), _Count);
                    const auto _First = _RANGES begin(_Range);

                    // The following are all per the proposed resolution of LWG-3407
                    if constexpr (_Strat == _St::_Reconstruct_span) {
                        return span(_First, _First + _Count);
                    } else if constexpr (_Strat == _St::_Reconstruct_string_view) {
                        return remove_cvref_t<_Rng>(_First, _First + _Count);
                    } else if constexpr (_Strat == _St::_Reconstruct_iota_view) {
                        using _Vt = range_value_t<_Rng>;
                        return iota_view<_Vt, _Vt>(_First, _First + _Count);
                    } else if constexpr (_Strat == _St::_Reconstruct_subrange) {
                        return subrange(_First, _First + _Count);
                    } else {
                        static_assert(_Always_false<_Rng>, "Should be unreachable");
                    }
                }
            }

            // clang-format off
            template <class _Ty>
                requires constructible_from<decay_t<_Ty>, _Ty>
            _NODISCARD constexpr auto operator()(_Ty&& _Length) const
            noexcept(is_nothrow_constructible_v<decay_t<_Ty>, _Ty>) {
                // clang-format on
                return _Range_closure<_Take_fn, decay_t<_Ty>>{_STD forward<_Ty>(_Length)};
            }
        };

        inline constexpr _Take_fn take;
    } // namespace views

    // clang-format off
    template <view _Vw, class _Pr>
        requires input_range<_Vw> && is_object_v<_Pr> && indirect_unary_predicate<const _Pr, iterator_t<_Vw>>
    class take_while_view : public view_interface<take_while_view<_Vw, _Pr>>  {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        /* [[no_unique_address]] */ _Copyable_box<_Pr> _Pred{};

        template <bool _Const, bool _Wrapped = true>
        class _Sentinel {
        private:
            template <bool, bool>
            friend class _Sentinel;

            using _Base_t        = _Maybe_const<_Const, _Vw>;
            using _Base_iterator = _Maybe_wrapped<_Wrapped, iterator_t<_Base_t>>;
            using _Base_sentinel = _Maybe_wrapped<_Wrapped, sentinel_t<_Base_t>>;

            template <bool _OtherConst>
            using _Maybe_const_iter = _Maybe_wrapped<_Wrapped, iterator_t<_Maybe_const<_OtherConst, _Vw>>>;

            /* [[no_unique_address]] */ _Base_sentinel _Last{};
            const _Pr* _Pred = nullptr;

        public:
            _Sentinel() = default;

            constexpr explicit _Sentinel(_Base_sentinel _Last_, const _Pr* const _Pred_) noexcept(
                is_nothrow_move_constructible_v<_Base_sentinel>) // strengthened
                : _Last(_STD move(_Last_)), _Pred(_Pred_) {}

            // clang-format off
            constexpr _Sentinel(_Sentinel<!_Const, _Wrapped> _That) noexcept(
                is_nothrow_constructible_v<_Base_sentinel, _Maybe_wrapped<_Wrapped, sentinel_t<_Vw>>>) // strengthened
                requires _Const && convertible_to<_Maybe_wrapped<_Wrapped, sentinel_t<_Vw>>, _Base_sentinel>
                : _Last(_STD move(_That._Last)), _Pred(_That._Pred) {}
            // clang-format on

            _NODISCARD constexpr _Base_sentinel base() const
                noexcept(is_nothrow_copy_constructible_v<_Base_sentinel>) /* strengthened */ {
                return _Last;
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Base_iterator& _Left, const _Sentinel& _Right) {
                return _Right._Last == _Left || !_STD invoke(*_Right._Pred, *_Left);
            }

            // clang-format off
            template <bool _OtherConst = !_Const>
                requires sentinel_for<_Base_sentinel, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr bool operator==(
                const _Maybe_const_iter<_OtherConst>& _Left, const _Sentinel& _Right) {
                // clang-format on
                return _Right._Last == _Left || !_STD invoke(*_Right._Pred, *_Left);
            }

            using _Prevent_inheriting_unwrap = _Sentinel;

            // clang-format off
            _NODISCARD constexpr auto _Unwrapped() const&
                requires _Wrapped && _Unwrappable_v<const iterator_t<_Base_t>&> {
                // clang-format on
                return _Sentinel<_Const, false>{_Get_unwrapped(_Last), _Pred};
            }
            // clang-format off
            _NODISCARD constexpr auto _Unwrapped() && requires _Wrapped && _Unwrappable_v<iterator_t<_Base_t>> {
                // clang-format on
                return _Sentinel<_Const, false>{_Get_unwrapped(_STD move(_Last)), _Pred};
            }

            static constexpr bool _Unwrap_when_unverified = _Do_unwrap_when_unverified_v<iterator_t<_Base_t>>;

            constexpr void _Seek_to(const _Sentinel<_Const, false>& _That) requires _Wrapped {
                _Seek_wrapped(_Last, _That._Last);
            }
            constexpr void _Seek_to(_Sentinel<_Const, false>&& _That) requires _Wrapped {
                _Seek_wrapped(_Last, _STD move(_That._Last));
            }
        };

    public:
        // clang-format off
        take_while_view() requires default_initializable<_Vw> && default_initializable<_Pr> = default;
        // clang-format on

        constexpr take_while_view(_Vw _Range_, _Pr _Pred_) noexcept(
            is_nothrow_move_constructible_v<_Vw>&& is_nothrow_move_constructible_v<_Pr>) // strengthened
            : _Range(_STD move(_Range_)), _Pred{in_place, _STD move(_Pred_)} {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr const _Pr& pred() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Pred, "take_while_view has no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return *_Pred;
        }

        // clang-format off
        _NODISCARD constexpr auto begin() noexcept(
            noexcept(_RANGES begin(_Range))) /* strengthened */ requires (!_Simple_view<_Vw>) {
            // clang-format on
            return _RANGES begin(_Range);
        }

        // clang-format off
        _NODISCARD constexpr auto begin() const noexcept(
            noexcept(_RANGES begin(_Range))) /* strengthened */ requires range<const _Vw>
            && indirect_unary_predicate<const _Pr, iterator_t<const _Vw>> {
            // clang-format on
            return _RANGES begin(_Range);
        }

        // clang-format off
        _NODISCARD constexpr auto end() noexcept(
            noexcept(_RANGES end(_Range)) && is_nothrow_move_constructible_v<_Sentinel<false>>) /* strengthened */
            requires (!_Simple_view<_Vw>) {
            // clang-format on
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Pred, "cannot call end on a take_while_view with no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return _Sentinel<false>{_RANGES end(_Range), _STD addressof(*_Pred)};
        }

        // clang-format off
        _NODISCARD constexpr auto end() const noexcept(
            noexcept(_RANGES end(_Range)) && is_nothrow_move_constructible_v<_Sentinel<true>>) /* strengthened */
            requires range<const _Vw> && indirect_unary_predicate<const _Pr, iterator_t<const _Vw>> {
            // clang-format on
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Pred, "cannot call end on a take_while_view with no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return _Sentinel<true>{_RANGES end(_Range), _STD addressof(*_Pred)};
        }
    };

    template <class _Rng, class _Pr>
    take_while_view(_Rng&&, _Pr) -> take_while_view<views::all_t<_Rng>, _Pr>;

    namespace views {
        struct _Take_while_fn {
            template <viewable_range _Rng, class _Pr>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, _Pr _Pred) const
                noexcept(noexcept(take_while_view(_STD forward<_Rng>(_Range), _STD move(_Pred)))) requires requires {
                take_while_view(static_cast<_Rng&&>(_Range), _STD move(_Pred));
            }
            { return take_while_view(_STD forward<_Rng>(_Range), _STD move(_Pred)); }

            // clang-format off
            template <class _Pr>
                requires constructible_from<decay_t<_Pr>, _Pr>
            _NODISCARD constexpr auto operator()(_Pr&& _Pred) const
                noexcept(is_nothrow_constructible_v<decay_t<_Pr>, _Pr>) {
                // clang-format on
                return _Range_closure<_Take_while_fn, decay_t<_Pr>>{_STD forward<_Pr>(_Pred)};
            }
        };

        inline constexpr _Take_while_fn take_while;
    } // namespace views

    template <view _Vw>
    class drop_view : public _Cached_position_t<forward_range<_Vw> && !(random_access_range<_Vw> && sized_range<_Vw>),
                          _Vw, drop_view<_Vw>> {
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        range_difference_t<_Vw> _Count = 0;

        _NODISCARD constexpr auto _Find_first() {
            if constexpr (sized_range<_Vw>) {
                _STL_INTERNAL_STATIC_ASSERT(!random_access_range<_Vw>);
                auto _Offset = _RANGES distance(_Range);
                if constexpr (bidirectional_range<_Vw> && common_range<_Vw>) {
                    if (_Count >= _Offset / 2) {
                        auto _Result = _RANGES end(_Range);
                        while (_Offset > _Count) {
                            --_Offset;
                            --_Result;
                        }

                        return _Result;
                    }
                }

                if (_Offset > _Count) {
                    _Offset = _Count;
                }

                return _RANGES next(_RANGES begin(_Range), _Offset);
            } else {
                return _RANGES next(_RANGES begin(_Range), _Count, _RANGES end(_Range));
            }
        }

    public:
        drop_view() requires default_initializable<_Vw>
        = default;

        constexpr drop_view(_Vw _Range_, const range_difference_t<_Vw> _Count_) noexcept(
            is_nothrow_move_constructible_v<_Vw>) // strengthened
            : _Range(_STD move(_Range_)), _Count{_Count_} {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Count_ >= 0, "Numer of elements to drop must be non-negative (N4861 [range.drop.view]/1");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        // clang-format off
        _NODISCARD constexpr auto begin()
            requires (!(_Simple_view<_Vw> && random_access_range<const _Vw> && sized_range<const _Vw>)) {
            // clang-format on
            if constexpr (sized_range<_Vw> && random_access_range<_Vw>) {
                const auto _Offset = (_STD min)(_RANGES distance(_Range), _Count);
                return _RANGES begin(_Range) + _Offset;
            } else {
                if constexpr (forward_range<_Vw>) {
                    if (this->_Has_cache()) {
                        return this->_Get_cache(_Range);
                    }
                }

                same_as<iterator_t<_Vw>> auto _Result = _Find_first();
                if constexpr (forward_range<_Vw>) {
                    this->_Set_cache(_Range, _Result);
                }
                return _Result;
            }
        }

        // clang-format off
        _NODISCARD constexpr auto begin() const
            requires random_access_range<const _Vw> && sized_range<const _Vw> {
            // clang-format on
            const auto _Offset = (_STD min)(_RANGES distance(_Range), _Count);
            return _RANGES begin(_Range) + _Offset;
        }

        // clang-format off
        _NODISCARD constexpr auto end() requires (!_Simple_view<_Vw>) {
            // clang-format on
            return _RANGES end(_Range);
        }

        _NODISCARD constexpr auto end() const requires range<const _Vw> {
            return _RANGES end(_Range);
        }

        _NODISCARD constexpr auto size() requires sized_range<_Vw> {
            const auto _Size          = _RANGES size(_Range);
            const auto _Count_as_size = static_cast<range_size_t<_Vw>>(_Count);
            if (_Size < _Count_as_size) {
                return range_size_t<_Vw>{0};
            } else {
                return static_cast<range_size_t<_Vw>>(_Size - _Count_as_size);
            }
        }

        _NODISCARD constexpr auto size() const requires sized_range<const _Vw> {
            const auto _Size          = _RANGES size(_Range);
            const auto _Count_as_size = static_cast<range_size_t<_Vw>>(_Count);
            if (_Size < _Count_as_size) {
                return range_size_t<_Vw>{0};
            } else {
                return static_cast<range_size_t<_Vw>>(_Size - _Count_as_size);
            }
        }
    };

    template <class _Rng>
    drop_view(_Rng&&, range_difference_t<_Rng>) -> drop_view<views::all_t<_Rng>>;

    template <class _Rng>
    inline constexpr bool enable_borrowed_range<drop_view<_Rng>> = enable_borrowed_range<_Rng>;

    namespace views {
        class _Drop_fn {
        private:
            enum class _St { _Empty, _Reconstruct_span, _Reconstruct_subrange, _Reconstruct_other, _Drop_view };

            template <class _Rng>
            _NODISCARD static _CONSTEVAL _Choice_t<_St> _Choose() noexcept {
                using _Ty = remove_cvref_t<_Rng>;

                if constexpr (_Is_specialization_v<_Ty, empty_view>) {
                    return {_St::_Empty, true};
                } else if constexpr (_Is_span_v<_Ty>) {
                    return {_St::_Reconstruct_span, true};
                } else if constexpr (_Is_specialization_v<_Ty, basic_string_view>) {
                    return {_St::_Reconstruct_other, true};
                } else if constexpr (_Random_sized_range<_Ty> && _Is_subrange<_Ty>) {
                    if constexpr (sized_sentinel_for<sentinel_t<_Ty>, iterator_t<_Ty>>) {
                        return {_St::_Reconstruct_subrange,
                            noexcept(_Ty(_RANGES begin(_STD declval<_Rng&>()) + _RANGES distance(_STD declval<_Rng&>()),
                                _RANGES end(_STD declval<_Rng&>())))};
                    } else {
                        return {_St::_Reconstruct_subrange,
                            noexcept(_Ty(_RANGES begin(_STD declval<_Rng&>()) + _RANGES distance(_STD declval<_Rng&>()),
                                _RANGES end(_STD declval<_Rng&>()), range_difference_t<_Rng>{0}))};
                    }
                } else if constexpr (_Random_sized_range<_Ty> && _Is_specialization_v<_Ty, iota_view>) {
                    return {_St::_Reconstruct_other,
                        noexcept(_Ty(_RANGES begin(_STD declval<_Rng&>()) + _RANGES distance(_STD declval<_Rng&>()),
                            _RANGES end(_STD declval<_Rng&>())))};
                } else {
                    return {_St::_Drop_view, noexcept(drop_view(_STD declval<_Rng>(), range_difference_t<_Rng>{0}))};
                }
            }

            template <class _Rng>
            static constexpr _Choice_t<_St> _Choice = _Choose<_Rng>();

        public:
            // clang-format off
            template <viewable_range _Rng>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, range_difference_t<_Rng> _Count) const noexcept(
                _Choice<_Rng>._No_throw) {
                // clang-format on
                constexpr _St _Strat = _Choice<_Rng>._Strategy;

                if constexpr (_Strat == _St::_Empty) {
                    // it's an empty_view: return another empty view
                    return remove_cvref_t<_Rng>{};
                } else if constexpr (_Strat == _St::_Drop_view) {
                    return drop_view(_STD forward<_Rng>(_Range), _Count);
                } else {
                    // it's a "reconstructible range"; return the same kind of range with a restricted extent
                    _Count = (_STD min)(_RANGES distance(_Range), _Count);

                    // The following are all per the proposed resolution of LWG-3407
                    if constexpr (_Strat == _St::_Reconstruct_span) {
                        return span(_Ubegin(_Range) + _Count, _Uend(_Range));
                    } else if constexpr (_Strat == _St::_Reconstruct_subrange) {
                        if constexpr (sized_sentinel_for<sentinel_t<_Rng>, iterator_t<_Rng>>) {
                            return remove_cvref_t<_Rng>(_RANGES begin(_Range) + _Count, _RANGES end(_Range));
                        } else {
                            return remove_cvref_t<_Rng>(
                                _RANGES begin(_Range) + _Count, _RANGES end(_Range), _RANGES size(_Range) - _Count);
                        }
                    } else if constexpr (_Strat == _St::_Reconstruct_other) {
                        return remove_cvref_t<_Rng>(_RANGES begin(_Range) + _Count, _RANGES end(_Range));
                    } else {
                        static_assert(_Always_false<_Rng>, "Should be unreachable");
                    }
                }
            }

            // clang-format off
            template <class _Ty>
                requires constructible_from<decay_t<_Ty>, _Ty>
            _NODISCARD constexpr auto operator()(_Ty&& _Length) const
                noexcept(is_nothrow_constructible_v<decay_t<_Ty>, _Ty>) {
                // clang-format on
                return _Range_closure<_Drop_fn, decay_t<_Ty>>{_STD forward<_Ty>(_Length)};
            }
        };

        inline constexpr _Drop_fn drop;
    } // namespace views

    // clang-format off
    template <view _Vw, class _Pr>
        requires input_range<_Vw> && is_object_v<_Pr> && indirect_unary_predicate<const _Pr, iterator_t<_Vw>>
    class drop_while_view : public _Cached_position_t<forward_range<_Vw>, _Vw, drop_while_view<_Vw, _Pr>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        /* [[no_unique_address]] */ _Copyable_box<_Pr> _Pred{};

    public:
        // clang-format off
        drop_while_view() requires default_initializable<_Vw> && default_initializable<_Pr> = default;
        // clang-format on

        constexpr drop_while_view(_Vw _Range_, _Pr _Pred_) noexcept(
            is_nothrow_move_constructible_v<_Vw>&& is_nothrow_move_constructible_v<_Pr>) // strengthened
            : _Range(_STD move(_Range_)), _Pred{in_place, _STD move(_Pred_)} {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr const _Pr& pred() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Pred, "drop_while_view has no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return *_Pred;
        }

        _NODISCARD constexpr auto begin() {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(
                _Pred, "N4885 [range.drop.while.view] forbids calling begin on a drop_while_view with no predicate");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            if constexpr (forward_range<_Vw>) {
                if (this->_Has_cache()) {
                    return this->_Get_cache(_Range);
                }
            }

            auto _First = _RANGES find_if_not(_Range, _STD cref(*_Pred));
            if constexpr (forward_range<_Vw>) {
                this->_Set_cache(_Range, _First);
            }

            return _First;
        }

        _NODISCARD constexpr auto end() noexcept(noexcept(_RANGES end(_Range))) /* strengthened */ {
            return _RANGES end(_Range);
        }
    };

    template <class _Rng, class _Pr>
    drop_while_view(_Rng&&, _Pr) -> drop_while_view<views::all_t<_Rng>, _Pr>;

    template <class _Rng, class _Pr>
    inline constexpr bool enable_borrowed_range<drop_while_view<_Rng, _Pr>> = enable_borrowed_range<_Rng>;

    namespace views {
        struct _Drop_while_fn {
            template <viewable_range _Rng, class _Pr>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, _Pr _Pred) const
                noexcept(noexcept(drop_while_view(_STD forward<_Rng>(_Range), _STD move(_Pred)))) requires requires {
                drop_while_view(static_cast<_Rng&&>(_Range), _STD move(_Pred));
            }
            { return drop_while_view(_STD forward<_Rng>(_Range), _STD move(_Pred)); }

            // clang-format off
            template <class _Pr>
                requires constructible_from<decay_t<_Pr>, _Pr>
            _NODISCARD constexpr auto operator()(_Pr&& _Pred) const
                noexcept(is_nothrow_constructible_v<decay_t<_Pr>, _Pr>) {
                // clang-format on
                return _Range_closure<_Drop_while_fn, decay_t<_Pr>>{_STD forward<_Pr>(_Pred)};
            }
        };

        inline constexpr _Drop_while_fn drop_while;
    } // namespace views

    // clang-format off
    template <input_range _Vw>
        requires view<_Vw> && input_range<range_reference_t<_Vw>>
    class join_view;
    // clang-format on

    template <class _Vw>
    class _Join_view_base : public view_interface<join_view<_Vw>> {
    private:
        struct _Cache_wrapper {
            template <input_iterator _Iter>
            constexpr _Cache_wrapper(_Not_quite_object::_Construct_tag, const _Iter& _It) noexcept(noexcept(*_It))
                : _Val(*_It) {}

            remove_cv_t<range_reference_t<_Vw>> _Val;
        };

    protected:
        /* [[no_unique_address]] */ _Non_propagating_cache<_Cache_wrapper, false> _Inner{};
    };

    template <class _Rng> // TRANSITION, LLVM-47414
    concept _Can_const_join = input_range<const _Rng> && is_reference_v<range_reference_t<const _Rng>>;

    // clang-format off
    template <class _Vw>
        requires is_reference_v<range_reference_t<_Vw>>
    class _Join_view_base<_Vw> : public view_interface<join_view<_Vw>> {};

    template <input_range _Vw>
        requires view<_Vw> && input_range<range_reference_t<_Vw>>
    class join_view : public _Join_view_base<_Vw> {
        // clang-format on
    private:
        template <bool _Const>
        using _InnerRng = range_reference_t<_Maybe_const<_Const, _Vw>>;
        /* [[no_unique_address]] */ _Vw _Range{};

        template <bool _Const>
        class _Sentinel;

        template <class _Outer, class _Inner, bool _Deref_is_glvalue>
        struct _Category_base {};

        template <forward_range _Outer, forward_range _Inner>
        struct _Category_base<_Outer, _Inner, true> {
            using iterator_category =
                conditional_t<common_range<_Inner> // per LWG-3535
                                  && derived_from<_Iter_cat_t<iterator_t<_Outer>>, bidirectional_iterator_tag> //
                                  && derived_from<_Iter_cat_t<iterator_t<_Inner>>, bidirectional_iterator_tag>,
                    bidirectional_iterator_tag,
                    conditional_t<derived_from<_Iter_cat_t<iterator_t<_Outer>>, forward_iterator_tag> //
                                      && derived_from<_Iter_cat_t<iterator_t<_Inner>>, forward_iterator_tag>,
                        forward_iterator_tag, input_iterator_tag>>;
        };

        template <bool _Const>
        class _Iterator
            : public _Category_base<_Maybe_const<_Const, _Vw>, _InnerRng<_Const>, is_reference_v<_InnerRng<_Const>>> {
        private:
            template <bool>
            friend class _Iterator;
            template <bool>
            friend class _Sentinel;

            using _Parent_t  = _Maybe_const<_Const, join_view>;
            using _Base      = _Maybe_const<_Const, _Vw>;
            using _OuterIter = iterator_t<_Base>;
            using _InnerIter = iterator_t<_InnerRng<_Const>>;

            // True if and only if the expression *i, where i is an iterator from the outer range, is a glvalue:
            static constexpr bool _Deref_is_glvalue = is_reference_v<_InnerRng<_Const>>;

            /* [[no_unique_address]] */ _OuterIter _Outer{};
            /* [[no_unique_address]] */ _Defaultabox<_InnerIter> _Inner{}; // per LWG issue unfiled as of 2021-06-14
            _Parent_t* _Parent{};

            constexpr auto&& _Update_inner() {
                if constexpr (_Deref_is_glvalue) {
                    return *_Outer;
                } else {
                    return _Parent->_Inner._Emplace(_Not_quite_object::_Construct_tag{}, _Outer)._Val;
                }
            }

            constexpr void _Satisfy() {
                const auto _Last = _RANGES end(_Parent->_Range);
                for (; _Outer != _Last; ++_Outer) {
                    auto&& _Tmp = _Update_inner();
                    _Inner      = _RANGES begin(_Tmp);
                    if (*_Inner != _RANGES end(_Tmp)) {
                        return;
                    }
                }
                if constexpr (_Deref_is_glvalue) {
                    _Inner._Reset();
                }
            }

#if _ITERATOR_DEBUG_LEVEL != 0
            constexpr void _Check_dereference() const noexcept {
                _STL_VERIFY(_Parent != nullptr, "cannot dereference value-initialized join_view iterator");
                _STL_VERIFY(_Outer != _RANGES end(_Parent->_Range), "cannot dereference join_view end iterator");
                sentinel_t<_InnerRng<_Const>> _Last;
                if constexpr (_Deref_is_glvalue) {
                    _Last = _RANGES end(*_Outer);
                } else {
                    _Last = _RANGES end((*_Parent->_Inner)._Val);
                }
                _STL_VERIFY(_Inner && *_Inner != _Last, "cannot dereference join_view end iterator");
            }

            constexpr void _Same_range(const _Iterator& _Right) const noexcept {
                _STL_VERIFY(_Parent == _Right._Parent, "cannot compare incompatible join_view iterators");
            }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        public:
            // clang-format off
            // Per LWG issue unnumbered as of 2021-03-16
            using iterator_concept = conditional_t<_Deref_is_glvalue
                && bidirectional_range<_Base> && bidirectional_range<_InnerRng<_Const>>
                && common_range<_InnerRng<_Const>>, bidirectional_iterator_tag,
                conditional_t<_Deref_is_glvalue && forward_range<_Base> && forward_range<_InnerRng<_Const>>,
                    forward_iterator_tag, input_iterator_tag>>;
            // clang-format on
            using value_type      = range_value_t<_InnerRng<_Const>>;
            using difference_type = common_type_t<range_difference_t<_Base>, range_difference_t<_InnerRng<_Const>>>;

            // clang-format off
            _Iterator() requires default_initializable<_OuterIter> = default; // per LWG-3569
            // clang-format on

            constexpr _Iterator(_Parent_t& _Parent_, _OuterIter _Outer_)
                : _Outer{_STD move(_Outer_)}, _Parent{_STD addressof(_Parent_)} {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Adl_verify_range(_Outer, _RANGES end(_Parent_._Range));
                if constexpr (forward_range<_Base>) {
                    _Adl_verify_range(_RANGES begin(_Parent_._Range), _Outer);
                }
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _Satisfy();
            }

            // clang-format off
            constexpr _Iterator(_Iterator<!_Const> _It)
                requires _Const && convertible_to<iterator_t<_Vw>, _OuterIter>
                      && convertible_to<iterator_t<_InnerRng<false>>, _InnerIter>
                : _Outer{_STD move(_It._Outer)}, _Inner{_STD move(_It._Inner)}, _Parent{_It._Parent} {}
            // clang-format on

            _NODISCARD constexpr decltype(auto) operator*() const noexcept(noexcept(**_Inner)) /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return **_Inner;
            }

            // clang-format off
            _NODISCARD constexpr _InnerIter operator->() const noexcept(is_nothrow_copy_constructible_v<_InnerIter>)
                /* strengthened */ requires _Has_arrow<_InnerIter> && copyable<_InnerIter> {
                // clang-format on
#if _ITERATOR_DEBUG_LEVEL != 0
                _Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return *_Inner;
            }

            constexpr _Iterator& operator++() {
                if constexpr (_Deref_is_glvalue) {
                    if (++*_Inner == _RANGES end(*_Outer)) {
                        ++_Outer;
                        _Satisfy();
                    }
                } else {
                    if (++*_Inner == _RANGES end((*_Parent->_Inner)._Val)) {
                        ++_Outer;
                        _Satisfy();
                    }
                }
                return *this;
            }

            constexpr decltype(auto) operator++(int) {
                if constexpr (_Deref_is_glvalue && forward_range<_Base> && forward_range<_InnerRng<_Const>>) {
                    auto _Tmp = *this;
                    ++*this;
                    return _Tmp;
                } else {
                    ++*this;
                }
            }

            // clang-format off
            constexpr _Iterator& operator--()
                requires _Deref_is_glvalue && bidirectional_range<_Base> && bidirectional_range<_InnerRng<_Const>>
                      && common_range<_InnerRng<_Const>> {
                // clang-format on
                if (_Outer == _RANGES end(_Parent->_Range)) {
                    --_Outer;
                    _Inner = _RANGES end(*_Outer);
                }
                while (*_Inner == _RANGES begin(*_Outer)) {
                    --_Outer;
                    *_Inner = _RANGES end(*_Outer);
                }
                --*_Inner;
                return *this;
            }

            // clang-format off
            constexpr _Iterator operator--(int)
                requires _Deref_is_glvalue && bidirectional_range<_Base> && bidirectional_range<_InnerRng<_Const>>
                      && common_range<_InnerRng<_Const>> {
                // clang-format on
                auto _Tmp = *this;
                --*this;
                return _Tmp;
            }

            // clang-format off
            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Implicitly_convert_to<bool>(_Left._Outer == _Right._Outer
                    && _Left._Inner == _Right._Inner))) /* strengthened */
                requires _Deref_is_glvalue && equality_comparable<_OuterIter> && equality_comparable<_InnerIter> {
                // clang-format on
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Same_range(_Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _Left._Outer == _Right._Outer && _Left._Inner == _Right._Inner;
            }

            _NODISCARD_FRIEND constexpr decltype(auto) iter_move(const _Iterator& _It) noexcept(
                noexcept(_RANGES iter_move(*_It._Inner))) {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _RANGES iter_move(*_It._Inner);
            }

            // clang-format off
            friend constexpr void iter_swap(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_RANGES iter_swap(*_Left._Inner, *_Right._Inner)))
                requires indirectly_swappable<_InnerIter> {
                // clang-format on
#if _ITERATOR_DEBUG_LEVEL != 0
                _Left._Check_dereference();
                _Right._Check_dereference();
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _RANGES iter_swap(*_Left._Inner, *_Right._Inner);
            }
        };

        template <bool _Const>
        class _Sentinel {
        private:
            template <bool>
            friend class _Iterator;
            template <bool>
            friend class _Sentinel;

            using _Parent_t = _Maybe_const<_Const, join_view>;
            using _Base     = _Maybe_const<_Const, _Vw>;

            template <bool _OtherConst>
            using _Maybe_const_iter = iterator_t<_Maybe_const<_OtherConst, _Vw>>;

            /* [[no_unique_address]] */ sentinel_t<_Base> _Last{};

            // clang-format off
            template <bool _OtherConst>
                requires sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD constexpr bool _Equal(const _Iterator<_OtherConst>& _It) const noexcept(
                    noexcept(_Implicitly_convert_to<bool>(_It._Outer == _Last))) {
                // clang-format on
                return _It._Outer == _Last;
            }

        public:
            _Sentinel() = default;
            constexpr explicit _Sentinel(_Parent_t& _Parent) noexcept(
                noexcept(_RANGES end(_Parent._Range))
                && is_nothrow_move_constructible_v<sentinel_t<_Base>>) // strengthened
                : _Last(_RANGES end(_Parent._Range)) {}

            // clang-format off
            constexpr _Sentinel(_Sentinel<!_Const> _Se)
                noexcept(is_nothrow_constructible_v<sentinel_t<_Base>, sentinel_t<_Vw>>) // strengthened
                requires _Const && convertible_to<sentinel_t<_Vw>, sentinel_t<_Base>>
                : _Last(_STD move(_Se._Last)) {}

            template <bool _OtherConst>
                requires sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator<_OtherConst>& _Left,
                const _Sentinel& _Right) noexcept(noexcept(_Right._Equal(_Left))) /* strengthened */ {
                // clang-format on
                return _Right._Equal(_Left);
            }
        };

    public:
        join_view() requires default_initializable<_Vw>
        = default;

        constexpr explicit join_view(_Vw _Range_) noexcept(is_nothrow_move_constructible_v<_Vw>) // strengthened
            : _Range(_STD move(_Range_)) {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr auto begin() {
            constexpr bool _Use_const = _Simple_view<_Vw> && is_reference_v<_InnerRng<false>>;
            return _Iterator<_Use_const>{*this, _RANGES begin(_Range)};
        }

        // clang-format off
        _NODISCARD constexpr _Iterator<true> begin() const
        #ifdef __clang__ // TRANSITION, LLVM-47414
            requires _Can_const_join<_Vw>
        #else // ^^^ workaround / no workaround vvv
            requires input_range<const _Vw> && is_reference_v<_InnerRng<true>>
        #endif // TRANSITION, LLVM-47414
        {
            // clang-format on
            return _Iterator<true>{*this, _RANGES begin(_Range)};
        }

        // clang-format off
        _NODISCARD constexpr auto end() {
            if constexpr (forward_range<_Vw> && is_reference_v<_InnerRng<false>>
                    && forward_range<_InnerRng<false>> && common_range<_Vw> && common_range<_InnerRng<false>>) {
                // clang-format on
                return _Iterator<_Simple_view<_Vw>>{*this, _RANGES end(_Range)};
            } else {
                return _Sentinel<_Simple_view<_Vw>>{*this};
            }
        }

        // clang-format off
        _NODISCARD constexpr auto end() const
        #ifdef __clang__ // TRANSITION, LLVM-47414
            requires _Can_const_join<_Vw>
        #else // ^^^ workaround / no workaround vvv
            requires input_range<const _Vw> && is_reference_v<_InnerRng<true>>
        #endif // TRANSITION, LLVM-47414
        {
            if constexpr (forward_range<const _Vw> && forward_range<_InnerRng<true>>
                    && common_range<const _Vw> && common_range<_InnerRng<true>>) {
                // clang-format on
                return _Iterator<true>{*this, _RANGES end(_Range)};
            } else {
                return _Sentinel<true>{*this};
            }
        }
    };

    template <class _Rng>
    explicit join_view(_Rng&&) -> join_view<views::all_t<_Rng>>;

    namespace views {
        class _Join_fn : public _Pipe::_Base<_Join_fn> {
        public:
            // clang-format off
            template <viewable_range _Rng>
            _NODISCARD constexpr auto operator()(_Rng&& _Range) const noexcept(noexcept(
                join_view<views::all_t<_Rng>>{_STD forward<_Rng>(_Range)})) requires requires {
                join_view<views::all_t<_Rng>>{static_cast<_Rng&&>(_Range)};
            } {
                // clang-format on
                return join_view<views::all_t<_Rng>>{_STD forward<_Rng>(_Range)};
            }
        };

        inline constexpr _Join_fn join;
    } // namespace views

    template <auto> // _Require_constant<E> is a valid template-id iff E is a constant expression of structural type
    struct _Require_constant;

    // clang-format off
    template <class _Ty>
    concept _Tiny_range = sized_range<_Ty>
        && requires { typename _Require_constant<remove_reference_t<_Ty>::size()>; }
        && (remove_reference_t<_Ty>::size() <= 1);

    template <input_range _Vw, forward_range _Pat>
        requires (view<_Vw> && view<_Pat>
            && indirectly_comparable<iterator_t<_Vw>, iterator_t<_Pat>, _RANGES equal_to>
            && (forward_range<_Vw> || _Tiny_range<_Pat>))
    class lazy_split_view;
    // clang-format on

    template <class _Vw, class _Pat>
    class _Lazy_split_view_base : public view_interface<lazy_split_view<_Vw, _Pat>> {
    protected:
        /* [[no_unique_address]] */ _Defaultabox<iterator_t<_Vw>> _Current{};
    };

    template <forward_range _Vw, class _Pat>
    class _Lazy_split_view_base<_Vw, _Pat> : public view_interface<lazy_split_view<_Vw, _Pat>> {};

    // clang-format off
    template <input_range _Vw, forward_range _Pat>
        requires (view<_Vw> && view<_Pat>
            && indirectly_comparable<iterator_t<_Vw>, iterator_t<_Pat>, _RANGES equal_to>
            && (forward_range<_Vw> || _Tiny_range<_Pat>))
    class lazy_split_view : public _Lazy_split_view_base<_Vw, _Pat> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        /* [[no_unique_address]] */ _Pat _Pattern{};

        template <bool>
        class _Inner_iter;

        template <class>
        class _Outer_iter_base {};

        template <forward_iterator _Iter>
        class _Outer_iter_base<_Iter> {
        protected:
            _Iter _Current{};

        public:
            using iterator_category = input_iterator_tag;

            _Outer_iter_base() = default;
            constexpr explicit _Outer_iter_base(_Iter _Current_) noexcept(is_nothrow_move_constructible_v<_Iter>)
                : _Current{_STD move(_Current_)} {}
        };

        template <bool _Const>
        class _Outer_iter : public _Outer_iter_base<iterator_t<_Maybe_const<_Const, _Vw>>> {
        private:
            template <bool>
            friend class _Inner_iter;
            friend _Outer_iter<true>;

            using _Mybase   = _Outer_iter_base<iterator_t<_Maybe_const<_Const, _Vw>>>;
            using _ParentTy = _Maybe_const<_Const, lazy_split_view>;
            using _BaseTy   = _Maybe_const<_Const, _Vw>;

            _ParentTy* _Parent   = nullptr;
            bool _Trailing_empty = false;

            _NODISCARD constexpr iterator_t<_BaseTy>& _Get_current() noexcept {
                if constexpr (forward_range<_BaseTy>) {
                    return this->_Current;
                } else {
                    return *_Parent->_Current;
                }
            }

            _NODISCARD constexpr const iterator_t<_BaseTy>& _Get_current() const noexcept {
                if constexpr (forward_range<_BaseTy>) {
                    return this->_Current;
                } else {
                    return *_Parent->_Current;
                }
            }

            _NODISCARD constexpr bool _At_end() const
                noexcept(noexcept(_Implicitly_convert_to<bool>(_Get_current() == _RANGES end(_Parent->_Range)))) {
                return _Get_current() == _RANGES end(_Parent->_Range);
            }

        public:
            using iterator_concept = conditional_t<forward_range<_BaseTy>, forward_iterator_tag, input_iterator_tag>;
            using difference_type  = range_difference_t<_BaseTy>;

            class value_type : public view_interface<value_type> {
            private:
                /* [[no_unique_address]] */ _Outer_iter _First{};

            public:
                value_type() = default;
                constexpr explicit value_type(_Outer_iter _First_) noexcept(
                    is_nothrow_move_constructible_v<_Outer_iter>) // strengthened
                    : _First{_STD move(_First_)} {}

                _NODISCARD constexpr auto begin() const {
                    return _Inner_iter<_Const>{_First};
                }

                _NODISCARD constexpr default_sentinel_t end() const noexcept {
                    return default_sentinel;
                }
            };

            _Outer_iter() = default;

            // clang-format off
            constexpr explicit _Outer_iter(_ParentTy& _Parent_) noexcept // strengthened
                requires (!forward_range<_BaseTy>) : _Parent{_STD addressof(_Parent_)} {}
            // clang-format on

            constexpr _Outer_iter(_ParentTy& _Parent_, iterator_t<_BaseTy> _Current_) noexcept(
                is_nothrow_move_constructible_v<iterator_t<_BaseTy>>) // strengthened
                requires forward_range<_BaseTy>
                : _Mybase{_STD move(_Current_)}, _Parent{_STD addressof(_Parent_)} {}

            // clang-format off
            constexpr _Outer_iter(_Outer_iter<!_Const> _It)
                requires _Const && convertible_to<iterator_t<_Vw>, iterator_t<_BaseTy>>
                : _Mybase{_STD move(_It._Current)}, _Parent{_It._Parent} {}
            // clang-format on

            _NODISCARD constexpr auto operator*() const noexcept(noexcept(value_type{*this})) /* strengthened */ {
                return value_type{*this};
            }

            constexpr _Outer_iter& operator++() {
                const auto _End = _RANGES end(_Parent->_Range);
                auto& _Cur      = _Get_current();
                if (_Cur == _End) {
                    _Trailing_empty = false;
                    return *this;
                }

                const auto _Pat_first = _RANGES begin(_Parent->_Pattern);
                const auto _Pat_last  = _RANGES end(_Parent->_Pattern);
                if (_Pat_first == _Pat_last) {
                    ++_Cur;
                } else if constexpr (_Tiny_range<_Pat>) {
                    _Cur = _RANGES _Find_unchecked(_STD move(_Cur), _End, *_Pat_first);
                    if (_Cur != _End) {
                        ++_Cur;
                        if (_Cur == _End) {
                            _Trailing_empty = true;
                        }
                    }
                } else {
                    do {
                        auto _Result = _RANGES mismatch(_Cur, _End, _Pat_first, _Pat_last);
                        if (_Result.in2 == _Pat_last) { // pattern matches
                            _Cur = _STD move(_Result.in1);
                            if (_Cur == _End) {
                                _Trailing_empty = true;
                            }
                            break;
                        }
                    } while (++_Cur != _End);
                }
                return *this;
            }

            constexpr decltype(auto) operator++(int) {
                if constexpr (forward_range<_BaseTy>) {
                    auto _Tmp = *this;
                    ++*this;
                    return _Tmp;
                } else {
                    ++*this;
                }
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Outer_iter& _Left, const _Outer_iter& _Right) noexcept(
                noexcept(_Left._Current == _Right._Current)) /* strengthened */ requires forward_range<_BaseTy> {
                return _Left._Current == _Right._Current && _Left._Trailing_empty == _Right._Trailing_empty;
            }
            _NODISCARD_FRIEND constexpr bool operator==(const _Outer_iter& _Left, default_sentinel_t) noexcept(
                noexcept(_Left._At_end())) /* strengthened */ {
                return _Left._At_end() && !_Left._Trailing_empty;
            }
        };

        template <class _BaseTy>
        class _Inner_iter_base {};

        template <forward_range _BaseTy>
        class _Inner_iter_base<_BaseTy> {
        private:
            using _BaseCategory = typename iterator_traits<iterator_t<_BaseTy>>::iterator_category;

        public:
            using iterator_category =
                conditional_t<derived_from<_BaseCategory, forward_iterator_tag>, forward_iterator_tag, _BaseCategory>;
        };

        template <bool _Const>
        class _Inner_iter : public _Inner_iter_base<_Maybe_const<_Const, _Vw>> {
        private:
            using _BaseTy = _Maybe_const<_Const, _Vw>;

            _Outer_iter<_Const> _It{};
            bool _Incremented = false;

            _NODISCARD constexpr bool _Equal(const _Inner_iter& _Right) const {
                return _It._Get_current() == _Right._It._Get_current();
            }

            _NODISCARD constexpr iterator_t<_BaseTy>& _Get_current() noexcept {
                return _It._Get_current();
            }

            _NODISCARD constexpr const iterator_t<_BaseTy>& _Get_current() const noexcept {
                return _It._Get_current();
            }

            _NODISCARD constexpr bool _At_end() const {
                auto _Pat_pos       = _RANGES begin(_It._Parent->_Pattern);
                const auto _Pat_end = _RANGES end(_It._Parent->_Pattern);
                auto _Last          = _RANGES end(_It._Parent->_Range);
                if constexpr (_Tiny_range<_Pat>) {
                    const auto& _Cur = _It._Get_current(); // Intentionally a reference. Since _Pat is tiny, this could
                                                           // be a move-only iterator type.
                    if (_Cur == _Last) {
                        return true;
                    }

                    if (_Pat_pos == _Pat_end) {
                        return _Incremented;
                    }
                    return *_Cur == *_Pat_pos;
                } else {
                    auto _Cur = _It._Get_current(); // Intentionally not a reference. _Pat is not tiny, so this is a
                                                    // forward (copyable) iterator.
                    if (_Cur == _Last) {
                        return true;
                    }

                    if (_Pat_pos == _Pat_end) {
                        return _Incremented;
                    }

                    do {
                        if (*_Cur != *_Pat_pos) {
                            return false;
                        }

                        if (++_Pat_pos == _Pat_end) {
                            return true;
                        }
                    } while (++_Cur != _Last);
                    return false;
                }
            }


        public:
            using iterator_concept = typename _Outer_iter<_Const>::iterator_concept;
            using value_type       = range_value_t<_BaseTy>;
            using difference_type  = range_difference_t<_BaseTy>;

            _Inner_iter() = default;
            constexpr explicit _Inner_iter(_Outer_iter<_Const> _It_) noexcept(
                is_nothrow_move_constructible_v<_Outer_iter<_Const>>) // strengthened
                : _It{_STD move(_It_)} {}

            _NODISCARD constexpr const iterator_t<_BaseTy>& base() const& noexcept /* strengthened */ {
                return _It._Get_current();
            }

            _NODISCARD constexpr iterator_t<_BaseTy> base() && noexcept(
                is_nothrow_move_constructible_v<iterator_t<_BaseTy>>) /* strengthened */
                requires forward_range<_Vw> {
                return _STD move(_It._Get_current());
            }

            _NODISCARD constexpr decltype(auto) operator*() const {
                return *_It._Get_current();
            }

            constexpr _Inner_iter& operator++() {
                _Incremented = true;
                if constexpr (!forward_range<_BaseTy>) {
                    if constexpr (_Pat::size() == 0) {
                        return *this;
                    }
                }
                ++_It._Get_current();
                return *this;
            }

            constexpr decltype(auto) operator++(int) {
                if constexpr (forward_range<_BaseTy>) {
                    auto _Tmp = *this;
                    ++*this;
                    return _Tmp;
                } else {
                    ++*this;
                }
            }

            _NODISCARD_FRIEND constexpr bool operator==(
                const _Inner_iter& _Left, const _Inner_iter& _Right) requires forward_range<_BaseTy> {
                return _Left._Equal(_Right);
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Inner_iter& _Left, default_sentinel_t) {
                return _Left._At_end();
            }

            _NODISCARD_FRIEND constexpr decltype(auto) iter_move(const _Inner_iter& _Iter) noexcept(
                noexcept(_RANGES iter_move(_Iter._Get_current()))) {
                return _RANGES iter_move(_Iter._Get_current());
            }

            // clang-format off
            friend constexpr void iter_swap(const _Inner_iter& _Left, const _Inner_iter& _Right)
                noexcept(noexcept(_RANGES iter_swap(_Left._Get_current(), _Right._Get_current())))
                requires indirectly_swappable<iterator_t<_BaseTy>> {
                // clang-format on
                _RANGES iter_swap(_Left._Get_current(), _Right._Get_current());
            }
        };

    public:
        // clang-format off
        lazy_split_view() requires default_initializable<_Vw> && default_initializable<_Pat> = default;
        // clang-format on

        constexpr lazy_split_view(_Vw _Range_, _Pat _Pattern_) noexcept(
            is_nothrow_move_constructible_v<_Vw>&& is_nothrow_move_constructible_v<_Pat>) // strengthened
            : _Range(_STD move(_Range_)), _Pattern(_STD move(_Pattern_)) {}

        // clang-format off
        template <input_range _Rng>
            requires constructible_from<_Vw, views::all_t<_Rng>>
                && constructible_from<_Pat, single_view<range_value_t<_Rng>>>
        constexpr lazy_split_view(_Rng&& _Range_, range_value_t<_Rng> _Elem)
            noexcept(noexcept(_Vw(views::all(_STD forward<_Rng>(_Range_))))
                && noexcept(_Pat(views::single(_STD move(_Elem))))) // strengthened
            : _Range(views::all(_STD forward<_Rng>(_Range_))), _Pattern(views::single(_STD move(_Elem))) {}
        // clang-format on

        _NODISCARD constexpr _Vw base() const& noexcept(is_nothrow_copy_constructible_v<_Vw>) /* strengthened */
            requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr auto begin() {
            if constexpr (forward_range<_Vw>) {
                constexpr bool _Both_simple = _Simple_view<_Vw> && _Simple_view<_Pat>;
                return _Outer_iter<_Both_simple>{*this, _RANGES begin(_Range)};
            } else {
                this->_Current = _RANGES begin(_Range);
                return _Outer_iter<false>{*this};
            }
        }

        // clang-format off
        _NODISCARD constexpr auto begin() const requires forward_range<_Vw> && forward_range<const _Vw> {
            // clang-format on
            return _Outer_iter<true>{*this, _RANGES begin(_Range)};
        }

        // clang-format off
        _NODISCARD constexpr auto end() requires forward_range<_Vw> && common_range<_Vw> {
            // clang-format on
            constexpr bool _Both_simple = _Simple_view<_Vw> && _Simple_view<_Pat>;
            return _Outer_iter<_Both_simple>{*this, _RANGES end(_Range)};
        }

        _NODISCARD constexpr auto end() const {
            if constexpr (forward_range<_Vw> && forward_range<const _Vw> && common_range<const _Vw>) {
                return _Outer_iter<true>{*this, _RANGES end(_Range)};
            } else {
                return default_sentinel;
            }
        }
    };

    template <class _Rng, class _Pat>
    lazy_split_view(_Rng&&, _Pat&&) -> lazy_split_view<views::all_t<_Rng>, views::all_t<_Pat>>;

    template <input_range _Rng>
    lazy_split_view(_Rng&&, range_value_t<_Rng>)
        -> lazy_split_view<views::all_t<_Rng>, single_view<range_value_t<_Rng>>>;

    namespace views {
        struct _Lazy_split_fn {
            // clang-format off
            template <viewable_range _Rng, class _Pat>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, _Pat&& _Pattern) const noexcept(noexcept(
                lazy_split_view(_STD forward<_Rng>(_Range), _STD forward<_Pat>(_Pattern)))) requires requires {
                lazy_split_view(static_cast<_Rng&&>(_Range), static_cast<_Pat&&>(_Pattern));
            } {
                // clang-format on
                return lazy_split_view(_STD forward<_Rng>(_Range), _STD forward<_Pat>(_Pattern));
            }

            // clang-format off
            template <class _Delim>
                requires constructible_from<decay_t<_Delim>, _Delim>
            _NODISCARD constexpr auto operator()(_Delim&& _Delimiter) const
                noexcept(is_nothrow_constructible_v<decay_t<_Delim>, _Delim>) {
                // clang-format on
                return _Range_closure<_Lazy_split_fn, decay_t<_Delim>>{_STD forward<_Delim>(_Delimiter)};
            }
        };

        inline constexpr _Lazy_split_fn lazy_split;
    } // namespace views

    // clang-format off
    template <forward_range _Vw, forward_range _Pat>
        requires view<_Vw> && view<_Pat> && indirectly_comparable<iterator_t<_Vw>, iterator_t<_Pat>, _RANGES equal_to>
    class split_view : public view_interface<split_view<_Vw, _Pat>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};
        /* [[no_unique_address]] */ _Pat _Pattern{};
        _Non_propagating_cache<subrange<iterator_t<_Vw>>> _Next{};

        class _Sentinel;

        class _Iterator {
        private:
            friend class _Sentinel;

            split_view* _Parent             = nullptr;
            iterator_t<_Vw> _Current        = {};
            subrange<iterator_t<_Vw>> _Next = {};
            bool _Trailing_empty            = false;

        public:
            using iterator_concept  = forward_iterator_tag;
            using iterator_category = input_iterator_tag;
            using value_type        = subrange<iterator_t<_Vw>>;
            using difference_type   = range_difference_t<_Vw>;

            _Iterator() = default;

            constexpr _Iterator(split_view& _Parent_, iterator_t<_Vw> _Current_, subrange<iterator_t<_Vw>> _Next_) //
                noexcept(is_nothrow_move_constructible_v<iterator_t<_Vw>>) // strengthened
                : _Parent{_STD addressof(_Parent_)}, _Current{_STD move(_Current_)}, _Next{_STD move(_Next_)} {}

            _NODISCARD constexpr iterator_t<_Vw> base() const
                noexcept(is_nothrow_copy_constructible_v<iterator_t<_Vw>>) /* strengthened */ {
                return _Current;
            }

            _NODISCARD constexpr value_type operator*() const
                noexcept(is_nothrow_copy_constructible_v<iterator_t<_Vw>>) /* strengthened */ {
                return {_Current, _Next.begin()};
            }

            constexpr _Iterator& operator++() {
                const auto _Last = _RANGES end(_Parent->_Range);
                _Current         = _Next.begin();
                if (_Current == _Last) {
                    _Trailing_empty = false;
                    return *this;
                }

                _Current = _Next.end();
                if (_Current == _Last) {
                    _Trailing_empty = true;
                    _Next           = {_Current, _Current};
                    return *this;
                }

#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1559808
                auto [_Begin, _End] = _RANGES search(subrange{_Current, _Last}, _Parent->_Pattern);
#else // ^^^ no workaround / workaround vvv
                auto _Match = _RANGES search(subrange{_Current, _Last}, _Parent->_Pattern);
                auto _Begin = _Match.begin();
                auto _End = _Match.end();
#endif // TRANSITION, DevCom-1559808
                if (_Begin != _Last && _RANGES empty(_Parent->_Pattern)) {
                    ++_Begin;
                    ++_End;
                }

                _Next = {_STD move(_Begin), _STD move(_End)};
                return *this;
            }

            constexpr _Iterator operator++(int) {
                auto _Tmp = *this;
                ++*this;
                return _Tmp;
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current == _Right._Current)) /* strengthened */ {
                return _Left._Current == _Right._Current && _Left._Trailing_empty == _Right._Trailing_empty;
            }
        };

        class _Sentinel {
        private:
            /* [[no_unique_address]] */ sentinel_t<_Vw> _Last{};

            _NODISCARD constexpr bool _Equal(const _Iterator& _It) const
                noexcept(noexcept(_Implicitly_convert_to<bool>(_It._Current == _Last))) {
                return !_It._Trailing_empty && _It._Current == _Last;
            }

        public:
            _Sentinel() = default;
            constexpr explicit _Sentinel(split_view& _Parent) noexcept(
                noexcept(_RANGES end(_Parent._Range))
                && is_nothrow_move_constructible_v<sentinel_t<_Vw>>) // strengthened
                : _Last(_RANGES end(_Parent._Range)) {}

            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _It, const _Sentinel& _Se) noexcept(
                noexcept(_Se._Equal(_It))) /* strengthened */ {
                return _Se._Equal(_It);
            }
        };

        constexpr subrange<iterator_t<_Vw>> _Find_next(iterator_t<_Vw> _It) {
            const auto _Last    = _RANGES end(_Range);
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1559808
            auto [_Begin, _End] = _RANGES search(subrange{_It, _Last}, _Pattern);
#else // ^^^ no workaround / workaround vvv
            auto _Match = _RANGES search(subrange{_It, _Last}, _Pattern);
            auto _Begin = _Match.begin();
            auto _End = _Match.end();
#endif // TRANSITION, DevCom-1559808
            if (_Begin != _Last && _RANGES empty(_Pattern)) {
                ++_Begin;
                ++_End;
            }

            return {_STD move(_Begin), _STD move(_End)};
        }

    public:
        // clang-format off
        split_view() requires default_initializable<_Vw> && default_initializable<_Pat> = default;
        // clang-format on

        constexpr split_view(_Vw _Range_, _Pat _Pattern_) noexcept(
            is_nothrow_move_constructible_v<_Vw>&& is_nothrow_move_constructible_v<_Pat>) // strengthened
            : _Range(_STD move(_Range_)), _Pattern(_STD move(_Pattern_)) {}

        // clang-format off
        template <forward_range _Rng>
            requires constructible_from<_Vw, views::all_t<_Rng>>
                && constructible_from<_Pat, single_view<range_value_t<_Rng>>>
        constexpr split_view(_Rng&& _Range_, range_value_t<_Rng> _Elem)
            noexcept(is_nothrow_constructible_v<_Vw, views::all_t<_Rng>>
                && is_nothrow_constructible_v<_Pat, single_view<range_value_t<_Rng>>>
                && is_nothrow_move_constructible_v<range_value_t<_Rng>>) // strengthened
            : _Range(views::all(_STD forward<_Rng>(_Range_))), _Pattern(views::single(_STD move(_Elem))) {}
        // clang-format on

        _NODISCARD constexpr _Vw base() const& noexcept(is_nothrow_copy_constructible_v<_Vw>) /* strengthened */
            requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr auto begin() {
            auto _First = _RANGES begin(_Range);
            if (!_Next) {
                _Next._Emplace(_Find_next(_First));
            }
            return _Iterator{*this, _First, *_Next};
        }

        _NODISCARD constexpr auto end() {
            if constexpr (common_range<_Vw>) {
                return _Iterator{*this, _RANGES end(_Range), {}};
            } else {
                return _Sentinel{*this};
            }
        }
    };

    template <class _Rng, class _Pat>
    split_view(_Rng&&, _Pat&&) -> split_view<views::all_t<_Rng>, views::all_t<_Pat>>;

    template <forward_range _Rng>
    split_view(_Rng&&, range_value_t<_Rng>) -> split_view<views::all_t<_Rng>, single_view<range_value_t<_Rng>>>;

    namespace views {
        struct _Split_fn {
            // clang-format off
            template <viewable_range _Rng, class _Pat>
            _NODISCARD constexpr auto operator()(_Rng&& _Range, _Pat&& _Pattern) const noexcept(noexcept(
                split_view(_STD forward<_Rng>(_Range), _STD forward<_Pat>(_Pattern)))) requires requires {
                split_view(static_cast<_Rng&&>(_Range), static_cast<_Pat&&>(_Pattern));
            } {
                // clang-format on
                return split_view(_STD forward<_Rng>(_Range), _STD forward<_Pat>(_Pattern));
            }

            // clang-format off
            template <class _Delim>
                requires constructible_from<decay_t<_Delim>, _Delim>
            _NODISCARD constexpr auto operator()(_Delim&& _Delimiter) const
                noexcept(is_nothrow_constructible_v<decay_t<_Delim>, _Delim>) {
                // clang-format on
                return _Range_closure<_Split_fn, decay_t<_Delim>>{_STD forward<_Delim>(_Delimiter)};
            }
        };

        inline constexpr _Split_fn split;

        class _Counted_fn {
        private:
            enum class _St { _None, _Span, _Subrange, _Subrange_counted };

            template <class _It>
            _NODISCARD static _CONSTEVAL _Choice_t<_St> _Choose() noexcept {
                using _Decayed = decay_t<_It>;
                _STL_INTERNAL_STATIC_ASSERT(input_or_output_iterator<_Decayed>);
                if constexpr (contiguous_iterator<_Decayed>) {
                    return {_St::_Span,
                        noexcept(span(_STD to_address(_STD declval<_It>()), iter_difference_t<_Decayed>{}))};
                } else if constexpr (random_access_iterator<_Decayed>) {
                    return {_St::_Subrange,
                        noexcept(subrange(_STD declval<_It>(), _STD declval<_It>() + iter_difference_t<_Decayed>{}))};
                } else if constexpr (constructible_from<_Decayed, _It>) {
                    return {_St::_Subrange_counted,
                        noexcept(subrange(
                            counted_iterator(_STD declval<_It>(), iter_difference_t<_Decayed>{}), default_sentinel))};
                } else {
                    return {_St::_None};
                }
            }

            template <class _It>
            static constexpr _Choice_t<_St> _Choice = _Choose<_It>();

        public:
            // clang-format off
            template <class _It>
                requires (input_or_output_iterator<decay_t<_It>> && _Choice<_It>._Strategy != _St::_None)
            _NODISCARD constexpr auto operator()(_It&& _First, const iter_difference_t<decay_t<_It>> _Count) const
                noexcept(_Choice<_It>._No_throw) {
                // clang-format on
                _STL_ASSERT(_Count >= 0, "The size passed to views::counted must be non-negative");
                constexpr _St _Strat = _Choice<_It>._Strategy;

                if constexpr (_Strat == _St::_Span) {
                    return span(_STD to_address(_STD forward<_It>(_First)), static_cast<size_t>(_Count));
                } else if constexpr (_Strat == _St::_Subrange) {
                    return subrange(_First, _First + _Count);
                } else if constexpr (_Strat == _St::_Subrange_counted) {
                    return subrange(counted_iterator(_STD forward<_It>(_First), _Count), default_sentinel);
                }
            }
        };

        inline constexpr _Counted_fn counted;
    } // namespace views

    // clang-format off
    template <view _Vw>
        requires (!common_range<_Vw> && copyable<iterator_t<_Vw>>)
    class common_view : public view_interface<common_view<_Vw>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Base{};

    public:
        common_view() requires default_initializable<_Vw>
        = default;

        constexpr explicit common_view(_Vw _Base_) noexcept(is_nothrow_move_constructible_v<_Vw>) // strengthened
            : _Base(_STD move(_Base_)) {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Base;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Base);
        }

        _NODISCARD constexpr auto begin() noexcept(
            noexcept(_RANGES begin(_Base)) && is_nothrow_move_constructible_v<iterator_t<_Vw>>) /* strengthened */ {
            if constexpr (random_access_range<_Vw> && sized_range<_Vw>) {
                return _RANGES begin(_Base);
            } else {
                return common_iterator<iterator_t<_Vw>, sentinel_t<_Vw>>{_RANGES begin(_Base)};
            }
        }

        _NODISCARD constexpr auto begin() const noexcept(
            noexcept(_RANGES begin(_Base))
            && is_nothrow_move_constructible_v<iterator_t<const _Vw>>) /* strengthened */ requires range<const _Vw> {
            if constexpr (random_access_range<const _Vw> && sized_range<const _Vw>) {
                return _RANGES begin(_Base);
            } else {
                return common_iterator<iterator_t<const _Vw>, sentinel_t<const _Vw>>{_RANGES begin(_Base)};
            }
        }

        _NODISCARD constexpr auto end() {
            if constexpr (random_access_range<_Vw> && sized_range<_Vw>) {
                return _RANGES begin(_Base) + _RANGES size(_Base);
            } else {
                return common_iterator<iterator_t<_Vw>, sentinel_t<_Vw>>{_RANGES end(_Base)};
            }
        }

        _NODISCARD constexpr auto end() const requires range<const _Vw> {
            if constexpr (random_access_range<const _Vw> && sized_range<const _Vw>) {
                return _RANGES begin(_Base) + _RANGES size(_Base);
            } else {
                return common_iterator<iterator_t<const _Vw>, sentinel_t<const _Vw>>{_RANGES end(_Base)};
            }
        }

        _NODISCARD constexpr auto size() noexcept(
            noexcept(_RANGES size(_Base))) /* strengthened */ requires sized_range<_Vw> {
            return _RANGES size(_Base);
        }
        _NODISCARD constexpr auto size() const
            noexcept(noexcept(_RANGES size(_Base))) /* strengthened */ requires sized_range<const _Vw> {
            return _RANGES size(_Base);
        }
    };

    template <class _Rng>
    common_view(_Rng&&) -> common_view<views::all_t<_Rng>>;

    template <class _Rng>
    inline constexpr bool enable_borrowed_range<common_view<_Rng>> = enable_borrowed_range<_Rng>;

    namespace views {
        class _Common_fn : public _Pipe::_Base<_Common_fn> {
        private:
            enum class _St { _None, _All, _Common };

            template <class _Rng>
            _NODISCARD static _CONSTEVAL _Choice_t<_St> _Choose() noexcept {
                if constexpr (common_range<_Rng>) {
                    return {_St::_All, noexcept(views::all(_STD declval<_Rng>()))};
                } else if constexpr (copyable<iterator_t<_Rng>>) {
                    return {_St::_Common, noexcept(common_view{_STD declval<_Rng>()})};
                } else {
                    return {_St::_None};
                }
            }

            template <class _Rng>
            static constexpr _Choice_t<_St> _Choice = _Choose<_Rng>();

        public:
            // clang-format off
            template <viewable_range _Rng>
                requires (_Choice<_Rng>._Strategy != _St::_None)
            _NODISCARD constexpr auto operator()(_Rng&& _Range) const noexcept(_Choice<_Rng>._No_throw) {
                // clang-format on
                constexpr _St _Strat = _Choice<_Rng>._Strategy;

                if constexpr (_Strat == _St::_All) {
                    return views::all(_STD forward<_Rng>(_Range));
                } else if constexpr (_Strat == _St::_Common) {
                    return common_view{_STD forward<_Rng>(_Range)};
                } else {
                    static_assert(_Always_false<_Rng>, "Should be unreachable");
                }
            }
        };

        inline constexpr _Common_fn common;
    } // namespace views

    // clang-format off
    template <view _Vw>
        requires bidirectional_range<_Vw>
    class reverse_view : public _Cached_position_t<!common_range<_Vw>, _Vw, reverse_view<_Vw>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};

        template <class _Rng>
        using _Rev_iter = reverse_iterator<iterator_t<_Rng>>;

    public:
        reverse_view() requires default_initializable<_Vw>
        = default;

        constexpr explicit reverse_view(_Vw _Range_) noexcept(is_nothrow_move_constructible_v<_Vw>) // strengthened
            : _Range(_STD move(_Range_)) {}

        _NODISCARD constexpr _Vw base() const& noexcept(
            is_nothrow_copy_constructible_v<_Vw>) /* strengthened */ requires copy_constructible<_Vw> {
            return _Range;
        }
        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        _NODISCARD constexpr _Rev_iter<_Vw> begin() {
            if constexpr (common_range<_Vw>) {
                return _Rev_iter<_Vw>{_RANGES end(_Range)};
            } else {
                if (this->_Has_cache()) {
                    return _Rev_iter<_Vw>{this->_Get_cache(_Range)};
                }

                iterator_t<_Vw> _First;
                if constexpr (sized_range<_Vw>) {
                    _First = _RANGES next(_RANGES begin(_Range), _RANGES distance(_Range));
                } else {
                    _First = _RANGES next(_RANGES begin(_Range), _RANGES end(_Range));
                }
                this->_Set_cache(_Range, _First);
                return _Rev_iter<_Vw>{_STD move(_First)};
            }
        }

        _NODISCARD constexpr auto begin() const noexcept(
            noexcept(_Rev_iter<const _Vw>{_RANGES end(_Range)})) /* strengthened */ requires common_range<const _Vw> {
            return _Rev_iter<const _Vw>{_RANGES end(_Range)};
        }

        _NODISCARD constexpr _Rev_iter<_Vw> end() noexcept(
            noexcept(_Rev_iter<_Vw>{_RANGES begin(_Range)})) /* strengthened */ {
            return _Rev_iter<_Vw>{_RANGES begin(_Range)};
        }
        _NODISCARD constexpr auto end() const noexcept(
            noexcept(_Rev_iter<const _Vw>{_RANGES begin(_Range)})) /* strengthened */ requires common_range<const _Vw> {
            return _Rev_iter<const _Vw>{_RANGES begin(_Range)};
        }

        _NODISCARD constexpr auto size() noexcept(
            noexcept(_RANGES size(_Range))) /* strengthened */ requires sized_range<_Vw> {
            return _RANGES size(_Range);
        }
        _NODISCARD constexpr auto size() const
            noexcept(noexcept(_RANGES size(_Range))) /* strengthened */ requires sized_range<const _Vw> {
            return _RANGES size(_Range);
        }
    };

    template <class _Rng>
    reverse_view(_Rng&&) -> reverse_view<views::all_t<_Rng>>;

    template <class _Rng>
    inline constexpr bool enable_borrowed_range<reverse_view<_Rng>> = enable_borrowed_range<_Rng>;

    namespace views {
        template <class _Rng>
        concept _Can_extract_base = requires(_Rng&& __r) {
            static_cast<_Rng&&>(__r).base();
        };

        template <class _Rng>
        concept _Can_reverse = requires(_Rng&& __r) {
            reverse_view{static_cast<_Rng&&>(__r)};
        };

        class _Reverse_fn : public _Pipe::_Base<_Reverse_fn> {
        private:
            enum class _St { _None, _Base, _Subrange_unsized, _Subrange_sized, _Reverse };

            template <class>
            static constexpr auto _Reversed_subrange = -1;

            template <class _It, subrange_kind _Ki>
            static constexpr auto
                _Reversed_subrange<subrange<reverse_iterator<_It>, reverse_iterator<_It>, _Ki>> = static_cast<int>(_Ki);

            template <class _Rng>
            _NODISCARD static _CONSTEVAL _Choice_t<_St> _Choose() noexcept {
                using _Ty = remove_cvref_t<_Rng>;

                if constexpr (_Is_specialization_v<_Ty, reverse_view>) {
                    if constexpr (_Can_extract_base<_Rng>) {
                        return {_St::_Base, noexcept(_STD declval<_Rng>().base())};
                    }
                } else if constexpr (_Reversed_subrange<_Ty> == 0) {
                    using _It = decltype(_STD declval<_Rng&>().begin().base());
                    return {_St::_Subrange_unsized,
                        noexcept(subrange<_It, _It, subrange_kind::unsized>{
                            _STD declval<_Rng&>().end().base(), _STD declval<_Rng&>().begin().base()})};
                } else if constexpr (_Reversed_subrange<_Ty> == 1) {
                    using _It = decltype(_STD declval<_Rng&>().begin().base());
                    return {_St::_Subrange_sized,
                        noexcept(subrange<_It, _It, subrange_kind::sized>{_STD declval<_Rng&>().end().base(),
                            _STD declval<_Rng&>().begin().base(), _STD declval<_Rng&>().size()})};
                } else if constexpr (_Can_reverse<_Rng>) {
                    return {_St::_Reverse, noexcept(reverse_view{_STD declval<_Rng>()})};
                }

                return {_St::_None};
            }

            template <class _Rng>
            static constexpr _Choice_t<_St> _Choice = _Choose<_Rng>();

        public:
            // clang-format off
            template <viewable_range _Rng>
                requires (_Choice<_Rng>._Strategy != _St::_None)
            _NODISCARD constexpr auto operator()(_Rng&& _Range) const noexcept(_Choice<_Rng>._No_throw) {
                // clang-format on
                constexpr _St _Strat = _Choice<_Rng>._Strategy;

                if constexpr (_Strat == _St::_Base) {
                    return _STD forward<_Rng>(_Range).base();
                } else if constexpr (_Strat == _St::_Subrange_unsized) {
                    return subrange{_Range.end().base(), _Range.begin().base()};
                } else if constexpr (_Strat == _St::_Subrange_sized) {
                    return subrange{_Range.end().base(), _Range.begin().base(), _Range.size()};
                } else if constexpr (_Strat == _St::_Reverse) {
                    return reverse_view{_STD forward<_Rng>(_Range)};
                } else {
                    static_assert(_Always_false<_Rng>, "Should be unreachable");
                }
            }
        };

        inline constexpr _Reverse_fn reverse;
    } // namespace views

    template <class _Tuple, size_t _Index>
    concept _Has_tuple_element = requires(_Tuple __t) {
        typename tuple_size<_Tuple>::type;
        requires _Index < tuple_size_v<_Tuple>;
        typename tuple_element_t<_Index, _Tuple>;
        // clang-format off
        { _STD get<_Index>(__t) } -> convertible_to<const tuple_element_t<_Index, _Tuple>&>;
        // clang-format on
    };

    template <class _Tuple, size_t _Index>
    concept _Returnable_element = is_reference_v<_Tuple> || move_constructible<tuple_element_t<_Index, _Tuple>>;

    // clang-format off
    template <input_range _Vw, size_t _Index>
        requires view<_Vw> && _Has_tuple_element<range_value_t<_Vw>, _Index>
            && _Has_tuple_element<remove_reference_t<range_reference_t<_Vw>>, _Index>
            && _Returnable_element<range_reference_t<_Vw>, _Index>
    class elements_view : public view_interface<elements_view<_Vw, _Index>> {
        // clang-format on
    private:
        /* [[no_unique_address]] */ _Vw _Range{};

        template <bool _Const>
        class _Sentinel;

        template <class _Base>
        struct _Category_base {};

        template <forward_range _Base>
        struct _Category_base<_Base> {
            using iterator_category =
                conditional_t<!is_lvalue_reference_v<decltype(_STD get<_Index>(*_STD declval<iterator_t<_Base>>()))>,
                    input_iterator_tag,
                    conditional_t<derived_from<_Iter_cat_t<iterator_t<_Base>>, random_access_iterator_tag>,
                        random_access_iterator_tag, _Iter_cat_t<iterator_t<_Base>>>>;
        };

        template <bool _Const>
        class _Iterator : public _Category_base<_Maybe_const<_Const, _Vw>> {
        private:
            template <bool>
            friend class _Iterator;
            template <bool>
            friend class _Sentinel;

            using _Base = _Maybe_const<_Const, _Vw>;

            iterator_t<_Base> _Current{};

        public:
            using iterator_concept = conditional_t<random_access_range<_Base>, random_access_iterator_tag,
                conditional_t<bidirectional_range<_Base>, bidirectional_iterator_tag,
                    conditional_t<forward_range<_Base>, forward_iterator_tag, input_iterator_tag>>>;
            using value_type       = remove_cvref_t<tuple_element_t<_Index, range_value_t<_Base>>>;
            using difference_type  = range_difference_t<_Base>;

            _Iterator() requires default_initializable<iterator_t<_Base>>
            = default;

            constexpr explicit _Iterator(iterator_t<_Base> _Current_) noexcept(
                is_nothrow_move_constructible_v<iterator_t<_Base>>) // strengthened
                : _Current{_STD move(_Current_)} {}

            // clang-format off
            constexpr _Iterator(_Iterator<!_Const> _It) noexcept(
                is_nothrow_constructible_v<iterator_t<_Base>, iterator_t<_Vw>>) // strengthened
                requires _Const && convertible_to<iterator_t<_Vw>, iterator_t<_Base>>
                : _Current{_STD move(_It._Current)} {}
            // clang-format on

            _NODISCARD constexpr const iterator_t<_Base>& base() const& noexcept /* strengthened */ {
                return _Current;
            }

            _NODISCARD constexpr iterator_t<_Base> base() && noexcept(
                is_nothrow_move_constructible_v<iterator_t<_Base>>) /* strengthened */ {
                return _STD move(_Current);
            }

            _NODISCARD constexpr decltype(auto) operator*() const
                noexcept(noexcept(_STD get<_Index>(*_Current))) /* strengthened */
                requires is_reference_v<range_reference_t<_Base>> {
                return _STD get<_Index>(*_Current);
            }

            _NODISCARD constexpr decltype(auto) operator*() const
                noexcept(is_nothrow_move_constructible_v<tuple_element_t<_Index, range_reference_t<_Base>>> //
                        && noexcept(_STD get<_Index>(*_Current))) /* strengthened */ {
                using _ElemTy = remove_cv_t<tuple_element_t<_Index, range_reference_t<_Base>>>;
                return static_cast<_ElemTy>(_STD get<_Index>(*_Current));
            }

            constexpr _Iterator& operator++() noexcept(noexcept(++_Current)) /* strengthened */ {
                ++_Current;
                return *this;
            }

            constexpr void operator++(int) noexcept(noexcept(++_Current)) /* strengthened */ {
                ++_Current;
            }

            constexpr _Iterator operator++(int) noexcept(
                noexcept(++_Current) && is_nothrow_copy_constructible_v<iterator_t<_Base>>) /* strengthened */
                requires forward_range<_Base> {
                auto _Tmp = *this;
                ++_Current;
                return _Tmp;
            }

            constexpr _Iterator& operator--() noexcept(noexcept(--_Current)) /* strengthened */
                requires bidirectional_range<_Base> {
                --_Current;
                return *this;
            }

            constexpr _Iterator operator--(int) noexcept(
                noexcept(--_Current) && is_nothrow_copy_constructible_v<iterator_t<_Base>>) /* strengthened */
                requires bidirectional_range<_Base> {
                auto _Tmp = *this;
                --_Current;
                return _Tmp;
            }

            constexpr void _Verify_offset(const difference_type _Off) const requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                (void) _Off;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 / _ITERATOR_DEBUG_LEVEL != 0 vvv
                if constexpr (_Offset_verifiable_v<iterator_t<_Base>>) {
                    _Current._Verify_offset(_Off);
                }
#endif // _ITERATOR_DEBUG_LEVEL == 0
            }

            constexpr _Iterator& operator+=(const difference_type _Off) noexcept(
                noexcept(_Current += _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _Current += _Off;
                return *this;
            }
            constexpr _Iterator& operator-=(const difference_type _Off) noexcept(
                noexcept(_Current -= _Off)) /* strengthened */ requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(-_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                _Current -= _Off;
                return *this;
            }

            _NODISCARD constexpr decltype(auto) operator[](const difference_type _Idx) const
                noexcept(noexcept(_STD get<_Index>(*(_Current + _Idx)))) /* strengthened */
                requires random_access_range<_Base> && is_reference_v<range_reference_t<_Base>> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(_Idx);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                return _STD get<_Index>(*(_Current + _Idx));
            }

            _NODISCARD constexpr decltype(auto) operator[](const difference_type _Idx) const
                noexcept(is_nothrow_move_constructible_v<tuple_element_t<_Index, range_reference_t<_Base>>> //
                        && noexcept(_STD get<_Index>(*(_Current + _Idx)))) /* strengthened */
                requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _Verify_offset(_Idx);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                using _ElemTy = remove_cv_t<tuple_element_t<_Index, range_reference_t<_Base>>>;
                return static_cast<_ElemTy>(_STD get<_Index>(*(_Current + _Idx)));
            }

            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current == _Right._Current)) /* strengthened */
                requires equality_comparable<iterator_t<_Base>> {
                return _Left._Current == _Right._Current;
            }

            _NODISCARD_FRIEND constexpr bool operator<(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return _Left._Current < _Right._Current;
            }
            _NODISCARD_FRIEND constexpr bool operator>(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return _Right < _Left;
            }
            _NODISCARD_FRIEND constexpr bool operator<=(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return !(_Right < _Left);
            }
            _NODISCARD_FRIEND constexpr bool operator>=(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current < _Right._Current)) /* strengthened */ requires random_access_range<_Base> {
                return !(_Left < _Right);
            }

            // clang-format off
            _NODISCARD_FRIEND constexpr auto operator<=>(const _Iterator& _Left, const _Iterator& _Right) noexcept(
                noexcept(_Left._Current <=> _Right._Current)) /* strengthened */
                requires random_access_range<_Base> && three_way_comparable<iterator_t<_Base>> {
                // clang-format on
                return _Left._Current <=> _Right._Current;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator+(const _Iterator& _It, const difference_type _Off) noexcept(
                noexcept(_STD declval<iterator_t<_Base>&>() += _Off)
                && is_nothrow_copy_constructible_v<iterator_t<_Base>>) /* strengthened */
                requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Verify_offset(_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                auto _Copy = _It;
                _Copy._Current += _Off;
                return _Copy;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator+(const difference_type _Off, const _Iterator& _It) noexcept(
                noexcept(_STD declval<iterator_t<_Base>&>() += _Off)
                && is_nothrow_copy_constructible_v<iterator_t<_Base>>) /* strengthened */
                requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Verify_offset(_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                auto _Copy = _It;
                _Copy._Current += _Off;
                return _Copy;
            }

            _NODISCARD_FRIEND constexpr _Iterator operator-(const _Iterator& _It, const difference_type _Off) noexcept(
                noexcept(_STD declval<iterator_t<_Base>&>() -= _Off)
                && is_nothrow_copy_constructible_v<iterator_t<_Base>>) /* strengthened */
                requires random_access_range<_Base> {
#if _ITERATOR_DEBUG_LEVEL != 0
                _It._Verify_offset(-_Off);
#endif // _ITERATOR_DEBUG_LEVEL != 0
                auto _Copy = _It;
                _Copy._Current -= _Off;
                return _Copy;
            }

            _NODISCARD_FRIEND constexpr difference_type operator-(const _Iterator& _Left,
                const _Iterator& _Right) noexcept(noexcept(_Left._Current - _Right._Current)) /* strengthened */
                requires sized_sentinel_for<iterator_t<_Base>, iterator_t<_Base>> {
                return _Left._Current - _Right._Current;
            }
        };

        template <bool _Const>
        class _Sentinel {
        private:
            template <bool>
            friend class _Sentinel;

            using _Base = _Maybe_const<_Const, _Vw>;
            template <bool _OtherConst>
            using _Maybe_const_iter = iterator_t<_Maybe_const<_OtherConst, _Vw>>;

            sentinel_t<_Base> _Last{};

            template <bool _OtherConst>
            _NODISCARD static constexpr const _Maybe_const_iter<_OtherConst>& _Get_current(
                const _Iterator<_OtherConst>& _It) noexcept {
                return _It._Current;
            }

        public:
            _Sentinel() = default;
            constexpr explicit _Sentinel(sentinel_t<_Base> _Last_) noexcept(
                is_nothrow_move_constructible_v<sentinel_t<_Base>>) // strengthened
                : _Last(_STD move(_Last_)) {}

            // clang-format off
            constexpr _Sentinel(_Sentinel<!_Const> _Se)
                noexcept(is_nothrow_constructible_v<sentinel_t<_Base>, sentinel_t<_Vw>>) // strengthened
                requires _Const && convertible_to<sentinel_t<_Vw>, sentinel_t<_Base>>
                : _Last(_STD move(_Se._Last)) {}
            // clang-format on

            _NODISCARD constexpr sentinel_t<_Base> base() const
                noexcept(is_nothrow_copy_constructible_v<sentinel_t<_Base>>) /* strengthened */ {
                return _Last;
            }

            // clang-format off
            template <bool _OtherConst>
                requires sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr bool operator==(const _Iterator<_OtherConst>& _Left,
                const _Sentinel& _Right) noexcept(noexcept(_Get_current(_Left) == _Right._Last)) /* strengthened */ {
                // clang-format on
                return _Get_current(_Left) == _Right._Last;
            }

            // clang-format off
            template <bool _OtherConst>
                requires sized_sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr range_difference_t<_Maybe_const<_OtherConst, _Vw>> operator-(
                const _Iterator<_OtherConst>& _Left, const _Sentinel& _Right) noexcept(
                noexcept(_Get_current(_Left) - _Right._Last)) /* strengthened */ {
                // clang-format on
                return _Get_current(_Left) - _Right._Last;
            }

            // clang-format off
            template <bool _OtherConst>
                requires sized_sentinel_for<sentinel_t<_Base>, _Maybe_const_iter<_OtherConst>>
            _NODISCARD_FRIEND constexpr range_difference_t<_Maybe_const<_OtherConst, _Vw>> operator-(
                const _Sentinel& _Left, const _Iterator<_OtherConst>& _Right) noexcept(
                noexcept(_Left._Last - _Get_current(_Right))) /* strengthened */ {
                // clang-format on
                return _Left._Last - _Get_current(_Right);
            }
        };

    public:
        elements_view() requires default_initializable<_Vw>
        = default;

        constexpr explicit elements_view(_Vw _Range_) noexcept(is_nothrow_move_constructible_v<_Vw>) // strengthened
            : _Range(_STD move(_Range_)) {}

        _NODISCARD constexpr _Vw base() const& noexcept(is_nothrow_copy_constructible_v<_Vw>) /* strengthened */
            requires copy_constructible<_Vw> {
            return _Range;
        }

        _NODISCARD constexpr _Vw base() && noexcept(is_nothrow_move_constructible_v<_Vw>) /* strengthened */ {
            return _STD move(_Range);
        }

        // clang-format off
        _NODISCARD constexpr _Iterator<false> begin() noexcept(
            noexcept(_RANGES begin(_Range)) && is_nothrow_move_constructible_v<iterator_t<_Vw>>) /* strengthened */
            requires (!_Simple_view<_Vw>) {
            // clang-format on
            return _Iterator<false>{_RANGES begin(_Range)};
        }

        _NODISCARD constexpr _Iterator<true> begin() const
            noexcept(noexcept(_RANGES begin(_Range))
                     && is_nothrow_move_constructible_v<iterator_t<const _Vw>>) /* strengthened */
            requires range<const _Vw> {
            return _Iterator<true>{_RANGES begin(_Range)};
        }

        // clang-format off
        _NODISCARD constexpr auto end() noexcept(noexcept(
            _RANGES end(_Range)) && is_nothrow_move_constructible_v<sentinel_t<_Vw>>) /* strengthened */
            requires (!_Simple_view<_Vw>) {
            // clang-format on
            if constexpr (common_range<_Vw>) {
                return _Iterator<false>{_RANGES end(_Range)};
            } else {
                return _Sentinel<false>{_RANGES end(_Range)};
            }
        }

        // clang-format off
        _NODISCARD constexpr auto end() const noexcept(noexcept(
            _RANGES end(_Range)) && is_nothrow_move_constructible_v<sentinel_t<const _Vw>>) /* strengthened */
            requires range<const _Vw> {
            // clang-format on
            if constexpr (common_range<const _Vw>) {
                return _Iterator<true>{_RANGES end(_Range)};
            } else {
                return _Sentinel<true>{_RANGES end(_Range)};
            }
        }

        _NODISCARD constexpr auto size() noexcept(noexcept(_RANGES size(_Range))) /* strengthened */
            requires sized_range<_Vw> {
            return _RANGES size(_Range);
        }
        _NODISCARD constexpr auto size() const noexcept(noexcept(_RANGES size(_Range))) /* strengthened */
            requires sized_range<const _Vw> {
            return _RANGES size(_Range);
        }
    };

    template <class _Rng, size_t _Index>
    inline constexpr bool enable_borrowed_range<elements_view<_Rng, _Index>> = enable_borrowed_range<_Rng>;

    template <class _Rng>
    using keys_view = elements_view<_Rng, 0>;
    template <class _Rng>
    using values_view = elements_view<_Rng, 1>;

    namespace views {
        template <size_t _Index>
        class _Elements_fn : public _Pipe::_Base<_Elements_fn<_Index>> {
        public:
            template <viewable_range _Rng>
            _NODISCARD constexpr auto operator()(_Rng&& _Range) const noexcept(
                noexcept(elements_view<views::all_t<_Rng>, _Index>{_STD forward<_Rng>(_Range)})) requires requires {
                elements_view<views::all_t<_Rng>, _Index>{static_cast<_Rng&&>(_Range)};
            }
            { return elements_view<views::all_t<_Rng>, _Index>{_STD forward<_Rng>(_Range)}; }
        };

        template <size_t _Index>
        inline constexpr _Elements_fn<_Index> elements;
        inline constexpr auto keys   = elements<0>;
        inline constexpr auto values = elements<1>;
    } // namespace views
} // namespace ranges

namespace views = ranges::views;

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // __cpp_lib_ranges
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _RANGES_
