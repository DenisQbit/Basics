// optional standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _OPTIONAL_
#define _OPTIONAL_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR
#if !_HAS_CXX17
#pragma message("The contents of <optional> are available only with C++17 or later.")
#else // ^^^ !_HAS_CXX17 / _HAS_CXX17 vvv
#if _HAS_CXX20
#include <compare>
#endif // _HAS_CXX20
#include <exception>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <xmemory>
#include <xsmf_control.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

struct nullopt_t { // no-value state indicator
    struct _Tag {};
    constexpr explicit nullopt_t(_Tag) {}
};
inline constexpr nullopt_t nullopt{nullopt_t::_Tag{}};

class bad_optional_access : public exception {
public:
    _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept override {
        return "Bad optional access";
    }

#if !_HAS_EXCEPTIONS
protected:
    virtual void _Doraise() const override { // perform class-specific exception handling
        _RAISE(*this);
    }
#endif // !_HAS_EXCEPTIONS
};

[[noreturn]] inline void _Throw_bad_optional_access() {
    _THROW(bad_optional_access{});
}

template <class _Ty, bool = is_trivially_destructible_v<_Ty>>
struct _Optional_destruct_base { // either contains a value of _Ty or is empty (trivial destructor)
    union {
        _Nontrivial_dummy_type _Dummy;
        remove_const_t<_Ty> _Value;
    };
    bool _Has_value;

    constexpr _Optional_destruct_base() noexcept : _Dummy{}, _Has_value{false} {} // initialize an empty optional

    template <class... _Types>
    constexpr explicit _Optional_destruct_base(in_place_t, _Types&&... _Args)
        : _Value(_STD forward<_Types>(_Args)...), _Has_value{true} {} // initialize contained value with _Args...

    _CONSTEXPR20 void reset() noexcept {
        _Has_value = false;
    }
};

template <class _Ty>
struct _Optional_destruct_base<_Ty, false> { // either contains a value of _Ty or is empty (non-trivial destructor)
    union {
        _Nontrivial_dummy_type _Dummy;
        remove_const_t<_Ty> _Value;
    };
    bool _Has_value;

    _CONSTEXPR20 ~_Optional_destruct_base() noexcept {
        if (_Has_value) {
            _Destroy_in_place(_Value);
        }
    }

    constexpr _Optional_destruct_base() noexcept : _Dummy{}, _Has_value{false} {} // initialize an empty optional

    template <class... _Types>
    constexpr explicit _Optional_destruct_base(in_place_t, _Types&&... _Args)
        : _Value(_STD forward<_Types>(_Args)...), _Has_value{true} {} // initialize contained value with _Args...

    _Optional_destruct_base(const _Optional_destruct_base&) = default;
    _Optional_destruct_base(_Optional_destruct_base&&)      = default;
    _Optional_destruct_base& operator=(const _Optional_destruct_base&) = default;
    _Optional_destruct_base& operator=(_Optional_destruct_base&&) = default;

    _CONSTEXPR20 void reset() noexcept {
        if (_Has_value) {
            _Destroy_in_place(_Value);
            _Has_value = false;
        }
    }
};

template <class _Ty>
struct _Optional_construct_base : _Optional_destruct_base<_Ty> {
    // Provide non-trivial SMF implementations for the _SMF_control machinery
    using _Optional_destruct_base<_Ty>::_Optional_destruct_base;

    template <class... _Types>
    _CONSTEXPR20 _Ty& _Construct(_Types&&... _Args) {
        // transition from the empty to the value-containing state
        _STL_INTERNAL_CHECK(!this->_Has_value);
        _Construct_in_place(this->_Value, _STD forward<_Types>(_Args)...);
        this->_Has_value = true;
        return this->_Value;
    }

    template <class _Ty2>
    _CONSTEXPR20 void _Assign(_Ty2&& _Right) { // assign / initialize the contained value from _Right
        if (this->_Has_value) {
            this->_Value = _STD forward<_Ty2>(_Right);
        } else {
            _Construct(_STD forward<_Ty2>(_Right));
        }
    }

