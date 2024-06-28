// span standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _SPAN_
#define _SPAN_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX20
#pragma message("The contents of <span> are available only with C++20 or later.")
#else // ^^^ !_HAS_CXX20 / _HAS_CXX20 vvv
#include <compare>
#include <cstddef>
#include <type_traits>
#include <xutility>


#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

inline constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template <class _Ty>
struct _Span_iterator {
#ifdef __cpp_lib_concepts
    using iterator_concept  = contiguous_iterator_tag;
#endif // __cpp_lib_concepts
    using iterator_category = random_access_iterator_tag;
    using value_type        = remove_cv_t<_Ty>;
    using difference_type   = ptrdiff_t;
    using pointer           = _Ty*;
    using reference         = _Ty&;

    _NODISCARD constexpr reference operator*() const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(_Mybegin, "cannot dereference value-initialized span iterator");
        _STL_VERIFY(_Myptr < _Myend, "cannot dereference end span iterator");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        return *_Myptr;
    }

    _NODISCARD constexpr pointer operator->() const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(_Mybegin, "cannot dereference value-initialized span iterator");
        _STL_VERIFY(_Myptr < _Myend, "cannot dereference end span iterator");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        return _Myptr;
    }

    constexpr _Span_iterator& operator++() noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(_Mybegin, "cannot increment value-initialized span iterator");
        _STL_VERIFY(_Myptr < _Myend, "cannot increment span iterator past end");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        ++_Myptr;
        return *this;
    }

    constexpr _Span_iterator operator++(int) noexcept {
        _Span_iterator _Tmp{*this};
        ++*this;
        return _Tmp;
    }

    constexpr _Span_iterator& operator--() noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(_Mybegin, "cannot decrement value-initialized span iterator");
        _STL_VERIFY(_Mybegin < _Myptr, "cannot decrement span iterator before begin");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        --_Myptr;
        return *this;
    }

    constexpr _Span_iterator operator--(int) noexcept {
        _Span_iterator _Tmp{*this};
        --*this;
        return _Tmp;
    }

    constexpr void _Verify_offset([[maybe_unused]] const difference_type _Off) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        if (_Off != 0) {
            _STL_VERIFY(_Mybegin, "cannot seek value-initialized span iterator");
        }

        if (_Off < 0) {
            _STL_VERIFY(_Myptr - _Mybegin >= -_Off, "cannot seek span iterator before begin");
        }

        if (_Off > 0) {
            _STL_VERIFY(_Myend - _Myptr >= _Off, "cannot seek span iterator after end");
        }
#endif // _ITERATOR_DEBUG_LEVEL >= 1
    }

    constexpr _Span_iterator& operator+=(const difference_type _Off) noexcept {
        _Verify_offset(_Off);
        _Myptr += _Off;
        return *this;
    }

    _NODISCARD constexpr _Span_iterator operator+(const difference_type _Off) const noexcept {
        _Span_iterator _Tmp{*this};
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND constexpr _Span_iterator operator+(const difference_type _Off, _Span_iterator _Next) noexcept {
        return _Next += _Off;
    }

    constexpr _Span_iterator& operator-=(const difference_type _Off) noexcept {
        _Verify_offset(-_Off);
        _Myptr -= _Off;
        return *this;
    }

    _NODISCARD constexpr _Span_iterator operator-(const difference_type _Off) const noexcept {
        _Span_iterator _Tmp{*this};
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD constexpr difference_type operator-(const _Span_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(
            _Mybegin == _Right._Mybegin && _Myend == _Right._Myend, "cannot subtract incompatible span iterators");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        return _Myptr - _Right._Myptr;
    }

    _NODISCARD constexpr reference operator[](const difference_type _Off) const noexcept {
        return *(*this + _Off);
    }

    _NODISCARD constexpr bool operator==(const _Span_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(_Mybegin == _Right._Mybegin && _Myend == _Right._Myend,
            "cannot compare incompatible span iterators for equality");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        return _Myptr == _Right._Myptr;
    }

    _NODISCARD constexpr strong_ordering operator<=>(const _Span_iterator& _Right) const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        _STL_VERIFY(
            _Mybegin == _Right._Mybegin && _Myend == _Right._Myend, "cannot compare incompatible span iterators");
#endif // _ITERATOR_DEBUG_LEVEL >= 1
        return _Myptr <=> _Right._Myptr;
    }

#if _ITERATOR_DEBUG_LEVEL >= 1
    friend constexpr void _Verify_range(const _Span_iterator& _First, const _Span_iterator& _Last) noexcept {
        _STL_VERIFY(_First._Mybegin == _Last._Mybegin && _First._Myend == _Last._Myend,
            "span iterators from different views do not form a range");
        _STL_VERIFY(_First._Myptr <= _Last._Myptr, "span iterator range transposed");
    }
#endif // _ITERATOR_DEBUG_LEVEL >= 1

    using _Prevent_inheriting_unwrap = _Span_iterator;

    _NODISCARD constexpr pointer _Unwrapped() const noexcept {
        return _Myptr;
    }

    static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;

    constexpr void _Seek_to(const pointer _It) noexcept {
        _Myptr = _It;
    }

    pointer _Myptr   = nullptr;
#if _ITERATOR_DEBUG_LEVEL >= 1
    pointer _Mybegin = nullptr;
    pointer _Myend   = nullptr;
#endif // _ITERATOR_DEBUG_LEVEL >= 1
};

