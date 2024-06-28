// variant standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _VARIANT_
#define _VARIANT_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX17
#pragma message("The contents of <variant> are available only with C++17 or later.")
#else // ^^^ !_HAS_CXX17 / _HAS_CXX17 vvv
#include <exception>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <xmemory>
#include <xsmf_control.h>
#include <xstddef>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

template <class...>
struct _All_same : true_type {};
template <class _First, class... _Rest>
struct _All_same<_First, _Rest...> : bool_constant<conjunction_v<is_same<_First, _Rest>...>> {}; // variadic is_same

template <class _To, class... _From>
struct _Convertible_from_all : bool_constant<conjunction_v<is_convertible<_From, _To>...>> {
    // variadic is_convertible
};

template <class...>
struct _Meta_list {}; // a sequence of types
struct _Meta_nil {};

template <class _List>
struct _Meta_front_;
template <class _List>
using _Meta_front =
    // extract the first type in a sequence (head of list)
    typename _Meta_front_<_List>::type;

template <template <class...> class _List, class _First, class... _Rest>
struct _Meta_front_<_List<_First, _Rest...>> {
    using type = _First;
};

template <class _List>
struct _Meta_pop_front_;
template <class _List>
using _Meta_pop_front =
    // subsequence including all but the first type (tail of list)
    typename _Meta_pop_front_<_List>::type;

template <template <class...> class _List, class _First, class... _Rest>
struct _Meta_pop_front_<_List<_First, _Rest...>> {
    using type = _List<_Rest...>;
};

template <class _List, class _Ty>
struct _Meta_push_front_;
template <class _List, class _Ty>
using _Meta_push_front =
    // prepend a new type onto a sequence
    typename _Meta_push_front_<_List, _Ty>::type;

template <template <class...> class _List, class... _Types, class _Ty>
struct _Meta_push_front_<_List<_Types...>, _Ty> {
    using type = _List<_Ty, _Types...>;
};

template <class _Void, template <class...> class _Fn, class... _Args>
struct _Meta_quote_helper_;
template <template <class...> class _Fn, class... _Args>
struct _Meta_quote_helper_<void_t<_Fn<_Args...>>, _Fn, _Args...> {
    using type = _Fn<_Args...>;
};
template <template <class...> class _Fn>
struct _Meta_quote { // encapsulate a template into a meta-callable type
    template <class... _Types>
    using _Invoke = typename _Meta_quote_helper_<void, _Fn, _Types...>::type;
};

template <class _Fn, class... _Args>
using _Meta_invoke = // invoke meta-callable _Fn with _Args
    typename _Fn::template _Invoke<_Args...>;

template <class _Fn, class... _Args>
struct _Meta_bind_back { // construct a meta-callable that passes its arguments and _Args to _Fn
    template <class... _Types>
    using _Invoke = _Meta_invoke<_Fn, _Types..., _Args...>;
};

template <class _Fn, class _List>
struct _Meta_apply_;
template <class _Fn, class _List>
using _Meta_apply =
    // explode _List into the parameters of meta-callable _Fn
    typename _Meta_apply_<_Fn, _List>::type;

template <class _Fn, template <class...> class _List, class... _Types>
struct _Meta_apply_<_Fn,
    _List<_Types...>> { // invoke meta-callable _Fn with the parameters of a template specialization
    using type = _Meta_invoke<_Fn, _Types...>;
};

template <class _Fn, class _Ty, _Ty... _Idxs>
struct _Meta_apply_<_Fn,
    integer_sequence<_Ty, _Idxs...>> { // invoke meta-callable _Fn with the elements of an integer_sequence
    using type = _Meta_invoke<_Fn, integral_constant<_Ty, _Idxs>...>;
};

template <class _Fn, class _List>
struct _Meta_transform_;
template <class _Fn, class _List>
using _Meta_transform =
    // transform sequence of _Types... into sequence of _Fn<_Types...>
    typename _Meta_transform_<_Fn, _List>::type;

template <template <class...> class _List, class _Fn, class... _Types>
struct _Meta_transform_<_Fn, _List<_Types...>> {
    using type = _List<_Meta_invoke<_Fn, _Types>...>;
};

template <class, class, template <class...> class>
struct _Meta_repeat_n_c_;
template <size_t _Count, class _Ty, template <class...> class _Continue>
using _Meta_repeat_n_c =
    // construct a sequence consisting of repetitions of _Ty
    typename _Meta_repeat_n_c_<_Ty, make_index_sequence<_Count>, _Continue>::type;

template <class _Ty, size_t>
using _Meta_repeat_first_helper = _Ty;

template <class _Ty, size_t... _Idxs, template <class...> class _Continue>
struct _Meta_repeat_n_c_<_Ty, index_sequence<_Idxs...>, _Continue> {
    using type = _Continue<_Meta_repeat_first_helper<_Ty, _Idxs>...>;
};

template <class _List, size_t _Idx, class = void>
struct _Meta_at_;
template <class _List, size_t _Idx>
using _Meta_at_c =
    // Extract the _Idx-th type from _List
    typename _Meta_at_<_List, _Idx>::type;

#ifdef __clang__
template <template <class...> class _List, class... _Types, size_t _Idx>
struct _Meta_at_<_List<_Types...>, _Idx, void_t<__type_pack_element<_Idx, _Types...>>> {
    using type = __type_pack_element<_Idx, _Types...>;
};
#else // ^^^ __clang__ / !__clang__ vvv
template <class... _VoidPtrs>
struct _Meta_at_impl {
    template <class _Ty, class... _Types>
    static _Ty _Eval(_VoidPtrs..., _Ty*, _Types*...); // undefined
};

template <class _Ty>
constexpr _Identity<_Ty>* _Type_as_pointer() {
    return nullptr;
}

template <template <class...> class _List, class... _Types, size_t _Idx>
struct _Meta_at_<_List<_Types...>, _Idx, enable_if_t<(_Idx < sizeof...(_Types))>> {
    using type =
        typename decltype(_Meta_repeat_n_c<_Idx, void*, _Meta_at_impl>::_Eval(_Type_as_pointer<_Types>()...))::type;
};
#endif // __clang__

inline constexpr auto _Meta_npos = ~size_t{0};

template <class _List, class _Ty>
struct _Meta_find_index_ {
    using type = integral_constant<size_t, _Meta_npos>;
};
template <class _List, class _Ty>
using _Meta_find_index =
    // find the index of the first occurrence of _Ty in _List
    typename _Meta_find_index_<_List, _Ty>::type;

constexpr size_t _Meta_find_index_i_(const bool* const _Ptr, const size_t _Count,
    size_t _Idx = 0) { // return the index of the first true in the _Count bools at _Ptr, or _Meta_npos if all are false
    for (; _Idx < _Count; ++_Idx) {
        if (_Ptr[_Idx]) {
            return _Idx;
        }
    }

    return _Meta_npos;
}

template <template <class...> class _List, class _First, class... _Rest, class _Ty>
struct _Meta_find_index_<_List<_First, _Rest...>, _Ty> {
    static constexpr bool _Bools[] = {is_same_v<_First, _Ty>, is_same_v<_Rest, _Ty>...};
    using type                     = integral_constant<size_t, _Meta_find_index_i_(_Bools, 1 + sizeof...(_Rest))>;
};

template <class _List, class _Ty>
struct _Meta_find_unique_index_ {
    using type = integral_constant<size_t, _Meta_npos>;
};
template <class _List, class _Ty>
using _Meta_find_unique_index =
    // The index of _Ty in _List if it occurs exactly once, otherwise _Meta_npos
    typename _Meta_find_unique_index_<_List, _Ty>::type;

constexpr size_t _Meta_find_unique_index_i_2(const bool* const _Ptr, const size_t _Count,
    const size_t
        _First) { // return _First if there is no _First < j < _Count such that _Ptr[j] is true, otherwise _Meta_npos
    return _First != _Meta_npos && _Meta_find_index_i_(_Ptr, _Count, _First + 1) == _Meta_npos ? _First : _Meta_npos;
}

constexpr size_t _Meta_find_unique_index_i_(const bool* const _Ptr,
    const size_t _Count) { // Pass the smallest i such that _Ptr[i] is true to _Meta_find_unique_index_i_2
    return _Meta_find_unique_index_i_2(_Ptr, _Count, _Meta_find_index_i_(_Ptr, _Count));
}

template <template <class...> class _List, class _First, class... _Rest, class _Ty>
struct _Meta_find_unique_index_<_List<_First, _Rest...>, _Ty> {
    using type = integral_constant<size_t,
        _Meta_find_unique_index_i_(_Meta_find_index_<_List<_First, _Rest...>, _Ty>::_Bools, 1 + sizeof...(_Rest))>;
};

template <class>
struct _Meta_as_list_;
template <class _Ty>
using _Meta_as_list =
    // convert _Ty to a _Meta_list
    typename _Meta_as_list_<_Ty>::type;

template <template <class...> class _List, class... _Types>
struct _Meta_as_list_<_List<_Types...>> { // convert the parameters of an arbitrary template specialization to a
                                          // _Meta_list of types
    using type = _Meta_list<_Types...>;
};

template <class _Ty, _Ty... _Idxs>
struct _Meta_as_list_<integer_sequence<_Ty, _Idxs...>> { // convert an integer_sequence to a _Meta_list of
                                                         // integral_constants
    using type = _Meta_list<integral_constant<_Ty, _Idxs>...>;
};

template <class _List>
struct _Meta_as_integer_sequence_;
template <class _List>
using _Meta_as_integer_sequence =
    // convert a list of integral_constants to an integer_sequence
    typename _Meta_as_integer_sequence_<_List>::type;

template <template <class...> class _List, class _Ty, _Ty... _Idxs>
struct _Meta_as_integer_sequence_<_List<integral_constant<_Ty, _Idxs>...>> {
    using type = integer_sequence<_Ty, _Idxs...>;
};

template <class...>
struct _Meta_concat_;
template <class... _Types>
using _Meta_concat =
    // merge several lists into one
    typename _Meta_concat_<_Types...>::type;

template <template <class...> class _List>
struct _Meta_concat_<_List<>> {
    using type = _List<>;
};

template <template <class...> class _List, class... _Items1>
struct _Meta_concat_<_List<_Items1...>> {
    using type = _List<_Items1...>;
};

template <template <class...> class _List, class... _Items1, class... _Items2>
struct _Meta_concat_<_List<_Items1...>, _List<_Items2...>> {
    using type = _List<_Items1..., _Items2...>;
};

template <template <class...> class _List, class... _Items1, class... _Items2, class... _Items3>
struct _Meta_concat_<_List<_Items1...>, _List<_Items2...>, _List<_Items3...>> {
    using type = _List<_Items1..., _Items2..., _Items3...>;
};

