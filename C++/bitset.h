// bitset standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _BITSET_
#define _BITSET_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <iosfwd>
#include <xstring>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <size_t _Bits>
class bitset { // store fixed-length sequence of Boolean elements
public:
#pragma warning(push)
#pragma warning(disable : 4296) // expression is always true (/Wall)
    using _Ty = conditional_t<_Bits <= sizeof(unsigned long) * CHAR_BIT, unsigned long, unsigned long long>;
#pragma warning(pop)

    class reference { // proxy for an element
        friend bitset<_Bits>;

    public:
        ~reference() noexcept {} // TRANSITION, ABI

        reference& operator=(bool _Val) noexcept {
            _Pbitset->_Set_unchecked(_Mypos, _Val);
            return *this;
        }

        reference& operator=(const reference& _Bitref) noexcept {
            _Pbitset->_Set_unchecked(_Mypos, static_cast<bool>(_Bitref));
            return *this;
        }

        reference& flip() noexcept {
            _Pbitset->_Flip_unchecked(_Mypos);
            return *this;
        }

        _NODISCARD bool operator~() const noexcept {
            return !_Pbitset->_Subscript(_Mypos);
        }

        operator bool() const noexcept {
            return _Pbitset->_Subscript(_Mypos);
        }

    private:
        reference() noexcept : _Pbitset(nullptr), _Mypos(0) {}

        reference(bitset<_Bits>& _Bitset, size_t _Pos) : _Pbitset(&_Bitset), _Mypos(_Pos) {}

        bitset<_Bits>* _Pbitset;
        size_t _Mypos; // position of element in bitset
    };

