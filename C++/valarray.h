// valarray standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _VALARRAY_
#define _VALARRAY_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <cmath>
#include <xmemory>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
class gslice;
class slice;

template <class _Ty>
class gslice_array;
template <class _Ty>
class indirect_array;
template <class _Ty>
class mask_array;
template <class _Ty>
class slice_array;
template <class _Ty>
class valarray;

template <class _Ty>
_Ty* _Allocate_for_op_delete(size_t _Count) {
    // allocates space for _Count objects of type _Ty
    if (_Count == 0) {
        return nullptr;
    }

    const size_t _Bytes = _Get_size_of_n<sizeof(_Ty)>(_Count);
#ifdef __cpp_aligned_new
    constexpr bool _Extended_alignment = alignof(_Ty) > __STDCPP_DEFAULT_NEW_ALIGNMENT__;
    if constexpr (_Extended_alignment) {
        return static_cast<_Ty*>(::operator new (_Bytes, align_val_t{alignof(_Ty)}));
    } else
#endif // __cpp_aligned_new
    {
        return static_cast<_Ty*>(::operator new(_Bytes));
    }
}

using _Boolarray = valarray<bool>;
using _Sizarray  = valarray<size_t>;

template <class _Ty>
class valarray { // store array with various indexing options
public:
    friend _Tidy_deallocate_guard<valarray>;

    template <class _Ty2>
    friend _Ty2* begin(valarray<_Ty2>& _Array);

    template <class _Ty2>
    friend const _Ty2* begin(const valarray<_Ty2>& _Array);

    template <class _Ty2>
    friend _Ty2* end(valarray<_Ty2>& _Array);

    template <class _Ty2>
    friend const _Ty2* end(const valarray<_Ty2>& _Array);

    using value_type = _Ty;

    valarray() { // construct empty valarray
        _Tidy_init();
    }

    explicit valarray(size_t _Count) { // construct with _Count * _Ty()
        _Tidy_init();
        _Grow(_Count);
    }

    valarray(const _Ty& _Val, size_t _Count) { // construct with _Count * _Val
        _Tidy_init();
        _Grow(_Count, &_Val);
    }

    valarray(const _Ty* _Ptr, size_t _Count) { // construct with [_Ptr, _Ptr + _Count)
        _Tidy_init();
        _Grow(_Count, _Ptr, 1);
    }

    valarray(const valarray& _Right) {
        _Tidy_init();
        _Grow(_Right.size(), _Right._Myptr, 1);
    }

    valarray(const slice_array<_Ty>& _Slicearr) {
        _Tidy_init();
        *this = _Slicearr;
    }

    valarray(const gslice_array<_Ty>& _Gslicearr) {
        _Tidy_init();
        *this = _Gslicearr;
    }

    valarray(const mask_array<_Ty>& _Maskarr) {
        _Tidy_init();
        *this = _Maskarr;
    }

    valarray(const indirect_array<_Ty>& _Indarr) {
        _Tidy_init();
        *this = _Indarr;
    }

    valarray(valarray&& _Right) noexcept {
        _Tidy_init();
        _Assign_rv(_STD move(_Right));
    }