template <class _Ty>
struct pointer_traits<_Span_iterator<_Ty>> {
    using pointer         = _Span_iterator<_Ty>;
    using element_type    = _Ty;
    using difference_type = ptrdiff_t;

    _NODISCARD static constexpr element_type* to_address(const pointer _Iter) noexcept {
        return _Iter._Unwrapped();
    }
};

template <class _Ty, size_t _Extent>
struct _Span_extent_type {
    using pointer = _Ty*;

    constexpr _Span_extent_type() noexcept = default;

    constexpr explicit _Span_extent_type(const pointer _Data, size_t) noexcept : _Mydata{_Data} {}

    pointer _Mydata{nullptr};
    static constexpr size_t _Mysize = _Extent;
};

template <class _Ty>
struct _Span_extent_type<_Ty, dynamic_extent> {
    using pointer = _Ty*;

    constexpr _Span_extent_type() noexcept = default;

    constexpr explicit _Span_extent_type(const pointer _Data, const size_t _Size) noexcept
        : _Mydata{_Data}, _Mysize{_Size} {}

    pointer _Mydata{nullptr};
    size_t _Mysize{0};
};

template <class _Ty, size_t _Size>
class array;

template <class _Ty, size_t _Extent>
class span;

#ifdef __cpp_lib_concepts
namespace ranges {
    template <class _Ty, size_t _Extent>
    inline constexpr bool enable_view<span<_Ty, _Extent>> = true;
    template <class _Ty, size_t _Extent>
    inline constexpr bool enable_borrowed_range<span<_Ty, _Extent>> = true;
} // namespace ranges

template <class>
inline constexpr bool _Is_span_v = false;

template <class _Ty, size_t _Extent>
inline constexpr bool _Is_span_v<span<_Ty, _Extent>> = true;

template <class>
inline constexpr bool _Is_std_array_v = false;

template <class _Ty, size_t _Size>
inline constexpr bool _Is_std_array_v<array<_Ty, _Size>> = true;

// clang-format off
template <class _It, class _Ty>
concept _Span_compatible_iterator = contiguous_iterator<_It>
                                 && is_convertible_v<remove_reference_t<iter_reference_t<_It>>(*)[], _Ty(*)[]>;

template <class _Sentinel, class _It>
concept _Span_compatible_sentinel = sized_sentinel_for<_Sentinel, _It>
                                 && !is_convertible_v<_Sentinel, size_t>;

template <class _Rng, class _Ty>
concept _Span_compatible_range =
    !is_array_v<remove_cvref_t<_Rng>>
 && !_Is_span_v<remove_cvref_t<_Rng>>
 && !_Is_std_array_v<remove_cvref_t<_Rng>>
 && _RANGES contiguous_range<_Rng>
 && _RANGES sized_range<_Rng>
 && (_RANGES borrowed_range<_Rng> || is_const_v<_Ty>)
 && is_convertible_v<remove_reference_t<_RANGES range_reference_t<_Rng>>(*)[], _Ty(*)[]>;