template <template <class...> class _List, class... _Items1, class... _Items2, class... _Items3, class... _Rest>
struct _Meta_concat_<_List<_Items1...>, _List<_Items2...>, _List<_Items3...>, _Rest...> {
    using type = _Meta_concat<_List<_Items1..., _Items2..., _Items3...>, _Rest...>;
};

template <class _ListOfLists>
using _Meta_join =
    // transform a list of lists of elements into a single list containing those elements
    _Meta_apply<_Meta_quote<_Meta_concat>, _ListOfLists>;

template <class>
struct _Meta_cartesian_product_;
template <class _ListOfLists>
using _Meta_cartesian_product =
    // find the n-ary Cartesian Product of the lists in the input list
    typename _Meta_cartesian_product_<_ListOfLists>::type;

template <template <class...> class _List>
struct _Meta_cartesian_product_<_List<>> {
    using type = _List<>;
};

template <template <class...> class _List1, template <class...> class _List2, class... _Items>
struct _Meta_cartesian_product_<_List1<_List2<_Items...>>> {
    using type = _List1<_List2<_Items>...>;
};

template <template <class...> class _List1, class... _Items, template <class...> class _List2, class... _Lists>
struct _Meta_cartesian_product_<_List1<_List2<_Items...>, _Lists...>> {
    using type = _Meta_join<_List1<_Meta_transform<_Meta_bind_back<_Meta_quote<_Meta_push_front>, _Items>,
        _Meta_cartesian_product<_List1<_Lists...>>>...>>;
};

#define _STL_STAMP4(n, x) \
    x(n);                 \
    x(n + 1);             \
    x(n + 2);             \
    x(n + 3)
#define _STL_STAMP16(n, x) \
    _STL_STAMP4(n, x);     \
    _STL_STAMP4(n + 4, x); \
    _STL_STAMP4(n + 8, x); \
    _STL_STAMP4(n + 12, x)
#define _STL_STAMP64(n, x)   \
    _STL_STAMP16(n, x);      \
    _STL_STAMP16(n + 16, x); \
    _STL_STAMP16(n + 32, x); \
    _STL_STAMP16(n + 48, x)
#define _STL_STAMP256(n, x)   \
    _STL_STAMP64(n, x);       \
    _STL_STAMP64(n + 64, x);  \
    _STL_STAMP64(n + 128, x); \
    _STL_STAMP64(n + 192, x)

#define _STL_STAMP(n, x) x(_STL_STAMP##n, n)

template <class... _Types>
class variant;

template <class _Ty>
struct variant_size; // undefined
template <class _Ty>
struct variant_size<const _Ty> : variant_size<_Ty>::type {};
template <class _Ty>
struct _CXX20_DEPRECATE_VOLATILE variant_size<volatile _Ty> : variant_size<_Ty>::type {};
template <class _Ty>
struct _CXX20_DEPRECATE_VOLATILE variant_size<const volatile _Ty> : variant_size<_Ty>::type {};
template <class _Ty>
inline constexpr size_t variant_size_v = variant_size<_Ty>::value;

template <class... _Types>
struct variant_size<variant<_Types...>> : integral_constant<size_t, sizeof...(_Types)> {};

template <size_t _Idx, class _Ty>
struct variant_alternative; // undefined
template <size_t _Idx, class _Ty>
using variant_alternative_t = typename variant_alternative<_Idx, _Ty>::type;
template <size_t _Idx, class _Ty>
struct variant_alternative<_Idx, const _Ty> {
    using type = add_const_t<variant_alternative_t<_Idx, _Ty>>;
};
template <size_t _Idx, class _Ty>
struct _CXX20_DEPRECATE_VOLATILE variant_alternative<_Idx, volatile _Ty> {
    using type = add_volatile_t<variant_alternative_t<_Idx, _Ty>>;
};
template <size_t _Idx, class _Ty>
struct _CXX20_DEPRECATE_VOLATILE variant_alternative<_Idx, const volatile _Ty> {
    using type = add_cv_t<variant_alternative_t<_Idx, _Ty>>;
};
template <size_t _Idx, class... _Types>
struct variant_alternative<_Idx, variant<_Types...>> {
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");

#ifdef __clang__
    using type = __type_pack_element<_Idx, _Types...>;
#else // ^^^ __clang__ ^^^ / vvv !__clang__ vvv
    using type = _Meta_at_c<variant<_Types...>, _Idx>;
#endif // __clang__
};

inline constexpr size_t variant_npos = _Meta_npos;

class bad_variant_access
    : public exception { // exception for visit of a valueless variant or get<I> on a variant with index() != I
public:
    bad_variant_access() noexcept = default;

    _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept override {
        return "bad variant access";
    }

#if !_HAS_EXCEPTIONS
protected:
    virtual void _Doraise() const override { // perform class-specific exception handling
        _RAISE(*this);
    }
#endif // !_HAS_EXCEPTIONS
};

[[noreturn]] inline void _Throw_bad_variant_access() {
    _THROW(bad_variant_access{});
}

template <bool _TrivialDestruction, class... _Types>
class _Variant_storage_ {}; // empty storage (empty "_Types" case)

template <class... _Types>
using _Variant_storage = _Variant_storage_<conjunction_v<is_trivially_destructible<_Types>...>, _Types...>;

template <class _First, class... _Rest>
class _Variant_storage_<true, _First, _Rest...> { // Storage for variant alternatives (trivially destructible case)
public:
    static constexpr size_t _Size = 1 + sizeof...(_Rest);
    union {
        remove_const_t<_First> _Head;
        _Variant_storage<_Rest...> _Tail;
    };

    _CONSTEXPR20 _Variant_storage_() noexcept {} // no initialization (no active member)

    template <class... _Types>
    constexpr explicit _Variant_storage_(integral_constant<size_t, 0>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_First, _Types...>)
        : _Head(static_cast<_Types&&>(_Args)...) {} // initialize _Head with _Args...

    template <size_t _Idx, class... _Types, enable_if_t<(_Idx > 0), int> = 0>
    constexpr explicit _Variant_storage_(integral_constant<size_t, _Idx>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_Variant_storage<_Rest...>, integral_constant<size_t, _Idx - 1>, _Types...>)
        : _Tail(integral_constant<size_t, _Idx - 1>{}, static_cast<_Types&&>(_Args)...) {} // initialize _Tail (recurse)

    _NODISCARD constexpr _First& _Get() & noexcept {
        return _Head;
    }
    _NODISCARD constexpr const _First& _Get() const& noexcept {
        return _Head;
    }
    _NODISCARD constexpr _First&& _Get() && noexcept {
        return _STD move(_Head);
    }
    _NODISCARD constexpr const _First&& _Get() const&& noexcept {
        return _STD move(_Head);
    }
};

template <class _First, class... _Rest>
class _Variant_storage_<false, _First, _Rest...> { // Storage for variant alternatives (non-trivially destructible case)
public:
    static constexpr size_t _Size = 1 + sizeof...(_Rest);
    union {
        remove_const_t<_First> _Head;
        _Variant_storage<_Rest...> _Tail;
    };

    _CONSTEXPR20 ~_Variant_storage_() noexcept {
        // explicitly non-trivial destructor (which would otherwise be defined as deleted
        // since the class has a variant member with a non-trivial destructor)
    }

    _CONSTEXPR20 _Variant_storage_() noexcept {} // no initialization (no active member)

    template <class... _Types>
    constexpr explicit _Variant_storage_(integral_constant<size_t, 0>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_First, _Types...>)
        : _Head(static_cast<_Types&&>(_Args)...) {} // initialize _Head with _Args...

    template <size_t _Idx, class... _Types, enable_if_t<(_Idx > 0), int> = 0>
    constexpr explicit _Variant_storage_(integral_constant<size_t, _Idx>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_Variant_storage<_Rest...>, integral_constant<size_t, _Idx - 1>, _Types...>)
        : _Tail(integral_constant<size_t, _Idx - 1>{}, static_cast<_Types&&>(_Args)...) {} // initialize _Tail (recurse)

    _Variant_storage_(_Variant_storage_&&)      = default;
    _Variant_storage_(const _Variant_storage_&) = default;
    _Variant_storage_& operator=(_Variant_storage_&&) = default;
    _Variant_storage_& operator=(const _Variant_storage_&) = default;

    _NODISCARD constexpr _First& _Get() & noexcept {
        return _Head;
    }
    _NODISCARD constexpr const _First& _Get() const& noexcept {
        return _Head;
    }
    _NODISCARD constexpr _First&& _Get() && noexcept {
        return _STD move(_Head);
    }
    _NODISCARD constexpr const _First&& _Get() const&& noexcept {
        return _STD move(_Head);
    }
};

#ifdef __cplusplus_winrt // TRANSITION, VSO-586813
// C++/CX is unable to store hats in unions. We instead store them inside a
// wrapper to enable minimal hats-in-variants support.
template <class _Ty>
struct _Variant_item {
    remove_const_t<_Ty> _Item;

    template <class... _Types>
    constexpr _Variant_item(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty, _Types...>)
        : _Item(static_cast<_Types&&>(_Args)...) {}
};

template <class _First, class... _Rest>
class _Variant_storage_<false, _First ^, _Rest...> { // Storage for variant alternatives (^ case)
public:
    static constexpr size_t _Size = 1 + sizeof...(_Rest);
    union {
        _Variant_item<_First ^> _Head;
        _Variant_storage<_Rest...> _Tail;
    };

    _CONSTEXPR20 ~_Variant_storage_() noexcept {
        // explicitly non-trivial destructor (which would otherwise be defined as deleted
        // since the class has a variant member with a non-trivial destructor)
    }

    _CONSTEXPR20 _Variant_storage_() noexcept {} // no initialization (no active member)

    template <class... _Types>
    constexpr explicit _Variant_storage_(integral_constant<size_t, 0>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_First ^, _Types...>)
        : _Head(static_cast<_Types&&>(_Args)...) {} // initialize _Head with _Args...

    template <size_t _Idx, class... _Types, enable_if_t<(_Idx > 0), int> = 0>
    constexpr explicit _Variant_storage_(integral_constant<size_t, _Idx>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_Variant_storage<_Rest...>, integral_constant<size_t, _Idx - 1>, _Types...>)
        : _Tail(integral_constant<size_t, _Idx - 1>{}, static_cast<_Types&&>(_Args)...) {} // initialize _Tail (recurse)

    _Variant_storage_(_Variant_storage_&&)      = default;
    _Variant_storage_(const _Variant_storage_&) = default;
    _Variant_storage_& operator=(_Variant_storage_&&) = default;
    _Variant_storage_& operator=(const _Variant_storage_&) = default;