    valarray& operator=(valarray&& _Right) noexcept {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    void _Assign_rv(valarray&& _Right) {
        if (this != _STD addressof(_Right)) { // clear this and steal from _Right
            _Tidy_deallocate();
            _Myptr  = _Right._Myptr;
            _Mysize = _Right._Mysize;
            _Right._Tidy_init();
        }
    }

    valarray(initializer_list<_Ty> _Ilist) {
        _Tidy_init();
        _Grow(_Ilist.size(), _Ilist.begin(), 1);
    }

    valarray& operator=(initializer_list<_Ty> _Ilist) {
        _Assign(_Ilist.size(), _Ilist.begin());
        return *this;
    }

    void swap(valarray& _Right) noexcept {
        if (this != _STD addressof(_Right)) {
            _STD swap(_Myptr, _Right._Myptr);
            _STD swap(_Mysize, _Right._Mysize);
        }
    }

    ~valarray() noexcept {
        _Tidy_deallocate();
    }

    valarray& operator=(const valarray& _Right) {
        if (this != _STD addressof(_Right)) {
            _Assign(_Right.size(), _Right._Myptr);
        }

        return *this;
    }

    valarray& operator=(const _Ty& _Val) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] = _Val;
        }
        return *this;
    }

    void resize(size_t _Newsize) { // determine new length, filling with _Ty() elements
        _Tidy_deallocate();
        _Grow(_Newsize);
    }

    void resize(size_t _Newsize, _Ty _Val) { // determine new length, filling with _Val elements
        _Tidy_deallocate();
        _Grow(_Newsize, &_Val, 0);
    }

    valarray& operator=(const slice_array<_Ty>& _Slicearr); // defined below

    valarray& operator=(const gslice_array<_Ty>& _Gslicearr); // defined below

    valarray& operator=(const mask_array<_Ty>& _Maskarr); // defined below

    valarray& operator=(const indirect_array<_Ty>& _Indarr); // defined below

    _NODISCARD valarray operator+() const {
        const size_t _Size = size();
        valarray<_Ty> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] = +_Myptr[_Idx];
        }
        return _Ans;
    }

    _NODISCARD valarray operator-() const {
        const size_t _Size = size();
        valarray<_Ty> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] = -_Myptr[_Idx];
        }
        return _Ans;
    }

    _NODISCARD valarray operator~() const {
        const size_t _Size = size();
        valarray<_Ty> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] = ~_Myptr[_Idx];
        }
        return _Ans;
    }

    _NODISCARD _Boolarray operator!() const {
        const size_t _Size = size();
        valarray<bool> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] = !_Myptr[_Idx];
        }
        return _Ans;
    }

    valarray& operator*=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] *= _Right;
        }
        return *this;
    }

    valarray& operator/=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] /= _Right;
        }
        return *this;
    }

    valarray& operator%=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] %= _Right;
        }
        return *this;
    }

    valarray& operator+=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] += _Right;
        }
        return *this;
    }

    valarray& operator-=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] -= _Right;
        }
        return *this;
    }

    valarray& operator^=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] ^= _Right;
        }
        return *this;
    }

    valarray& operator&=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] &= _Right;
        }
        return *this;
    }

    valarray& operator|=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] |= _Right;
        }
        return *this;
    }

    valarray& operator<<=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] <<= _Right;
        }
        return *this;
    }

    valarray& operator>>=(const _Ty& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] >>= _Right;
        }
        return *this;
    }

    valarray& operator*=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] *= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator/=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] /= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator%=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] %= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator+=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] += _Right[_Idx];
        }
        return *this;
    }

    valarray& operator-=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] -= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator^=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] ^= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator|=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] |= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator&=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] &= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator<<=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] <<= _Right[_Idx];
        }
        return *this;
    }

    valarray& operator>>=(const valarray& _Right) {
        const size_t _Size = size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Idx] >>= _Right[_Idx];
        }
        return *this;
    }

    _NODISCARD size_t size() const noexcept /* strengthened */ {
        return _Mysize;
    }

    _NODISCARD const _Ty& operator[](size_t _Off) const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Off < _Mysize, "valarray subscript out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return _Myptr[_Off];
    }

    _NODISCARD _Ty& operator[](size_t _Off) noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Off < _Mysize, "valarray subscript out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return _Myptr[_Off];
    }

    _NODISCARD valarray operator[](slice _Slicearr) const; // defined below

    _NODISCARD slice_array<_Ty> operator[](slice _Slicearr); // defined below

    _NODISCARD valarray operator[](const gslice& _Gslicearr) const; // defined below

    _NODISCARD gslice_array<_Ty> operator[](const gslice& _Gslicearr); // defined below

    _NODISCARD valarray operator[](const _Boolarray& _Boolarr) const; // defined below

    _NODISCARD mask_array<_Ty> operator[](const _Boolarray& _Boolarr); // defined below

    _NODISCARD valarray operator[](const _Sizarray& _Indarr) const; // defined below

    _NODISCARD indirect_array<_Ty> operator[](const _Sizarray& _Indarr); // defined below

    _NODISCARD _Ty sum() const {
        const size_t _Size = size();
        _Ty _Sum           = _Myptr[0];
        for (size_t _Idx = 1; _Idx < _Size; ++_Idx) {
            _Sum += _Myptr[_Idx];
        }

        return _Sum;
    }

    _NODISCARD _Ty(min)() const {
        const size_t _Size = size();
        _Ty _Min           = _Myptr[0];
        for (size_t _Idx = 1; _Idx < _Size; ++_Idx) {
            if (_Myptr[_Idx] < _Min) {
                _Min = _Myptr[_Idx];
            }
        }

        return _Min;
    }

    _NODISCARD _Ty(max)() const {
        const size_t _Size = size();
        _Ty _Max           = _Myptr[0];
        for (size_t _Idx = 1; _Idx < _Size; ++_Idx) {
            if (_Max < _Myptr[_Idx]) {
                _Max = _Myptr[_Idx];
            }
        }

        return _Max;
    }

    _NODISCARD valarray shift(int _Count) const {
        const size_t _Size = size();
        valarray<_Ty> _Ans(_Size);
        size_t _Min = 0;
        size_t _Max = _Size;
        if (_Count < 0) {
            const size_t _Skip = static_cast<size_t>(-_Count);
            _Min += _Skip;
        } else {
            const size_t _Skip = static_cast<size_t>(_Count);
            if (_Skip < _Size) {
                _Max -= _Skip;
            } else {
                _Max = 0;
            }
        }
        for (size_t _Idx = _Min; _Idx < _Max; ++_Idx) {
            _Ans[_Idx] = _Myptr[_Idx + _Count];
        }
        return _Ans;
    }

    _NODISCARD valarray cshift(int _Count) const {
        const size_t _Size = size();
        if (_Size != 0) {
            if (_Count < 0) { // right shift
                if (_Size < size_t{0} - _Count) {
                    _Count = static_cast<int>(_Size - (size_t{0} - _Count - _Size) % _Size);
                } else {
                    _Count = static_cast<int>(_Size + _Count);
                }
            } else if (_Size <= static_cast<size_t>(_Count)) {
                _Count %= _Size;
            }
        }

        valarray<_Ty> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] =
                _Size - _Idx <= static_cast<size_t>(_Count) ? _Myptr[_Idx - _Size + _Count] : _Myptr[_Idx + _Count];
        }
        return _Ans;
    }

    _NODISCARD valarray apply(_Ty _Func(_Ty)) const {
        const size_t _Size = size();
        valarray<_Ty> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] = _Func(_Myptr[_Idx]);
        }
        return _Ans;
    }

    _NODISCARD valarray apply(_Ty _Func(const _Ty&)) const {
        // return valarray transformed by _Func, nonmutable argument
        const size_t _Size = size();
        valarray<_Ty> _Ans(_Size);
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans[_Idx] = _Func(_Myptr[_Idx]);
        }
        return _Ans;
    }