// clang-format on
#else // ^^^ __cpp_lib_concepts / !__cpp_lib_concepts vvv

template <class>
struct _Is_span : false_type {};

template <class _Ty, size_t _Extent>
struct _Is_span<span<_Ty, _Extent>> : true_type {};

template <class>
struct _Is_std_array : false_type {};

template <class _Ty, size_t _Size>
struct _Is_std_array<array<_Ty, _Size>> : true_type {};

template <class _Rng, class _Ty>
struct _Is_span_convertible_range
    : bool_constant<is_convertible_v<remove_pointer_t<decltype(_STD data(_STD declval<_Rng&>()))> (*)[], _Ty (*)[]>> {};

template <class, class = void>
struct _Has_container_interface : false_type {};

template <class _Rng>
struct _Has_container_interface<_Rng,
    void_t<decltype(_STD data(_STD declval<_Rng&>())), decltype(_STD size(_STD declval<_Rng&>()))>> : true_type {};

// clang-format off
template <class _Rng, class _Ty>
inline constexpr bool _Is_span_compatible_range = conjunction_v<
    negation<is_array<_Rng>>,
    negation<_Is_span<remove_const_t<_Rng>>>,
    negation<_Is_std_array<remove_const_t<_Rng>>>,
    _Has_container_interface<_Rng>,
    _Is_span_convertible_range<_Rng, _Ty>>;
// clang-format on

#endif // !__cpp_lib_concepts

// [views.span]
template <class _Ty, size_t _Extent = dynamic_extent>
class span : private _Span_extent_type<_Ty, _Extent> {
private:
    using _Mybase = _Span_extent_type<_Ty, _Extent>;
    using _Mybase::_Mydata;
    using _Mybase::_Mysize;

public:
    using element_type     = _Ty;
    using value_type       = remove_cv_t<_Ty>;
    using size_type        = size_t;
    using difference_type  = ptrdiff_t;
    using pointer          = _Ty*;
    using const_pointer    = const _Ty*;
    using reference        = _Ty&;
    using const_reference  = const _Ty&;
    using iterator         = _Span_iterator<_Ty>;
    using reverse_iterator = _STD reverse_iterator<iterator>;

    static constexpr size_type extent = _Extent;

    // [span.cons] Constructors, copy, and assignment
#ifdef __cpp_lib_concepts

    // clang-format off
    constexpr span() noexcept requires (_Extent == 0 || _Extent == dynamic_extent) = default;

    template <_Span_compatible_iterator<element_type> _It>
    constexpr explicit(_Extent != dynamic_extent) span(_It _First, size_type _Count) noexcept // strengthened
        : _Mybase(_STD to_address(_Get_unwrapped_n(_First, _Count)), _Count) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_Count == _Extent,
                "Cannot construct span with static extent from range [first, first + count) as count != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