    template <class _Self>
    _CONSTEXPR20 void _Construct_from(_Self&& _Right) noexcept(
        is_nothrow_constructible_v<_Ty, decltype((_STD forward<_Self>(_Right)._Value))>) {
        // initialize contained value from _Right iff it contains a value
        if (_Right._Has_value) {
            _Construct(_STD forward<_Self>(_Right)._Value);
        }
    }

    template <class _Self>
    _CONSTEXPR20 void _Assign_from(_Self&& _Right) noexcept(
        is_nothrow_constructible_v<_Ty, decltype((_STD forward<_Self>(_Right)._Value))>&&
            is_nothrow_assignable_v<_Ty&, decltype((_STD forward<_Self>(_Right)._Value))>) {
        // assign/initialize/destroy contained value from _Right
        if (_Right._Has_value) {
            _Assign(_STD forward<_Self>(_Right)._Value);
        } else {
            this->reset();
        }
    }
};

template <class _Ty>
class optional : private _SMF_control<_Optional_construct_base<_Ty>, _Ty> {
private:
    using _Mybase = _SMF_control<_Optional_construct_base<_Ty>, _Ty>;

public:
    static_assert(!_Is_any_of_v<remove_cv_t<_Ty>, nullopt_t, in_place_t>,
        "T in optional<T> must be a type other than nullopt_t or in_place_t (N4828 [optional.optional]/3).");
    static_assert(is_object_v<_Ty> && is_destructible_v<_Ty> && !is_array_v<_Ty>,
        "T in optional<T> must meet the Cpp17Destructible requirements (N4828 [optional.optional]/3).");

    using value_type = _Ty;

    constexpr optional() noexcept {}
    constexpr optional(nullopt_t) noexcept {}

    template <class... _Types, enable_if_t<is_constructible_v<_Ty, _Types...>, int> = 0>
    constexpr explicit optional(in_place_t, _Types&&... _Args) : _Mybase(in_place, _STD forward<_Types>(_Args)...) {}

    template <class _Elem, class... _Types,
        enable_if_t<is_constructible_v<_Ty, initializer_list<_Elem>&, _Types...>, int> = 0>
    constexpr explicit optional(in_place_t, initializer_list<_Elem> _Ilist, _Types&&... _Args)
        : _Mybase(in_place, _Ilist, _STD forward<_Types>(_Args)...) {}

    template <class _Ty2>
    using _AllowDirectConversion = bool_constant<conjunction_v<negation<is_same<_Remove_cvref_t<_Ty2>, optional>>,
        negation<is_same<_Remove_cvref_t<_Ty2>, in_place_t>>, is_constructible<_Ty, _Ty2>>>;

#if _HAS_CONDITIONAL_EXPLICIT
    template <class _Ty2 = _Ty, enable_if_t<_AllowDirectConversion<_Ty2>::value, int> = 0>
    constexpr explicit(!is_convertible_v<_Ty2, _Ty>) optional(_Ty2&& _Right)
        : _Mybase(in_place, _STD forward<_Ty2>(_Right)) {}
#else // ^^^ _HAS_CONDITIONAL_EXPLICIT ^^^ / vvv !_HAS_CONDITIONAL_EXPLICIT vvv
    template <class _Ty2                                                                         = _Ty,
        enable_if_t<conjunction_v<_AllowDirectConversion<_Ty2>, is_convertible<_Ty2, _Ty>>, int> = 0>
    constexpr optional(_Ty2&& _Right) : _Mybase(in_place, _STD forward<_Ty2>(_Right)) {}
    template <class _Ty2                                                                                   = _Ty,
        enable_if_t<conjunction_v<_AllowDirectConversion<_Ty2>, negation<is_convertible<_Ty2, _Ty>>>, int> = 0>
    constexpr explicit optional(_Ty2&& _Right) : _Mybase(in_place, _STD forward<_Ty2>(_Right)) {}
#endif // ^^^ !_HAS_CONDITIONAL_EXPLICIT ^^^