private:
    void _Grow(size_t _Newsize) { // allocate space for _Count elements and fill with default values
        if (0 < _Newsize) { // worth doing, allocate
            _Myptr = _Allocate_for_op_delete<_Ty>(_Newsize);
            _Tidy_deallocate_guard<valarray> _Guard{this};
            for (size_t _Idx = 0; _Idx < _Newsize; ++_Idx) {
                _Construct_in_place(_Myptr[_Idx]);
            }

            _Guard._Target = nullptr;
            _Mysize        = _Newsize;
        }
    }

    void _Grow(size_t _Newsize, const _Ty* _Ptr, size_t _Inc = 0) {
        // allocate space for _Count elements and fill with *_Ptr
        if (0 < _Newsize) { // worth doing, allocate
            _Myptr = _Allocate_for_op_delete<_Ty>(_Newsize);
            _Tidy_deallocate_guard<valarray> _Guard{this};
            for (size_t _Idx = 0; _Idx < _Newsize; ++_Idx, _Ptr += _Inc) {
                _Construct_in_place(_Myptr[_Idx], *_Ptr);
            }

            _Guard._Target = nullptr;
            _Mysize        = _Newsize;
        }
    }

    void _Tidy_init() noexcept {
        _Mysize = 0;
        _Myptr  = nullptr;
    }

    void _Tidy_deallocate() noexcept {
        if (_Myptr) { // destroy elements
            for (size_t _Idx = 0; _Idx < _Mysize; ++_Idx) {
                _Destroy_in_place(_Myptr[_Idx]);
            }

#ifdef __cpp_aligned_new
            constexpr bool _Extended_alignment = alignof(_Ty) > __STDCPP_DEFAULT_NEW_ALIGNMENT__;
            if constexpr (_Extended_alignment) {
                ::operator delete (static_cast<void*>(_Myptr), align_val_t{alignof(_Ty)});
            } else
#endif // __cpp_aligned_new
            {
                ::operator delete(static_cast<void*>(_Myptr));
            }
        }

        _Tidy_init();
    }

    void _Assign(size_t _Newsize, const _Ty* _Ptr) {
        const size_t _Size = size();
        if (_Size == _Newsize) {
            for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
                _Myptr[_Idx] = _Ptr[_Idx];
            }
        } else { // resize and copy
            _Tidy_deallocate();
            _Grow(_Newsize, _Ptr, 1);
        }
    }

    _Ty* _Myptr; // current storage reserved for array
    size_t _Mysize; // current length of sequence
};