    template <_Span_compatible_iterator<element_type> _It, _Span_compatible_sentinel<_It> _Sentinel>
    constexpr explicit(_Extent != dynamic_extent) span(_It _First, _Sentinel _Last)
        noexcept(noexcept(_Last - _First)) // strengthened
        : _Mybase(_STD to_address(_First), static_cast<size_type>(_Last - _First)) {
        _Adl_verify_range(_First, _Last);
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_Last - _First == _Extent,
                "Cannot construct span with static extent from range [first, last) as last - first != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

    template <size_t _Size>
        requires (_Extent == dynamic_extent || _Extent == _Size)
    constexpr span(type_identity_t<element_type> (&_Arr)[_Size]) noexcept : _Mybase(_Arr, _Size) {}

    template <class _OtherTy, size_t _Size>
        requires (_Extent == dynamic_extent || _Extent == _Size)
              && is_convertible_v<_OtherTy (*)[], element_type (*)[]>
    constexpr span(array<_OtherTy, _Size>& _Arr) noexcept : _Mybase(_Arr.data(), _Size) {}

    template <class _OtherTy, size_t _Size>
        requires (_Extent == dynamic_extent || _Extent == _Size)
              && is_convertible_v<const _OtherTy (*)[], element_type (*)[]>
    constexpr span(const array<_OtherTy, _Size>& _Arr) noexcept : _Mybase(_Arr.data(), _Size) {}

    template <_Span_compatible_range<element_type> _Rng>
    constexpr explicit(_Extent != dynamic_extent) span(_Rng&& _Range)
        : _Mybase(_RANGES data(_Range), static_cast<size_type>(_RANGES size(_Range))) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_RANGES size(_Range) == _Extent,
                "Cannot construct span with static extent from range r as std::ranges::size(r) != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }

    template <class _OtherTy, size_t _OtherExtent>
        requires (_Extent == dynamic_extent || _OtherExtent == dynamic_extent || _Extent == _OtherExtent)
              && is_convertible_v<_OtherTy (*)[], element_type (*)[]>
    constexpr explicit(_Extent != dynamic_extent && _OtherExtent == dynamic_extent)
    span(const span<_OtherTy, _OtherExtent>& _Other) noexcept
        : _Mybase(_Other.data(), _Other.size()) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_Other.size() == _Extent,
                "Cannot construct span with static extent from other span as other.size() != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }
    // clang-format on

#else // ^^^ __cpp_lib_concepts / !__cpp_lib_concepts vvv

    template <size_t _Ext = _Extent, enable_if_t<_Ext == 0 || _Ext == dynamic_extent, int> = 0>
    constexpr span() noexcept {}

    constexpr explicit(_Extent != dynamic_extent) span(pointer _Ptr, size_type _Count) noexcept // strengthened
        : _Mybase(_Ptr, _Count) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_Count == _Extent,
                "Cannot construct span with static extent from range [ptr, ptr + count) as count != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

    constexpr explicit(_Extent != dynamic_extent) span(pointer _First, pointer _Last) noexcept // strengthened
        : _Mybase(_First, static_cast<size_type>(_Last - _First)) {
        _Adl_verify_range(_First, _Last);
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_Last - _First == _Extent,
                "Cannot construct span with static extent from range [first, last) as last - first != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

    template <size_t _Size, enable_if_t<_Extent == dynamic_extent || _Extent == _Size, int> = 0>
    constexpr span(element_type (&_Arr)[_Size]) noexcept : _Mybase(_Arr, _Size) {}

    template <class _OtherTy, size_t _Size,
        enable_if_t<conjunction_v<bool_constant<_Extent == dynamic_extent || _Extent == _Size>,
                        is_convertible<_OtherTy (*)[], element_type (*)[]>>,
            int> = 0>
    constexpr span(array<_OtherTy, _Size>& _Arr) noexcept : _Mybase(_Arr.data(), _Size) {}

    template <class _OtherTy, size_t _Size,
        enable_if_t<conjunction_v<bool_constant<_Extent == dynamic_extent || _Extent == _Size>,
                        is_convertible<const _OtherTy (*)[], element_type (*)[]>>,
            int> = 0>
    constexpr span(const array<_OtherTy, _Size>& _Arr) noexcept : _Mybase(_Arr.data(), _Size) {}

    template <class _Rng, enable_if_t<_Is_span_compatible_range<_Rng, element_type>, int> = 0>
    constexpr explicit(_Extent != dynamic_extent) span(_Rng& _Range)
        : _Mybase(_STD data(_Range), static_cast<size_type>(_STD size(_Range))) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_STD size(_Range) == _Extent,
                "Cannot construct span with static extent from range r as std::size(r) != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

    template <class _Rng, enable_if_t<_Is_span_compatible_range<const _Rng, element_type>, int> = 0>
    constexpr explicit(_Extent != dynamic_extent) span(const _Rng& _Range)
        : _Mybase(_STD data(_Range), static_cast<size_type>(_STD size(_Range))) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_STD size(_Range) == _Extent,
                "Cannot construct span with static extent from range r as std::size(r) != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

    template <class _OtherTy, size_t _OtherExtent,
        enable_if_t<conjunction_v<bool_constant<_Extent == dynamic_extent || _OtherExtent == dynamic_extent
                                                || _Extent == _OtherExtent>,
                        is_convertible<_OtherTy (*)[], element_type (*)[]>>,
            int> = 0>
    constexpr explicit(_Extent != dynamic_extent && _OtherExtent == dynamic_extent)
        span(const span<_OtherTy, _OtherExtent>& _Other) noexcept
        : _Mybase(_Other.data(), _Other.size()) {
#if _CONTAINER_DEBUG_LEVEL > 0
        if constexpr (_Extent != dynamic_extent) {
            _STL_VERIFY(_Other.size() == _Extent,
                "Cannot construct span with static extent from other span as other.size() != extent");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
    }

#endif // !__cpp_lib_concepts

    // [span.sub] Subviews
    template <size_t _Count>
    _NODISCARD constexpr auto first() const noexcept /* strengthened */ {
        if constexpr (_Extent != dynamic_extent) {
            static_assert(_Count <= _Extent, "Count out of range in span::first()");
        }
#if _CONTAINER_DEBUG_LEVEL > 0
        else {
            _STL_VERIFY(_Count <= _Mysize, "Count out of range in span::first()");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return span<element_type, _Count>{_Mydata, _Count};
    }

    _NODISCARD constexpr auto first(const size_type _Count) const noexcept
    /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Count <= _Mysize, "Count out of range in span::first(count)");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return span<element_type, dynamic_extent>{_Mydata, _Count};
    }

    template <size_t _Count>
    _NODISCARD constexpr auto last() const noexcept /* strengthened */ {
        if constexpr (_Extent != dynamic_extent) {
            static_assert(_Count <= _Extent, "Count out of range in span::last()");
        }
#if _CONTAINER_DEBUG_LEVEL > 0
        else {
            _STL_VERIFY(_Count <= _Mysize, "Count out of range in span::last()");
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return span<element_type, _Count>{_Mydata + (_Mysize - _Count), _Count};
    }

    _NODISCARD constexpr auto last(const size_type _Count) const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Count <= _Mysize, "Count out of range in span::last(count)");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return span<element_type, dynamic_extent>{_Mydata + (_Mysize - _Count), _Count};
    }

    template <size_t _Offset, size_t _Count = dynamic_extent>
    _NODISCARD constexpr auto subspan() const noexcept /* strengthened */ {
        if constexpr (_Extent != dynamic_extent) {
            static_assert(_Offset <= _Extent, "Offset out of range in span::subspan()");
            static_assert(
                _Count == dynamic_extent || _Count <= _Extent - _Offset, "Count out of range in span::subspan()");
        }
#if _CONTAINER_DEBUG_LEVEL > 0
        else {
            _STL_VERIFY(_Offset <= _Mysize, "Offset out of range in span::subspan()");

            if constexpr (_Count != dynamic_extent) {
                _STL_VERIFY(_Count <= _Mysize - _Offset, "Count out of range in span::subspan()");
            }
        }
#endif // _CONTAINER_DEBUG_LEVEL > 0
        using _ReturnType = span<element_type,
            _Count != dynamic_extent ? _Count : (_Extent != dynamic_extent ? _Extent - _Offset : dynamic_extent)>;
        return _ReturnType{_Mydata + _Offset, _Count == dynamic_extent ? _Mysize - _Offset : _Count};
    }

    _NODISCARD constexpr auto subspan(const size_type _Offset, const size_type _Count = dynamic_extent) const noexcept
    /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Offset <= _Mysize, "Offset out of range in span::subspan(offset, count)");
        _STL_VERIFY(_Count == dynamic_extent || _Count <= _Mysize - _Offset,
            "Count out of range in span::subspan(offset, count)");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        using _ReturnType = span<element_type, dynamic_extent>;
        return _ReturnType{_Mydata + _Offset, _Count == dynamic_extent ? _Mysize - _Offset : _Count};
    }

    // [span.obs] Observers
    _NODISCARD constexpr size_t size() const noexcept {
        return _Mysize;
    }

    _NODISCARD constexpr size_type size_bytes() const noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