    template <class _Ty2>
    struct _AllowUnwrapping : bool_constant<!disjunction_v<is_same<_Ty, _Ty2>, is_constructible<_Ty, optional<_Ty2>&>,
                                  is_constructible<_Ty, const optional<_Ty2>&>,
                                  is_constructible<_Ty, const optional<_Ty2>>, is_constructible<_Ty, optional<_Ty2>>,
                                  is_convertible<optional<_Ty2>&, _Ty>, is_convertible<const optional<_Ty2>&, _Ty>,
                                  is_convertible<const optional<_Ty2>, _Ty>, is_convertible<optional<_Ty2>, _Ty>>> {};

#if _HAS_CONDITIONAL_EXPLICIT
    template <class _Ty2,
        enable_if_t<conjunction_v<_AllowUnwrapping<_Ty2>, is_constructible<_Ty, const _Ty2&>>, int> = 0>
    _CONSTEXPR20 explicit(!is_convertible_v<const _Ty2&, _Ty>) optional(const optional<_Ty2>& _Right) {
        if (_Right) {
            this->_Construct(*_Right);
        }
    }
#else // ^^^ _HAS_CONDITIONAL_EXPLICIT ^^^ / vvv !_HAS_CONDITIONAL_EXPLICIT vvv
    template <class _Ty2, enable_if_t<conjunction_v<_AllowUnwrapping<_Ty2>, is_constructible<_Ty, const _Ty2&>,
                                          is_convertible<const _Ty2&, _Ty>>,
                              int> = 0>
    optional(const optional<_Ty2>& _Right) {
        if (_Right) {
            this->_Construct(*_Right);
        }
    }
    template <class _Ty2, enable_if_t<conjunction_v<_AllowUnwrapping<_Ty2>, is_constructible<_Ty, const _Ty2&>,
                                          negation<is_convertible<const _Ty2&, _Ty>>>,
                              int> = 0>
    explicit optional(const optional<_Ty2>& _Right) {
        if (_Right) {
            this->_Construct(*_Right);
        }
    }
#endif // ^^^ !_HAS_CONDITIONAL_EXPLICIT ^^^

#if _HAS_CONDITIONAL_EXPLICIT
    template <class _Ty2, enable_if_t<conjunction_v<_AllowUnwrapping<_Ty2>, is_constructible<_Ty, _Ty2>>, int> = 0>
    _CONSTEXPR20 explicit(!is_convertible_v<_Ty2, _Ty>) optional(optional<_Ty2>&& _Right) {
        if (_Right) {
            this->_Construct(_STD move(*_Right));
        }
    }
#else // ^^^ _HAS_CONDITIONAL_EXPLICIT ^^^ / vvv !_HAS_CONDITIONAL_EXPLICIT vvv
    template <class _Ty2,
        enable_if_t<conjunction_v<_AllowUnwrapping<_Ty2>, is_constructible<_Ty, _Ty2>, is_convertible<_Ty2, _Ty>>,
            int> = 0>
    optional(optional<_Ty2>&& _Right) {
        if (_Right) {
            this->_Construct(_STD move(*_Right));
        }
    }
    template <class _Ty2, enable_if_t<conjunction_v<_AllowUnwrapping<_Ty2>, is_constructible<_Ty, _Ty2>,
                                          negation<is_convertible<_Ty2, _Ty>>>,
                              int> = 0>
    explicit optional(optional<_Ty2>&& _Right) {
        if (_Right) {
            this->_Construct(_STD move(*_Right));
        }
    }
#endif // ^^^ !_HAS_CONDITIONAL_EXPLICIT ^^^

    _CONSTEXPR20 optional& operator=(nullopt_t) noexcept {
        reset();
        return *this;
    }