#if _HAS_CXX17
template <class _Ty, size_t _Nx>
valarray(const _Ty (&)[_Nx], size_t) -> valarray<_Ty>;
#endif // _HAS_CXX17

template <class _Ty>
void swap(valarray<_Ty>& _Left, valarray<_Ty>& _Right) noexcept {
    _Left.swap(_Right);
}

template <class _Ty>
_NODISCARD _Ty* begin(valarray<_Ty>& _Array) {
    return _Array._Myptr;
}

template <class _Ty>
_NODISCARD const _Ty* begin(const valarray<_Ty>& _Array) {
    return _Array._Myptr;
}

template <class _Ty>
_NODISCARD _Ty* end(valarray<_Ty>& _Array) {
    return _Array._Myptr + _Array.size();
}

template <class _Ty>
_NODISCARD const _Ty* end(const valarray<_Ty>& _Array) {
    return _Array._Myptr + _Array.size();
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator*(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] * _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator*(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left * _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator/(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] / _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator/(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left / _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator%(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] % _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator%(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left % _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator+(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] + _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator+(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left + _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator-(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] - _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator-(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left - _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator^(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] ^ _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator^(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left ^ _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator&(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] & _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator&(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left & _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator|(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] | _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator|(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left | _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator<<(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] << _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator<<(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left << _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator>>(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] >> _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator>>(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left >> _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator&&(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] && _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator&&(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left && _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator||(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] || _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator||(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left || _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator*(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] * _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator/(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] / _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator%(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] % _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator+(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] + _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator-(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] - _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator^(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] ^ _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator&(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] & _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator|(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] | _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator<<(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] << _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> operator>>(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] >> _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator&&(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] && _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator||(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] || _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator==(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] == _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator==(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left == _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator==(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] == _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator!=(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] != _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator!=(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left != _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator!=(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] != _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator<(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] < _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator<(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left < _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator<(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] < _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator>(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] > _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator>(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left > _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator>(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] > _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator<=(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] <= _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator<=(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left <= _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator<=(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] <= _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator>=(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] >= _Right;
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator>=(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left >= _Right[_Idx];
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD _Boolarray operator>=(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<bool> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = _Left[_Idx] >= _Right[_Idx];
    }
    return _Ans;
}

// [valarray.transcend] Transcendentals
template <class _Ty>
_NODISCARD valarray<_Ty> abs(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = abs(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> acos(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = acos(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> asin(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = asin(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> atan(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = atan(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> atan2(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = atan2(_Left[_Idx], _Right[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> atan2(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = atan2(_Left[_Idx], _Right); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> atan2(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = atan2(_Left, _Right[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> cos(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = cos(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> cosh(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = cosh(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> exp(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = exp(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> log(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = log(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> log10(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = log10(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> pow(const valarray<_Ty>& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = pow(_Left[_Idx], _Right[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> pow(const valarray<_Ty>& _Left, const typename valarray<_Ty>::value_type& _Right) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = pow(_Left[_Idx], _Right); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> pow(const typename valarray<_Ty>::value_type& _Left, const valarray<_Ty>& _Right) {
    const size_t _Size = _Right.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = pow(_Left, _Right[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> sin(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = sin(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> sinh(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = sinh(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> sqrt(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = sqrt(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> tan(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = tan(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

template <class _Ty>
_NODISCARD valarray<_Ty> tanh(const valarray<_Ty>& _Left) {
    const size_t _Size = _Left.size();
    valarray<_Ty> _Ans(_Size);
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Ans[_Idx] = tanh(_Left[_Idx]); // using ADL, N4835 [valarray.transcend]/1
    }
    return _Ans;
}

class slice { // define a slice of a valarray
public:
    slice() = default;

    slice(size_t _Off, size_t _Count, size_t _Inc) noexcept // strengthened
        : _Start(_Off), _Len(_Count), _Stride(_Inc) {}

    _NODISCARD size_t start() const noexcept /* strengthened */ {
        return _Start;
    }

    _NODISCARD size_t size() const noexcept /* strengthened */ {
        return _Len;
    }

    _NODISCARD size_t stride() const noexcept /* strengthened */ {
        return _Stride;
    }

#if _HAS_CXX20
    _NODISCARD_FRIEND bool operator==(const slice& _Left, const slice& _Right) noexcept /* strengthened */ {
        return _Left.start() == _Right.start() && _Left.size() == _Right.size() && _Left.stride() == _Right.stride();
    }
#endif // _HAS_CXX20

protected:
    size_t _Start  = 0; // the starting offset
    size_t _Len    = 0; // the number of elements
    size_t _Stride = 0; // the distance between elements
};

template <class _Ty>
class slice_array : public slice { // define a slice of a valarray
public:
    using value_type = _Ty;

    void operator=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] = _Right[_Idx];
        }
    }

    void operator=(const _Ty& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] = _Right;
        }
    }

    void operator*=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] *= _Right[_Idx];
        }
    }

    void operator/=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] /= _Right[_Idx];
        }
    }

    void operator%=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] %= _Right[_Idx];
        }
    }

    void operator+=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] += _Right[_Idx];
        }
    }

    void operator-=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] -= _Right[_Idx];
        }
    }

    void operator^=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] ^= _Right[_Idx];
        }
    }

    void operator&=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] &= _Right[_Idx];
        }
    }

    void operator|=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] |= _Right[_Idx];
        }
    }

    void operator<<=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] <<= _Right[_Idx];
        }
    }

    void operator>>=(const valarray<_Ty>& _Right) const {
        size_t _Off = _Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Off += _Stride) {
            _Myptr[_Off] >>= _Right[_Idx];
        }
    }

    _Ty& _Data(size_t _Idx) const {
        return _Myptr[_Idx];
    }

    slice_array() = delete;

    slice_array(const slice_array&) = default;

    const slice_array& operator=(const slice_array& _Right) const {
        size_t _Dst_off = _Start;
        size_t _Src_off = _Right._Start;
        for (size_t _Idx = 0; _Idx < _Len; ++_Idx, _Dst_off += _Stride, _Src_off += _Right._Stride) {
            _Myptr[_Dst_off] = _Right._Myptr[_Src_off];
        }
        return *this;
    }

private:
    friend valarray<_Ty>;

    slice_array(const slice& _Slice, _Ty* _Pdata) : slice(_Slice), _Myptr(_Pdata) {}

    _Ty* _Myptr; // pointer to valarray contents
};

class gslice { // define a generalized (multidimensional) slice of a valarray
public:
    gslice() : _Start(0) {} // construct with all zeros

    gslice(size_t _Off, const _Sizarray& _Lenarr, const _Sizarray& _Incarr)
        : _Start(_Off), _Len(_Lenarr), _Stride(_Incarr) {}

    _NODISCARD size_t start() const {
        return _Start;
    }

    _NODISCARD _Sizarray size() const {
        return _Len;
    }

    _NODISCARD _Sizarray stride() const {
        return _Stride;
    }

    size_t _Nslice() const {
        return _Len.size();
    }

    size_t _Off(_Sizarray& _Indexarr) const {
        size_t _Idx;
        size_t _Ans        = _Start;
        const size_t _Size = _Indexarr.size();
        for (_Idx = 0; _Idx < _Size; ++_Idx) {
            _Ans += _Indexarr[_Idx] * _Stride[_Idx]; // compute offset
        }

        while (0 < _Idx--) {
            if (++_Indexarr[_Idx] < _Len[_Idx]) {
                break; // increment done, quit
            } else {
                _Indexarr[_Idx] = 0; // carry to more-significant index
            }
        }

        return _Ans;
    }

    _NODISCARD size_t _Totlen() const {
        const size_t _Size = _Len.size();
        if (_Size == 0) {
            return 0;
        }

        size_t _Count = _Len[0];
        for (size_t _Idx = 1; _Idx < _Size; ++_Idx) {
            _Count *= _Len[_Idx];
        }

        return _Count;
    }

private:
    size_t _Start; // the starting offset
    _Sizarray _Len; // array of numbers of elements
    _Sizarray _Stride; // array of distances between elements
};

template <class _Ty>
class gslice_array : public gslice { // define a generalized slice of a valarray
public:
    using value_type = _Ty;

    void operator=(const valarray<_Ty>& _Right) const {
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] = _Right[_Idx];
        }
    }

    void operator=(const _Ty& _Right) const {
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] = _Right;
        }
    }

    void operator*=(const valarray<_Ty>& _Right) const { // multiply generalized slice by valarray
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] *= _Right[_Idx];
        }
    }

    void operator/=(const valarray<_Ty>& _Right) const { // divide generalized slice by valarray
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] /= _Right[_Idx];
        }
    }

    void operator%=(const valarray<_Ty>& _Right) const { // remainder generalized slice by valarray
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] %= _Right[_Idx];
        }
    }

    void operator+=(const valarray<_Ty>& _Right) const { // add valarray to generalized slice
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] += _Right[_Idx];
        }
    }

    void operator-=(const valarray<_Ty>& _Right) const { // subtract valarray from generalized slice
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] -= _Right[_Idx];
        }
    }

    void operator^=(const valarray<_Ty>& _Right) const { // XOR valarray into generalized slice
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] ^= _Right[_Idx];
        }
    }

    void operator&=(const valarray<_Ty>& _Right) const { // AND valarray into generalized slice
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] &= _Right[_Idx];
        }
    }

    void operator|=(const valarray<_Ty>& _Right) const { // OR valarray into generalized slice
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] |= _Right[_Idx];
        }
    }

    void operator<<=(const valarray<_Ty>& _Right) const { // left shift generalized slice by valarray
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] <<= _Right[_Idx];
        }
    }

    void operator>>=(const valarray<_Ty>& _Right) const { // right shift generalized slice by valarray
        _Sizarray _Indexarray(size_t{0}, _Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Indexarray)] >>= _Right[_Idx];
        }
    }

    _NODISCARD _Ty& _Data(size_t _Idx) const {
        return _Myptr[_Idx];
    }

    gslice_array() = delete;

    gslice_array(const gslice_array&) = default;

    const gslice_array& operator=(const gslice_array& _Right) const {
        _Sizarray _Dst_indexarray(size_t{0}, _Nslice());
        _Sizarray _Src_indexarray(size_t{0}, _Right._Nslice());
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Off(_Dst_indexarray)] = _Right._Myptr[_Right._Off(_Src_indexarray)];
        }
        return *this;
    }