    _NODISCARD constexpr _First ^ &_Get() & noexcept {
        return _Head._Item;
    }
    _NODISCARD constexpr _First ^ const& _Get() const& noexcept {
        return _Head._Item;
    }
    _NODISCARD constexpr _First ^ &&_Get() && noexcept {
        return _STD move(_Head)._Item;
    }
    _NODISCARD constexpr _First ^ const&& _Get() const&& noexcept {
        return _STD move(_Head)._Item;
    }
};
#endif // __cplusplus_winrt

template <size_t _Idx, class _Storage>
_NODISCARD constexpr decltype(auto) _Variant_raw_get(
    _Storage&& _Obj) noexcept { // access the _Idx-th element of a _Variant_storage
    if constexpr (_Idx == 0) {
        return static_cast<_Storage&&>(_Obj)._Get();
    } else if constexpr (_Idx == 1) {
        return static_cast<_Storage&&>(_Obj)._Tail._Get();
    } else if constexpr (_Idx == 2) {
        return static_cast<_Storage&&>(_Obj)._Tail._Tail._Get();
    } else if constexpr (_Idx == 3) {
        return static_cast<_Storage&&>(_Obj)._Tail._Tail._Tail._Get();
    } else if constexpr (_Idx == 4) {
        return static_cast<_Storage&&>(_Obj)._Tail._Tail._Tail._Tail._Get();
    } else if constexpr (_Idx == 5) {
        return static_cast<_Storage&&>(_Obj)._Tail._Tail._Tail._Tail._Tail._Get();
    } else if constexpr (_Idx == 6) {
        return static_cast<_Storage&&>(_Obj)._Tail._Tail._Tail._Tail._Tail._Tail._Get();
    } else if constexpr (_Idx == 7) {
        return static_cast<_Storage&&>(_Obj)._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Get();
    } else if constexpr (_Idx < 16) {
        return _Variant_raw_get<_Idx - 8>(
            static_cast<_Storage&&>(_Obj)._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail);
    } else if constexpr (_Idx < 32) {
        return _Variant_raw_get<_Idx - 16>(
            static_cast<_Storage&&>(_Obj)
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail);
    } else if constexpr (_Idx < 64) {
        return _Variant_raw_get<_Idx - 32>(
            static_cast<_Storage&&>(_Obj)
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail);
    } else { // _Idx >= 64
        return _Variant_raw_get<_Idx - 64>(
            static_cast<_Storage&&>(_Obj)
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail
                ._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail._Tail);
    }
}

template <class _Ty, size_t _Tag>
struct _Tagged { // aggregate a runtime value and a compile-time tag value
    static constexpr size_t _Idx = _Tag;
    _Ty _Val;
};

template <class _Storage, size_t _Idx>
using _Variant_tagged_ref_t = _Tagged<decltype(_Variant_raw_get<_Idx>(_STD declval<_Storage>()))&&, _Idx>;

template <class _Fn, class _Storage>
using _Variant_raw_visit_t = decltype(_STD declval<_Fn>()(_STD declval<_Variant_tagged_ref_t<_Storage, 0>>()));

template <size_t _Idx, class _Fn, class _Storage>
_NODISCARD constexpr _Variant_raw_visit_t<_Fn, _Storage> _Variant_raw_visit_dispatch(
    _Fn&& _Func, _Storage&& _Var) noexcept(is_nothrow_invocable_v<_Fn, _Variant_tagged_ref_t<_Storage, _Idx>>) {
    // call _Func with the _Idx-th element in _Storage (tagged with _Idx)
    return static_cast<_Fn&&>(_Func)(
        _Variant_tagged_ref_t<_Storage, _Idx>{_Variant_raw_get<_Idx>(static_cast<_Storage&&>(_Var))});
}

template <class _Fn, class _Storage>
_NODISCARD constexpr _Variant_raw_visit_t<_Fn, _Storage> _Variant_raw_visit_valueless(
    _Fn&& _Func, _Storage&& _Obj) noexcept(is_nothrow_invocable_v<_Fn, _Tagged<_Storage&&, variant_npos>>) {
    // call _Func with _Storage (tagged with variant_npos)
    return static_cast<_Fn&&>(_Func)(_Tagged<_Storage&&, variant_npos>{static_cast<_Storage&&>(_Obj)});
}

template <class _Fn, class _Storage, class _Indices = make_index_sequence<remove_reference_t<_Storage>::_Size>>
inline constexpr bool _Variant_raw_visit_noexcept = false;

template <class _Fn, class _Storage, size_t... _Idxs>
inline constexpr bool _Variant_raw_visit_noexcept<_Fn, _Storage, index_sequence<_Idxs...>> =
    conjunction_v<is_nothrow_invocable<_Fn, _Tagged<_Storage&&, variant_npos>>,
        is_nothrow_invocable<_Fn, _Variant_tagged_ref_t<_Storage, _Idxs>>...>;

template <class _Fn, class _Storage,
    class _Indices = make_index_sequence<remove_reference_t<_Storage>::_Size>>
struct _Variant_raw_dispatch_table1; // undefined

template <class _Fn, class _Storage, size_t... _Idxs>
struct _Variant_raw_dispatch_table1<_Fn, _Storage,
    index_sequence<_Idxs...>> { // map from canonical index to visitation target
    using _Dispatch_t = _Variant_raw_visit_t<_Fn, _Storage> (*)(_Fn&&, _Storage&&) noexcept(
        _Variant_raw_visit_noexcept<_Fn, _Storage>);
    static constexpr _Dispatch_t _Array[] = {
        &_Variant_raw_visit_valueless<_Fn, _Storage>, &_Variant_raw_visit_dispatch<_Idxs, _Fn, _Storage>...};
};

template <int _Strategy>
struct _Variant_raw_visit1;

template <>
struct _Variant_raw_visit1<-1> { // Fallback case for variants too large for any of the following "switch" strategies
    template <class _Fn, class _Storage>
    _NODISCARD static constexpr _Variant_raw_visit_t<_Fn, _Storage> _Visit(
        size_t _Idx, _Fn&& _Func, _Storage&& _Obj) noexcept(_Variant_raw_visit_noexcept<_Fn, _Storage>) {
        // dispatch a visitor for a _Variant_storage with many states
        constexpr size_t _Size = remove_reference_t<_Storage>::_Size;
        static_assert(_Size > 256);
        constexpr auto& _Array = _Variant_raw_dispatch_table1<_Fn, _Storage>::_Array;
        return _Array[_Idx](static_cast<_Fn&&>(_Func), static_cast<_Storage&&>(_Obj));
    }
};

#define _STL_CASE(n)                                                                                         \
    case (n) + 1:                                                                                            \
        if constexpr ((n) < _Size) {                                                                         \
            return static_cast<_Fn&&>(_Func)(                                                                \
                _Variant_tagged_ref_t<_Storage, (n)>{_Variant_raw_get<(n)>(static_cast<_Storage&&>(_Obj))}); \
        }                                                                                                    \
        _STL_UNREACHABLE

#define _STL_VISIT_STAMP(stamper, n)                                                                        \
    constexpr size_t _Size = remove_reference_t<_Storage>::_Size;                                           \
    static_assert(((n) == 4 || _Size > (n) / 4) && _Size <= (n));                                           \
    switch (_Idx) {                                                                                         \
    case 0:                                                                                                 \
        return static_cast<_Fn&&>(_Func)(_Tagged<_Storage&&, variant_npos>{static_cast<_Storage&&>(_Obj)}); \
                                                                                                            \
        stamper(0, _STL_CASE);                                                                              \
    default:                                                                                                \
        _STL_UNREACHABLE;                                                                                   \
    }

template <>
struct _Variant_raw_visit1<1> {
    template <class _Fn, class _Storage>
    _NODISCARD static constexpr _Variant_raw_visit_t<_Fn, _Storage> _Visit(
        size_t _Idx, _Fn&& _Func, _Storage&& _Obj) noexcept(_Variant_raw_visit_noexcept<_Fn, _Storage>) {
        // dispatch a visitor for a _Variant_storage with at most 4^1 states
        _STL_STAMP(4, _STL_VISIT_STAMP);
    }
};

template <>
struct _Variant_raw_visit1<2> {
    template <class _Fn, class _Storage>
    _NODISCARD static constexpr _Variant_raw_visit_t<_Fn, _Storage> _Visit(
        size_t _Idx, _Fn&& _Func, _Storage&& _Obj) noexcept(_Variant_raw_visit_noexcept<_Fn, _Storage>) {
        // dispatch a visitor for a _Variant_storage with at most 4^2 states
        _STL_STAMP(16, _STL_VISIT_STAMP);
    }
};

template <>
struct _Variant_raw_visit1<3> {
    template <class _Fn, class _Storage>
    _NODISCARD static constexpr _Variant_raw_visit_t<_Fn, _Storage> _Visit(
        size_t _Idx, _Fn&& _Func, _Storage&& _Obj) noexcept(_Variant_raw_visit_noexcept<_Fn, _Storage>) {
        // dispatch a visitor for a _Variant_storage with at most 4^3 states
        _STL_STAMP(64, _STL_VISIT_STAMP);
    }
};

template <>
struct _Variant_raw_visit1<4> {
    template <class _Fn, class _Storage>
    _NODISCARD static constexpr _Variant_raw_visit_t<_Fn, _Storage> _Visit(
        size_t _Idx, _Fn&& _Func, _Storage&& _Obj) noexcept(_Variant_raw_visit_noexcept<_Fn, _Storage>) {
        // dispatch a visitor for a _Variant_storage with at most 4^4 states
        _STL_STAMP(256, _STL_VISIT_STAMP);
    }
};

#undef _STL_VISIT_STAMP
#undef _STL_CASE

template <class _Storage, class _Fn>
_NODISCARD constexpr _Variant_raw_visit_t<_Fn, _Storage> _Variant_raw_visit(
    size_t _Idx, _Storage&& _Obj, _Fn&& _Func) noexcept(_Variant_raw_visit_noexcept<_Fn, _Storage>) {
    // Call _Func with _Storage if _Idx is variant_npos, and otherwise the _Idx-th element in _Storage.
    // pre: _Idx + 1 <= remove_reference_t<_Storage>::_Size
    constexpr size_t _Size  = remove_reference_t<_Storage>::_Size;
    constexpr int _Strategy = _Size <= 4 ? 1 : _Size <= 16 ? 2 : _Size <= 64 ? 3 : _Size <= 256 ? 4 : -1;
    ++_Idx; // bias index by +1 to map {variant_npos} U [0, _Size) to the contiguous range [0, _Size]
    return _Variant_raw_visit1<_Strategy>::_Visit(_Idx, static_cast<_Fn&&>(_Func), static_cast<_Storage&&>(_Obj));
}