    template <class _Ty2 = _Ty, enable_if_t<conjunction_v<negation<is_same<optional, _Remove_cvref_t<_Ty2>>>,
                                                negation<conjunction<is_scalar<_Ty>, is_same<_Ty, decay_t<_Ty2>>>>,
                                                is_constructible<_Ty, _Ty2>, is_assignable<_Ty&, _Ty2>>,
                                    int> = 0>
    _CONSTEXPR20 optional& operator=(_Ty2&& _Right) {
        this->_Assign(_STD forward<_Ty2>(_Right));
        return *this;
    }

    template <class _Ty2>
    struct _AllowUnwrappingAssignment
        : bool_constant<!disjunction_v<is_same<_Ty, _Ty2>, is_assignable<_Ty&, optional<_Ty2>&>,
              is_assignable<_Ty&, const optional<_Ty2>&>, is_assignable<_Ty&, const optional<_Ty2>>,
              is_assignable<_Ty&, optional<_Ty2>>>> {};

    template <class _Ty2, enable_if_t<conjunction_v<_AllowUnwrappingAssignment<_Ty2>,
                                          is_constructible<_Ty, const _Ty2&>, is_assignable<_Ty&, const _Ty2&>>,
                              int> = 0>
    _CONSTEXPR20 optional& operator=(const optional<_Ty2>& _Right) {
        if (_Right) {
            this->_Assign(*_Right);
        } else {
            reset();
        }

        return *this;
    }

    template <class _Ty2, enable_if_t<conjunction_v<_AllowUnwrappingAssignment<_Ty2>, is_constructible<_Ty, _Ty2>,
                                          is_assignable<_Ty&, _Ty2>>,
                              int> = 0>
    _CONSTEXPR20 optional& operator=(optional<_Ty2>&& _Right) {
        if (_Right) {
            this->_Assign(_STD move(*_Right));
        } else {
            reset();
        }

        return *this;
    }

    template <class... _Types>
    _CONSTEXPR20 _Ty& emplace(_Types&&... _Args) {
        reset();
        return this->_Construct(_STD forward<_Types>(_Args)...);
    }

    template <class _Elem, class... _Types,
        enable_if_t<is_constructible_v<_Ty, initializer_list<_Elem>&, _Types...>, int> = 0>
    _CONSTEXPR20 _Ty& emplace(initializer_list<_Elem> _Ilist, _Types&&... _Args) {
        reset();
        return this->_Construct(_Ilist, _STD forward<_Types>(_Args)...);
    }

    _CONSTEXPR20 void swap(optional& _Right) noexcept(
        is_nothrow_move_constructible_v<_Ty>&& is_nothrow_swappable_v<_Ty>) {
        static_assert(is_move_constructible_v<_Ty>,
            "optional<T>::swap requires T to be move constructible (N4828 [optional.swap]/1).");
        static_assert(!is_move_constructible_v<_Ty> || is_swappable_v<_Ty>,
            "optional<T>::swap requires T to be swappable (N4828 [optional.swap]/1).");
        if constexpr (_Is_trivially_swappable_v<_Ty>) {
            using _TrivialBaseTy = _Optional_destruct_base<_Ty>;
            _STD swap(static_cast<_TrivialBaseTy&>(*this), static_cast<_TrivialBaseTy&>(_Right));
        } else {
            const bool _Engaged = this->_Has_value;
            if (_Engaged == _Right._Has_value) {
                if (_Engaged) {
                    _Swap_adl(**this, *_Right);
                }
            } else {
                optional& _Source = _Engaged ? *this : _Right;
                optional& _Target = _Engaged ? _Right : *this;
                _Target._Construct(_STD move(*_Source));
                _Source.reset();
            }
        }
    }

    _NODISCARD constexpr const _Ty* operator->() const {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(this->_Has_value, "Cannot access value of empty optional");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _STD addressof(this->_Value);
    }
    _NODISCARD constexpr _Ty* operator->() {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(this->_Has_value, "Cannot access value of empty optional");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _STD addressof(this->_Value);
    }