    static void _Validate(size_t _Pos) { // verify that _Pos is within bounds
#if _ITERATOR_DEBUG_LEVEL == 0
        (void) _Pos;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 ^^^ // vvv _ITERATOR_DEBUG_LEVEL != 0 vvv
        _STL_VERIFY(_Pos < _Bits, "bitset index outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 0
    }

    constexpr bool _Subscript(size_t _Pos) const {
        return (_Array[_Pos / _Bitsperword] & (_Ty{1} << _Pos % _Bitsperword)) != 0;
    }

    _NODISCARD constexpr bool operator[](size_t _Pos) const {
#if _ITERATOR_DEBUG_LEVEL == 0
        return _Subscript(_Pos);

#else // _ITERATOR_DEBUG_LEVEL == 0
        return _Bits <= _Pos ? (_Validate(_Pos), false) : _Subscript(_Pos);
#endif // _ITERATOR_DEBUG_LEVEL == 0
    }

    _NODISCARD reference operator[](size_t _Pos) {
        _Validate(_Pos);
        return reference(*this, _Pos);
    }

    constexpr bitset() noexcept : _Array() {} // construct with all false values

    static constexpr bool _Need_mask = _Bits < CHAR_BIT * sizeof(unsigned long long);

    static constexpr unsigned long long _Mask = (1ULL << (_Need_mask ? _Bits : 0)) - 1ULL;

    constexpr bitset(unsigned long long _Val) noexcept : _Array{static_cast<_Ty>(_Need_mask ? _Val & _Mask : _Val)} {}

    template <class _Traits, class _Elem>
    void _Construct(const _Elem* const _Ptr, size_t _Count, const _Elem _Elem0, const _Elem _Elem1) {
        if (_Count > _Bits) {
            for (size_t _Idx = _Bits; _Idx < _Count; ++_Idx) {
                const auto _Ch = _Ptr[_Idx];
                if (!_Traits::eq(_Elem1, _Ch) && !_Traits::eq(_Elem0, _Ch)) {
                    _Xinv();
                }
            }

            _Count = _Bits;
        }

        size_t _Wpos = 0;
        if (_Count != 0) {
            size_t _Bits_used_in_word = 0;
            auto _Last                = _Ptr + _Count;
            _Ty _This_word            = 0;
            do {
                --_Last;
                const auto _Ch = *_Last;
                _This_word |= static_cast<_Ty>(_Traits::eq(_Elem1, _Ch)) << _Bits_used_in_word;
                if (!_Traits::eq(_Elem1, _Ch) && !_Traits::eq(_Elem0, _Ch)) {
                    _Xinv();
                }

                if (++_Bits_used_in_word == _Bitsperword) {
                    _Array[_Wpos] = _This_word;
                    ++_Wpos;
                    _This_word         = 0;
                    _Bits_used_in_word = 0;
                }
            } while (_Ptr != _Last);

            if (_Bits_used_in_word != 0) {
                _Array[_Wpos] = _This_word;
                ++_Wpos;
            }
        }

        for (; _Wpos <= _Words; ++_Wpos) {
            _Array[_Wpos] = 0;
        }
    }

    template <class _Elem, class _Traits, class _Alloc>
    explicit bitset(const basic_string<_Elem, _Traits, _Alloc>& _Str,
        typename basic_string<_Elem, _Traits, _Alloc>::size_type _Pos   = 0,
        typename basic_string<_Elem, _Traits, _Alloc>::size_type _Count = basic_string<_Elem, _Traits, _Alloc>::npos,
        _Elem _Elem0 = static_cast<_Elem>('0'), _Elem _Elem1 = static_cast<_Elem>('1')) {
        // construct from [_Pos, _Pos + _Count) elements in string
        if (_Str.size() < _Pos) {
            _Xran(); // _Pos off end
        }

        if (_Str.size() - _Pos < _Count) {
            _Count = _Str.size() - _Pos; // trim _Count to size
        }

        _Construct<_Traits>(_Str.data() + _Pos, _Count, _Elem0, _Elem1);
    }

    template <class _Elem>
    explicit bitset(const _Elem* _Ntcts, typename basic_string<_Elem>::size_type _Count = basic_string<_Elem>::npos,
        _Elem _Elem0 = static_cast<_Elem>('0'), _Elem _Elem1 = static_cast<_Elem>('1')) {
        if (_Count == basic_string<_Elem>::npos) {
            _Count = char_traits<_Elem>::length(_Ntcts);
        }

        _Construct<char_traits<_Elem>>(_Ntcts, _Count, _Elem0, _Elem1);
    }

    bitset& operator&=(const bitset& _Right) noexcept {
        for (size_t _Wpos = 0; _Wpos <= _Words; ++_Wpos) {
            _Array[_Wpos] &= _Right._Array[_Wpos];
        }

        return *this;
    }

    bitset& operator|=(const bitset& _Right) noexcept {
        for (size_t _Wpos = 0; _Wpos <= _Words; ++_Wpos) {
            _Array[_Wpos] |= _Right._Array[_Wpos];
        }

        return *this;
    }

    bitset& operator^=(const bitset& _Right) noexcept {
        for (size_t _Wpos = 0; _Wpos <= _Words; ++_Wpos) {
            _Array[_Wpos] ^= _Right._Array[_Wpos];
        }

        return *this;
    }

    bitset& operator<<=(size_t _Pos) noexcept { // shift left by _Pos, first by words then by bits
        const auto _Wordshift = static_cast<ptrdiff_t>(_Pos / _Bitsperword);
        if (_Wordshift != 0) {
            for (ptrdiff_t _Wpos = _Words; 0 <= _Wpos; --_Wpos) {
                _Array[_Wpos] = _Wordshift <= _Wpos ? _Array[_Wpos - _Wordshift] : 0;
            }
        }

        if ((_Pos %= _Bitsperword) != 0) { // 0 < _Pos < _Bitsperword, shift by bits
            for (ptrdiff_t _Wpos = _Words; 0 < _Wpos; --_Wpos) {
                _Array[_Wpos] = (_Array[_Wpos] << _Pos) | (_Array[_Wpos - 1] >> (_Bitsperword - _Pos));
            }

            _Array[0] <<= _Pos;
        }
        _Trim();
        return *this;
    }

    bitset& operator>>=(size_t _Pos) noexcept { // shift right by _Pos, first by words then by bits
        const auto _Wordshift = static_cast<ptrdiff_t>(_Pos / _Bitsperword);
        if (_Wordshift != 0) {
            for (ptrdiff_t _Wpos = 0; _Wpos <= _Words; ++_Wpos) {
                _Array[_Wpos] = _Wordshift <= _Words - _Wpos ? _Array[_Wpos + _Wordshift] : 0;
            }
        }

        if ((_Pos %= _Bitsperword) != 0) { // 0 < _Pos < _Bitsperword, shift by bits
            for (ptrdiff_t _Wpos = 0; _Wpos < _Words; ++_Wpos) {
                _Array[_Wpos] = (_Array[_Wpos] >> _Pos) | (_Array[_Wpos + 1] << (_Bitsperword - _Pos));
            }

            _Array[_Words] >>= _Pos;
        }
        return *this;
    }

    bitset& set() noexcept { // set all bits true
        _CSTD memset(&_Array, 0xFF, sizeof(_Array));
        _Trim();
        return *this;
    }

    bitset& set(size_t _Pos, bool _Val = true) { // set bit at _Pos to _Val
        if (_Bits <= _Pos) {
            _Xran(); // _Pos off end
        }

        return _Set_unchecked(_Pos, _Val);
    }

    bitset& reset() noexcept { // set all bits false
        _CSTD memset(&_Array, 0, sizeof(_Array));
        return *this;
    }

    bitset& reset(size_t _Pos) { // set bit at _Pos to false
        return set(_Pos, false);
    }

    _NODISCARD bitset operator~() const noexcept { // flip all bits
        return bitset(*this).flip();
    }

    bitset& flip() noexcept { // flip all bits
        for (size_t _Wpos = 0; _Wpos <= _Words; ++_Wpos) {
            _Array[_Wpos] = ~_Array[_Wpos];
        }

        _Trim();
        return *this;
    }

    bitset& flip(size_t _Pos) { // flip bit at _Pos
        if (_Bits <= _Pos) {
            _Xran(); // _Pos off end
        }

        return _Flip_unchecked(_Pos);
    }

    _NODISCARD unsigned long to_ulong() const {
        constexpr bool _Bits_zero  = _Bits == 0;
        constexpr bool _Bits_small = _Bits <= 32;
        constexpr bool _Bits_large = _Bits > 64;
        if constexpr (_Bits_zero) {
            return 0;
        } else if constexpr (_Bits_small) {
            return static_cast<unsigned long>(_Array[0]);
        } else {
            if constexpr (_Bits_large) {
                for (size_t _Idx = 1; _Idx <= _Words; ++_Idx) {
                    if (_Array[_Idx] != 0) {
                        _Xoflo(); // fail if any high-order words are nonzero
                    }
                }
            }

            if (_Array[0] > ULONG_MAX) {
                _Xoflo();
            }

            return static_cast<unsigned long>(_Array[0]);
        }
    }

    _NODISCARD unsigned long long to_ullong() const {
        constexpr bool _Bits_zero  = _Bits == 0;
        constexpr bool _Bits_large = _Bits > 64;
        if constexpr (_Bits_zero) {
            return 0;
        } else {
            if constexpr (_Bits_large) {
                for (size_t _Idx = 1; _Idx <= _Words; ++_Idx) {
                    if (_Array[_Idx] != 0) {
                        _Xoflo(); // fail if any high-order words are nonzero
                    }
                }
            }

            return _Array[0];
        }
    }

    template <class _Elem = char, class _Tr = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
    _NODISCARD basic_string<_Elem, _Tr, _Alloc> to_string(
        _Elem _Elem0 = static_cast<_Elem>('0'), _Elem _Elem1 = static_cast<_Elem>('1')) const {
        // convert bitset to string
        basic_string<_Elem, _Tr, _Alloc> _Str;
        _Str.reserve(_Bits);

        for (auto _Pos = _Bits; 0 < _Pos;) {
            _Str.push_back(_Subscript(--_Pos) ? _Elem1 : _Elem0);
        }

        return _Str;
    }

    _NODISCARD size_t count() const noexcept { // count number of set bits
        const char* const _Bitsperbyte = "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
                                         "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
                                         "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
                                         "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
                                         "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
                                         "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
                                         "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
                                         "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
                                         "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
                                         "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
                                         "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
                                         "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
                                         "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
                                         "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
                                         "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
                                         "\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";
        const unsigned char* _Ptr       = &reinterpret_cast<const unsigned char&>(_Array);
        const unsigned char* const _End = _Ptr + sizeof(_Array);
        size_t _Val                     = 0;
        for (; _Ptr != _End; ++_Ptr) {
            _Val += _Bitsperbyte[*_Ptr];
        }

        return _Val;
    }

    _NODISCARD constexpr size_t size() const noexcept {
        return _Bits;
    }

    _NODISCARD bool operator==(const bitset& _Right) const noexcept {
        return _CSTD memcmp(&_Array[0], &_Right._Array[0], sizeof(_Array)) == 0;
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(const bitset& _Right) const noexcept {
        return !(*this == _Right);
    }
#endif // !_HAS_CXX20

    _NODISCARD bool test(size_t _Pos) const {
        if (_Bits <= _Pos) {
            _Xran(); // _Pos off end
        }

        return _Subscript(_Pos);
    }

    _NODISCARD bool any() const noexcept {
        for (size_t _Wpos = 0; _Wpos <= _Words; ++_Wpos) {
            if (_Array[_Wpos] != 0) {
                return true;
            }
        }

        return false;
    }

    _NODISCARD bool none() const noexcept {
        return !any();
    }

    _NODISCARD bool all() const noexcept {
        constexpr bool _Zero_length = _Bits == 0;
        if constexpr (_Zero_length) { // must test for this, otherwise would count one full word
            return true;
        }

        constexpr bool _No_padding = _Bits % _Bitsperword == 0;
        for (size_t _Wpos = 0; _Wpos < _Words + _No_padding; ++_Wpos) {
            if (_Array[_Wpos] != ~static_cast<_Ty>(0)) {
                return false;
            }
        }

        return _No_padding || _Array[_Words] == (static_cast<_Ty>(1) << (_Bits % _Bitsperword)) - 1;
    }

    _NODISCARD bitset operator<<(size_t _Pos) const noexcept {
        return bitset(*this) <<= _Pos;
    }

    _NODISCARD bitset operator>>(size_t _Pos) const noexcept {
        return bitset(*this) >>= _Pos;
    }

    _NODISCARD _Ty _Getword(size_t _Wpos) const noexcept { // nonstandard extension; get underlying word
        return _Array[_Wpos];
    }

private:
    friend hash<bitset<_Bits>>;

    static constexpr ptrdiff_t _Bitsperword = CHAR_BIT * sizeof(_Ty);
    static constexpr ptrdiff_t _Words       = _Bits == 0 ? 0 : (_Bits - 1) / _Bitsperword; // NB: number of words - 1

    void _Trim() noexcept { // clear any trailing bits in last word
        constexpr bool _Work_to_do = _Bits == 0 || _Bits % _Bitsperword != 0;
        if constexpr (_Work_to_do) {
            _Array[_Words] &= (_Ty{1} << _Bits % _Bitsperword) - 1;
        }
    }

    bitset& _Set_unchecked(size_t _Pos, bool _Val) noexcept { // set bit at _Pos to _Val, no checking
        auto& _Selected_word = _Array[_Pos / _Bitsperword];
        const auto _Bit      = _Ty{1} << _Pos % _Bitsperword;
        if (_Val) {
            _Selected_word |= _Bit;
        } else {
            _Selected_word &= ~_Bit;
        }

        return *this;
    }

    bitset& _Flip_unchecked(size_t _Pos) noexcept { // flip bit at _Pos, no checking
        _Array[_Pos / _Bitsperword] ^= _Ty{1} << _Pos % _Bitsperword;
        return *this;
    }

    [[noreturn]] void _Xinv() const {
        _Xinvalid_argument("invalid bitset char");
    }

    [[noreturn]] void _Xoflo() const {
        _Xoverflow_error("bitset overflow");
    }

    [[noreturn]] void _Xran() const {
        _Xout_of_range("invalid bitset position");
    }

    _Ty _Array[_Words + 1];
};

template <size_t _Bits>
_NODISCARD bitset<_Bits> operator&(const bitset<_Bits>& _Left, const bitset<_Bits>& _Right) noexcept {
    bitset<_Bits> _Ans = _Left;
    return _Ans &= _Right;
}

template <size_t _Bits>
_NODISCARD bitset<_Bits> operator|(const bitset<_Bits>& _Left, const bitset<_Bits>& _Right) noexcept {
    bitset<_Bits> _Ans = _Left;
    return _Ans |= _Right;
}

template <size_t _Bits>
_NODISCARD bitset<_Bits> operator^(const bitset<_Bits>& _Left, const bitset<_Bits>& _Right) noexcept {
    bitset<_Bits> _Ans = _Left;
    return _Ans ^= _Right;
}

template <class _Elem, class _Tr, size_t _Bits>
basic_ostream<_Elem, _Tr>& operator<<(basic_ostream<_Elem, _Tr>& _Ostr, const bitset<_Bits>& _Right) {
    using _Ctype             = typename basic_ostream<_Elem, _Tr>::_Ctype;
    const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Ostr.getloc());
    const _Elem _Elem0       = _Ctype_fac.widen('0');
    const _Elem _Elem1       = _Ctype_fac.widen('1');

    return _Ostr << _Right.template to_string<_Elem, _Tr, allocator<_Elem>>(_Elem0, _Elem1);
}

template <class _Elem, class _Tr, size_t _Bits>
basic_istream<_Elem, _Tr>& operator>>(basic_istream<_Elem, _Tr>& _Istr, bitset<_Bits>& _Right) {
    using _Istr_t                    = basic_istream<_Elem, _Tr>;
    using _Ctype                     = typename _Istr_t::_Ctype;
    const _Ctype& _Ctype_fac         = _STD use_facet<_Ctype>(_Istr.getloc());
    const _Elem _Elem0               = _Ctype_fac.widen('0');
    const _Elem _Elem1               = _Ctype_fac.widen('1');
    typename _Istr_t::iostate _State = _Istr_t::goodbit;
    bool _Changed                    = false;
    string _Str;
    const typename _Istr_t::sentry _Ok(_Istr);

    if (_Ok) { // valid stream, extract elements
        _TRY_IO_BEGIN
        typename _Tr::int_type _Meta = _Istr.rdbuf()->sgetc();
        for (size_t _Count = _Right.size(); 0 < _Count; _Meta = _Istr.rdbuf()->snextc(), (void) --_Count) {
            // test _Meta
            _Elem _Char;
            if (_Tr::eq_int_type(_Tr::eof(), _Meta)) { // end of file, quit
                _State |= _Istr_t::eofbit;
                break;
            } else if ((_Char = _Tr::to_char_type(_Meta)) != _Elem0 && _Char != _Elem1) {
                break; // invalid element
            } else if (_Str.max_size() <= _Str.size()) { // no room in string, give up (unlikely)
                _State |= _Istr_t::failbit;
                break;
            } else { // valid, append '0' or '1'
                _Str.push_back('0' + (_Char == _Elem1));
                _Changed = true;
            }
        }
        _CATCH_IO_(_Istr_t, _Istr)
    }

    constexpr bool _Has_bits = _Bits > 0;

    if constexpr (_Has_bits) {
        if (!_Changed) {
            _State |= _Istr_t::failbit;
        }
    }

    _Istr.setstate(_State);
    _Right = bitset<_Bits>(_Str); // convert string and store
    return _Istr;
}

template <size_t _Bits>
struct hash<bitset<_Bits>> {
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bitset<_Bits> _ARGUMENT_TYPE_NAME;
    _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef size_t _RESULT_TYPE_NAME;

    _NODISCARD size_t operator()(const bitset<_Bits>& _Keyval) const noexcept {
        return _Hash_representation(_Keyval._Array);
    }
};
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _BITSET_