template <class...>
class _Variant_base;

template <size_t _Count>
using _Variant_index_t = // signed so that conversion of -1 to size_t can cheaply sign extend
    conditional_t<(_Count < static_cast<size_t>((numeric_limits<signed char>::max)())), signed char,
        conditional_t<(_Count < static_cast<size_t>((numeric_limits<short>::max)())), short, int>>;

template <class... _Types>
struct _Variant_construct_visitor { // visitor that constructs the same alternative in a target _Variant_base as is
                                    // currently active in a source _Variant_base from the source's contained value
    _Variant_base<_Types...>& _Self;

    template <class _Ty, size_t _Idx>
    _CONSTEXPR20 void operator()(_Tagged<_Ty, _Idx> _Source) const noexcept(
        disjunction_v<bool_constant<_Idx == variant_npos>, is_nothrow_constructible<remove_reference_t<_Ty>, _Ty>>) {
        // initialize _Idx-th item in _Self from _Source
        _STL_INTERNAL_CHECK(_Self.valueless_by_exception());
        (void) _Source; // TRANSITION, DevCom-1004719
        if constexpr (_Idx != variant_npos) {
            _Construct_in_place(_Self._Storage(), integral_constant<size_t, _Idx>{}, static_cast<_Ty&&>(_Source._Val));
            _Self._Set_index(_Idx);
        }
    }
};

template <class _Target, class... _Types>
inline constexpr bool _Variant_should_directly_construct_v =
    disjunction_v<is_nothrow_constructible<_Target, _Types...>, negation<is_nothrow_move_constructible<_Target>>>;

template <class... _Types>
struct _Variant_assign_visitor { // visitor that implements assignment for variants with non-trivial alternatives
    _Variant_base<_Types...>& _Self;

    template <class _Ty, size_t _Idx>
    _CONSTEXPR20 void operator()(_Tagged<_Ty, _Idx> _Source) const
        noexcept(disjunction_v<bool_constant<_Idx == variant_npos>,
            conjunction<is_nothrow_assignable<_Remove_cvref_t<_Ty>&, _Ty>,
                is_nothrow_constructible<_Remove_cvref_t<_Ty>, _Ty>>>) {
        // assign the _Idx-th alternative of _Self from _Source
        if constexpr (_Idx == variant_npos) { // assign from valueless _Source
            (void) _Source; // TRANSITION, DevCom-1004719
            _Self._Reset();
        } else {
            if (_Self._Which == _Idx) { // same alternative: assign directly
                auto& _Target = _Variant_raw_get<_Idx>(_Self._Storage());
                _Target       = static_cast<_Ty&&>(_Source._Val);
            } else { // different alternative
                if constexpr (is_lvalue_reference_v<_Ty>) { // RHS is an lvalue: copy
                    if constexpr (_Variant_should_directly_construct_v<_Remove_cvref_t<_Ty>, _Ty>) {
                        // copy is nothrow or move throws; construct in place
                        _Self._Reset();
                        _Construct_in_place(_Self._Storage(), integral_constant<size_t, _Idx>{}, _Source._Val);
                    } else { // copy throws and move does not; move from a temporary copy
                        auto _Temp = _Source._Val;
                        _Self._Reset();
                        _Construct_in_place(_Self._Storage(), integral_constant<size_t, _Idx>{}, _STD move(_Temp));
                    }
                } else { // RHS is an rvalue: move
                    _Self._Reset();
                    _Construct_in_place(
                        _Self._Storage(), integral_constant<size_t, _Idx>{}, static_cast<_Ty&&>(_Source._Val));
                }

                _Self._Set_index(_Idx);
            }
        }
    }
};

template <class... _Types> // Associate an integral discriminator with a _Variant_storage
class _Variant_base : private _Variant_storage<_Types...> {
public:
    using _Index_t                       = _Variant_index_t<sizeof...(_Types)>;
    static constexpr auto _Invalid_index = static_cast<_Index_t>(-1);
    _Index_t _Which;

    using _Storage_t = _Variant_storage<_Types...>;
    _NODISCARD constexpr _Storage_t& _Storage() & noexcept { // access this variant's storage
        return *this;
    }
    _NODISCARD constexpr const _Storage_t& _Storage() const& noexcept { // access this variant's storage
        return *this;
    }
    _NODISCARD constexpr _Storage_t&& _Storage() && noexcept { // access this variant's storage
        return _STD move(*this);
    }
    _NODISCARD constexpr const _Storage_t&& _Storage() const&& noexcept { // access this variant's storage
        return _STD move(*this);
    }

    // initialize to the value-less state
    _CONSTEXPR20 _Variant_base() noexcept : _Storage_t{}, _Which{_Invalid_index} {}

    template <size_t _Idx, class... _UTypes,
        enable_if_t<is_constructible_v<_Meta_at_c<variant<_Types...>, _Idx>, _UTypes...>, int> = 0>
    constexpr explicit _Variant_base(in_place_index_t<_Idx>, _UTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Meta_at_c<variant<_Types...>, _Idx>, _UTypes...>)
        : _Storage_t(integral_constant<size_t, _Idx>{}, static_cast<_UTypes&&>(_Args)...),
          _Which{static_cast<_Index_t>(_Idx)} { // initialize alternative _Idx from _Args...
    }

    _NODISCARD constexpr bool valueless_by_exception() const noexcept { // does this variant NOT hold a value?
        return _Which < 0;
    }
    _NODISCARD constexpr size_t index() const noexcept {
        // index of the contained alternative or variant_npos if valueless_by_exception
        return static_cast<size_t>(_Which);
    }
    _CONSTEXPR20 void _Set_index(const size_t _Idx) noexcept {
        // record _Idx as the active alternative
        // pre: the active alternative of *this is _Idx
        _Which = static_cast<_Index_t>(_Idx);
    }

    template <size_t _Idx>
    _CONSTEXPR20 void _Destroy() noexcept {
        // destroy the contained value
        // pre: _Idx == index()
        if constexpr (_Idx != variant_npos && !is_trivially_destructible_v<_Meta_at_c<variant<_Types...>, _Idx>>) {
            _Destroy_in_place(_Variant_raw_get<_Idx>(_Storage()));
        }
    }

    _CONSTEXPR20 void _Destroy() noexcept { // destroy the contained value, if any
        if constexpr (!conjunction_v<is_trivially_destructible<_Types>...>) {
            _Variant_raw_visit(index(), _Storage(), [](auto _Ref) noexcept {
                if constexpr (decltype(_Ref)::_Idx != variant_npos) {
                    _Destroy_in_place(_Ref._Val);
                }
            });
        }
    }

    _CONSTEXPR20 void _Reset() noexcept { // transition to the valueless_by_exception state
        _Destroy();
        _Set_index(variant_npos);
    }

    template <size_t _Idx>
    _CONSTEXPR20 void _Reset() noexcept {
        // transition to the valueless_by_exception state
        // pre: _Idx == index()
        if constexpr (_Idx != variant_npos) {
            _Destroy<_Idx>();
            _Set_index(variant_npos);
        }
    }

    _CONSTEXPR20 void _Construct_from(const _Variant_base& _That) noexcept(
        conjunction_v<is_nothrow_copy_constructible<_Types>...>) {
        // copy _That's contained value into *this
        // pre: valueless_by_exception()
        _Variant_raw_visit(_That.index(), _That._Storage(), _Variant_construct_visitor<_Types...>{*this});
    }

    _CONSTEXPR20 void _Construct_from(_Variant_base&& _That) noexcept(
        conjunction_v<is_nothrow_move_constructible<_Types>...>) {
        // move _That's contained value into *this
        // pre: valueless_by_exception()
        _Variant_raw_visit(_That.index(), _STD move(_That)._Storage(), _Variant_construct_visitor<_Types...>{*this});
    }

    _CONSTEXPR20 void _Assign_from(const _Variant_base& _That) noexcept(
        conjunction_v<is_nothrow_copy_constructible<_Types>..., is_nothrow_copy_assignable<_Types>...>) {
        // copy assign _That's contained value (if any) into *this
        _Variant_raw_visit(_That.index(), _That._Storage(), _Variant_assign_visitor<_Types...>{*this});
    }

    _CONSTEXPR20 void _Assign_from(_Variant_base&& _That) noexcept(
        conjunction_v<is_nothrow_move_constructible<_Types>..., is_nothrow_move_assignable<_Types>...>) {
        // move assign _That's contained value (if any) into *this
        _Variant_raw_visit(_That.index(), _STD move(_That)._Storage(), _Variant_assign_visitor<_Types...>{*this});
    }
};

template <class... _Types>
struct _Variant_destroy_layer_ : _Variant_base<_Types...> { // destruction behavior facade (non-trivial case)
    using _Variant_base<_Types...>::_Variant_base;

    _CONSTEXPR20 ~_Variant_destroy_layer_() noexcept { // Destroy contained value, if any
        this->_Destroy();
    }

    _Variant_destroy_layer_()                               = default;
    _Variant_destroy_layer_(const _Variant_destroy_layer_&) = default;
    _Variant_destroy_layer_(_Variant_destroy_layer_&&)      = default;
    _Variant_destroy_layer_& operator=(const _Variant_destroy_layer_&) = default;
    _Variant_destroy_layer_& operator=(_Variant_destroy_layer_&&) = default;
};

template <class... _Types>
using _Variant_destroy_layer = conditional_t<conjunction_v<is_trivially_destructible<_Types>...>,
    _Variant_base<_Types...>, _Variant_destroy_layer_<_Types...>>;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
#else // ^^^ Clang / not Clang vvv
#pragma warning(push)
#pragma warning(disable : 4242) // '%s': conversion from '%s' to '%s', possible loss of data
#pragma warning(disable : 4244) // '%s': conversion from '%s' to '%s', possible loss of data (Yes, duplicated message.)
#pragma warning(disable : 4365) // '%s': conversion from '%s' to '%s', signed/unsigned mismatch
#pragma warning(disable : 5215) // '%s' a function parameter with volatile qualified type is deprecated in C++20
#endif // __clang__

#if _HAS_CXX20
// build Ti x[] = {std::forward<T>(t)};
template <size_t _Idx, class _TargetType>
auto _Construct_array(_TargetType(&&)[1]) -> _Meta_list<integral_constant<size_t, _Idx>, _TargetType>;

template <size_t _Idx, class _TargetType, class _InitializerType>
using _Variant_type_resolver = decltype(_Construct_array<_Idx, _TargetType>({_STD declval<_InitializerType>()}));
#endif // _HAS_CXX20