#pragma warning(suppress : 4127) // conditional expression is constant
        _STL_VERIFY(_Mysize <= dynamic_extent / sizeof(element_type),
            "size of span in bytes exceeds std::numeric_limits<size_t>::max()");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _Mysize * sizeof(element_type);
    }

    _NODISCARD constexpr bool empty() const noexcept {
        return _Mysize == 0;
    }

    // [span.elem] Element access
    _NODISCARD constexpr reference operator[](const size_type _Off) const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Off < _Mysize, "span index out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _Mydata[_Off];
    }

    _NODISCARD constexpr reference front() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
#pragma warning(suppress : 4127) // conditional expression is constant
        _STL_VERIFY(_Mysize > 0, "front of empty span");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _Mydata[0];
    }

    _NODISCARD constexpr reference back() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
#pragma warning(suppress : 4127) // conditional expression is constant
        _STL_VERIFY(_Mysize > 0, "back of empty span");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        return _Mydata[_Mysize - 1];
    }

    _NODISCARD constexpr pointer data() const noexcept {
        return _Mydata;
    }

    // [span.iterators] Iterator support
    _NODISCARD constexpr iterator begin() const noexcept {
#if _ITERATOR_DEBUG_LEVEL >= 1
        return {_Mydata, _Mydata, _Mydata + _Mysize};
#else // ^^^ _ITERATOR_DEBUG_LEVEL >= 1 ^^^ // vvv _ITERATOR_DEBUG_LEVEL == 0 vvv
        return {_Mydata};
#endif // _ITERATOR_DEBUG_LEVEL
    }

    _NODISCARD constexpr iterator end() const noexcept {
        const auto _End = _Mydata + _Mysize;
#if _ITERATOR_DEBUG_LEVEL >= 1
        return {_End, _Mydata, _End};
#else // ^^^ _ITERATOR_DEBUG_LEVEL >= 1 ^^^ // vvv _ITERATOR_DEBUG_LEVEL == 0 vvv
        return {_End};
#endif // _ITERATOR_DEBUG_LEVEL
    }

    _NODISCARD constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator{end()};
    }

    _NODISCARD constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator{begin()};
    }

    _NODISCARD constexpr pointer _Unchecked_begin() const noexcept {
        return _Mydata;
    }

    _NODISCARD constexpr pointer _Unchecked_end() const noexcept {
        return _Mydata + _Mysize;
    }
};