private:
    friend valarray<_Ty>;

    gslice_array(const gslice& _Gslice, _Ty* _Ptr) : gslice(_Gslice), _Myptr(_Ptr) {}

    _Ty* _Myptr; // pointer to valarray contents
};

template <class _Ty>
class mask_array { // define a subset of a valarray with an array of mask bits
public:
    using value_type = _Ty;

    void operator=(const valarray<_Ty>& _Right) const {
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] = _Right[_Idx];
        }
    }

    void operator=(const _Ty& _Right) const {
        const size_t _Size = _Mybool.size();
        for (size_t _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off)) {
            _Myptr[_Off] = _Right;
        }
    }

    void operator*=(const valarray<_Ty>& _Right) const { // multiply masked array by valarray
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] *= _Right[_Idx];
        }
    }

    void operator/=(const valarray<_Ty>& _Right) const { // divide masked array by valarray
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] /= _Right[_Idx];
        }
    }

    void operator%=(const valarray<_Ty>& _Right) const { // remainder masked array by valarray
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] %= _Right[_Idx];
        }
    }

    void operator+=(const valarray<_Ty>& _Right) const { // add valarray to masked array
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] += _Right[_Idx];
        }
    }

    void operator-=(const valarray<_Ty>& _Right) const { // subtract valarray from masked array
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] -= _Right[_Idx];
        }
    }

    void operator^=(const valarray<_Ty>& _Right) const { // XOR valarray into masked array
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] ^= _Right[_Idx];
        }
    }

    void operator&=(const valarray<_Ty>& _Right) const { // OR valarray into masked array
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] &= _Right[_Idx];
        }
    }

    void operator|=(const valarray<_Ty>& _Right) const { // OR valarray into masked array
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] |= _Right[_Idx];
        }
    }

    void operator<<=(const valarray<_Ty>& _Right) const { // left shift masked array by valarray
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] <<= _Right[_Idx];
        }
    }

    void operator>>=(const valarray<_Ty>& _Right) const { // right shift masked array by valarray
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0, _Off = _Start_off(); _Off < _Size; _Off = _Next_off(_Off), ++_Idx) {
            _Myptr[_Off] >>= _Right[_Idx];
        }
    }

    _NODISCARD _Ty& _Data(size_t _Idx) const {
        return _Myptr[_Idx];
    }

    _NODISCARD bool _Mask(size_t _Idx) const {
        return _Mybool[_Idx];
    }

    _NODISCARD size_t _Start_off() const {
        size_t _Off        = 0;
        const size_t _Size = _Mybool.size();
        while (_Off < _Size && !_Mybool[_Off]) {
            ++_Off;
        }
        return _Off;
    }

    _NODISCARD size_t _Next_off(size_t _Off) const {
        const size_t _Size = _Mybool.size();
        do {
            ++_Off;
        } while (_Off < _Size && !_Mybool[_Off]);
        return _Off;
    }

    _NODISCARD size_t _Totlen() const {
        size_t _Count      = 0;
        const size_t _Size = _Mybool.size();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            if (_Mybool[_Idx]) {
                ++_Count;
            }
        }

        return _Count;
    }

    mask_array() = delete;

    mask_array(const mask_array&) = default;

    const mask_array& operator=(const mask_array& _Right) const {
        const size_t _Size = _Mybool.size();
        size_t _Dst_off    = _Start_off();
        size_t _Src_off    = _Right._Start_off();
        for (; _Dst_off < _Size; _Src_off = _Right._Next_off(_Src_off), _Dst_off = _Next_off(_Dst_off)) {
            _Myptr[_Dst_off] = _Right._Myptr[_Src_off];
        }
        return *this;
    }