template <size_t _Idx, class _TargetType>
struct _Variant_init_single_overload {
#if _HAS_CXX20
    template <class _InitializerType>
    auto operator()(_TargetType, _InitializerType&&) -> _Variant_type_resolver<_Idx, _TargetType, _InitializerType>;
#else // _HAS_CXX20
    template <class _InitializerType>
    auto operator()(_TargetType, _InitializerType&&) -> _Meta_list<integral_constant<size_t, _Idx>, _TargetType>;
#endif // _HAS_CXX20
};

template <class _Indices, class... _Types>
struct _Variant_init_overload_set_;

template <size_t... _Indices, class... _Types>
struct _Variant_init_overload_set_<index_sequence<_Indices...>, _Types...>
    : _Variant_init_single_overload<_Indices, _Types>... {
    using _Variant_init_single_overload<_Indices, _Types>::operator()...;
};

template <class... _Types>
using _Variant_init_overload_set = _Variant_init_overload_set_<index_sequence_for<_Types...>, _Types...>;

template <class Enable, class _Ty, class... _Types>
struct _Variant_init_helper {}; // failure case (has no member "type")

template <class _Ty, class... _Types>
struct _Variant_init_helper<
    void_t<decltype(_Variant_init_overload_set<_Types...>{}(_STD declval<_Ty>(), _STD declval<_Ty>()))>, _Ty,
    _Types...> {
    // perform overload resolution to determine the unique alternative that should be initialized in
    // variant<_Types...> from an argument expression with type and value category _Ty
    using type = decltype(_Variant_init_overload_set<_Types...>{}(_STD declval<_Ty>(), _STD declval<_Ty>()));
};

template <class _Ty, class... _Types> // extract the type from _Variant_init_helper
using _Variant_init_type = _Meta_front<_Meta_pop_front<typename _Variant_init_helper<void, _Ty, _Types...>::type>>;

template <class _Ty, class... _Types> // extract the index from _Variant_init_helper
using _Variant_init_index = _Meta_front<typename _Variant_init_helper<void, _Ty, _Types...>::type>;
#ifdef __clang__
#pragma clang diagnostic pop
#else // ^^^ Clang / not Clang vvv
#pragma warning(pop)
#endif // __clang__

template <class>
inline constexpr bool _Is_in_place_index_specialization = false;
template <size_t _Idx>
inline constexpr bool _Is_in_place_index_specialization<in_place_index_t<_Idx>> = true;

template <class... _Types>
class variant : private _SMF_control<_Variant_destroy_layer<_Types...>, _Types...> { // discriminated union
public:
    static_assert(conjunction_v<is_object<_Types>..., negation<is_array<_Types>>..., is_destructible<_Types>...>,
        "variant<Types...> requires all of the Types to meet the Cpp17Destructible requirements "
        "N4828 [variant.variant]/2.");
    static_assert(sizeof...(_Types) > 0,
        "variant<> (with no template arguments) may not be instantiated (N4835 [variant.variant]/3).");
    using _Mybase = _SMF_control<_Variant_destroy_layer<_Types...>, _Types...>;

    template <class _First = _Meta_front<variant>, enable_if_t<is_default_constructible_v<_First>, int> = 0>
    constexpr variant() noexcept(is_nothrow_default_constructible_v<_First>)
        : _Mybase(in_place_index<0>) {} // value-initialize alternative 0

    template <class _Ty,
        enable_if_t<sizeof...(_Types) != 0 //
                        && !is_same_v<_Remove_cvref_t<_Ty>, variant> //
                        && !_Is_specialization_v<_Remove_cvref_t<_Ty>, in_place_type_t> //
                        && !_Is_in_place_index_specialization<_Remove_cvref_t<_Ty>> //
                        && is_constructible_v<_Variant_init_type<_Ty, _Types...>, _Ty>, //
            int> = 0>
    constexpr variant(_Ty&& _Obj) noexcept(is_nothrow_constructible_v<_Variant_init_type<_Ty, _Types...>, _Ty>)
        : _Mybase(in_place_index<_Variant_init_index<_Ty, _Types...>::value>, static_cast<_Ty&&>(_Obj)) {
        // initialize to the type selected by passing _Obj to the overload set f(Types)...
    }

    template <class _Ty, class... _UTypes, class _Idx = _Meta_find_unique_index<variant, _Ty>,
        enable_if_t<_Idx::value != _Meta_npos && is_constructible_v<_Ty, _UTypes...>, int> = 0>
    constexpr explicit variant(in_place_type_t<_Ty>, _UTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Ty, _UTypes...>) // strengthened
        : _Mybase(in_place_index<_Idx::value>, static_cast<_UTypes&&>(_Args)...) {
        // initialize alternative _Ty from _Args...
    }
    template <class _Ty, class _Elem, class... _UTypes, class _Idx = _Meta_find_unique_index<variant, _Ty>,
        enable_if_t<_Idx::value != _Meta_npos && is_constructible_v<_Ty, initializer_list<_Elem>&, _UTypes...>, int> =
            0>
    constexpr explicit variant(in_place_type_t<_Ty>, initializer_list<_Elem> _Ilist, _UTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Ty, initializer_list<_Elem>&, _UTypes...>) // strengthened
        : _Mybase(in_place_index<_Idx::value>, _Ilist, static_cast<_UTypes&&>(_Args)...) {
        // initialize alternative _Ty from _Ilist and _Args...
    }

    template <size_t _Idx, class... _UTypes,
        enable_if_t<is_constructible_v<_Meta_at_c<variant, _Idx>, _UTypes...>, int> = 0>
    constexpr explicit variant(in_place_index_t<_Idx>, _UTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Meta_at_c<variant, _Idx>, _UTypes...>) // strengthened
        : _Mybase(in_place_index<_Idx>, static_cast<_UTypes&&>(_Args)...) {
        // initialize alternative _Idx from _Args...
    }
    template <size_t _Idx, class _Elem, class... _UTypes,
        enable_if_t<is_constructible_v<_Meta_at_c<variant, _Idx>, initializer_list<_Elem>&, _UTypes...>, int> = 0>
    constexpr explicit variant(in_place_index_t<_Idx>, initializer_list<_Elem> _Ilist, _UTypes&&... _Args) noexcept(
        is_constructible_v<_Meta_at_c<variant, _Idx>, initializer_list<_Elem>&, _UTypes...>) // strengthened
        : _Mybase(in_place_index<_Idx>, _Ilist, static_cast<_UTypes&&>(_Args)...) {
        // initialize alternative _Idx from _Ilist and _Args...
    }

    template <class _Ty, enable_if_t<!is_same_v<_Remove_cvref_t<_Ty>, variant> //
                                         && is_constructible_v<_Variant_init_type<_Ty, _Types...>, _Ty> //
                                         && is_assignable_v<_Variant_init_type<_Ty, _Types...>&, _Ty>, //
                             int> = 0>
    _CONSTEXPR20 variant& operator=(_Ty&& _Obj) noexcept(
        is_nothrow_assignable_v<_Variant_init_type<_Ty, _Types...>&, _Ty>&&
            is_nothrow_constructible_v<_Variant_init_type<_Ty, _Types...>, _Ty>) {
        // assign/emplace the alternative chosen by overload resolution of _Obj with f(_Types)...
        constexpr size_t _TargetIdx = _Variant_init_index<_Ty, _Types...>::value;
        if (index() == _TargetIdx) {
            auto& _Target = _Variant_raw_get<_TargetIdx>(_Storage());
            _Target       = static_cast<_Ty&&>(_Obj);
        } else {
            using _TargetTy = _Variant_init_type<_Ty, _Types...>;
            if constexpr (_Variant_should_directly_construct_v<_TargetTy, _Ty>) {
                this->_Reset();
                _Emplace_valueless<_TargetIdx>(static_cast<_Ty&&>(_Obj));
            } else {
                _TargetTy _Temp(static_cast<_Ty&&>(_Obj));
                this->_Reset();
                _Emplace_valueless<_TargetIdx>(_STD move(_Temp));
            }
        }

        return *this;
    }

    using _Mybase::_Storage;

    template <class _Ty, class... _ArgTypes, size_t _Idx = _Meta_find_unique_index<variant, _Ty>::value,
        enable_if_t<_Idx != _Meta_npos && is_constructible_v<_Ty, _ArgTypes...>, int> = 0>
    _CONSTEXPR20 _Ty& emplace(_ArgTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Ty, _ArgTypes...>) /* strengthened */ {
        // emplace alternative _Ty from _Args...
        this->_Reset();
        return _Emplace_valueless<_Idx>(static_cast<_ArgTypes&&>(_Args)...);
    }
    template <class _Ty, class _Elem, class... _ArgTypes, size_t _Idx = _Meta_find_unique_index<variant, _Ty>::value,
        enable_if_t<_Idx != _Meta_npos && is_constructible_v<_Ty, initializer_list<_Elem>&, _ArgTypes...>, int> = 0>
    _CONSTEXPR20 _Ty& emplace(initializer_list<_Elem> _Ilist, _ArgTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Ty, initializer_list<_Elem>&, _ArgTypes...>) /* strengthened */ {
        // emplace alternative _Ty from _Ilist and _Args...
        this->_Reset();
        return _Emplace_valueless<_Idx>(_Ilist, static_cast<_ArgTypes&&>(_Args)...);
    }

    template <size_t _Idx, class... _ArgTypes,
        enable_if_t<is_constructible_v<_Meta_at_c<variant, _Idx>, _ArgTypes...>, int> = 0>
    _CONSTEXPR20 _Meta_at_c<variant, _Idx>& emplace(_ArgTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Meta_at_c<variant, _Idx>, _ArgTypes...>) /* strengthened */ {
        // emplace alternative _Idx from _Args...
        this->_Reset();
        return _Emplace_valueless<_Idx>(static_cast<_ArgTypes&&>(_Args)...);
    }
    template <size_t _Idx, class _Elem, class... _ArgTypes,
        enable_if_t<is_constructible_v<_Meta_at_c<variant, _Idx>, initializer_list<_Elem>&, _ArgTypes...>, int> = 0>
    _CONSTEXPR20 _Meta_at_c<variant, _Idx>& emplace(initializer_list<_Elem> _Ilist, _ArgTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Meta_at_c<variant, _Idx>, initializer_list<_Elem>&,
            _ArgTypes...>) /* strengthened */ {
        // emplace alternative _Idx from _Ilist and _Args...
        this->_Reset();
        return _Emplace_valueless<_Idx>(_Ilist, static_cast<_ArgTypes&&>(_Args)...);
    }