    _NODISCARD constexpr const _Ty& operator*() const& {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(this->_Has_value, "Cannot access value of empty optional");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return this->_Value;
    }
    _NODISCARD constexpr _Ty& operator*() & {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(this->_Has_value, "Cannot access value of empty optional");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return this->_Value;
    }
    _NODISCARD constexpr _Ty&& operator*() && {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(this->_Has_value, "Cannot access value of empty optional");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _STD move(this->_Value);
    }
    _NODISCARD constexpr const _Ty&& operator*() const&& {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(this->_Has_value, "Cannot access value of empty optional");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _STD move(this->_Value);
    }

    constexpr explicit operator bool() const noexcept {
        return this->_Has_value;
    }
    _NODISCARD constexpr bool has_value() const noexcept {
        return this->_Has_value;
    }

    _NODISCARD constexpr const _Ty& value() const& {
        if (!this->_Has_value) {
            _Throw_bad_optional_access();
        }

        return this->_Value;
    }
    _NODISCARD constexpr _Ty& value() & {
        if (!this->_Has_value) {
            _Throw_bad_optional_access();
        }

        return this->_Value;
    }
    _NODISCARD constexpr _Ty&& value() && {
        if (!this->_Has_value) {
            _Throw_bad_optional_access();
        }

        return _STD move(this->_Value);
    }
    _NODISCARD constexpr const _Ty&& value() const&& {
        if (!this->_Has_value) {
            _Throw_bad_optional_access();
        }

        return _STD move(this->_Value);
    }

    template <class _Ty2>
    _NODISCARD constexpr remove_cv_t<_Ty> value_or(_Ty2&& _Right) const& {
        static_assert(is_convertible_v<const _Ty&, remove_cv_t<_Ty>>,
            "The const overload of optional<T>::value_or requires const T& to be convertible to remove_cv_t<T> "
            "(N4885 [optional.observe]/17 as modified by LWG-3424).");
        static_assert(is_convertible_v<_Ty2, _Ty>,
            "optional<T>::value_or(U) requires U to be convertible to T (N4828 [optional.observe]/18).");

        if (this->_Has_value) {
            return this->_Value;
        }

        return static_cast<remove_cv_t<_Ty>>(_STD forward<_Ty2>(_Right));
    }
    template <class _Ty2>
    _NODISCARD constexpr remove_cv_t<_Ty> value_or(_Ty2&& _Right) && {
        static_assert(is_convertible_v<_Ty, remove_cv_t<_Ty>>,
            "The rvalue overload of optional<T>::value_or requires T to be convertible to remove_cv_t<T> "
            "(N4885 [optional.observe]/19 as modified by LWG-3424).");
        static_assert(is_convertible_v<_Ty2, _Ty>,
            "optional<T>::value_or(U) requires U to be convertible to T (N4828 [optional.observe]/20).");

        if (this->_Has_value) {
            return _STD move(this->_Value);
        }

        return static_cast<remove_cv_t<_Ty>>(_STD forward<_Ty2>(_Right));
    }

    using _Mybase::reset;
};

template <class _Ty>
optional(_Ty) -> optional<_Ty>;

template <class _Ty1, class _Ty2>
_NODISCARD constexpr bool operator==(const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    const bool _Left_has_value = _Left.has_value();
    return _Left_has_value == _Right.has_value() && (!_Left_has_value || *_Left == *_Right);
}

template <class _Ty1, class _Ty2>
_NODISCARD constexpr bool operator!=(const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    const bool _Left_has_value = _Left.has_value();
    return _Left_has_value != _Right.has_value() || (_Left_has_value && *_Left != *_Right);
}

template <class _Ty1, class _Ty2>
_NODISCARD constexpr bool operator<(const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    return _Right.has_value() && (!_Left.has_value() || *_Left < *_Right);
}