private:
    friend valarray<_Ty>;

    mask_array(const _Boolarray& _Maskarr, _Ty* _Pdata) : _Mybool(_Maskarr), _Myptr(_Pdata) {}

    _Boolarray _Mybool; // array of mask bits
    _Ty* _Myptr; // pointer to valarray contents
};

template <class _Ty>
class indirect_array { // define a subset of a valarray with an array of indexes
public:
    using value_type = _Ty;

    void operator=(const valarray<_Ty>& _Right) const {
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] = _Right[_Idx];
        }
    }

    void operator=(const _Ty& _Right) const {
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] = _Right;
        }
    }

    void operator*=(const valarray<_Ty>& _Right) const { // multiply indirect array by valarray
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] *= _Right[_Idx];
        }
    }

    void operator/=(const valarray<_Ty>& _Right) const { // divide indirect array by valarray
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] /= _Right[_Idx];
        }
    }

    void operator%=(const valarray<_Ty>& _Right) const { // remainder indirect array by valarray
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] %= _Right[_Idx];
        }
    }

    void operator+=(const valarray<_Ty>& _Right) const { // add valarray to indirect array
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] += _Right[_Idx];
        }
    }

    void operator-=(const valarray<_Ty>& _Right) const { // subtract valarray from indirect array
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] -= _Right[_Idx];
        }
    }

    void operator^=(const valarray<_Ty>& _Right) const { // XOR valarray into indirect array
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] ^= _Right[_Idx];
        }
    }

    void operator&=(const valarray<_Ty>& _Right) const { // AND valarray into indirect array
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] &= _Right[_Idx];
        }
    }

    void operator|=(const valarray<_Ty>& _Right) const { // OR valarray into indirect array
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] |= _Right[_Idx];
        }
    }

    void operator<<=(const valarray<_Ty>& _Right) const { // left shift indirect array by valarray
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] <<= _Right[_Idx];
        }
    }

    void operator>>=(const valarray<_Ty>& _Right) const { // right shift indirect array by valarray
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] >>= _Right[_Idx];
        }
    }

    _NODISCARD _Ty& _Data(size_t _Idx) const {
        return _Myptr[_Idx];
    }

    _NODISCARD size_t _Indir(size_t _Idx) const {
        return _Myindarr[_Idx];
    }

    _NODISCARD size_t _Totlen() const {
        return _Myindarr.size();
    }

    indirect_array() = delete;

    indirect_array(const indirect_array&) = default;

    const indirect_array& operator=(const indirect_array& _Right) const {
        const size_t _Size = _Totlen();
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Myptr[_Indir(_Idx)] = _Right._Myptr[_Right._Indir(_Idx)];
        }
        return *this;
    }