    using _Mybase::index;
    using _Mybase::valueless_by_exception;

#ifdef __clang__ // TRANSITION, LLVM-45398
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"
#endif // TRANSITION, LLVM-45398
    _CONSTEXPR20 void swap(variant& _That) noexcept(
        conjunction_v<is_nothrow_move_constructible<_Types>..., is_nothrow_swappable<_Types>...>) {
        // exchange the contained values if *this and _That hold the same alternative, otherwise exchange the values of
        // the variants themselves
        static_assert(conjunction_v<is_move_constructible<_Types>...>,
            "variant<Types...>::swap requires all of the Types... to be move constructible. (N4835 [variant.swap]/1)");
        static_assert(disjunction_v<negation<is_move_constructible<_Types>>..., conjunction<is_swappable<_Types>...>>,
            "variant<Types...>::swap requires all of the Types... to be swappable. (N4835 [variant.swap]/1)");
        if constexpr (conjunction_v<_Is_trivially_swappable<_Types>...>) {
            using _BaseTy = _Variant_base<_Types...>;
            _STD swap(static_cast<_BaseTy&>(*this), static_cast<_BaseTy&>(_That));
        } else if constexpr (sizeof...(_Types) < 32) {
            // Limit the size of variants that use this quadratic code size implementation of swap.
            _Variant_raw_visit(index(), _Storage(),
                [this, &_That](auto _My_ref)
#ifndef __EDG__ // TRANSITION, VSO-897887
                    noexcept(conjunction_v<is_nothrow_move_constructible<_Types>..., is_nothrow_swappable<_Types>...>)
#endif // TRANSITION, VSO-897887
                {
                    _Variant_raw_visit(_That.index(), _That._Storage(),
                        [this, &_That, _My_ref](auto _That_ref)
#ifndef __EDG__ // TRANSITION, VSO-897887
                            noexcept(conjunction_v<is_nothrow_move_constructible<_Types>...,
                                is_nothrow_swappable<_Types>...>)
#endif // TRANSITION, VSO-897887
                        {
                            constexpr size_t _That_idx = decltype(_That_ref)::_Idx;
#ifdef __EDG__ // TRANSITION, VSO-657455
                            constexpr size_t _My_idx = decltype(_My_ref)::_Idx + 0 * _That_idx;
#else // ^^^ workaround ^^^ / vvv no workaround vvv
                            constexpr size_t _My_idx = decltype(_My_ref)::_Idx;
#endif // TRANSITION, VSO-657455
                            if constexpr (_My_idx == _That_idx) { // Same alternatives...
                                if constexpr (_My_idx != variant_npos) { // ...and not valueless, swap directly
                                    _Swap_adl(_My_ref._Val, _That_ref._Val);
                                }
                            } else if constexpr (_My_idx == variant_npos) { // *this is valueless, _That is not
                                this->_Emplace_valueless<_That_idx>(_STD move(_That_ref._Val));
                                _That.template _Reset<_That_idx>();
                            } else if constexpr (_That_idx == variant_npos) { // _That is valueless, *this is not
                                _That._Emplace_valueless<_My_idx>(_STD move(_My_ref._Val));
                                this->template _Reset<_My_idx>();
                            } else { // different non-valueless alternatives
#ifdef __EDG__ // TRANSITION, VSO-657455
                                using _Workaround = enable_if_t<_That_idx != variant_npos, decltype(_My_ref._Val)>;
                                auto _Tmp         = _STD move(static_cast<_Workaround>(_My_ref._Val));
#else // ^^^ workaround ^^^ / vvv no workaround vvv
                                auto _Tmp = _STD move(_My_ref._Val);
#endif // TRANSITION, VSO-657455
                                this->template _Reset<_My_idx>();
                                this->_Emplace_valueless<_That_idx>(_STD move(_That_ref._Val));
                                _That.template _Reset<_That_idx>();
                                _That._Emplace_valueless<_My_idx>(_STD move(_Tmp));
                            }
                        });
                });
        } else {
            if (this->_Which == _That._Which) {
                _Variant_raw_visit(static_cast<size_t>(this->_Which), _That._Storage(),
                    [this](auto _Ref)
#ifndef __EDG__ // TRANSITION, VSO-897887
                        noexcept(conjunction_v<is_nothrow_swappable<_Types>...>)
#endif // TRANSITION, VSO-897887
                    {
                        constexpr size_t _Idx = decltype(_Ref)::_Idx;
                        if constexpr (_Idx != variant_npos) {
                            _Swap_adl(_Variant_raw_get<_Idx>(this->_Storage()), _Ref._Val);
                        }
                    });
            } else {
                variant _Tmp = _STD move(*this);
                this->_Emplace_from(_STD move(_That));
                _That._Emplace_from(_STD move(_Tmp));
            }
        }
    }
#ifdef __clang__ // TRANSITION, LLVM-45398
#pragma clang diagnostic pop
#endif // TRANSITION, LLVM-45398

private:
    template <size_t _Idx, class... _ArgTypes>
    _CONSTEXPR20 _Meta_at_c<variant, _Idx>& _Emplace_valueless(_ArgTypes&&... _Args) noexcept(
        is_nothrow_constructible_v<_Meta_at_c<variant, _Idx>, _ArgTypes...>) {
        // initialize alternative _Idx from _Args...
        _STL_INTERNAL_CHECK(valueless_by_exception());
        _Construct_in_place(_Storage(), integral_constant<size_t, _Idx>{}, static_cast<_ArgTypes&&>(_Args)...);
        this->_Set_index(_Idx);
        return _Variant_raw_get<_Idx>(_Storage());
    }

    _CONSTEXPR20 void _Emplace_from(variant&& _That) noexcept(conjunction_v<is_nothrow_move_constructible<_Types>...>) {
        // steal the contained value from _That
        this->_Reset();
        _Variant_raw_visit(_That.index(), _That._Storage(),
            [this](auto _Ref)
#ifndef __EDG__ // TRANSITION, VSO-897887
                noexcept(conjunction_v<is_nothrow_move_constructible<_Types>...>)
#endif // TRANSITION, VSO-897887
            {
                constexpr size_t _Idx = decltype(_Ref)::_Idx;
                if constexpr (_Idx != variant_npos) {
                    this->_Emplace_valueless<_Idx>(_STD move(_Ref._Val));
                }
            });
    }
};

template <class _Ty, class... _Types>
_NODISCARD constexpr bool holds_alternative(
    const variant<_Types...>& _Var) noexcept { // true iff _Var holds alternative _Ty
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx != _Meta_npos, "holds_alternative<T>(const variant<Types...>&) requires T to occur exactly "
                                      "once in Types. (N4835 [variant.get]/1)");
    return _Var.index() == _Idx;
}

template <size_t _Idx, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    variant<_Types...>& _Var) { // access the contained value of _Var if its _Idx-th alternative is active
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");
    if (_Var.index() == _Idx) {
        return _Variant_raw_get<_Idx>(_Var._Storage());
    }

    _Throw_bad_variant_access();
}
template <size_t _Idx, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    variant<_Types...>&& _Var) { // access the contained value of _Var if its _Idx-th alternative is active
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");
    if (_Var.index() == _Idx) {
        return _Variant_raw_get<_Idx>(_STD move(_Var)._Storage());
    }

    _Throw_bad_variant_access();
}
template <size_t _Idx, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    const variant<_Types...>& _Var) { // access the contained value of _Var if its _Idx-th alternative is active
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");
    if (_Var.index() == _Idx) {
        return _Variant_raw_get<_Idx>(_Var._Storage());
    }

    _Throw_bad_variant_access();
}
template <size_t _Idx, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    const variant<_Types...>&& _Var) { // access the contained value of _Var if its _Idx-th alternative is active
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");
    if (_Var.index() == _Idx) {
        return _Variant_raw_get<_Idx>(_STD move(_Var)._Storage());
    }

    _Throw_bad_variant_access();
}

template <class _Ty, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    variant<_Types...>& _Var) { // access the contained value of _Var if its alternative _Ty is active
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx < sizeof...(_Types),
        "get<T>(variant<Types...>&) requires T to occur exactly once in Types. (N4835 [variant.get]/5)");
    return _STD get<_Idx>(_Var);
}
template <class _Ty, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    variant<_Types...>&& _Var) { // access the contained value of _Var if its alternative _Ty is active
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx < sizeof...(_Types),
        "get<T>(variant<Types...>&&) requires T to occur exactly once in Types. (N4835 [variant.get]/5)");
    return _STD get<_Idx>(_STD move(_Var));
}
template <class _Ty, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    const variant<_Types...>& _Var) { // access the contained value of _Var if its alternative _Ty is active
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx < sizeof...(_Types),
        "get<T>(const variant<Types...>&) requires T to occur exactly once in Types. (N4835 [variant.get]/5)");
    return _STD get<_Idx>(_Var);
}
template <class _Ty, class... _Types>
_NODISCARD constexpr decltype(auto) get(
    const variant<_Types...>&& _Var) { // access the contained value of _Var if its alternative _Ty is active
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx < sizeof...(_Types),
        "get<T>(const variant<Types...>&&) requires T to occur exactly once in Types. (N4835 [variant.get]/5)");
    return _STD get<_Idx>(_STD move(_Var));
}

template <size_t _Idx, class... _Types>
_NODISCARD constexpr auto get_if(
    variant<_Types...>* _Ptr) noexcept { // get the address of *_Ptr's contained value if it holds alternative _Idx
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");
    return _Ptr && _Ptr->index() == _Idx ? _STD addressof(_Variant_raw_get<_Idx>(_Ptr->_Storage())) : nullptr;
}
template <size_t _Idx, class... _Types>
_NODISCARD constexpr auto get_if(const variant<_Types...>*
        _Ptr) noexcept { // get the address of *_Ptr's contained value if it holds alternative _Idx
    static_assert(_Idx < sizeof...(_Types), "variant index out of bounds");
    return _Ptr && _Ptr->index() == _Idx ? _STD addressof(_Variant_raw_get<_Idx>(_Ptr->_Storage())) : nullptr;
}

template <class _Ty, class... _Types>
_NODISCARD constexpr add_pointer_t<_Ty> get_if(
    variant<_Types...>* _Ptr) noexcept { // get the address of *_Ptr's contained value if it holds alternative _Ty
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx != _Meta_npos,
        "get_if<T>(variant<Types...> *) requires T to occur exactly once in Types. (N4835 [variant.get]/9)");
    return _STD get_if<_Idx>(_Ptr);
}
template <class _Ty, class... _Types>
_NODISCARD constexpr add_pointer_t<const _Ty> get_if(
    const variant<_Types...>* _Ptr) noexcept { // get the address of *_Ptr's contained value if it holds alternative _Ty
    constexpr size_t _Idx = _Meta_find_unique_index<variant<_Types...>, _Ty>::value;
    static_assert(_Idx != _Meta_npos,
        "get_if<T>(const variant<Types...> *) requires T to occur exactly once in Types. (N4835 [variant.get]/9)");
    return _STD get_if<_Idx>(_Ptr);
}