template <class _Ty, size_t _Extent>
span(_Ty (&)[_Extent]) -> span<_Ty, _Extent>;

template <class _Ty, size_t _Size>
span(array<_Ty, _Size>&) -> span<_Ty, _Size>;

template <class _Ty, size_t _Size>
span(const array<_Ty, _Size>&) -> span<const _Ty, _Size>;

#ifdef __cpp_lib_concepts

template <contiguous_iterator _It, class _End>
span(_It, _End) -> span<remove_reference_t<iter_reference_t<_It>>>;

template <_RANGES contiguous_range _Rng>
span(_Rng&&) -> span<remove_reference_t<_RANGES range_reference_t<_Rng>>>;

#else // ^^^ __cpp_lib_concepts / !__cpp_lib_concepts vvv

template <class _Rng>
span(_Rng&) -> span<typename _Rng::value_type>;

template <class _Rng>
span(const _Rng&) -> span<const typename _Rng::value_type>;

#endif // !__cpp_lib_concepts

#ifdef __cpp_lib_byte
// [span.objectrep] Views of object representation
template <class _Ty, size_t _Extent>
_NODISCARD auto as_bytes(span<_Ty, _Extent> _Sp) noexcept {
    using _ReturnType = span<const byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_Ty) * _Extent>;
    return _ReturnType{reinterpret_cast<const byte*>(_Sp.data()), _Sp.size_bytes()};
}

template <class _Ty, size_t _Extent, enable_if_t<!is_const_v<_Ty>, int> = 0>
_NODISCARD auto as_writable_bytes(span<_Ty, _Extent> _Sp) noexcept {
    using _ReturnType = span<byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_Ty) * _Extent>;
    return _ReturnType{reinterpret_cast<byte*>(_Sp.data()), _Sp.size_bytes()};
}
#endif // __cpp_lib_byte

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _HAS_CXX20
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _SPAN_