private:
    friend valarray<_Ty>;

    indirect_array(const _Sizarray& _Indarr, _Ty* _Ptr) : _Myindarr(_Indarr), _Myptr(_Ptr) {}

    _Sizarray _Myindarr; // array of indirect indexes
    _Ty* _Myptr; // pointer to valarray contents
};

template <class _Ty>
valarray<_Ty>& valarray<_Ty>::operator=(const slice_array<_Ty>& _Slicearr) {
    _Tidy_deallocate();
    _Grow(_Slicearr.size(), &_Slicearr._Data(_Slicearr.start()), _Slicearr.stride());
    return *this;
}

template <class _Ty>
_NODISCARD valarray<_Ty> valarray<_Ty>::operator[](slice _Slice) const {
    return valarray<_Ty>(slice_array<_Ty>(_Slice, _Myptr));
}

template <class _Ty>
_NODISCARD slice_array<_Ty> valarray<_Ty>::operator[](slice _Slice) {
    return slice_array<_Ty>(_Slice, _Myptr);
}

template <class _Ty>
valarray<_Ty>& valarray<_Ty>::operator=(const gslice_array<_Ty>& _Gslicearr) {
    _Tidy_deallocate();
    _Grow(_Gslicearr._Totlen());
    _Sizarray _Indexarray(size_t{0}, _Gslicearr._Nslice());
    const size_t _Size = size();
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Myptr[_Idx] = _Gslicearr._Data(_Gslicearr._Off(_Indexarray));
    }
    return *this;
}