template <class _Ty1, class _Ty2>
_NODISCARD constexpr bool operator>(const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    return _Left.has_value() && (!_Right.has_value() || *_Left > *_Right);
}

template <class _Ty1, class _Ty2>
_NODISCARD constexpr bool operator<=(const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    return !_Left.has_value() || (_Right.has_value() && *_Left <= *_Right);
}

template <class _Ty1, class _Ty2>
_NODISCARD constexpr bool operator>=(const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    return !_Right.has_value() || (_Left.has_value() && *_Left >= *_Right);
}

#ifdef __cpp_lib_concepts
template <class _Ty1, three_way_comparable_with<_Ty1> _Ty2>
_NODISCARD constexpr compare_three_way_result_t<_Ty1, _Ty2> operator<=>(
    const optional<_Ty1>& _Left, const optional<_Ty2>& _Right) {
    if (_Left && _Right) {
        return *_Left <=> *_Right;
    }

    return _Left.has_value() <=> _Right.has_value();
}
#endif // __cpp_lib_concepts

template <class _Ty>
_NODISCARD constexpr bool operator==(const optional<_Ty>& _Left, nullopt_t) noexcept {
    return !_Left.has_value();
}

#if _HAS_CXX20
template <class _Ty>
_NODISCARD constexpr strong_ordering operator<=>(const optional<_Ty>& _Left, nullopt_t) noexcept {
    return _Left.has_value() <=> false;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
template <class _Ty>
_NODISCARD constexpr bool operator==(nullopt_t, const optional<_Ty>& _Right) noexcept {
    return !_Right.has_value();
}

template <class _Ty>
_NODISCARD constexpr bool operator!=(const optional<_Ty>& _Left, nullopt_t) noexcept {
    return _Left.has_value();
}
template <class _Ty>
_NODISCARD constexpr bool operator!=(nullopt_t, const optional<_Ty>& _Right) noexcept {
    return _Right.has_value();
}

template <class _Ty>
_NODISCARD constexpr bool operator<(const optional<_Ty>&, nullopt_t) noexcept {
    return false;
}
template <class _Ty>
_NODISCARD constexpr bool operator<(nullopt_t, const optional<_Ty>& _Right) noexcept {
    return _Right.has_value();
}

template <class _Ty>
_NODISCARD constexpr bool operator>(const optional<_Ty>& _Left, nullopt_t) noexcept {
    return _Left.has_value();
}
template <class _Ty>
_NODISCARD constexpr bool operator>(nullopt_t, const optional<_Ty>&) noexcept {
    return false;
}

template <class _Ty>
_NODISCARD constexpr bool operator<=(const optional<_Ty>& _Left, nullopt_t) noexcept {
    return !_Left.has_value();
}
template <class _Ty>
_NODISCARD constexpr bool operator<=(nullopt_t, const optional<_Ty>&) noexcept {
    return true;
}

template <class _Ty>
_NODISCARD constexpr bool operator>=(const optional<_Ty>&, nullopt_t) noexcept {
    return true;
}
template <class _Ty>
_NODISCARD constexpr bool operator>=(nullopt_t, const optional<_Ty>& _Right) noexcept {
    return !_Right.has_value();
}
#endif // !_HAS_CXX20

template <class _Ty>
using _Enable_if_bool_convertible = enable_if_t<is_convertible_v<_Ty, bool>, int>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_equal =
    _Enable_if_bool_convertible<decltype(_STD declval<const _Lhs&>() == _STD declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_not_equal =
    _Enable_if_bool_convertible<decltype(_STD declval<const _Lhs&>() != _STD declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_less =
    _Enable_if_bool_convertible<decltype(_STD declval<const _Lhs&>() < _STD declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_greater =
    _Enable_if_bool_convertible<decltype(_STD declval<const _Lhs&>() > _STD declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_less_equal =
    _Enable_if_bool_convertible<decltype(_STD declval<const _Lhs&>() <= _STD declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_greater_equal =
    _Enable_if_bool_convertible<decltype(_STD declval<const _Lhs&>() >= _STD declval<const _Rhs&>())>;

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator==(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    return _Left ? *_Left == _Right : false;
}

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator==(const _Ty1& _Left, const optional<_Ty2>& _Right) {
    return _Right ? _Left == *_Right : false;
}

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_not_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator!=(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    return _Left ? *_Left != _Right : true;
}
template <class _Ty1, class _Ty2, _Enable_if_comparable_with_not_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator!=(const _Ty1& _Left, const optional<_Ty2>& _Right) {
    return _Right ? _Left != *_Right : true;
}

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator<(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    return _Left ? *_Left < _Right : true;
}
template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator<(const _Ty1& _Left, const optional<_Ty2>& _Right) {
    return _Right ? _Left < *_Right : false;
}

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator>(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    return _Left ? *_Left > _Right : false;
}
template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator>(const _Ty1& _Left, const optional<_Ty2>& _Right) {
    return _Right ? _Left > *_Right : true;
}

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator<=(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    return _Left ? *_Left <= _Right : true;
}
template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator<=(const _Ty1& _Left, const optional<_Ty2>& _Right) {
    return _Right ? _Left <= *_Right : false;
}

template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator>=(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    return _Left ? *_Left >= _Right : false;
}
template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater_equal<_Ty1, _Ty2> = 0>
_NODISCARD constexpr bool operator>=(const _Ty1& _Left, const optional<_Ty2>& _Right) {
    return _Right ? _Left >= *_Right : true;
}

#ifdef __cpp_lib_concepts
// clang-format off
template <class _Ty1, class _Ty2>
    requires (!_Is_specialization_v<_Ty2, optional>) // LWG-3566
        && three_way_comparable_with<_Ty1, _Ty2>
_NODISCARD constexpr compare_three_way_result_t<_Ty1, _Ty2>
    operator<=>(const optional<_Ty1>& _Left, const _Ty2& _Right) {
    // clang-format on
    if (_Left) {
        return *_Left <=> _Right;
    }

    return strong_ordering::less;
}
#endif // __cpp_lib_concepts

template <class _Ty, enable_if_t<is_move_constructible_v<_Ty> && is_swappable_v<_Ty>, int> = 0>
_CONSTEXPR20 void swap(optional<_Ty>& _Left, optional<_Ty>& _Right) noexcept(noexcept(_Left.swap(_Right))) {
    _Left.swap(_Right);
}

template <class _Ty>
_NODISCARD constexpr optional<decay_t<_Ty>> make_optional(_Ty&& _Value) {
    return optional<decay_t<_Ty>>{_STD forward<_Ty>(_Value)};
}
template <class _Ty, class... _Types>
_NODISCARD constexpr optional<_Ty> make_optional(_Types&&... _Args) {
    return optional<_Ty>{in_place, _STD forward<_Types>(_Args)...};
}
template <class _Ty, class _Elem, class... _Types>
_NODISCARD constexpr optional<_Ty> make_optional(initializer_list<_Elem> _Ilist, _Types&&... _Args) {
    return optional<_Ty>{in_place, _Ilist, _STD forward<_Types>(_Args)...};
}

template <class _Ty>
struct hash<optional<_Ty>>
    : _Conditionally_enabled_hash<optional<_Ty>, is_default_constructible_v<hash<remove_const_t<_Ty>>>> {
    static size_t _Do_hash(const optional<_Ty>& _Opt) noexcept(_Is_nothrow_hashable<remove_const_t<_Ty>>::value) {
        constexpr size_t _Unspecified_value = 0;
        if (_Opt) {
            return hash<remove_const_t<_Ty>>{}(*_Opt);
        }

        return _Unspecified_value;
    }
};

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _HAS_CXX17
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _OPTIONAL_