template <class _Op, class _Result, class... _Types>
struct _Variant_relop_visitor2 { // evaluate _Op with the contained value of two variants that hold the same alternative
    const _Variant_storage<_Types...>& _Left;

    template <class _Ty, size_t _Idx>
    _NODISCARD constexpr _Result operator()(_Tagged<const _Ty&, _Idx> _Right) const
        noexcept(disjunction_v<bool_constant<_Idx == variant_npos>,
            is_nothrow_invocable_r<_Result, _Op, const _Ty&, const _Ty&>>) {
        // determine the relationship between the stored values of _Left and _Right
        // pre: _Left.index() == _Idx && _Right.index() == _Idx
        if constexpr (_Idx != variant_npos) {
            return _Op{}(_Variant_raw_get<_Idx>(_Left), _Right._Val);
        } else { // return whatever _Op returns for equal values
            (void) _Right; // TRANSITION, DevCom-1004719
            return _Op{}(0, 0);
        }
    }
};

template <class... _Types>
_NODISCARD constexpr bool operator==(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
    conjunction_v<is_nothrow_invocable_r<bool, equal_to<>, const _Types&, const _Types&>...>) /* strengthened */ {
    // determine if the arguments are both valueless or contain equal values
    using _Visitor            = _Variant_relop_visitor2<equal_to<>, bool, _Types...>;
    const size_t _Right_index = _Right.index();
    return _Left.index() == _Right_index
        && _Variant_raw_visit(_Right_index, _Right._Storage(), _Visitor{_Left._Storage()});
}

template <class... _Types>
_NODISCARD constexpr bool operator!=(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
    conjunction_v<is_nothrow_invocable_r<bool, not_equal_to<>, const _Types&, const _Types&>...>) /* strengthened */ {
    // determine if the arguments have different active alternatives or contain unequal values
    using _Visitor            = _Variant_relop_visitor2<not_equal_to<>, bool, _Types...>;
    const size_t _Right_index = _Right.index();
    return _Left.index() != _Right_index
        || _Variant_raw_visit(_Right_index, _Right._Storage(), _Visitor{_Left._Storage()});
}

template <class... _Types>
_NODISCARD constexpr bool operator<(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
    conjunction_v<is_nothrow_invocable_r<bool, less<>, const _Types&, const _Types&>...>) /* strengthened */ {
    // determine if _Left has a lesser index(), or equal index() and lesser
    // contained value than _Right
    using _Visitor             = _Variant_relop_visitor2<less<>, bool, _Types...>;
    const size_t _Left_offset  = _Left.index() + 1;
    const size_t _Right_offset = _Right.index() + 1;
    return _Left_offset < _Right_offset
        || (_Left_offset == _Right_offset
            && _Variant_raw_visit(_Right_offset - 1, _Right._Storage(), _Visitor{_Left._Storage()}));
}

template <class... _Types>
_NODISCARD constexpr bool operator>(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
    conjunction_v<is_nothrow_invocable_r<bool, greater<>, const _Types&, const _Types&>...>) /* strengthened */ {
    // determine if _Left has a greater index(), or equal index() and
    // greater contained value than _Right
    using _Visitor             = _Variant_relop_visitor2<greater<>, bool, _Types...>;
    const size_t _Left_offset  = _Left.index() + 1;
    const size_t _Right_offset = _Right.index() + 1;
    return _Left_offset > _Right_offset
        || (_Left_offset == _Right_offset
            && _Variant_raw_visit(_Right_offset - 1, _Right._Storage(), _Visitor{_Left._Storage()}));
}

template <class... _Types>
_NODISCARD constexpr bool operator<=(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
    conjunction_v<is_nothrow_invocable_r<bool, less_equal<>, const _Types&, const _Types&>...>) /* strengthened */ {
    // determine if _Left's index() is less than _Right's, or equal and
    // _Left contains a value less than or equal to _Right
    using _Visitor             = _Variant_relop_visitor2<less_equal<>, bool, _Types...>;
    const size_t _Left_offset  = _Left.index() + 1;
    const size_t _Right_offset = _Right.index() + 1;
    return _Left_offset < _Right_offset
        || (_Left_offset == _Right_offset
            && _Variant_raw_visit(_Right_offset - 1, _Right._Storage(), _Visitor{_Left._Storage()}));
}

template <class... _Types>
_NODISCARD constexpr bool operator>=(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
    conjunction_v<is_nothrow_invocable_r<bool, greater_equal<>, const _Types&, const _Types&>...>) /* strengthened */ {
    // determine if _Left's index() is greater than _Right's, or equal and
    // _Left contains a value greater than or equal to _Right
    using _Visitor             = _Variant_relop_visitor2<greater_equal<>, bool, _Types...>;
    const size_t _Left_offset  = _Left.index() + 1;
    const size_t _Right_offset = _Right.index() + 1;
    return _Left_offset > _Right_offset
        || (_Left_offset == _Right_offset
            && _Variant_raw_visit(_Right_offset - 1, _Right._Storage(), _Visitor{_Left._Storage()}));
}

#ifdef __cpp_lib_concepts
// clang-format off
template <class... _Types>
    requires (three_way_comparable<_Types> && ...)
_NODISCARD constexpr common_comparison_category_t<compare_three_way_result_t<_Types>...>
    operator<=>(const variant<_Types...>& _Left, const variant<_Types...>& _Right) noexcept(
        conjunction_v<is_nothrow_invocable_r<common_comparison_category_t<compare_three_way_result_t<_Types>...>,
            compare_three_way, const _Types&, const _Types&>...>) /* strengthened */ {
    // clang-format on
    // determine the three-way comparison of _Left's and _Right's index, if equal
    // return the three-way comparison of the contained values of _Left and _Right
    using _Visitor             = _Variant_relop_visitor2<compare_three_way,
        common_comparison_category_t<compare_three_way_result_t<_Types>...>, _Types...>;
    const size_t _Left_offset  = _Left.index() + 1;
    const size_t _Right_offset = _Right.index() + 1;
    const auto _Offset_order   = _Left_offset <=> _Right_offset;
    return _Offset_order != 0 ? _Offset_order
                              : _Variant_raw_visit(_Right_offset - 1, _Right._Storage(), _Visitor{_Left._Storage()});
}
#endif // __cpp_lib_concepts

template <class... _Variants>
inline constexpr size_t _Variant_total_states =
    (size_t{1} * ... * (variant_size_v<_Variants> + 1)); // +1 to account for the valueless state

_NODISCARD constexpr size_t _Variant_visit_index1(const size_t _Acc) noexcept {
    return _Acc;
}
template <class _FirstTy, class... _RestTys>
_NODISCARD constexpr size_t _Variant_visit_index1(size_t _Acc, const _FirstTy& _First,
    const _RestTys&... _Rest) noexcept { // calculate a canonical index from the biased indices of the variants _First
                                         // and _Rest...
    _Acc += (_First.index() + 1) * _Variant_total_states<_RestTys...>;
    return _Variant_visit_index1(_Acc, _Rest...);
}

template <class _Callable, class... _Types>
using _Variant_visit_result_t =
    decltype(_STD invoke(_STD declval<_Callable>(), _Variant_raw_get<0>(_STD declval<_Types>()._Storage())...));

template <class>
struct _Variant_dispatcher;

template <size_t... _Is>
struct _Variant_dispatcher<index_sequence<_Is...>> {
    template <class _Ret, class _Callable, class... _Types, bool _Any_valueless = ((_Is == 0) || ...)>
    _NODISCARD static constexpr _Ret _Dispatch2(_Callable&& _Obj, _Types&&... _Args) {
        if constexpr (_Any_valueless) {
            (void) _Obj; // TRANSITION, DevCom-1004719
            ((void) _Args, ...); // TRANSITION, DevCom-1004719
            _Throw_bad_variant_access();
        }
#if _HAS_CXX20
        else if constexpr (is_void_v<_Ret>) {
            static_cast<void>(_STD invoke(
                static_cast<_Callable&&>(_Obj), _Variant_raw_get<_Is - 1>(static_cast<_Types&&>(_Args)._Storage())...));
        }
#endif // _HAS_CXX20
        else {
            return _STD invoke(
                static_cast<_Callable&&>(_Obj), _Variant_raw_get<_Is - 1>(static_cast<_Types&&>(_Args)._Storage())...);
        }
    }
};

template <class _Ret, class _Ordinals, class _Callable, class _Variants>
struct _Variant_dispatch_table; // undefined

template <class _Ret, class... _Ordinals, class _Callable, class... _Variants>
struct _Variant_dispatch_table<_Ret, _Meta_list<_Ordinals...>, _Callable,
    _Meta_list<_Variants...>> { // map from canonical index to visitation target
    using _Dispatch_t                     = _Ret (*)(_Callable&&, _Variants&&...);
    static constexpr _Dispatch_t _Array[] = {
        &_Variant_dispatcher<_Ordinals>::template _Dispatch2<_Ret, _Callable, _Variants...>...};
};

template <class _Callable, class _IndexSequence, class... _Variants>
struct _Variant_single_visit_result; // undefined

template <class _Callable, size_t... _Idxs, class... _Variants>
struct _Variant_single_visit_result<_Callable, index_sequence<_Idxs...>,
    _Variants...> { // result type/category from invoking _Callable with the elements of
                    // _Variants... at (_Idxs - 1)...
    using type = decltype(_STD invoke(_STD declval<_Callable>(),
        _Variant_raw_get<_Idxs == 0 ? 0 : _Idxs - 1>(_STD declval<_Variants>()._Storage())...));
};

template <class _Callable, class _ListOfIndexVectors, class... _Variants>
struct _Variant_all_visit_results_same; // undefined

template <class _Callable, class... _IndexVectors, class... _Variants>
struct _Variant_all_visit_results_same<_Callable, _Meta_list<_IndexVectors...>, _Variants...>
    : _All_same<typename _Variant_single_visit_result<_Callable, _IndexVectors,
          _Variants...>::type...>::type { // true_type iff invocation of _Callable on the elements of _Variants with all
                                          // sequences of indices in _IndexVectors has the same type and value category.
};

template <class _To, class _Callable, class _ListOfIndexVectors, class... _Variants>
struct _Variant_all_visit_results_implicitly_convertible; // undefined