template <class _Ty>
_NODISCARD valarray<_Ty> valarray<_Ty>::operator[](const gslice& _Gslice) const {
    return valarray<_Ty>(gslice_array<_Ty>(_Gslice, _Myptr));
}

template <class _Ty>
_NODISCARD gslice_array<_Ty> valarray<_Ty>::operator[](const gslice& _Gslicearr) {
    return gslice_array<_Ty>(_Gslicearr, _Myptr);
}

template <class _Ty>
valarray<_Ty>& valarray<_Ty>::operator=(const mask_array<_Ty>& _Maskarr) {
    _Tidy_deallocate();
    _Grow(_Maskarr._Totlen());
    size_t _Count = 0;

    const size_t _Size = size();
    for (size_t _Idx = 0; _Idx < _Size; ++_Count) {
        if (_Maskarr._Mask(_Count)) {
            _Myptr[_Idx++] = _Maskarr._Data(_Count);
        }
    }

    return *this;
}

template <class _Ty>
_NODISCARD valarray<_Ty> valarray<_Ty>::operator[](const _Boolarray& _Boolarr) const {
    return valarray<_Ty>(mask_array<_Ty>(_Boolarr, _Myptr));
}

template <class _Ty>
_NODISCARD mask_array<_Ty> valarray<_Ty>::operator[](const _Boolarray& _Boolarr) {
    return mask_array<_Ty>(_Boolarr, _Myptr);
}

template <class _Ty>
valarray<_Ty>& valarray<_Ty>::operator=(const indirect_array<_Ty>& _Indarr) {
    _Tidy_deallocate();
    _Grow(_Indarr._Totlen());
    const size_t _Size = size();
    for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
        _Myptr[_Idx] = _Indarr._Data(_Indarr._Indir(_Idx));
    }
    return *this;
}

template <class _Ty>
_NODISCARD valarray<_Ty> valarray<_Ty>::operator[](const _Sizarray& _Indarr) const {
    return valarray<_Ty>(indirect_array<_Ty>(_Indarr, _Myptr));
}

template <class _Ty>
_NODISCARD indirect_array<_Ty> valarray<_Ty>::operator[](const _Sizarray& _Indarr) {
    return indirect_array<_Ty>(_Indarr, _Myptr);
}
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _VALARRAY_