template <class _To, class _Callable, class... _IndexVectors, class... _Variants>
struct _Variant_all_visit_results_implicitly_convertible<_To, _Callable, _Meta_list<_IndexVectors...>, _Variants...>
    : _Convertible_from_all<_To,
          typename _Variant_single_visit_result<_Callable, _IndexVectors, _Variants...>::type...>::type {
    // true_type if invocation of _Callable on the elements of _Variants with all sequences of indices in _IndexVectors
    // is implicitly convertible to _To.
};

template <int _Strategy>
struct _Visit_strategy;

template <>
struct _Visit_strategy<-1> { // Fallback strategy for visitations with too many total states for the
                             // following "switch" strategies.
    template <class _Ret, class _ListOfIndexVectors, class _Callable, class... _Variants>
    static constexpr _Ret _Visit2(
        size_t _Idx, _Callable&& _Obj, _Variants&&... _Args) { // dispatch a visitation with many potential states
        constexpr size_t _Size = _Variant_total_states<_Remove_cvref_t<_Variants>...>;
        static_assert(_Size > 256);
        constexpr auto& _Array =
            _Variant_dispatch_table<_Ret, _ListOfIndexVectors, _Callable, _Meta_list<_Variants...>>::_Array;
        return _Array[_Idx](static_cast<_Callable&&>(_Obj), static_cast<_Variants&&>(_Args)...);
    }
};

template <>
struct _Visit_strategy<0> {
    template <class _Ret, class, class _Callable>
    static constexpr _Ret _Visit2(size_t, _Callable&& _Obj) { // dispatch a visitation with 4^0 potential states
        if constexpr (is_void_v<_Ret>) {
            return static_cast<void>(static_cast<_Callable&&>(_Obj)());
        } else {
            return static_cast<_Callable&&>(_Obj)();
        }
    }
};

#define _STL_CASE(n)                                                                                  \
    case (n):                                                                                         \
        if constexpr ((n) < _Size) {                                                                  \
            using _Indices = _Meta_at_c<_ListOfIndexVectors, (n)>;                                    \
            return _Variant_dispatcher<_Indices>::template _Dispatch2<_Ret, _Callable, _Variants...>( \
                static_cast<_Callable&&>(_Obj), static_cast<_Variants&&>(_Args)...);                  \
        }                                                                                             \
        _STL_UNREACHABLE

#define _STL_VISIT_STAMP(stamper, n)                                               \
    constexpr size_t _Size = _Variant_total_states<_Remove_cvref_t<_Variants>...>; \
    static_assert(_Size > (n) / 4 && _Size <= (n));                                \
    switch (_Idx) {                                                                \
        stamper(0, _STL_CASE);                                                     \
    default:                                                                       \
        _STL_UNREACHABLE;                                                          \
    }

template <>
struct _Visit_strategy<1> {
    template <class _Ret, class _ListOfIndexVectors, class _Callable, class... _Variants>
    static constexpr _Ret _Visit2(
        size_t _Idx, _Callable&& _Obj, _Variants&&... _Args) { // dispatch a visitation with 4^1 potential states
        _STL_STAMP(4, _STL_VISIT_STAMP);
    }
};

template <>
struct _Visit_strategy<2> {
    template <class _Ret, class _ListOfIndexVectors, class _Callable, class... _Variants>
    static constexpr _Ret _Visit2(
        size_t _Idx, _Callable&& _Obj, _Variants&&... _Args) { // dispatch a visitation with 4^2 potential states
        _STL_STAMP(16, _STL_VISIT_STAMP);
    }
};

template <>
struct _Visit_strategy<3> {
    template <class _Ret, class _ListOfIndexVectors, class _Callable, class... _Variants>
    static constexpr _Ret _Visit2(
        size_t _Idx, _Callable&& _Obj, _Variants&&... _Args) { // dispatch a visitation with 4^3 potential states
        _STL_STAMP(64, _STL_VISIT_STAMP);
    }
};

template <>
struct _Visit_strategy<4> {
    template <class _Ret, class _ListOfIndexVectors, class _Callable, class... _Variants>
    static constexpr _Ret _Visit2(
        size_t _Idx, _Callable&& _Obj, _Variants&&... _Args) { // dispatch a visitation with 4^4 potential states
        _STL_STAMP(256, _STL_VISIT_STAMP);
    }
};

#undef _STL_VISIT_STAMP
#undef _STL_CASE

template <class... _Types>
variant<_Types...>& _As_variant_(variant<_Types...>&);
template <class... _Types>
const variant<_Types...>& _As_variant_(const variant<_Types...>&);
template <class... _Types>
variant<_Types...>&& _As_variant_(variant<_Types...>&&);
template <class... _Types>
const variant<_Types...>&& _As_variant_(const variant<_Types...>&&);
template <class _Ty>
using _As_variant = // Deduce variant specialization from a derived type
    decltype(_As_variant_(_STD declval<_Ty>()));

template <size_t _Size, class _Ret, class _ListOfIndexVectors, class _Callable, class... _Variants>
constexpr _Ret _Visit_impl(_Callable&& _Obj, _Variants&&... _Args) {
    constexpr int _Strategy = _Size == 1   ? 0
                            : _Size <= 4   ? 1
                            : _Size <= 16  ? 2
                            : _Size <= 64  ? 3
                            : _Size <= 256 ? 4
                                           : -1;
    return _Visit_strategy<_Strategy>::template _Visit2<_Ret, _ListOfIndexVectors>(
        _Variant_visit_index1(0, static_cast<_As_variant<_Variants>&>(_Args)...), static_cast<_Callable&&>(_Obj),
        static_cast<_As_variant<_Variants>&&>(_Args)...);
}

template <class _Callable, class... _Variants, class = void_t<_As_variant<_Variants>...>>
constexpr _Variant_visit_result_t<_Callable, _As_variant<_Variants>...> visit(_Callable&& _Obj, _Variants&&... _Args) {
    // Invoke _Obj with the contained values of _Args...
    constexpr auto _Size = _Variant_total_states<_Remove_cvref_t<_As_variant<_Variants>>...>;
    using _ListOfIndexLists =
        _Meta_list<_Meta_as_list<make_index_sequence<1 + variant_size_v<_Remove_cvref_t<_As_variant<_Variants>>>>>...>;
    using _ListOfIndexVectors =
        _Meta_transform<_Meta_quote<_Meta_as_integer_sequence>, _Meta_cartesian_product<_ListOfIndexLists>>;
    using _Ret = _Variant_visit_result_t<_Callable, _As_variant<_Variants>...>;
    static_assert(_Variant_all_visit_results_same<_Callable, _ListOfIndexVectors, _As_variant<_Variants>...>::value,
        "visit() requires the result of all potential invocations to have the same type and value category "
        "(N4835 [variant.visit]/2).");

    return _Visit_impl<_Size, _Ret, _ListOfIndexVectors>(
        static_cast<_Callable&&>(_Obj), static_cast<_Variants&&>(_Args)...);
}

#if _HAS_CXX20
template <class _Ret, class _Callable, class... _Variants, class = void_t<_As_variant<_Variants>...>>
constexpr _Ret visit(_Callable&& _Obj, _Variants&&... _Args) {
    constexpr auto _Size = _Variant_total_states<_Remove_cvref_t<_As_variant<_Variants>>...>;
    using _ListOfIndexLists =
        _Meta_list<_Meta_as_list<make_index_sequence<1 + variant_size_v<_Remove_cvref_t<_As_variant<_Variants>>>>>...>;
    using _ListOfIndexVectors =
        _Meta_transform<_Meta_quote<_Meta_as_integer_sequence>, _Meta_cartesian_product<_ListOfIndexLists>>;
    if constexpr (!is_void_v<_Ret>) {
        static_assert(_Variant_all_visit_results_implicitly_convertible<_Ret, _Callable, _ListOfIndexVectors,
                          _As_variant<_Variants>...>::value,
            "visit<R>() requires the result of all potential invocations to be implicitly convertible to R "
            "(N4835 [variant.visit]/2).");
    }

    return _Visit_impl<_Size, _Ret, _ListOfIndexVectors>(
        static_cast<_Callable&&>(_Obj), static_cast<_Variants&&>(_Args)...);
}
#endif // _HAS_CXX20

_NODISCARD constexpr bool operator==(monostate, monostate) noexcept {
    return true;
}

#if _HAS_CXX20
_NODISCARD constexpr strong_ordering operator<=>(monostate, monostate) noexcept {
    return strong_ordering::equal;
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
_NODISCARD constexpr bool operator!=(monostate, monostate) noexcept {
    return false;
}
_NODISCARD constexpr bool operator<(monostate, monostate) noexcept {
    return false;
}
_NODISCARD constexpr bool operator>(monostate, monostate) noexcept {
    return false;
}
_NODISCARD constexpr bool operator<=(monostate, monostate) noexcept {
    return true;
}
_NODISCARD constexpr bool operator>=(monostate, monostate) noexcept {
    return true;
}
#endif // !_HAS_CXX20

template <class... _Types,
    enable_if_t<conjunction_v<is_move_constructible<_Types>..., is_swappable<_Types>...>, int> = 0>
_CONSTEXPR20 void swap(variant<_Types...>& _Left, variant<_Types...>& _Right) noexcept(noexcept(_Left.swap(_Right))) {
    _Left.swap(_Right);
}

struct _Variant_hash_visitor { // visitation function for hashing variants
    template <class _Ty, size_t _Idx>
    _NODISCARD size_t operator()(_Tagged<const _Ty&, _Idx> _Obj) const
        noexcept(disjunction_v<bool_constant<_Idx == variant_npos>,
            is_nothrow_invocable<hash<_Ty>, const _Ty&>>) { // hash contained value _Obj
        if constexpr (_Idx == variant_npos) { // hash a valueless variant
            return 0;
        } else { // hash the contained value
            return hash<_Ty>{}(_Obj._Val);
        }
    }
};

template <class... _Types>
struct hash<variant<_Types...>> : _Conditionally_enabled_hash<variant<_Types...>,
                                      conjunction_v<is_default_constructible<hash<remove_const_t<_Types>>>...>> {
    _NODISCARD static size_t _Do_hash(const variant<_Types...>& _Var) noexcept(
        conjunction_v<_Is_nothrow_hashable<remove_const_t<_Types>>...>) {
        // called from the CRTP base to hash _Var iff the hash is enabled
        return _Variant_raw_visit(_Var.index(), _Var._Storage(), _Variant_hash_visitor{});
    }
};

template <>
struct hash<monostate> {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef monostate _ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef size_t _RESULT_TYPE_NAME;

    _NODISCARD size_t operator()(monostate) const noexcept {
        return 1729; // Arbitrary value
    }
};

_STD_END

#undef _STL_STAMP
#undef _STL_STAMP256
#undef _STL_STAMP64
#undef _STL_STAMP16
#undef _STL_STAMP4

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _HAS_CXX17
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _VARIANT_
