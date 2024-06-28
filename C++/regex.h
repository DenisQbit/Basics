// regex standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _REGEX_
#define _REGEX_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if _HAS_CXX17
#include <xpolymorphic_allocator.h>
#endif // _HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#ifndef _REGEX_MAX_COMPLEXITY_COUNT
#define _REGEX_MAX_COMPLEXITY_COUNT 10000000L // set to 0 to disable
#endif // _REGEX_MAX_COMPLEXITY_COUNT

#ifndef _REGEX_MAX_STACK_COUNT
#ifdef _WIN64
#define _REGEX_MAX_STACK_COUNT 600L // set to 0 to disable
#else // _WIN64
#define _REGEX_MAX_STACK_COUNT 1000L // set to 0 to disable
#endif // _WIN64
#endif // _REGEX_MAX_STACK_COUNT

#ifndef _ENHANCED_REGEX_VISUALIZER

#ifdef _DEBUG
#define _ENHANCED_REGEX_VISUALIZER 1

#else // _DEBUG
#define _ENHANCED_REGEX_VISUALIZER 0
#endif // _DEBUG
#endif // _ENHANCED_REGEX_VISUALIZER

_STD_BEGIN

enum _Meta_type { // meta character representations for parser
    _Meta_lpar  = '(',
    _Meta_rpar  = ')',
    _Meta_dlr   = '$',
    _Meta_caret = '^',
    _Meta_dot   = '.',
    _Meta_star  = '*',
    _Meta_plus  = '+',
    _Meta_query = '?',
    _Meta_lsq   = '[',
    _Meta_rsq   = ']',
    _Meta_bar   = '|',
    _Meta_esc   = '\\',
    _Meta_dash  = '-',
    _Meta_lbr   = '{',
    _Meta_rbr   = '}',
    _Meta_comma = ',',
    _Meta_colon = ':',
    _Meta_equal = '=',
    _Meta_exc   = '!',
    _Meta_eos   = -1,
    _Meta_nl    = '\n',
    _Meta_cr    = '\r',
    _Meta_bsp   = '\b',
    _Meta_chr   = 0,

    _Esc_bsl      = '\\',
    _Esc_word     = 'b',
    _Esc_not_word = 'B',
    _Esc_ctrl_a   = 'a',
    _Esc_ctrl_b   = 'b',
    _Esc_ctrl_f   = 'f',
    _Esc_ctrl_n   = 'n',
    _Esc_ctrl_r   = 'r',
    _Esc_ctrl_t   = 't',
    _Esc_ctrl_v   = 'v',
    _Esc_ctrl     = 'c',
    _Esc_hex      = 'x',
    _Esc_uni      = 'u'
};

namespace regex_constants {
    // constants used in regular expressions
    enum syntax_option_type : int { // specify RE syntax rules
        ECMAScript = 0x01,
        basic      = 0x02,
        extended   = 0x04,
        awk        = 0x08,
        grep       = 0x10,
        egrep      = 0x20,
        _Gmask     = 0x3F,

        icase    = 0x0100,
        nosubs   = 0x0200,
        optimize = 0x0400,
        collate  = 0x0800
    };

    _BITMASK_OPS(syntax_option_type)

    enum match_flag_type : int { // specify matching and formatting rules
        match_default     = 0x0000,
        match_not_bol     = 0x0001,
        match_not_eol     = 0x0002,
        match_not_bow     = 0x0004,
        match_not_eow     = 0x0008,
        match_any         = 0x0010,
        match_not_null    = 0x0020,
        match_continuous  = 0x0040,
        match_prev_avail  = 0x0100,
        format_default    = 0x0000,
        format_sed        = 0x0400,
        format_no_copy    = 0x0800,
        format_first_only = 0x1000,
        _Match_not_null   = 0x2000,
        _Skip_zero_length = 0x4000,
    };

    _BITMASK_OPS(match_flag_type)

    enum error_type { // identify error
        error_collate,
        error_ctype,
        error_escape,
        error_backref,
        error_brack,
        error_paren,
        error_brace,
        error_badbrace,
        error_range,
        error_space,
        error_badrepeat,
        error_complexity,
        error_stack,
        error_parse,
        error_syntax
    };
} // namespace regex_constants

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xregex_error(regex_constants::error_type _Code);

template <class _Elem>
class regex_traits;

struct _Cl_names { // structure to associate class name with mask value
    const char* _Narrow;
    const wchar_t* _Wide;
    unsigned int _Len;
    ctype_base::mask _Ctype;

    template <class _Elem>
    const _Elem* _Get() const;
};

template <>
inline const char* _Cl_names::_Get<char>() const { // get narrow pointer
    return _Narrow;
}

template <>
inline const wchar_t* _Cl_names::_Get<wchar_t>() const { // get wide pointer
    return _Wide;
}

template <class _RxTraits>
struct _Cmp_cs { // functor to compare two character values for equality
    using _Elem = typename _RxTraits::char_type;
    bool operator()(_Elem _Ex1, _Elem _Ex2) {
        return _Ex1 == _Ex2;
    }
};

template <class _RxTraits>
struct _Cmp_icase { // functor to compare for case-insensitive equality
    using _Elem = typename _RxTraits::char_type;

    explicit _Cmp_icase(const _RxTraits& _Tr) : _Traits(_Tr) {}

    bool operator()(_Elem _Ex1, _Elem _Ex2) {
        return _Traits.translate_nocase(_Ex1) == _Traits.translate_nocase(_Ex2);
    }

    const _RxTraits& _Traits;
};

template <class _RxTraits>
struct _Cmp_collate { // functor to compare for locale-specific equality
    using _Elem = typename _RxTraits::char_type;

    explicit _Cmp_collate(const _RxTraits& _Tr) : _Traits(_Tr) {}

    bool operator()(_Elem _Ex1, _Elem _Ex2) {
        return _Traits.translate(_Ex1) == _Traits.translate(_Ex2);
    }

    const _RxTraits& _Traits;
};

struct _Regex_traits_base { // base of all regular expression traits
    enum _Char_class_type {
        _Ch_none   = 0,
        _Ch_alnum  = ctype_base::alnum,
        _Ch_alpha  = ctype_base::alpha,
        _Ch_cntrl  = ctype_base::cntrl,
        _Ch_digit  = ctype_base::digit,
        _Ch_graph  = ctype_base::graph,
        _Ch_lower  = ctype_base::lower,
        _Ch_print  = ctype_base::print,
        _Ch_punct  = ctype_base::punct,
        _Ch_space  = ctype_base::space,
        _Ch_upper  = ctype_base::upper,
        _Ch_xdigit = ctype_base::xdigit,
        _Ch_blank  = ctype_base::blank
    };
    using char_class_type = ctype_base::mask;
};

template <class _Elem>
class _Regex_traits : public _Regex_traits_base { // base class for regular expression traits
public:
    using _Uelem      = make_unsigned_t<_Elem>;
    using char_type   = _Elem;
    using size_type   = size_t;
    using string_type = basic_string<_Elem>;
    using locale_type = locale;

    static size_type length(const _Elem* _Str) {
        return char_traits<_Elem>::length(_Str);
    }

    _Regex_traits() {
        _Cache_locale();
    }

    _Regex_traits(const _Regex_traits& _Right) : _Loc(_Right._Loc) {
        _Cache_locale();
    }

    _Regex_traits& operator=(const _Regex_traits& _Right) {
        _Loc = _Right._Loc;
        _Cache_locale();
        return *this;
    }

    _Elem translate(_Elem _Ch) const { // provide locale-sensitive mapping
        string_type _Res = _Getcoll()->transform(_STD addressof(_Ch), _STD addressof(_Ch) + 1);
        return _Res.size() == 1 ? _Res[0] : _Ch;
    }

    _Elem translate_nocase(_Elem _Ch) const { // provide case-insensitive mapping
        return _Getctype()->tolower(_Ch);
    }

    template <class _FwdIt>
    string_type transform(_FwdIt _First, _FwdIt _Last) const { // apply locale-specific transformation
        const string_type _Str(_First, _Last);
        return _Getcoll()->transform(_Str.data(), _Str.data() + _Str.size());
    }

    template <class _FwdIt>
    string_type transform_primary(_FwdIt _First, _FwdIt _Last) const {
        // apply locale-specific case-insensitive transformation
        string_type _Res;

        if (_First != _Last) { // non-empty string, transform it
            vector<_Elem> _Temp(_First, _Last);

            _Getctype()->tolower(_Temp.data(), _Temp.data() + _Temp.size());
            _Res = _Getcoll()->transform(_Temp.data(), _Temp.data() + _Temp.size());
        }
        return _Res;
    }

    bool isctype(_Elem _Ch, char_class_type _Fx) const {
        if (_Fx != static_cast<char_class_type>(-1)) {
            return _Getctype()->is(_Fx, _Ch);
        } else {
            return _Ch == '_' // assumes L'_' == '_'
                || _Getctype()->is(_Ch_alnum, _Ch);
        }
    }

    template <class _Iter>
    char_class_type lookup_classname(_Iter _First, _Iter _Last, bool _Icase = false) const {
        // map [_First, _Last) to character class mask value
#define _REGEX_CHAR_CLASS_NAME(n, c) \
    { n, L##n, sizeof(n) / sizeof(n[0]) - 1, c }
        static constexpr _Cl_names _Names[] = {
            // map class names to numeric constants
            _REGEX_CHAR_CLASS_NAME("alnum", _Ch_alnum),
            _REGEX_CHAR_CLASS_NAME("alpha", _Ch_alpha),
            _REGEX_CHAR_CLASS_NAME("blank", _Ch_blank),
            _REGEX_CHAR_CLASS_NAME("cntrl", _Ch_cntrl),
            _REGEX_CHAR_CLASS_NAME("d", _Ch_digit),
            _REGEX_CHAR_CLASS_NAME("digit", _Ch_digit),
            _REGEX_CHAR_CLASS_NAME("graph", _Ch_graph),
            _REGEX_CHAR_CLASS_NAME("lower", _Ch_lower),
            _REGEX_CHAR_CLASS_NAME("print", _Ch_print),
            _REGEX_CHAR_CLASS_NAME("punct", _Ch_punct),
            _REGEX_CHAR_CLASS_NAME("space", _Ch_space),
            _REGEX_CHAR_CLASS_NAME("s", _Ch_space),
            _REGEX_CHAR_CLASS_NAME("upper", _Ch_upper),
            _REGEX_CHAR_CLASS_NAME("w", static_cast<ctype_base::mask>(-1)),
            _REGEX_CHAR_CLASS_NAME("xdigit", _Ch_xdigit),
            {nullptr, nullptr, 0, 0},
        };
#undef _REGEX_CHAR_CLASS_NAME

        _Adl_verify_range(_First, _Last);
        unsigned int _Ix = 0;
        for (; _Names[_Ix]._Get<_Elem>(); ++_Ix) {
            if (_STD equal(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Names[_Ix]._Get<_Elem>(),
                    _Names[_Ix]._Get<_Elem>() + _Names[_Ix]._Len, _Cmp_icase<_Regex_traits<_Elem>>{*this})) {
                break;
            }
        }

        char_class_type _Mask{};
        if (_Names[_Ix]._Get<_Elem>()) {
            _Mask = _Names[_Ix]._Ctype;
        }

        if (_Icase && (_Mask & (_Ch_lower | _Ch_upper))) {
            _Mask |= _Ch_lower | _Ch_upper;
        }

        return _Mask;
    }

    template <class _FwdIt>
    string_type lookup_collatename(_FwdIt _First, _FwdIt _Last) const { // map [_First, _Last) to collation element
        return string_type(_First, _Last);
    }

    locale_type imbue(locale_type _Lx) { // store locale object
        locale_type _Tmp = _Loc;
        _Loc             = _Lx;
        _Cache_locale();
        return _Tmp;
    }

    locale_type getloc() const {
        return _Loc;
    }

    const collate<_Elem>* _Getcoll() const { // get collate facet pointer
        return _Pcoll;
    }

    const ctype<_Elem>* _Getctype() const { // get ctype facet pointer
        return _Pctype;
    }

private:
    void _Cache_locale() { // populate _Pcoll and _Pctype with _Loc locale
        _Pcoll  = _STD addressof(_STD use_facet<collate<_Elem>>(_Loc));
        _Pctype = _STD addressof(_STD use_facet<ctype<_Elem>>(_Loc));
    }

    const collate<_Elem>* _Pcoll;
    const ctype<_Elem>* _Pctype;
    locale_type _Loc;
};

template <>
class regex_traits<char> : public _Regex_traits<char> {
public:
    int value(char _Ch, int _Base) const { // map character value to numeric value
        if ((_Base != 8 && '0' <= _Ch && _Ch <= '9') || (_Base == 8 && '0' <= _Ch && _Ch <= '7')) {
            return _Ch - '0';
        }

        if (_Base != 16) {
            return -1;
        }

        if ('a' <= _Ch && _Ch <= 'f') {
            return _Ch - 'a' + 10;
        }

        if ('A' <= _Ch && _Ch <= 'F') {
            return _Ch - 'A' + 10;
        }

        return -1;
    }
};

template <>
class regex_traits<wchar_t> : public _Regex_traits<wchar_t> {
public:
    int value(wchar_t _Ch, int _Base) const { // map character value to numeric value
        if ((_Base != 8 && L'0' <= _Ch && _Ch <= L'9') || (_Base == 8 && L'0' <= _Ch && _Ch <= L'7')) {
            return _Ch - L'0';
        }

        if (_Base != 16) {
            return -1;
        }

        if (L'a' <= _Ch && _Ch <= L'f') {
            return _Ch - L'a' + 10;
        }

        if (L'A' <= _Ch && _Ch <= L'F') {
            return _Ch - L'A' + 10;
        }

        return -1;
    }
};

class regex_error : public runtime_error { // type of all regular expression exceptions
public:
    explicit regex_error(regex_constants::error_type _Ex) : runtime_error(_Stringify(_Ex)), _Err(_Ex) {}

    _NODISCARD regex_constants::error_type code() const {
        return _Err;
    }

private:
    static const char* _Stringify(regex_constants::error_type _Ex) { // map error code to string
        switch (_Ex) { // select known error_type message
        case regex_constants::error_collate:
            return "regex_error(error_collate): The expression "
                   "contained an invalid collating element name.";
        case regex_constants::error_ctype:
            return "regex_error(error_ctype): The expression "
                   "contained an invalid character class name.";
        case regex_constants::error_escape:
            return "regex_error(error_escape): The expression "
                   "contained an invalid escaped character, "
                   "or a trailing escape.";
        case regex_constants::error_backref:
            return "regex_error(error_backref): The expression "
                   "contained an invalid back reference.";
        case regex_constants::error_brack:
            return "regex_error(error_brack): The expression "
                   "contained mismatched [ and ].";
        case regex_constants::error_paren:
            return "regex_error(error_paren): The expression "
                   "contained mismatched ( and ).";
        case regex_constants::error_brace:
            return "regex_error(error_brace): The expression "
                   "contained mismatched { and }.";
        case regex_constants::error_badbrace:
            return "regex_error(error_badbrace): The expression "
                   "contained an invalid range in a { expression }.";
        case regex_constants::error_range:
            return "regex_error(error_range): The expression "
                   "contained an invalid character range, "
                   "such as [b-a] in most encodings.";
        case regex_constants::error_space:
            return "regex_error(error_space): There was insufficient "
                   "memory to convert the expression into "
                   "a finite state machine.";
        case regex_constants::error_badrepeat:
            return "regex_error(error_badrepeat): One of *?+{ "
                   "was not preceded by a valid regular expression.";
        case regex_constants::error_complexity:
            return "regex_error(error_complexity): The complexity of "
                   "an attempted match against a regular expression "
                   "exceeded a pre-set level.";
        case regex_constants::error_stack:
            return "regex_error(error_stack): There was insufficient "
                   "memory to determine whether the regular expression "
                   "could match the specified character sequence.";
        case regex_constants::error_parse:
            return "regex_error(error_parse)";
        case regex_constants::error_syntax:
            return "regex_error(error_syntax)";
        default:
            return "regex_error";
        }
    }

    regex_constants::error_type _Err;
};

template <class _Traits, class _FwdIt1, class _FwdIt2>
int _Iter_compare3(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, true_type) {
    // _Iter_compare for memory buffer ranges
    return _Traits_compare<_Traits>(
        _First1, static_cast<size_t>(_Last1 - _First1), _First2, static_cast<size_t>(_Last2 - _First2));
}

template <class _Traits, class _FwdIt1, class _FwdIt2>
int _Iter_compare3(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, false_type) {
    // _Iter_compare for general ranges
    const pair<_FwdIt1, _FwdIt2> _Cmp = _STD mismatch(_First1, _Last1, _First2, _Last2, _Char_traits_eq<_Traits>{});

    if (_Cmp.first == _Last1) {
        if (_Cmp.second == _Last2) {
            return 0;
        } else {
            return -1;
        }
    }

    if (_Cmp.second == _Last2) {
        return 1;
    }

    if (_Traits::lt(*_Cmp.first, *_Cmp.second)) {
        return -1;
    } else {
        return 1;
    }
}

template <class _Traits, class _FwdIt1, class _FwdIt2>
int _Iter_compare2(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2) {
    // decide which _Iter_compare implementation to use
    return _Iter_compare3<_Traits>(
        _First1, _Last1, _First2, _Last2, bool_constant<conjunction_v<is_pointer<_FwdIt1>, is_pointer<_FwdIt2>>>{});
}

template <class _Traits, class _FwdIt1, class _FwdIt2>
int _Iter_compare(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2) {
    // compare two iterator ranges:
    // if [_First1, _Last1) is lexicographically less than [_First2, _Last2), a negative value
    // if [_First2, _Last2) is lexicographically less than [_First1, _Last1), a positive value
    // otherwise, zero
    using _Elem = typename _Traits::char_type;
    static_assert(is_same_v<_Iter_value_t<_FwdIt1>, _Elem>, "bad _FwdIt1 to _Iter_compare");
    static_assert(is_same_v<_Iter_value_t<_FwdIt2>, _Elem>, "bad _FwdIt2 to _Iter_compare");

    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    return _Iter_compare2<_Traits>(
        _Get_unwrapped(_First1), _Get_unwrapped(_Last1), _Get_unwrapped(_First2), _Get_unwrapped(_Last2));
}

inline bool _Is_word(unsigned char _UCh) {
    // special casing char to avoid branches for std::regex in this path
    static constexpr bool _Is_word_table[(numeric_limits<unsigned char>::max)() + 1] = {
        //        X0 X1 X2 X3 X4 X5 X6 X7 X8 X9 XA XB XC XD XE XF
        /* 0X */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 1X */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 2X */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 3X */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 0-9
        /* 4X */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A-Z
        /* 5X */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 5F == _
        /* 6X */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // a-z
        /* 7X */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
        // non-ASCII values initialized to 0
    };
    return _Is_word_table[_UCh];
}

inline bool _Is_word(char _Ch) {
    return _Is_word(static_cast<unsigned char>(_Ch));
}

template <class _Elem>
bool _Is_word(_Elem _Ch) {
    // assumes 'x' == L'x' for the ASCII range
    using _UElem = make_unsigned_t<_Elem>;

    const auto _UCh = static_cast<_UElem>(_Ch);
    return _UCh <= static_cast<_UElem>('z') && _Is_word(static_cast<unsigned char>(_UCh));
}

#if _HAS_CXX20
template <class _Ty, class = void>
struct _Get_member_comparison_category {
    using type = weak_ordering;
};

template <class _Ty>
struct _Get_member_comparison_category<_Ty, void_t<typename _Ty::comparison_category>> {
    using type = typename _Ty::comparison_category;
};
#endif // _HAS_CXX20

template <class _BidIt>
class sub_match : public pair<_BidIt, _BidIt> { // class to hold contents of a capture group
public:
    using _Mybase         = pair<_BidIt, _BidIt>;
    using value_type      = typename iterator_traits<_BidIt>::value_type;
    using difference_type = typename iterator_traits<_BidIt>::difference_type;
    using iterator        = _BidIt;
    using string_type     = basic_string<value_type>;
    // Note that _Traits should always be std::char_traits<value_type>
    using _Traits = typename string_type::traits_type;
    // Note that _Size_type should always be std::size_t
    using _Size_type = typename string_type::size_type;

#if _HAS_CXX20
    using _Comparison_category = typename _Get_member_comparison_category<_Traits>::type;
#endif // _HAS_CXX20

    constexpr sub_match() : _Mybase(), matched(false) {}

    bool matched;

    _NODISCARD difference_type length() const {
        const _Mybase _Range(_Effective_range());
        return _STD distance(_Range.first, _Range.second);
    }

    operator string_type() const { // convert matched text to string
        return str();
    }

    _NODISCARD string_type str() const { // convert matched text to string
        const _Mybase _Range(_Effective_range());
        return string_type(_Range.first, _Range.second);
    }

    _NODISCARD int compare(const sub_match& _Right) const { // compare *this to _Right
        const _Mybase _LRange(_Effective_range());
        const _Mybase _RRange(_Right._Effective_range());
        return _Iter_compare<_Traits>(_LRange.first, _LRange.second, _RRange.first, _RRange.second);
    }

    _NODISCARD int compare(const string_type& _Right) const { // compare *this to _Right
        return _Compare(_Right.data(), _Right.size());
    }

    _NODISCARD int compare(_In_z_ const value_type* _Ptr) const { // compare *this to array pointed to by _Ptr
        return _Compare(_Ptr, _Traits::length(_Ptr));
    }

    int _Compare(const value_type* const _Ptr, const _Size_type _Count) const {
        // compare *this to array [_Ptr, _Ptr + _Count)
        const _Mybase _Range(_Effective_range());
        return _Iter_compare<_Traits>(_Range.first, _Range.second, _Ptr, _Ptr + _Count);
    }

    bool _Match_equal(const sub_match& _Right) const { // check *this to _Right for equality
        const _Mybase _LRange(_Effective_range());
        const _Mybase _RRange(_Right._Effective_range());
        return _STD equal(_LRange.first, _LRange.second, _RRange.first, _RRange.second, _Char_traits_eq<_Traits>{});
    }

    bool _Match_equal(const value_type* const _Ptr, const _Size_type _Count) const {
        // check *this to array [_Ptr, _Ptr + _Count) for equality
        const _Mybase _Range(_Effective_range());
        return _STD equal(_Range.first, _Range.second, _Ptr, _Ptr + _Count, _Char_traits_eq<_Traits>{});
    }

    bool _Match_equal(const value_type* const _Ptr) const { // check *this to C-string _Ptr for equality
        return _Match_equal(_Ptr, _Traits::length(_Ptr));
    }

    bool _Less(const sub_match& _Right) const { // check whether *this is less than _Right
        const _Mybase _LRange(_Effective_range());
        const _Mybase _RRange(_Right._Effective_range());
        return _STD lexicographical_compare(
            _LRange.first, _LRange.second, _RRange.first, _RRange.second, _Char_traits_lt<_Traits>{});
    }

    bool _Less(const value_type* const _Ptr, const _Size_type _Count) const {
        // check whether *this is less than [_Ptr, _Ptr + _Count)
        const _Mybase _Range(_Effective_range());
        return _STD lexicographical_compare(
            _Range.first, _Range.second, _Ptr, _Ptr + _Count, _Char_traits_lt<_Traits>{});
    }

    bool _Less(const value_type* const _Ptr) const { // check whether *this is less than C-string _Ptr
        return _Less(_Ptr, _Traits::length(_Ptr));
    }

    bool _Greater(const value_type* const _Ptr, const _Size_type _Count) const {
        // check whether *this is greater than [_Ptr, _Ptr + _Count)
        const _Mybase _Range(_Effective_range());
        return _STD lexicographical_compare(
            _Ptr, _Ptr + _Count, _Range.first, _Range.second, _Char_traits_lt<_Traits>{});
    }

    bool _Greater(const value_type* const _Ptr) const { // check whether *this is greater than C-string _Ptr
        return _Greater(_Ptr, _Traits::length(_Ptr));
    }

    _Mybase _Effective_range() const { // if matched, returns *this; otherwise returns an empty range
        if (matched) {
            return *this;
        } else {
            return _Mybase{};
        }
    }
};

using csub_match  = sub_match<const char*>;
using wcsub_match = sub_match<const wchar_t*>;
using ssub_match  = sub_match<string::const_iterator>;
using wssub_match = sub_match<wstring::const_iterator>;

template <class _BidIt>
_NODISCARD bool operator==(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return _Left._Match_equal(_Right);
}

#if _HAS_CXX20
template <class _BidIt>
_NODISCARD auto operator<=>(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return static_cast<typename sub_match<_BidIt>::_Comparison_category>(_Left.compare(_Right) <=> 0);
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
template <class _BidIt>
_NODISCARD bool operator!=(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left == _Right);
}

template <class _BidIt>
_NODISCARD bool operator<(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return _Left._Less(_Right);
}

template <class _BidIt>
_NODISCARD bool operator>(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right < _Left;
}

template <class _BidIt>
_NODISCARD bool operator<=(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Right < _Left);
}

template <class _BidIt>
_NODISCARD bool operator>=(const sub_match<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left < _Right);
}
#endif // !_HAS_CXX20

template <class _BidIt>
_NODISCARD bool operator==(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return _Left._Match_equal(_Right);
}

#if _HAS_CXX20
template <class _BidIt>
_NODISCARD auto operator<=>(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return static_cast<typename sub_match<_BidIt>::_Comparison_category>(_Left.compare(_Right) <=> 0);
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
template <class _BidIt>
_NODISCARD bool operator==(const _Iter_value_t<_BidIt>* _Left, const sub_match<_BidIt>& _Right) {
    return _Right._Match_equal(_Left);
}

template <class _BidIt>
_NODISCARD bool operator!=(const _Iter_value_t<_BidIt>* _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left == _Right);
}

template <class _BidIt>
_NODISCARD bool operator<(const _Iter_value_t<_BidIt>* _Left, const sub_match<_BidIt>& _Right) {
    return _Right._Greater(_Left);
}

template <class _BidIt>
_NODISCARD bool operator>(const _Iter_value_t<_BidIt>* _Left, const sub_match<_BidIt>& _Right) {
    return _Right < _Left;
}

template <class _BidIt>
_NODISCARD bool operator<=(const _Iter_value_t<_BidIt>* _Left, const sub_match<_BidIt>& _Right) {
    return !(_Right < _Left);
}

template <class _BidIt>
_NODISCARD bool operator>=(const _Iter_value_t<_BidIt>* _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left < _Right);
}

template <class _BidIt>
_NODISCARD bool operator!=(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return !(_Left == _Right);
}

template <class _BidIt>
_NODISCARD bool operator<(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return _Left._Less(_Right);
}

template <class _BidIt>
_NODISCARD bool operator>(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return _Right < _Left;
}

template <class _BidIt>
_NODISCARD bool operator<=(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return !(_Right < _Left);
}

template <class _BidIt>
_NODISCARD bool operator>=(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>* _Right) {
    return !(_Left < _Right);
}
#endif // !_HAS_CXX20

template <class _BidIt>
_NODISCARD bool operator==(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return _Left._Match_equal(_STD addressof(_Right), 1);
}

#if _HAS_CXX20
template <class _BidIt>
_NODISCARD auto operator<=>(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return static_cast<typename sub_match<_BidIt>::_Comparison_category>(
        _Left._Compare(_STD addressof(_Right), 1) <=> 0);
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
template <class _BidIt>
_NODISCARD bool operator==(const _Iter_value_t<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right._Match_equal(_STD addressof(_Left), 1);
}

template <class _BidIt>
_NODISCARD bool operator!=(const _Iter_value_t<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left == _Right);
}

template <class _BidIt>
_NODISCARD bool operator<(const _Iter_value_t<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right._Greater(_STD addressof(_Left), 1);
}

template <class _BidIt>
_NODISCARD bool operator>(const _Iter_value_t<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right < _Left;
}

template <class _BidIt>
_NODISCARD bool operator<=(const _Iter_value_t<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Right < _Left);
}

template <class _BidIt>
_NODISCARD bool operator>=(const _Iter_value_t<_BidIt>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left < _Right);
}

template <class _BidIt>
_NODISCARD bool operator!=(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return !(_Left == _Right);
}

template <class _BidIt>
_NODISCARD bool operator<(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return _Left._Less(_STD addressof(_Right), 1);
}

template <class _BidIt>
_NODISCARD bool operator>(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return _Right < _Left;
}

template <class _BidIt>
_NODISCARD bool operator<=(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return !(_Right < _Left);
}

template <class _BidIt>
_NODISCARD bool operator>=(const sub_match<_BidIt>& _Left, const _Iter_value_t<_BidIt>& _Right) {
    return !(_Left < _Right);
}
#endif // !_HAS_CXX20

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator==(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return _Left._Match_equal(_Right.data(), _Right.size());
}

#if _HAS_CXX20
template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD auto operator<=>(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return static_cast<typename sub_match<_BidIt>::_Comparison_category>(_Left.compare(_Right) <=> 0);
}
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator==(
    const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right._Match_equal(_Left.data(), _Left.size());
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator!=(
    const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left == _Right);
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator<(
    const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right._Greater(_Left.data(), _Left.size());
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator>(
    const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Left, const sub_match<_BidIt>& _Right) {
    return _Right < _Left;
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator<=(
    const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Right < _Left);
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator>=(
    const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Left, const sub_match<_BidIt>& _Right) {
    return !(_Left < _Right);
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator!=(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return !(_Left == _Right);
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator<(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return _Left._Less(_Right.data(), _Right.size());
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator>(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return _Right < _Left;
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator<=(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return !(_Right < _Left);
}

template <class _BidIt, class _Traits, class _Alloc>
_NODISCARD bool operator>=(
    const sub_match<_BidIt>& _Left, const basic_string<_Iter_value_t<_BidIt>, _Traits, _Alloc>& _Right) {
    return !(_Left < _Right);
}
#endif // !_HAS_CXX20

template <class _Elem, class _Traits, class _BidIt>
basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const sub_match<_BidIt>& _Match) {
    return _Ostr << _Match.str();
}

template <class _BidIt, class _Alloc = allocator<sub_match<_BidIt>>>
class match_results;

template <class _BidIt, class _Alloc, class _InIt, class _OutIt>
_OutIt _Format_default(const match_results<_BidIt, _Alloc>& _Match, _OutIt _Out, _InIt _First, _InIt _Last,
    regex_constants::match_flag_type _Flags = regex_constants::format_default);

template <class _BidIt, class _Alloc, class _InIt, class _OutIt>
_OutIt _Format_sed(const match_results<_BidIt, _Alloc>& _Match, _OutIt _Out, _InIt _First, _InIt _Last,
    regex_constants::match_flag_type _Flags = regex_constants::format_default);

template <class _BidIt, class _Alloc>
class match_results { // class to hold contents of all capture groups
public:
    using _Elem     = sub_match<_BidIt>;
    using _MyCont   = vector<_Elem, _Alloc>;
    using _Mytraits = allocator_traits<_Alloc>;

    using value_type      = _Elem;
    using const_reference = const value_type&;
    using reference       = value_type&;
    using const_iterator  = typename _MyCont::const_iterator;
    using iterator        = const_iterator;
    using difference_type = typename iterator_traits<_BidIt>::difference_type;
    using size_type       = typename _Mytraits::size_type;
    using allocator_type  = _Alloc;
    using char_type       = typename iterator_traits<_BidIt>::value_type;
    using string_type     = basic_string<char_type>;

    match_results() : _Org(), _Ready(false) {}

    explicit match_results(const _Alloc& _Al) : _Org(), _Ready(false), _Matches(_Al) {}

    _NODISCARD bool ready() const noexcept /* strengthened */ {
        return _Ready;
    }

    _NODISCARD size_type size() const noexcept /* strengthened */ {
        return _Matches.size();
    }

    _NODISCARD size_type max_size() const {
        return _Matches.max_size();
    }

    _NODISCARD bool empty() const noexcept /* strengthened */ {
        return _Matches.empty();
    }

    _NODISCARD difference_type length(size_type _Sub = 0) const {
        return (*this)[_Sub].length();
    }

    _NODISCARD difference_type position(size_type _Sub = 0) const {
        return _STD distance(_Org, (*this)[_Sub].first);
    }

    _NODISCARD string_type str(size_type _Sub = 0) const {
        return string_type((*this)[_Sub]);
    }

    _NODISCARD const_reference operator[](size_type _Sub) const noexcept /* strengthened */ {
        return _Matches.size() <= _Sub ? _Null_elem : _Matches[_Sub];
    }

    _NODISCARD const_reference prefix() const noexcept /* strengthened */ {
        return _Prefix;
    }

    _NODISCARD const_reference suffix() const noexcept /* strengthened */ {
        return _Suffix;
    }

    _NODISCARD const_iterator begin() const noexcept /* strengthened */ {
        return _Matches.begin();
    }

    _NODISCARD const_iterator end() const noexcept /* strengthened */ {
        return _Matches.end();
    }

    _NODISCARD const_iterator cbegin() const noexcept /* strengthened */ {
        return _Matches.begin();
    }

    _NODISCARD const_iterator cend() const noexcept /* strengthened */ {
        return _Matches.end();
    }

    const _Elem* _Unchecked_begin() const noexcept {
        return _Matches._Unchecked_begin();
    }

    const _Elem* _Unchecked_end() const noexcept {
        return _Matches._Unchecked_end();
    }

    template <class _OutIt>
    _OutIt _Format1(_OutIt _Out, const char_type* _Fmt_begin, const char_type* _Fmt_end,
        regex_constants::match_flag_type _Flags) const { // format text, replacing matches
        return _Flags & regex_constants::format_sed ? _Format_sed(*this, _Out, _Fmt_begin, _Fmt_end, _Flags)
                                                    : _Format_default(*this, _Out, _Fmt_begin, _Fmt_end, _Flags);
    }

    template <class _OutIt>
    _OutIt format(_OutIt _Out, const char_type* _Fmt_begin, const char_type* _Fmt_end,
        regex_constants::match_flag_type _Flags = regex_constants::format_default) const {
        // format text, replacing matches
        _Adl_verify_range(_Fmt_begin, _Fmt_end);
        _Seek_wrapped(_Out, _Format1(_Get_unwrapped_unverified(_Out), _Fmt_begin, _Fmt_end, _Flags));
        return _Out;
    }

    template <class _OutIt, class _Traits1, class _Alloc1>
    _OutIt format(_OutIt _Out, const basic_string<char_type, _Traits1, _Alloc1>& _Fmt,
        regex_constants::match_flag_type _Flags = regex_constants::format_default) const {
        // format text, replacing matches
        _Seek_wrapped(_Out, _Format1(_Get_unwrapped_unverified(_Out), _Fmt.data(), _Fmt.data() + _Fmt.size(), _Flags));
        return _Out;
    }

    template <class _Traits1, class _Alloc1>
    _NODISCARD basic_string<char_type, _Traits1, _Alloc1> format(const basic_string<char_type, _Traits1, _Alloc1>& _Fmt,
        regex_constants::match_flag_type _Flags = regex_constants::format_default) const {
        // format text, replacing matches
        basic_string<char_type, _Traits1, _Alloc1> _Str;
        format(_STD back_inserter(_Str), _Fmt.data(), _Fmt.data() + _Fmt.size(), _Flags);
        return _Str;
    }

    _NODISCARD string_type format(
        const char_type* _Fmt_begin, regex_constants::match_flag_type _Flags = regex_constants::format_default) const {
        // format text, replacing matches
        return format(static_cast<string_type>(_Fmt_begin), _Flags);
    }

    _NODISCARD allocator_type get_allocator() const noexcept /* strengthened */ {
        return static_cast<allocator_type>(_Matches.get_allocator());
    }

    void swap(match_results& _Right) noexcept(_Is_nothrow_swappable<_BidIt>::value) /* strengthened */ {
        _STD swap(_Ready, _Right._Ready);
        _Swap_adl(_Org, _Right._Org);
        _Matches.swap(_Right._Matches);
        _STD swap(_Prefix, _Right._Prefix);
        _STD swap(_Suffix, _Right._Suffix);
    }

    void _Resize(unsigned int _Nx) {
        _Matches.resize(_Nx);
    }

    _Elem& _Pfx() noexcept {
        return _Prefix;
    }

    _Elem& _Sfx() noexcept {
        return _Suffix;
    }

    _Elem& _Null() noexcept {
        return _Null_elem;
    }

    _Elem& _At(unsigned int _Sub) noexcept {
        return _Matches[_Sub];
    }

    _Elem _At(unsigned int _Sub) const noexcept {
        return _Matches[_Sub];
    }

    _BidIt _Org;
    bool _Ready;

private:
    _MyCont _Matches;
    _Elem _Prefix;
    _Elem _Suffix;
    _Elem _Null_elem;
};

template <class _BidIt, class _Alloc>
_NODISCARD bool operator==(const match_results<_BidIt, _Alloc>& _Left, const match_results<_BidIt, _Alloc>& _Right) {
    if (!_Left.ready() && !_Right.ready()) {
        return true;
    } else if (!_Left.ready() || !_Right.ready()) {
        return false;
    } else if (_Left.empty() && _Right.empty()) {
        return true;
    } else if (_Left.empty() || _Right.empty()) {
        return false;
    } else {
        return _Left.size() == _Right.size() && _Left.prefix() == _Right.prefix() && _Left.suffix() == _Right.suffix()
            && _STD equal(_Left.begin(), _Left.end(), _Right.begin());
    }
}

#if !_HAS_CXX20
template <class _BidIt, class _Alloc>
_NODISCARD bool operator!=(const match_results<_BidIt, _Alloc>& _Left, const match_results<_BidIt, _Alloc>& _Right) {
    return !(_Left == _Right);
}
#endif // !_HAS_CXX20

const unsigned int _BRE_MAX_GRP = 9U;

const unsigned int _Bmp_max   = 256U; // must fit in an unsigned int
const unsigned int _Bmp_shift = 3U;
const unsigned int _Bmp_chrs  = 1U << _Bmp_shift; // # of bits to be stored in each char
const unsigned int _Bmp_mask  = _Bmp_chrs - 1U;
const unsigned int _Bmp_size  = (_Bmp_max + _Bmp_chrs - 1U) / _Bmp_chrs;

const unsigned int _Buf_incr        = 16U;
const unsigned int _ARRAY_THRESHOLD = 4U;

enum _Node_flags : int { // flags for nfa nodes with special properties
    _Fl_none    = 0x00,
    _Fl_negate  = 0x01,
    _Fl_greedy  = 0x02,
    _Fl_final   = 0x04,
    _Fl_longest = 0x08
};

_BITMASK_OPS(_Node_flags)

enum _Node_type { // type flag for nfa nodes
    _N_none,
    _N_nop,
    _N_bol,
    _N_eol,
    _N_wbound,
    _N_dot,
    _N_str,
    _N_class,
    _N_group,
    _N_end_group,
    _N_assert,
    _N_neg_assert,
    _N_end_assert,
    _N_capture,
    _N_end_capture,
    _N_back,
    _N_if,
    _N_endif,
    _N_rep,
    _N_end_rep,
    _N_begin,
    _N_end
};

template <class _Elem>
struct _Buf { // character buffer
    _Buf() : _Sz(0), _Nchrs(0), _Chrs(nullptr) {}

    ~_Buf() noexcept {
        _CSTD free(_Chrs);
    }

    unsigned int _Size() const {
        return _Nchrs;
    }

    _Elem _At(unsigned int _Idx) const {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Idx < _Nchrs, "regex buffer subscript out of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        return _Chrs[_Idx];
    }

    const _Elem* _Str() const {
        return _Chrs;
    }

    void _Insert(_Elem _Ch) { // append _Ch
        if (_Sz <= _Nchrs) {
            _Expand(_Nchrs + _Buf_incr);
        }

        _Chrs[_Nchrs++] = _Ch;
    }

    _Elem _Del() { // remove and return last character
        return _Chrs[--_Nchrs];
    }

    template <class _FwdIt>
    void _Insert(_FwdIt _First, _FwdIt _Last) { // append multiple characters
        while (_First != _Last) {
            _Insert(*_First++);
        }
    }

private:
    void _Expand(unsigned int _Len) { // expand buffer to hold _Len characters
        _Elem* _Tmp = static_cast<_Elem*>(_CSTD realloc(_Chrs, _Get_size_of_n<sizeof(_Elem)>(_Len)));
        if (!_Tmp) {
            _Xbad_alloc();
        }

        _Chrs = _Tmp;
        _Sz   = _Len;
    }

    unsigned int _Sz;
    unsigned int _Nchrs;
    _Elem* _Chrs;
};

struct _Bitmap { // accelerator table for small character values
    _Bitmap() {
        _CSTD memset(_Chrs, '\0', _Bmp_size);
    }

    template <class _Elem>
    void _Mark(_Elem _Ch) { // mark character _Ch
        static_assert(is_unsigned_v<_Elem>, "_Elem must be unsigned");
        unsigned int _Wide = _Ch;
        _Chrs[_Wide >> _Bmp_shift] |= (1 << (_Wide & _Bmp_mask));
    }

    template <class _Elem>
    bool _Find(_Elem _Ch) const {
        static_assert(is_unsigned_v<_Elem>, "_Elem must be unsigned");
        unsigned int _Wide = _Ch;
        return (_Chrs[_Wide >> _Bmp_shift] & (1 << (_Wide & _Bmp_mask))) != 0;
    }

private:
    unsigned char _Chrs[_Bmp_size];
};

template <class _Elem>
struct _Sequence { // holds sequences of _Sz elements
    explicit _Sequence(unsigned int _Len) : _Sz(_Len) {}

    unsigned int _Sz;
    _Buf<_Elem> _Data;
    _Sequence* _Next;
};

class _Node_base { // base class for all nfa nodes
public:
    explicit _Node_base(_Node_type _Ty, _Node_flags _Fl = _Fl_none)
        : _Kind(_Ty), _Flags(_Fl), _Next(nullptr), _Prev(nullptr) {}

    _Node_type _Kind;
    _Node_flags _Flags;
    _Node_base* _Next;
    _Node_base* _Prev;

    virtual ~_Node_base() noexcept {}
};

inline void _Destroy_node(_Node_base* _Nx, _Node_base* _Ne = nullptr) noexcept { // destroy sublist of nodes
    while (_Nx != _Ne && _Nx) { // destroy node
        _Node_base* _Tmp = _Nx;
        _Nx              = _Nx->_Next;
        _Tmp->_Next      = nullptr;
        delete _Tmp;
    }
}

class _Root_node : public _Node_base { // root of parse tree
public:
    _Root_node() : _Node_base(_N_begin), _Loops(0), _Marks(0), _Refs(0) {
        static_assert(sizeof(_Refs) == sizeof(_Atomic_counter_t), "invalid _Refs size");
    }

    regex_constants::syntax_option_type _Fl;
    unsigned int _Loops;
    unsigned int _Marks;
    unsigned int _Refs;
};

class _Node_end_group : public _Node_base { // node that marks end of a group
public:
    _Node_end_group(_Node_type _Ty, _Node_flags _Fl, _Node_base* _Bx) : _Node_base(_Ty, _Fl), _Back(_Bx) {}

    _Node_base* _Back;
};

class _Node_assert : public _Node_base { // node that holds an ECMAScript assertion
public:
    explicit _Node_assert(_Node_type _Ty, _Node_flags _Fl = _Fl_none) : _Node_base(_Ty, _Fl), _Child(nullptr) {}

    ~_Node_assert() noexcept {
        _Destroy_node(_Child);
    }

    _Node_base* _Child;
};

class _Node_capture : public _Node_base { // node that marks beginning of a capture group
public:
    explicit _Node_capture(unsigned int _Ix) : _Node_base(_N_capture, _Fl_none), _Idx(_Ix) {}

    unsigned int _Idx;
};

class _Node_back : public _Node_base { // node that holds a back reference
public:
    explicit _Node_back(unsigned int _Ix) : _Node_base(_N_back, _Fl_none), _Idx(_Ix) {}

    unsigned int _Idx;
};

template <class _Elem>
class _Node_str : public _Node_base { // node that holds text
public:
    explicit _Node_str(_Node_flags _Fl = _Fl_none) : _Node_base(_N_str, _Fl) {}

    _Buf<_Elem> _Data;
};

template <class _Elem, class _RxTraits>
class _Node_class : public _Node_base { // node that holds a character class (POSIX bracket expression)
public:
    explicit _Node_class(_Node_type _Ty = _N_class, _Node_flags _Fl = _Fl_none)
        : _Node_base(_Ty, _Fl), _Coll(nullptr), _Small(nullptr), _Large(nullptr), _Ranges(nullptr), _Classes{},
          _Equiv(nullptr) {}

    ~_Node_class() noexcept {
        _Tidy(_Coll);
        delete _Small;
        delete _Large;
        delete _Ranges;
        _Tidy(_Equiv);
    }

    void _Tidy(_Sequence<_Elem>* _Head) noexcept { // clean up a list of sequences
        while (_Head) { // delete the head of the list
            _Sequence<_Elem>* _Temp = _Head;
            _Head                   = _Head->_Next;
            delete _Temp;
        }
    }

    _Sequence<_Elem>* _Coll;
    _Bitmap* _Small;
    _Buf<_Elem>* _Large;
    _Buf<_Elem>* _Ranges;
    typename _RxTraits::char_class_type _Classes;
    _Sequence<_Elem>* _Equiv;
};

class _Node_endif : public _Node_base { // node that marks the end of an alternative
public:
    _Node_endif() : _Node_base(_N_endif, _Fl_none) {}
};

class _Node_if : public _Node_base { // node that marks the beginning of an alternative
public:
    explicit _Node_if(_Node_base* _End)
        : _Node_base(_N_if, _Fl_none), _Endif(static_cast<_Node_endif*>(_End)), _Child(nullptr) {}

    ~_Node_if() noexcept {
        _Node_if* _Cur = _Child;
        while (_Cur) { // destroy branch
            _Node_if* _Tmp = _Cur;
            _Cur           = _Cur->_Child;
            _Tmp->_Child   = nullptr;
            _Destroy_node(_Tmp, _Endif);
        }
    }

    _Node_endif* _Endif;
    _Node_if* _Child;
};

class _Node_rep;

class _Node_end_rep : public _Node_base { // node that marks the end of a repetition
public:
    _Node_end_rep() : _Node_base(_N_end_rep), _Begin_rep(nullptr) {}

    _Node_rep* _Begin_rep;

    _Node_end_rep& operator=(const _Node_end_rep&) = delete;
};

struct _Loop_vals_t { // storage for loop administration
    int _Loop_idx;
    void* _Loop_iter;
};

class _Node_rep : public _Node_base { // node that marks the beginning of a repetition
public:
    _Node_rep(bool _Greedy, int _Mn, int _Mx, _Node_end_rep* _End, unsigned int _Number)
        : _Node_base(_N_rep, _Greedy ? _Fl_greedy : _Fl_none), _Min(_Mn), _Max(_Mx), _End_rep(_End),
          _Loop_number(_Number), _Simple_loop(-1) {}

    const int _Min;
    const int _Max;
    _Node_end_rep* _End_rep;
    unsigned int _Loop_number;
    int _Simple_loop; // -1 undetermined, 0 contains if/do, 1 simple

    _Node_rep& operator=(const _Node_rep&) = delete;
};

template <class _FwdIt, class _Elem, class _RxTraits>
class _Builder { // provides operations used by _Parser to build the nfa
public:
    using _Difft = typename iterator_traits<_FwdIt>::difference_type;

    _Builder(const _RxTraits& _Tr, regex_constants::syntax_option_type);
    bool _Beg_expr() const;
    void _Setlong();
    // _Discard_pattern is an ABI zombie name
    void _Tidy() noexcept;
    _Node_base* _Getmark() const;

    void _Add_nop();
    void _Add_bol();
    void _Add_eol();
    void _Add_wbound();
    void _Add_dot();
    void _Add_char(_Elem _Ch);
    void _Add_class();
    void _Add_char_to_class(_Elem _Ch);
    void _Add_range(_Elem _Ex0, _Elem _Ex1);
    void _Add_named_class(_Regex_traits_base::char_class_type, bool = false);
    void _Add_equiv(_FwdIt, _FwdIt, _Difft);
    void _Add_coll(_FwdIt, _FwdIt, _Difft);
    _Node_base* _Begin_group();
    void _End_group(_Node_base* _Back);
    _Node_base* _Begin_assert_group(bool);
    void _End_assert_group(_Node_base*);
    _Node_base* _Begin_capture_group(unsigned int _Idx);
    void _Add_backreference(unsigned int _Idx);
    _Node_base* _Begin_if(_Node_base* _Start);
    void _Else_if(_Node_base*, _Node_base*);
    void _Add_rep(int _Min, int _Max, bool _Greedy);
    void _Negate();
    void _Mark_final();
    _Root_node* _End_pattern();

private:
    _Node_base* _Link_node(_Node_base*);
    static void _Insert_node(_Node_base*, _Node_base*);
    _Node_base* _New_node(_Node_type _Kind);
    void _Add_str_node();
    bool _Beg_expr(_Node_base*) const;
    void _Add_char_to_bitmap(_Elem _Ch);
    void _Add_char_to_array(_Elem _Ch);
    void _Add_elts(_Node_class<_Elem, _RxTraits>*, _Regex_traits_base::char_class_type, bool);
    void _Char_to_elts(_FwdIt, _FwdIt, _Difft, _Sequence<_Elem>**);

    _Root_node* _Root;
    _Node_base* _Current;
    regex_constants::syntax_option_type _Flags;
    const _RxTraits& _Traits;
    const int _Bmax; // Do not use; use _Get_bmax instead.
    const int _Tmax; // Do not use; use _Get_tmax instead.

    unsigned int _Get_bmax() const;
    unsigned int _Get_tmax() const;

public:
    _Builder& operator=(const _Builder&) = delete;
};

template <class _BidIt>
class _Bt_state_t { // holds the state needed for backtracking
public:
    _BidIt _Cur;
    vector<bool> _Grp_valid;
};

template <class _BidIt>
class _Tgt_state_t : public _Bt_state_t<_BidIt> { // holds the current state of the match
public:
    struct _Grp_t { // stores a pair of iterators
        _BidIt _Begin;
        _BidIt _End;
    };

    vector<_Grp_t> _Grps;

    void operator=(const _Bt_state_t<_BidIt>& _Other) {
        static_cast<_Bt_state_t<_BidIt>&>(*this) = _Other;
    }
};

template <class _BidIt, class _Elem, class _RxTraits, class _It>
class _Matcher { // provides ways to match a regular expression to a text sequence
public:
    _Matcher(_It _Pfirst, _It _Plast, const _RxTraits& _Tr, _Root_node* _Re, unsigned int _Nx,
        regex_constants::syntax_option_type _Sf, regex_constants::match_flag_type _Mf)
        : _End(_Plast), _First(_Pfirst), _Rep(_Re), _Sflags(_Sf), _Mflags(_Mf), _Matched(false),
          _Ncap(static_cast<int>(_Nx)), _Longest((_Re->_Flags & _Fl_longest) && !(_Mf & regex_constants::match_any)),
          _Traits(_Tr) {
        _Loop_vals.resize(_Re->_Loops);
        _Adl_verify_range(_Pfirst, _Plast);
    }

    void _Setf(regex_constants::match_flag_type _Mf) { // set specified flags
        _Mflags |= _Mf;
    }

    void _Clearf(regex_constants::match_flag_type _Mf) { // clear specified flags
        _Mflags &= ~_Mf;
    }

    template <class _Alloc>
    bool _Match(_It _Pfirst, match_results<_BidIt, _Alloc>* _Matches, bool _Full_match) {
        // try to match
        _First = _Pfirst;
        return _Match(_Matches, _Full_match);
    }

    template <class _Alloc>
    bool _Match(match_results<_BidIt, _Alloc>* _Matches, bool _Full_match) {
        // try to match
        if (_Matches) { // clear _Matches before doing work
            _Matches->_Ready = true;
            _Matches->_Resize(0);
        }

        _Begin          = _First;
        _Tgt_state._Cur = _First;
        _Tgt_state._Grp_valid.resize(_Get_ncap());
        _Tgt_state._Grps.resize(_Get_ncap());
        _Cap                  = static_cast<bool>(_Matches);
        _Full                 = _Full_match;
        _Max_complexity_count = _REGEX_MAX_COMPLEXITY_COUNT;
        _Max_stack_count      = _REGEX_MAX_STACK_COUNT;

        _Matched = false;

        if (!_Match_pat(_Rep)) {
            return false;
        }

        if (_Matches) { // copy results to _Matches
            _Matches->_Resize(_Get_ncap());
            for (unsigned int _Idx = 0; _Idx < _Get_ncap(); ++_Idx) { // copy submatch _Idx
                if (_Res._Grp_valid[_Idx]) { // copy successful match
                    _Matches->_At(_Idx).matched = true;
                    _Matches->_At(_Idx).first   = _Res._Grps[_Idx]._Begin;
                    _Matches->_At(_Idx).second  = _Res._Grps[_Idx]._End;
                } else { // copy failed match
                    _Matches->_At(_Idx).matched = false;
                    _Matches->_At(_Idx).first   = _End;
                    _Matches->_At(_Idx).second  = _End;
                }
            }
            _Matches->_Org           = _Begin;
            _Matches->_Pfx().first   = _Begin;
            _Matches->_Pfx().second  = _Matches->_At(0).first;
            _Matches->_Pfx().matched = _Matches->_Pfx().first != _Matches->_Pfx().second;

            _Matches->_Sfx().first   = _Matches->_At(0).second;
            _Matches->_Sfx().second  = _End;
            _Matches->_Sfx().matched = _Matches->_Sfx().first != _Matches->_Sfx().second;

            _Matches->_Null().first  = _End;
            _Matches->_Null().second = _End;
        }
        return true;
    }

    _BidIt _Skip(_BidIt, _BidIt, _Node_base* = nullptr);

private:
    _Tgt_state_t<_It> _Tgt_state;
    _Tgt_state_t<_It> _Res;
    vector<_Loop_vals_t> _Loop_vals;

    bool _Do_if(_Node_if*);
    bool _Do_rep0(_Node_rep*, bool);
    bool _Do_rep(_Node_rep*, bool, int);
    bool _Do_class(_Node_base*);
    bool _Match_pat(_Node_base*);
    bool _Better_match();
    bool _Is_wbound() const;

    unsigned int _Get_ncap() const;

    _It _Begin;
    _It _End;
    _It _First;
    _Node_base* _Rep;
    regex_constants::syntax_option_type _Sflags;
    regex_constants::match_flag_type _Mflags;
    bool _Matched;
    bool _Cap;
    int _Ncap; // Do not use. Use _Get_ncap instead.
    bool _Longest;
    const _RxTraits& _Traits;
    bool _Full;
    long _Max_complexity_count;
    long _Max_stack_count;

public:
    _Matcher& operator=(const _Matcher&) = delete;
};

enum _Prs_ret { // indicate class element type
    _Prs_none,
    _Prs_chr,
    _Prs_set
};

template <class _FwdIt, class _Elem, class _RxTraits>
class _Parser { // parse a regular expression
public:
    using char_class_type = typename _RxTraits::char_class_type;

    _Parser(const _RxTraits& _Tr, _FwdIt _Pfirst, _FwdIt _Plast, regex_constants::syntax_option_type _Fx);
    _Root_node* _Compile();

    unsigned int _Mark_count() const {
        return _Grp_idx + 1;
    }

private:
    // lexing
    void _Error(regex_constants::error_type);

    bool _Is_esc() const;
    void _Trans();
    void _Next();
    void _Expect(_Meta_type, regex_constants::error_type);

    // parsing
    int _Do_digits(int _Base, int _Count);
    bool _DecimalDigits();
    void _HexDigits(int);
    bool _OctalDigits();
    void _Do_ex_class(_Meta_type);
    bool _CharacterClassEscape(bool);
    _Prs_ret _ClassEscape(bool);
    _Prs_ret _ClassAtom();
    void _ClassRanges();
    void _CharacterClass();
    bool _IdentityEscape();
    bool _IsIdentityEscape() const;
    bool _Do_ffn(_Elem);
    bool _Do_ffnx(_Elem);
    bool _CharacterEscape();
    void _AtomEscape();
    void _Do_capture_group();
    void _Do_noncapture_group();
    void _Do_assert_group(bool);
    bool _Wrapped_disjunction();
    void _Quantifier();
    bool _Alternative();
    void _Disjunction();

    _FwdIt _Pat;
    _FwdIt _Begin;
    _FwdIt _End;
    unsigned int _Grp_idx;
    int _Disj_count;
    vector<bool> _Finished_grps;
    _Builder<_FwdIt, _Elem, _RxTraits> _Nfa;
    const _RxTraits& _Traits;
    regex_constants::syntax_option_type _Flags;
    int _Val;
    _Elem _Char;
    _Meta_type _Mchar;
    unsigned int _L_flags;
};

enum _Lang_flags { // describe language properties
    _L_ext_rep    = 0x00000001, // + and ? repetitions
    _L_alt_pipe   = 0x00000002, // uses '|' for alternation
    _L_alt_nl     = 0x00000004, // uses '\n' for alternation (grep, egrep)
    _L_nex_grp    = 0x00000008, // has non-escaped capture groups
    _L_nex_rep    = 0x00000010, // has non-escaped repeats
    _L_nc_grp     = 0x00000020, // has non-capture groups (?:xxx)
    _L_asrt_gen   = 0x00000040, // has generalized assertions (?=xxx), (?!xxx)
    _L_asrt_wrd   = 0x00000080, // has word boundary assertions (\b, \B)
    _L_bckr       = 0x00000100, // has backreferences (ERE doesn't)
    _L_lim_bckr   = 0x00000200, // has limited backreferences (BRE \1-\9)
    _L_ngr_rep    = 0x00000400, // has non-greedy repeats
    _L_esc_uni    = 0x00000800, // has Unicode escape sequences
    _L_esc_hex    = 0x00001000, // has hexadecimal escape sequences
    _L_esc_oct    = 0x00002000, // has octal escape sequences
    _L_esc_bsl    = 0x00004000, // has escape backslash in character classes
    _L_esc_ffnx   = 0x00008000, // has extra file escapes (\a and \b)
    _L_esc_ffn    = 0x00010000, // has limited file escapes (\[fnrtv])
    _L_esc_wsd    = 0x00020000, // has w, s, and d character set escapes
    _L_esc_ctrl   = 0x00040000, // has control escape
    _L_no_nl      = 0x00080000, // no newline in pattern or matching text
    _L_bzr_chr    = 0x00100000, // \0 is a valid character constant
    _L_grp_esc    = 0x00200000, // \ is special character in group
    _L_ident_ECMA = 0x00400000, // ECMA identity escape (not identifierpart)
    _L_ident_ERE  = 0x00800000, // ERE identity escape (.[\*^$, plus {+?}()
    _L_ident_awk  = 0x01000000, // awk identity escape ( ERE plus "/)
    _L_anch_rstr  = 0x02000000, // anchor restricted to beginning/end
    _L_star_beg   = 0x04000000, // star okay at beginning of RE/expr (BRE)
    _L_empty_grp  = 0x08000000, // empty group allowed (ERE prohibits "()")
    _L_paren_bal  = 0x10000000, // ')'/'}'/']' special only after '('/'{'/']'
    _L_brk_rstr   = 0x20000000, // ']' not special when first character in set
    _L_mtch_long  = 0x40000000, // find longest match (ERE, BRE)
};

class _Regex_base : public _Container_base { // base class for basic_regex to construct and destroy proxy
public:
#if 0 < _ITERATOR_DEBUG_LEVEL
    _Regex_base() { // construct proxy
        this->_Myproxy = new _Container_proxy;
        this->_Myproxy->_Mycont = this;
    }

    ~_Regex_base() noexcept {
        this->_Orphan_all();
        delete this->_Myproxy;
        this->_Myproxy = nullptr;
    }
#endif // 0 < _ITERATOR_DEBUG_LEVEL
};

template <class _Elem, class _RxTraits = regex_traits<_Elem>>
class basic_regex : public _Regex_base { // regular expression
public:
    using value_type  = _Elem;
    using traits_type = _RxTraits;
    using locale_type = typename _RxTraits::locale_type;
    using string_type = typename _RxTraits::string_type;
    using flag_type   = regex_constants::syntax_option_type;

    static constexpr flag_type icase      = regex_constants::icase;
    static constexpr flag_type nosubs     = regex_constants::nosubs;
    static constexpr flag_type optimize   = regex_constants::optimize;
    static constexpr flag_type collate    = regex_constants::collate;
    static constexpr flag_type ECMAScript = regex_constants::ECMAScript;
    static constexpr flag_type basic      = regex_constants::basic;
    static constexpr flag_type extended   = regex_constants::extended;
    static constexpr flag_type awk        = regex_constants::awk;
    static constexpr flag_type grep       = regex_constants::grep;
    static constexpr flag_type egrep      = regex_constants::egrep;

    basic_regex() : _Rep(nullptr) {} // construct empty object

    explicit basic_regex(_In_z_ const _Elem* _Ptr, flag_type _Flags = regex_constants::ECMAScript) : _Rep(nullptr) {
        _Reset(_Ptr, _Ptr + _RxTraits::length(_Ptr), _Flags, random_access_iterator_tag{});
    }

    basic_regex(_In_reads_(_Count) const _Elem* _Ptr, size_t _Count, flag_type _Flags = regex_constants::ECMAScript)
        : _Rep(nullptr) {
        if (_Ptr) {
            _Reset(_Ptr, _Ptr + _Count, _Flags, random_access_iterator_tag{});
            return;
        }

        _Xregex_error(regex_constants::error_parse);
    }

    template <class _STtraits, class _STalloc>
    explicit basic_regex(
        const basic_string<_Elem, _STtraits, _STalloc>& _Str, flag_type _Flags = regex_constants::ECMAScript)
        : _Rep(nullptr) {
        _Reset(_Str.data(), _Str.data() + static_cast<ptrdiff_t>(_Str.size()), _Flags, random_access_iterator_tag{});
    }

    template <class _InIt>
    basic_regex(_InIt _First, _InIt _Last, flag_type _Flags) : _Rep(nullptr) {
        _Adl_verify_range(_First, _Last);
        _Reset(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Flags, _Iter_cat_t<_InIt>{});
    }

    template <class _InIt>
    basic_regex(_InIt _First, _InIt _Last) : _Rep(nullptr) {
        _Adl_verify_range(_First, _Last);
        _Reset(_Get_unwrapped(_First), _Get_unwrapped(_Last), regex_constants::ECMAScript, _Iter_cat_t<_InIt>{});
    }

    basic_regex(const basic_regex& _Right)
#if _ENHANCED_REGEX_VISUALIZER
        : _Rep(nullptr), _Traits(_Right._Traits), _Visualization(_Right._Visualization)
#else // _ENHANCED_REGEX_VISUALIZER
        : _Rep(nullptr), _Traits(_Right._Traits)
#endif // _ENHANCED_REGEX_VISUALIZER
    { // construct copy of _Right
        _Reset(_Right._Rep);
    }

    basic_regex(initializer_list<_Elem> _Ilist, flag_type _Flags = regex_constants::ECMAScript) : _Rep(nullptr) {
        _Reset(_Ilist.begin(), _Ilist.end(), _Flags, random_access_iterator_tag{});
    }

    basic_regex& operator=(initializer_list<_Elem> _Ilist) { // replace with regular expression in initializer_list
        return assign(_Ilist);
    }

    basic_regex& assign(initializer_list<_Elem> _Ilist, flag_type _Flags = regex_constants::ECMAScript) {
        // replace with regular expression in initializer_list
        _Reset(_Ilist.begin(), _Ilist.end(), _Flags, random_access_iterator_tag{});
        return *this;
    }

    basic_regex(basic_regex&& _Right) noexcept : _Rep(nullptr) {
        _Assign_rv(_STD move(_Right));
    }

    basic_regex& operator=(basic_regex&& _Right) noexcept {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    void _Assign_rv(basic_regex&& _Right) {
        if (this != _STD addressof(_Right)) { // clear this and steal from _Right
            _Tidy();

#if _ENHANCED_REGEX_VISUALIZER
            _Visualization = _STD move(_Right._Visualization);
#endif // _ENHANCED_REGEX_VISUALIZER

            _Rep        = _Right._Rep;
            _Right._Rep = nullptr;
        }
    }

    basic_regex& assign(basic_regex&& _Right) noexcept {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    ~basic_regex() noexcept {
        _Tidy();
    }

    basic_regex& operator=(const basic_regex& _Right) {
        return assign(_Right);
    }

    basic_regex& operator=(_In_z_ const _Elem* _Ptr) {
        _Reset(_Ptr, _Ptr + _RxTraits::length(_Ptr), ECMAScript, random_access_iterator_tag{});
        return *this;
    }

    template <class _STtraits, class _STalloc>
    basic_regex& operator=(const basic_string<_Elem, _STtraits, _STalloc>& _Str) {
        _Reset(
            _Str.data(), _Str.data() + static_cast<ptrdiff_t>(_Str.size()), ECMAScript, random_access_iterator_tag{});
        return *this;
    }

    unsigned int _Loop_count() const {
        return _Rep ? _Rep->_Loops : 0;
    }

    _NODISCARD unsigned int mark_count() const {
        return _Rep ? _Rep->_Marks - 1 : 0;
    }

    basic_regex& assign(const basic_regex& _Right) {
#if _ENHANCED_REGEX_VISUALIZER
        _Visualization = _Right._Visualization;
#endif // _ENHANCED_REGEX_VISUALIZER

        _Reset(_Right._Rep);
        return *this;
    }

    basic_regex& assign(_In_z_ const _Elem* _Ptr, flag_type _Flags = regex_constants::ECMAScript) {
        return assign(_Ptr, _RxTraits::length(_Ptr), _Flags);
    }

    basic_regex& assign(
        _In_reads_(_Count) const _Elem* _Ptr, size_t _Count, flag_type _Flags = regex_constants::ECMAScript) {
        _Reset(_Ptr, _Ptr + _Count, _Flags, random_access_iterator_tag{});
        return *this;
    }

    template <class _STtraits, class _STalloc>
    basic_regex& assign(
        const basic_string<_Elem, _STtraits, _STalloc>& _Str, flag_type _Flags = regex_constants::ECMAScript) {
        _Reset(_Str.data(), _Str.data() + static_cast<ptrdiff_t>(_Str.size()), _Flags, random_access_iterator_tag{});
        return *this;
    }

    template <class _InIt>
    basic_regex& assign(_InIt _First, _InIt _Last, flag_type _Flags = regex_constants::ECMAScript) {
        _Adl_verify_range(_First, _Last);
        _Reset(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Flags, _Iter_cat_t<_InIt>{});
        return *this;
    }

    _NODISCARD flag_type flags() const {
        return _Rep ? _Rep->_Fl : flag_type{};
    }

    locale_type imbue(locale_type _Loc) { // clear regular expression and set locale to argument
        _Tidy();
        return _Traits.imbue(_Loc);
    }

    locale_type getloc() const {
        return _Traits.getloc();
    }

    void swap(basic_regex& _Right) noexcept /* strengthened */ {
        _STD swap(_Rep, _Right._Rep);

#if _ENHANCED_REGEX_VISUALIZER
        _Visualization.swap(_Right._Visualization);
#endif // _ENHANCED_REGEX_VISUALIZER
    }

    _Root_node* _Get() const {
        return _Rep;
    }

    bool _Empty() const {
        return _Rep == nullptr;
    }

    const _RxTraits& _Get_traits() const {
        return _Traits;
    }

private:
    _Root_node* _Rep;
    _RxTraits _Traits;

#if _ENHANCED_REGEX_VISUALIZER
    basic_string<_Elem> _Visualization;
#endif // _ENHANCED_REGEX_VISUALIZER

    void _Tidy() noexcept { // free all storage
        if (_Rep && _MT_DECR(reinterpret_cast<_Atomic_counter_t&>(_Rep->_Refs)) == 0) {
            _Destroy_node(_Rep);
        }

        _Rep = nullptr;
    }

    template <class _InIt>
    void _Reset(_InIt _First, _InIt _Last, flag_type _Flags, input_iterator_tag) {
        // build regular expression from input iterators
        basic_string<_Iter_value_t<_InIt>> _Str(_First, _Last);

        _Reset(_Str.data(), _Str.data() + static_cast<ptrdiff_t>(_Str.size()), _Flags, forward_iterator_tag{});
    }

    template <class _FwdIt>
    void _Reset(_FwdIt _First, _FwdIt _Last, flag_type _Flags, forward_iterator_tag) {
        // build regular expression from forward iterators
#if _ENHANCED_REGEX_VISUALIZER
        _Visualization.assign(_First, _Last);
#endif // _ENHANCED_REGEX_VISUALIZER

        _Parser<_FwdIt, _Elem, _RxTraits> _Prs(_Traits, _First, _Last, _Flags);
        _Root_node* _Rx = _Prs._Compile();
        _Reset(_Rx);
    }

    void _Reset(_Root_node* _Rx) { // build regular expression holding root node _Rx
        if (_Rx) {
            _MT_INCR(reinterpret_cast<_Atomic_counter_t&>(_Rx->_Refs));
        }

        _Tidy();
        _Rep = _Rx;
    }
};

#if _HAS_CXX17
template <class _FwdIt>
basic_regex(_FwdIt, _FwdIt, regex_constants::syntax_option_type = regex_constants::ECMAScript)
    -> basic_regex<typename iterator_traits<_FwdIt>::value_type>;
#endif // _HAS_CXX17

template <class _Elem, class _RxTraits>
void swap(basic_regex<_Elem, _RxTraits>& _Left, basic_regex<_Elem, _RxTraits>& _Right) noexcept /* strengthened */ {
    _Left.swap(_Right);
}

template <class _BidIt, class _Alloc>
void swap(match_results<_BidIt, _Alloc>& _Left, match_results<_BidIt, _Alloc>& _Right) noexcept(
    noexcept(_Left.swap(_Right))) /* strengthened */ {
    _Left.swap(_Right);
}

using regex   = basic_regex<char>;
using wregex  = basic_regex<wchar_t>;
using cmatch  = match_results<const char*>;
using wcmatch = match_results<const wchar_t*>;
using smatch  = match_results<string::const_iterator>;
using wsmatch = match_results<wstring::const_iterator>;

template <class _BidIt, class _Alloc, class _InIt, class _OutIt>
_OutIt _Format_default(const match_results<_BidIt, _Alloc>& _Match, _OutIt _Out, _InIt _First, _InIt _Last,
    regex_constants::match_flag_type) { // format with ECMAScript rules
    while (_First != _Last) { // process one character or escape sequence
        if (*_First != '$') {
            *_Out++ = *_First++;
        } else if (++_First == _Last) {
            *_Out++ = '$'; // preserve $ at end
        } else if (*_First == '$') { // replace $$
            *_Out++ = '$';
            ++_First;
        } else if (*_First == '`') { // replace $`
            _Out = _STD copy(_Match.prefix().first, _Match.prefix().second, _Out);
            ++_First;
        } else if (*_First == '\'') { // replace $'
            _Out = _STD copy(_Match.suffix().first, _Match.suffix().second, _Out);
            ++_First;
        } else if (*_First == '&') { // replace $&
            ++_First;
            if (_Match.size() != 0) {
                _Out = _STD copy(_Match._At(0).first, _Match._At(0).second, _Out);
            }
        } else if ('0' <= *_First && *_First <= '9') { // replace capture group descriptors $n, $nn
            auto _Num              = static_cast<unsigned int>(*_First++ - '0');
            const bool _Two_digits = _First != _Last && '0' <= *_First && *_First <= '9';
            if (_Two_digits) { // process second digit
                _Num *= 10;
                _Num += static_cast<unsigned int>(*_First++ - '0');
            }
            if (_Num == 0) { // preserve $0, $00
                *_Out++ = '$';
                *_Out++ = '0';
                if (_Two_digits) {
                    *_Out++ = '0';
                }
            } else if (_Num < _Match.size()) {
                _Out = _STD copy(_Match._At(_Num).first, _Match._At(_Num).second, _Out);
            }
        } else { // replace $x
            *_Out++ = '$';
            *_Out++ = *_First++;
        }
    }
    return _Out;
}

template <class _BidIt, class _Alloc, class _InIt, class _OutIt>
_OutIt _Format_sed(const match_results<_BidIt, _Alloc>& _Match, _OutIt _Out, _InIt _First, _InIt _Last,
    regex_constants::match_flag_type) { // format with sed rules
    while (_First != _Last) { // process one character or escape sequence
        if (*_First == '&') { // replace with full match
            ++_First;
            if (_Match.size() != 0) {
                _Out = _STD copy(_Match._At(0).first, _Match._At(0).second, _Out);
            }
        } else if (*_First != '\\') {
            *_Out++ = *_First++;
        } else if (++_First != _Last) {
            if ('0' <= *_First && *_First <= '9') { // replace \n, including \0
                const auto _Num = static_cast<unsigned int>(*_First++ - '0');
                if (_Num < _Match.size()) {
                    _Out = _STD copy(_Match._At(_Num).first, _Match._At(_Num).second, _Out);
                }
            } else {
                *_Out++ = *_First++; // replace \x, including \\, \&
            }
        }
    }

    return _Out;
}

template <class _BidIt, class _Alloc, class _Elem, class _RxTraits, class _It>
bool _Regex_match1(_It _First, _It _Last, match_results<_BidIt, _Alloc>* _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs,
    bool _Full) { // try to match regular expression to target text
    if (_Re._Empty()) {
        return false;
    }

    _Matcher<_BidIt, _Elem, _RxTraits, _It> _Mx(
        _First, _Last, _Re._Get_traits(), _Re._Get(), _Re.mark_count() + 1, _Re.flags(), _Flgs);
    return _Mx._Match(_Matches, _Full);
}

template <class _BidIt, class _Alloc, class _Elem, class _RxTraits>
bool regex_match(_BidIt _First, _BidIt _Last, match_results<_BidIt, _Alloc>& _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // try to match regular expression to target text
    _Adl_verify_range(_First, _Last);
    return _Regex_match1(_First, _Last, _STD addressof(_Matches), _Re, _Flgs, true);
}

template <class _BidIt, class _Elem, class _RxTraits>
_NODISCARD bool regex_match(_BidIt _First, _BidIt _Last, const basic_regex<_Elem, _RxTraits>& _Re,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // try to match regular expression to target text
    _Adl_verify_range(_First, _Last);
    return _Regex_match1(_Get_unwrapped(_First), _Get_unwrapped(_Last),
        static_cast<match_results<_Unwrapped_t<const _BidIt&>>*>(nullptr), _Re, _Flgs | regex_constants::match_any,
        true);
}

template <class _Elem, class _RxTraits>
_NODISCARD bool regex_match(_In_z_ const _Elem* _Str, const basic_regex<_Elem, _RxTraits>& _Re,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // try to match regular expression to target text
    const _Elem* _Last = _Str + char_traits<_Elem>::length(_Str);
    return _Regex_match1(
        _Str, _Last, static_cast<match_results<const _Elem*>*>(nullptr), _Re, _Flgs | regex_constants::match_any, true);
}

template <class _Elem, class _Alloc, class _RxTraits>
bool regex_match(_In_z_ const _Elem* _Str, match_results<const _Elem*, _Alloc>& _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // try to match regular expression to target text
    const _Elem* _Last = _Str + char_traits<_Elem>::length(_Str);
    return _Regex_match1(_Str, _Last, _STD addressof(_Matches), _Re, _Flgs, true);
}

template <class _StTraits, class _StAlloc, class _Alloc, class _Elem, class _RxTraits>
bool regex_match(const basic_string<_Elem, _StTraits, _StAlloc>& _Str,
    match_results<typename basic_string<_Elem, _StTraits, _StAlloc>::const_iterator, _Alloc>& _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // try to match regular expression to target text
    return _Regex_match1(_Str.begin(), _Str.end(), _STD addressof(_Matches), _Re, _Flgs, true);
}

template <class _StTraits, class _StAlloc, class _Alloc, class _Elem, class _RxTraits>
bool regex_match(const basic_string<_Elem, _StTraits, _StAlloc>&&,
    match_results<typename basic_string<_Elem, _StTraits, _StAlloc>::const_iterator, _Alloc>&,
    const basic_regex<_Elem, _RxTraits>&, regex_constants::match_flag_type = regex_constants::match_default) = delete;

template <class _StTraits, class _StAlloc, class _Elem, class _RxTraits>
_NODISCARD bool regex_match(const basic_string<_Elem, _StTraits, _StAlloc>& _Str,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // try to match regular expression to target text
    return _Regex_match1(_Str.data(), _Str.data() + _Str.size(), static_cast<match_results<const _Elem*>*>(nullptr),
        _Re, _Flgs | regex_constants::match_any, true);
}

template <class _BidIt, class _Alloc, class _Elem, class _RxTraits, class _It>
bool _Regex_search2(_It _First, _It _Last, match_results<_BidIt, _Alloc>* _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs, _It _Org) {
    // search for regular expression match in target text
    if (_Re._Empty()) {
        return false;
    }

    bool _Found      = false;
    const _It _Begin = _First;
    if ((_Flgs & regex_constants::_Skip_zero_length) && _First != _Last) {
        ++_First;
    }

    _Matcher<_BidIt, _Elem, _RxTraits, _It> _Mx(
        _First, _Last, _Re._Get_traits(), _Re._Get(), _Re.mark_count() + 1, _Re.flags(), _Flgs);

    if (_Mx._Match(_Matches, false)) {
        _Found = true;
    } else if (_First != _Last && !(_Flgs & regex_constants::match_continuous)) { // try more on suffixes
        _Mx._Setf(regex_constants::match_prev_avail);
        _Mx._Clearf(regex_constants::_Match_not_null);
        while ((_First = _Mx._Skip(++_First, _Last)) != _Last) {
            if (_Mx._Match(_First, _Matches, false)) { // found match starting at _First
                _Found = true;
                break;
            }
        }

        if (!_Found && _Mx._Match(_Last, _Matches, false)) {
            _Found = true;
        }
    }

    if (_Found && _Matches) { // update _Matches
        _Matches->_Org           = _Org;
        _Matches->_Pfx().first   = _Begin;
        _Matches->_Pfx().matched = _Matches->_Pfx().first != _Matches->_Pfx().second;
    }
    return _Found;
}

template <class _BidIt, class _Alloc, class _Elem, class _RxTraits>
bool regex_search(_BidIt _First, _BidIt _Last, match_results<_BidIt, _Alloc>& _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search for regular expression match in target text
    _Adl_verify_range(_First, _Last);
    return _Regex_search2(_First, _Last, _STD addressof(_Matches), _Re, _Flgs, _First);
}

template <class _BidIt, class _Elem, class _RxTraits>
_NODISCARD bool regex_search(_BidIt _First, _BidIt _Last, const basic_regex<_Elem, _RxTraits>& _Re,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search for regular expression match in target text
    _Adl_verify_range(_First, _Last);
    return _Regex_search2(_Get_unwrapped(_First), _Get_unwrapped(_Last),
        static_cast<match_results<_Unwrapped_t<const _BidIt&>>*>(nullptr), _Re, _Flgs | regex_constants::match_any,
        _Get_unwrapped(_First));
}

template <class _Elem, class _RxTraits>
_NODISCARD bool regex_search(_In_z_ const _Elem* _Str, const basic_regex<_Elem, _RxTraits>& _Re,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search for regular expression match in target text
    const _Elem* _Last = _Str + char_traits<_Elem>::length(_Str);
    return _Regex_search2(
        _Str, _Last, static_cast<match_results<const _Elem*>*>(nullptr), _Re, _Flgs | regex_constants::match_any, _Str);
}

template <class _Elem, class _Alloc, class _RxTraits>
bool regex_search(_In_z_ const _Elem* _Str, match_results<const _Elem*, _Alloc>& _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search for regular expression match in target text
    const _Elem* _Last = _Str + char_traits<_Elem>::length(_Str);
    return _Regex_search2(_Str, _Last, _STD addressof(_Matches), _Re, _Flgs, _Str);
}

template <class _StTraits, class _StAlloc, class _Alloc, class _Elem, class _RxTraits>
bool regex_search(const basic_string<_Elem, _StTraits, _StAlloc>& _Str,
    match_results<typename basic_string<_Elem, _StTraits, _StAlloc>::const_iterator, _Alloc>& _Matches,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search for regular expression match in target text
    return _Regex_search2(_Str.begin(), _Str.end(), _STD addressof(_Matches), _Re, _Flgs, _Str.begin());
}

template <class _StTraits, class _StAlloc, class _Alloc, class _Elem, class _RxTraits>
bool regex_search(const basic_string<_Elem, _StTraits, _StAlloc>&&,
    match_results<typename basic_string<_Elem, _StTraits, _StAlloc>::const_iterator, _Alloc>&,
    const basic_regex<_Elem, _RxTraits>&, regex_constants::match_flag_type = regex_constants::match_default) = delete;

template <class _StTraits, class _StAlloc, class _Elem, class _RxTraits>
_NODISCARD bool regex_search(const basic_string<_Elem, _StTraits, _StAlloc>& _Str,
    const basic_regex<_Elem, _RxTraits>& _Re, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search for regular expression match in target text
    using _Iter = typename basic_string<_Elem, _StTraits, _StAlloc>::const_pointer;

    _Iter _First = _Str.c_str();
    _Iter _Last  = _First + _Str.size();
    return _Regex_search2(
        _First, _Last, static_cast<match_results<_Iter>*>(nullptr), _Re, _Flgs | regex_constants::match_any, _First);
}

template <class _OutIt, class _BidIt, class _RxTraits, class _Elem, class _Traits, class _Alloc>
_OutIt _Regex_replace1(_OutIt _Result, _BidIt _First, _BidIt _Last, const basic_regex<_Elem, _RxTraits>& _Re,
    const basic_string<_Elem, _Traits, _Alloc>& _Fmt, regex_constants::match_flag_type _Flgs) {
    // search and replace
    match_results<_BidIt> _Matches;
    _BidIt _Pos                             = _First;
    regex_constants::match_flag_type _Flags = _Flgs;
    regex_constants::match_flag_type _Not_null{};

    while (
        _Regex_search2(_Pos, _Last, _STD addressof(_Matches), _Re, _Flags | _Not_null, _Pos)) { // replace at each match
        if (!(_Flgs & regex_constants::format_no_copy)) {
            _Result = _STD copy(_Matches.prefix().first, _Matches.prefix().second, _Result);
        }

        _Result = _Matches._Format1(_Result, _Fmt.data(), _Fmt.data() + _Fmt.size(), _Flags);

        _Pos = _Matches[0].second;
        if (_Pos == _Last || (_Flgs & regex_constants::format_first_only)) {
            break;
        }

        if (_Matches[0].first == _Matches[0].second) {
            _Not_null = regex_constants::_Match_not_null;
        } else { // non-null match, recognize earlier text
            _Not_null = regex_constants::match_flag_type{};
            _Flags |= regex_constants::match_prev_avail;
        }
    }
    return _Flgs & regex_constants::format_no_copy ? _Result : _Copy_unchecked(_Pos, _Last, _Result);
}

template <class _OutIt, class _BidIt, class _RxTraits, class _Elem, class _Traits, class _Alloc>
_OutIt regex_replace(_OutIt _Result, _BidIt _First, _BidIt _Last, const basic_regex<_Elem, _RxTraits>& _Re,
    const basic_string<_Elem, _Traits, _Alloc>& _Fmt,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search and replace, iterator result, string format
    _Adl_verify_range(_First, _Last);
    _Seek_wrapped(_Result, _Regex_replace1(_Get_unwrapped_unverified(_Result), _Get_unwrapped(_First),
                               _Get_unwrapped(_Last), _Re, _Fmt, _Flgs));
    return _Result;
}

template <class _OutIt, class _BidIt, class _RxTraits, class _Elem>
_OutIt regex_replace(_OutIt _Result, _BidIt _First, _BidIt _Last, const basic_regex<_Elem, _RxTraits>& _Re,
    const _Elem* _Ptr, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search and replace, iterator result, NTBS format
    const basic_string<_Elem> _Fmt(_Ptr);
    return _STD regex_replace(_Result, _First, _Last, _Re, _Fmt, _Flgs);
}

template <class _RxTraits, class _Elem, class _Traits1, class _Alloc1, class _Traits2, class _Alloc2>
_NODISCARD basic_string<_Elem, _Traits1, _Alloc1> regex_replace(const basic_string<_Elem, _Traits1, _Alloc1>& _Str,
    const basic_regex<_Elem, _RxTraits>& _Re, const basic_string<_Elem, _Traits2, _Alloc2>& _Fmt,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search and replace, string result, string target, string format
    basic_string<_Elem, _Traits1, _Alloc1> _Res;
    _STD regex_replace(_STD back_inserter(_Res), _Str.begin(), _Str.end(), _Re, _Fmt, _Flgs);
    return _Res;
}

template <class _RxTraits, class _Elem, class _Traits1, class _Alloc1>
_NODISCARD basic_string<_Elem, _Traits1, _Alloc1> regex_replace(const basic_string<_Elem, _Traits1, _Alloc1>& _Str,
    const basic_regex<_Elem, _RxTraits>& _Re, const _Elem* _Ptr,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search and replace, string result, string target, NTBS format
    basic_string<_Elem, _Traits1, _Alloc1> _Res;
    const basic_string<_Elem> _Fmt(_Ptr);
    _STD regex_replace(_STD back_inserter(_Res), _Str.begin(), _Str.end(), _Re, _Fmt, _Flgs);
    return _Res;
}

template <class _RxTraits, class _Elem, class _Traits2, class _Alloc2>
_NODISCARD basic_string<_Elem> regex_replace(const _Elem* _Pstr, const basic_regex<_Elem, _RxTraits>& _Re,
    const basic_string<_Elem, _Traits2, _Alloc2>& _Fmt,
    regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search and replace, string result, NTBS target, string format
    basic_string<_Elem> _Res;
    const basic_string<_Elem> _Str(_Pstr);
    _STD regex_replace(_STD back_inserter(_Res), _Str.begin(), _Str.end(), _Re, _Fmt, _Flgs);
    return _Res;
}

template <class _RxTraits, class _Elem>
_NODISCARD basic_string<_Elem> regex_replace(const _Elem* _Pstr, const basic_regex<_Elem, _RxTraits>& _Re,
    const _Elem* _Ptr, regex_constants::match_flag_type _Flgs = regex_constants::match_default) {
    // search and replace, string result, NTBS target, NTBS format
    basic_string<_Elem> _Res;
    const basic_string<_Elem> _Str(_Pstr);
    const basic_string<_Elem> _Fmt(_Ptr);
    _STD regex_replace(_STD back_inserter(_Res), _Str.begin(), _Str.end(), _Re, _Fmt, _Flgs);
    return _Res;
}

template <class _BidIt, class _Elem = _Iter_value_t<_BidIt>, class _RxTraits = regex_traits<_Elem>>
class regex_iterator : public _Iterator_base {
public:
    using regex_type        = basic_regex<_Elem, _RxTraits>;
    using value_type        = match_results<_BidIt>;
    using difference_type   = ptrdiff_t;
    using pointer           = const value_type*;
    using reference         = const value_type&;
    using iterator_category = forward_iterator_tag;

    regex_iterator() : _MyRe(nullptr) {} // construct end of sequence iterator

    regex_iterator(_BidIt _First, _BidIt _Last, const regex_type& _Re,
        regex_constants::match_flag_type _Fl = regex_constants::match_default)
        : _Begin(_First), _End(_Last), _MyRe(_STD addressof(_Re)), _Flags(_Fl) {
        _Adl_verify_range(_Begin, _End);
        if (!_Regex_search2(_Begin, _End, _STD addressof(_MyVal), *_MyRe, _Flags, _Begin)) {
            _MyRe = nullptr;
        } else {
            this->_Adopt(_MyRe);
        }
    }

    regex_iterator(
        _BidIt, _BidIt, const regex_type&&, regex_constants::match_flag_type = regex_constants::match_default) = delete;

    _NODISCARD bool operator==(const regex_iterator& _Right) const {
        if (_MyRe != _Right._MyRe) {
            return false;
        } else if (!_MyRe) {
            return true;
        }

        return _Begin == _Right._Begin && _End == _Right._End && _Flags == _Right._Flags
            && _MyVal._At(0) == _Right._MyVal._At(0);
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(const regex_iterator& _Right) const {
        return !(*this == _Right);
    }
#endif // !_HAS_CXX20

    _NODISCARD const value_type& operator*() const {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_MyRe, "regex_iterator not dereferenceable");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        return _MyVal;
    }

    _NODISCARD const value_type* operator->() const {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_MyRe, "regex_iterator not dereferenceable");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        return _STD addressof(_MyVal);
    }

    regex_iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_MyRe, "regex_iterator not incrementable");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        _BidIt _Start = _MyVal._At(0).second;

#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(this->_Getcont(), "regex_iterator orphaned");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        bool _Skip_empty_match = false;
        if (_MyVal._At(0).first == _MyVal._At(0).second) { // handle zero-length match
            if (_Start == _End) { // store end-of-sequence iterator
                _MyRe = nullptr;

#if _ITERATOR_DEBUG_LEVEL == 2
                this->_Adopt(nullptr);
#endif // _ITERATOR_DEBUG_LEVEL == 2

                return *this;
            }

            // _Adl_verify_range(_Start, _End) checked in constructor
            if (_Regex_search2(_Start, _End, _STD addressof(_MyVal), *_MyRe,
                    _Flags | regex_constants::match_not_null | regex_constants::match_continuous, _Begin)) {
                return *this;
            }

            _Skip_empty_match = true;
        }
        _Flags = _Flags | regex_constants::match_prev_avail;

        auto _Tmp_flags = _Flags;
        if (_Skip_empty_match) {
            _Tmp_flags |= regex_constants::_Skip_zero_length;
        }

        // _Adl_verify_range(_Start, _End) checked in constructor
        if (!_Regex_search2(_Start, _End, _STD addressof(_MyVal), *_MyRe, _Tmp_flags, _Begin)) {
            // mark at end of sequence
            _MyRe = nullptr;
        }

        return *this;
    }

    regex_iterator operator++(int) {
        regex_iterator _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    bool _Atend() const { // test for end iterator
        return !_MyRe;
    }

private:
    _BidIt _Begin; // input sequence
    _BidIt _End; // input sequence
    const regex_type* _MyRe; // pointer to basic_regex object
    regex_constants::match_flag_type _Flags;
    match_results<_BidIt> _MyVal; // lookahead value (if _MyRe not null)
};

using cregex_iterator  = regex_iterator<const char*>;
using wcregex_iterator = regex_iterator<const wchar_t*>;
using sregex_iterator  = regex_iterator<string::const_iterator>;
using wsregex_iterator = regex_iterator<wstring::const_iterator>;

template <class _BidIt, class _Elem = _Iter_value_t<_BidIt>, class _RxTraits = regex_traits<_Elem>>
class regex_token_iterator {
public:
    using _Position         = regex_iterator<_BidIt, _Elem, _RxTraits>;
    using regex_type        = basic_regex<_Elem, _RxTraits>;
    using value_type        = sub_match<_BidIt>;
    using difference_type   = ptrdiff_t;
    using pointer           = const value_type*;
    using reference         = const value_type&;
    using iterator_category = forward_iterator_tag;

    regex_token_iterator() : _Res(nullptr) {} // construct end of sequence iterator

    regex_token_iterator(_BidIt _First, _BidIt _Last, const regex_type& _Re, int _Sub = 0,
        regex_constants::match_flag_type _Fl = regex_constants::match_default)
        : _Pos(_First, _Last, _Re, _Fl), _Cur(0), _Subs(&_Sub, &_Sub + 1) {
        _Init(_First, _Last);
    }

    regex_token_iterator(_BidIt _First, _BidIt _Last, const regex_type& _Re, const vector<int>& _Subx,
        regex_constants::match_flag_type _Fl = regex_constants::match_default)
        : _Pos(_First, _Last, _Re, _Fl), _Cur(0), _Subs(_Subx.begin(), _Subx.end()) {
        if (_Subs.empty()) {
            _Res = nullptr; // treat empty vector as end of sequence
        } else {
            _Init(_First, _Last);
        }
    }

    template <size_t _Nx>
    regex_token_iterator(_BidIt _First, _BidIt _Last, const regex_type& _Re, const int (&_Subx)[_Nx],
        regex_constants::match_flag_type _Fl = regex_constants::match_default)
        : _Pos(_First, _Last, _Re, _Fl), _Cur(0), _Subs(_Subx, _Subx + _Nx) {
        _Init(_First, _Last);
    }

    regex_token_iterator(_BidIt _First, _BidIt _Last, const regex_type& _Re, initializer_list<int> _Ilist,
        regex_constants::match_flag_type _Fl = regex_constants::match_default)
        : _Pos(_First, _Last, _Re, _Fl), _Cur(0), _Subs(_Ilist.begin(), _Ilist.end()) {
        _Init(_First, _Last);
    }

    regex_token_iterator(_BidIt, _BidIt, const regex_type&&, int = 0,
        regex_constants::match_flag_type = regex_constants::match_default) = delete;

    regex_token_iterator(_BidIt, _BidIt, const regex_type&&, const vector<int>&,
        regex_constants::match_flag_type = regex_constants::match_default) = delete;

    template <size_t _Nx>
    regex_token_iterator(_BidIt, _BidIt, const regex_type&&, const int (&)[_Nx],
        regex_constants::match_flag_type = regex_constants::match_default) = delete;

    regex_token_iterator(_BidIt, _BidIt, const regex_type&&, initializer_list<int>,
        regex_constants::match_flag_type = regex_constants::match_default) = delete;

    regex_token_iterator(const regex_token_iterator& _Right)
        : _Pos(_Right._Pos), _Suffix(_Right._Suffix), _Cur(_Right._Cur), _Subs(_Right._Subs) {
        // construct copy of _Right
        if (!_Right._Res) {
            _Res = nullptr;
        } else if (_Right._Res == _STD addressof(_Right._Suffix)) {
            _Res = _STD addressof(_Suffix);
        } else {
            _Res = _Current();
        }
    }

    regex_token_iterator& operator=(const regex_token_iterator& _Right) {
        if (this != _STD addressof(_Right)) { // copy from _Right
            _Pos    = _Right._Pos;
            _Cur    = _Right._Cur;
            _Suffix = _Right._Suffix;
            _Subs   = _Right._Subs;
            if (!_Right._Res) {
                _Res = nullptr;
            } else if (_Right._Res == _STD addressof(_Right._Suffix)) {
                _Res = _STD addressof(_Suffix);
            } else {
                _Res = _Current();
            }
        }
        return *this;
    }

    _NODISCARD bool operator==(const regex_token_iterator& _Right) const {
        if (!_Res || !_Right._Res) {
            return _Res == _Right._Res;
        }

        return *_Res == *_Right._Res && _Pos == _Right._Pos && _Subs == _Right._Subs;
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(const regex_token_iterator& _Right) const {
        return !(*this == _Right);
    }
#endif // !_HAS_CXX20

    _NODISCARD const value_type& operator*() const {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Res, "regex_token_iterator not dereferenceable");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        _Analysis_assume_(_Res);
        return *_Res;
    }

    _NODISCARD const value_type* operator->() const {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Res, "regex_token_iterator not dereferenceable");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        return _Res;
    }

    regex_token_iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Res, "regex_token_iterator not incrementable");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        if (_Res == _STD addressof(_Suffix)) {
            _Res = nullptr;
        } else if (++_Cur < _Subs.size()) {
            _Res = _Current();
        } else { // advance to next full match
            _Cur = 0;
            _Position _Prev(_Pos);
            ++_Pos;
            if (!_Pos._Atend()) {
                _Res = _Current();
            } else if (_Has_suffix() && _Prev->suffix().length() != 0) { // mark suffix
                _Suffix.matched = true;
                _Suffix.first   = _Prev->suffix().first;
                _Suffix.second  = _Prev->suffix().second;
                _Res            = _STD addressof(_Suffix);
            } else {
                _Res = nullptr;
            }
        }

        return *this;
    }

    regex_token_iterator operator++(int) {
        regex_token_iterator _Tmp = *this;
        ++*this;
        return _Tmp;
    }

private:
    _Position _Pos;
    const value_type* _Res;
    value_type _Suffix;
    size_t _Cur;
    vector<int> _Subs;

    bool _Has_suffix() const { // check for suffix specifier
        return _STD find(_Subs.begin(), _Subs.end(), -1) != _Subs.end();
    }

    void _Init(_BidIt _First, _BidIt _Last) { // initialize
        _Adl_verify_range(_First, _Last);
        if (!_Pos._Atend()) {
            _Res = _Current();
        } else if (_Has_suffix()) { // mark suffix (no match)
            _Suffix.matched = true;
            _Suffix.first   = _First;
            _Suffix.second  = _Last;
            _Res            = _STD addressof(_Suffix);
        } else {
            _Res = nullptr;
        }
    }

    const value_type* _Current() const {
        return &(_Subs[_Cur] == -1 ? _Pos->prefix() : (*_Pos)[static_cast<size_t>(_Subs[_Cur])]);
    }
};

using cregex_token_iterator  = regex_token_iterator<const char*>;
using wcregex_token_iterator = regex_token_iterator<const wchar_t*>;
using sregex_token_iterator  = regex_token_iterator<string::const_iterator>;
using wsregex_token_iterator = regex_token_iterator<wstring::const_iterator>;

template <class _FwdIt, class _Elem, class _RxTraits>
_Builder<_FwdIt, _Elem, _RxTraits>::_Builder(const _RxTraits& _Tr, regex_constants::syntax_option_type _Fx)
    : _Root(new _Root_node), _Current(_Root), _Flags(_Fx), _Traits(_Tr),
      _Bmax(static_cast<int>(_Fx & regex_constants::collate ? 0U : _Bmp_max)),
      _Tmax(static_cast<int>(_Fx & regex_constants::collate ? 0U : _ARRAY_THRESHOLD)) {}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Setlong() { // set flag
    _Root->_Flags |= _Fl_longest;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Negate() { // set flag
    _Current->_Flags ^= _Fl_negate;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Mark_final() { // set flag
    _Current->_Flags |= _Fl_final;
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_Getmark() const {
    return _Current;
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Builder<_FwdIt, _Elem, _RxTraits>::_Beg_expr(_Node_base* _Nx) const {
    // test for beginning of expression or subexpression
    return _Nx->_Kind == _N_begin || _Nx->_Kind == _N_group || _Nx->_Kind == _N_capture;
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Builder<_FwdIt, _Elem, _RxTraits>::_Beg_expr() const { // test for beginning of expression or subexpression
    return _Beg_expr(_Current) || (_Current->_Kind == _N_bol && _Beg_expr(_Current->_Prev));
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_Link_node(_Node_base* _Nx) { // insert _Nx at current location
    _Nx->_Prev = _Current;
    if (_Current->_Next) { // set back pointer
        _Nx->_Next             = _Current->_Next;
        _Current->_Next->_Prev = _Nx;
    }
    _Current->_Next = _Nx;
    _Current        = _Nx;
    return _Nx;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Insert_node(_Node_base* _Insert_before, _Node_base* _To_insert) {
    // insert _To_insert into the graph before the node _Insert_before
    _Insert_before->_Prev->_Next = _To_insert;
    _To_insert->_Prev            = _Insert_before->_Prev;
    _Insert_before->_Prev        = _To_insert;
    _To_insert->_Next            = _Insert_before;
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_New_node(_Node_type _Kind) { // allocate and link simple node
    return _Link_node(new _Node_base(_Kind));
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_nop() { // add nop node
    _New_node(_N_nop);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_bol() { // add bol node
    _New_node(_N_bol);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_eol() { // add eol node
    _New_node(_N_eol);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_wbound() { // add wbound node
    _New_node(_N_wbound);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_dot() { // add dot node
    _New_node(_N_dot);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_str_node() { // add string node
    _Link_node(new _Node_str<_Elem>);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_char(_Elem _Ch) { // append character
    if (_Current->_Kind != _N_str || (_Current->_Flags & _Fl_final)) {
        _Add_str_node();
    }

    if (_Flags & regex_constants::icase) {
        _Ch = _Traits.translate_nocase(_Ch);
    } else if (_Flags & regex_constants::collate) {
        _Ch = _Traits.translate(_Ch);
    }

    _Node_str<_Elem>* _Node = static_cast<_Node_str<_Elem>*>(_Current);
    _Node->_Data._Insert(_Ch);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_class() { // add bracket expression node
    _Link_node(new _Node_class<_Elem, _RxTraits>);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_char_to_bitmap(_Elem _Ch) { // add character to accelerator table
    if (_Flags & regex_constants::icase) {
        _Ch = _Traits.translate_nocase(_Ch);
    }

    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Current);

    if (!_Node->_Small) {
        _Node->_Small = new _Bitmap;
    }

    _Node->_Small->_Mark(static_cast<typename _RxTraits::_Uelem>(_Ch));
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_char_to_array(_Elem _Ch) { // append character to character array
    if (_Flags & regex_constants::icase) {
        _Ch = _Traits.translate_nocase(_Ch);
    }

    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Current);
    if (!_Node->_Large) {
        _Node->_Large = new _Buf<_Elem>;
    }

    _Node->_Large->_Insert(_Ch);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_char_to_class(_Elem _Ch) { // add character to bracket expression
    if (static_cast<typename _RxTraits::_Uelem>(_Ch) < _Bmp_max) {
        _Add_char_to_bitmap(_Ch);
    } else {
        _Add_char_to_array(_Ch);
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_range(_Elem _Arg0, _Elem _Arg1) {
    // add character range to set
    unsigned int _Ex0;
    unsigned int _Ex1;
    if (_Flags & regex_constants::icase) { // change to lowercase range
        _Ex0 = static_cast<unsigned int>(_Traits.translate_nocase(_Arg0));
        _Ex1 = static_cast<unsigned int>(_Traits.translate_nocase(_Arg1));
    } else {
        _Ex0 = static_cast<typename _RxTraits::_Uelem>(_Arg0);
        _Ex1 = static_cast<typename _RxTraits::_Uelem>(_Arg1);
    }

    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Current);
    for (; _Ex0 <= _Ex1 && _Ex1 < _Get_bmax(); ++_Ex0) { // set a bit
        if (!_Node->_Small) {
            _Node->_Small = new _Bitmap;
        }

        _Node->_Small->_Mark(_Ex0);
    }
    if (_Ex1 >= _Ex0) {
        if (_Ex1 - _Ex0 < _Get_tmax()) {
            for (; _Ex0 <= _Ex1; ++_Ex0) {
                _Add_char_to_array(static_cast<_Elem>(_Ex0));
            }
        } else { // store remaining range as pair
            if (!_Node->_Ranges) {
                _Node->_Ranges = new _Buf<_Elem>;
            }

            _Node->_Ranges->_Insert(static_cast<_Elem>(_Ex0));
            _Node->_Ranges->_Insert(static_cast<_Elem>(_Ex1));
        }
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_elts(
    _Node_class<_Elem, _RxTraits>* _Node, _Regex_traits_base::char_class_type _Cl, bool _Negate) {
    // add characters in named class to set
    for (unsigned int _Ch = 0; _Ch < _Bmp_max; ++_Ch) { // add elements or their inverse
        bool _Matches = _Traits.isctype(static_cast<_Elem>(_Ch), _Cl);
        if (_Matches != _Negate) { // add contents of named class to accelerator table
            if (!_Node->_Small) {
                _Node->_Small = new _Bitmap;
            }

            _Node->_Small->_Mark(_Ch);
        }
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_named_class(typename _Regex_traits_base::char_class_type _Cl,
    bool _Negate) { // add contents of named class to bracket expression
    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Current);
    _Add_elts(_Node, _Cl, _Negate);
    if (_Bmp_max < static_cast<unsigned int>((numeric_limits<_Elem>::max)())) {
        _Node->_Classes = static_cast<_Regex_traits_base::char_class_type>(_Node->_Classes | _Cl);
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Char_to_elts(_FwdIt _First, _FwdIt _Last, _Difft _Diff,
    _Sequence<_Elem>** _Cur) { // add collation element to element sequence
    while (*_Cur && static_cast<unsigned int>(_Diff) < (*_Cur)->_Sz) {
        _Cur = &(*_Cur)->_Next;
    }

    if (!(*_Cur) || static_cast<unsigned int>(_Diff) != (*_Cur)->_Sz) {
        // add new sequence holding elements of the same length
        _Sequence<_Elem>* _Node = *_Cur;
        *_Cur                   = new _Sequence<_Elem>(static_cast<unsigned int>(_Diff));
        (*_Cur)->_Next          = _Node;
    }
    (*_Cur)->_Data._Insert(_First, _Last);
}

template <class _FwdIt, class _Elem, class _RxTraits>
unsigned int _Builder<_FwdIt, _Elem, _RxTraits>::_Get_bmax() const {
    return static_cast<unsigned int>(_Bmax);
}

template <class _FwdIt, class _Elem, class _RxTraits>
unsigned int _Builder<_FwdIt, _Elem, _RxTraits>::_Get_tmax() const {
    return static_cast<unsigned int>(_Tmax);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_equiv(_FwdIt _First, _FwdIt _Last, _Difft _Diff) {
    // add elements of equivalence class to bracket expression
    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Current);
    typename _RxTraits::string_type _Str = _Traits.transform_primary(_First, _Last);
    for (unsigned int _Ch = 0; _Ch < _Bmp_max; ++_Ch) { // add elements
        _Elem _Ex = static_cast<_Elem>(_Ch);
        if (_Traits.transform_primary(_STD addressof(_Ex), _STD addressof(_Ex) + 1)
            == _Str) { // insert equivalent character into bitmap
            if (!_Node->_Small) {
                _Node->_Small = new _Bitmap;
            }

            _Node->_Small->_Mark(_Ch);
        }
    }
    if (_Bmp_max < static_cast<unsigned int>((numeric_limits<_Elem>::max)())) { // map range
        _Sequence<_Elem>** _Cur = _STD addressof(_Node->_Equiv);
        _Char_to_elts(_First, _Last, _Diff, _Cur);
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_coll(_FwdIt _First, _FwdIt _Last, _Difft _Diff) {
    // add collation element to bracket expression
    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Current);
    _Sequence<_Elem>** _Cur              = _STD addressof(_Node->_Coll);
    _Char_to_elts(_First, _Last, _Diff, _Cur);
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_Begin_group() { // add group node
    return _New_node(_N_group);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_End_group(_Node_base* _Back) { // add end of group node
    _Node_type _Elt;
    if (_Back->_Kind == _N_group) {
        _Elt = _N_end_group;
    } else if (_Back->_Kind == _N_assert || _Back->_Kind == _N_neg_assert) {
        _Elt = _N_end_assert;
    } else {
        _Elt = _N_end_capture;
    }

    _Link_node(new _Node_end_group(_Elt, _Fl_none, _Back));
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_Begin_assert_group(const bool _Neg) { // add assert node
    auto _Node1_unique   = _STD make_unique<_Node_assert>(_Neg ? _N_neg_assert : _N_assert);
    _Node_base* _Node2   = new _Node_base(_N_nop);
    _Node_assert* _Node1 = _Node1_unique.release();
    _Link_node(_Node1);
    _Node1->_Child = _Node2;
    _Node2->_Prev  = _Node1;
    _Current       = _Node2;
    return _Node1;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_End_assert_group(_Node_base* _Nx) { // add end of assert node
    _End_group(_Nx);
    _Current = _Nx;
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_Begin_capture_group(unsigned int _Idx) { // add capture group node
    return _Link_node(new _Node_capture(_Idx));
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_backreference(unsigned int _Idx) { // add back reference node
    _Link_node(new _Node_back(_Idx));
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Node_base* _Builder<_FwdIt, _Elem, _RxTraits>::_Begin_if(_Node_base* _Start) { // add if node
    // append endif node
    _Node_base* _Res = new _Node_endif;
    _Link_node(_Res);

    // insert if_node
    _Node_if* _Node1 = new _Node_if(_Res);
    _Node_base* _Pos = _Start->_Next;
    _Insert_node(_Pos, _Node1);
    return _Res;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Else_if(_Node_base* _Start, _Node_base* _End) { // add else node
    _Node_if* _Parent  = static_cast<_Node_if*>(_Start->_Next);
    _Node_base* _First = _End->_Next;
    _End->_Next        = nullptr;
    _Node_base* _Last  = _Current;
    _Current           = _End;
    _End->_Next        = nullptr;
    _Last->_Next       = _End;
    while (_Parent->_Child) {
        _Parent = _Parent->_Child;
    }

    _Parent->_Child        = new _Node_if(_End);
    _Parent->_Child->_Next = _First;
    _First->_Prev          = _Parent->_Child;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Add_rep(int _Min, int _Max, bool _Greedy) { // add repeat node
    if (_Current->_Kind == _N_str
        && static_cast<_Node_str<_Elem>*>(_Current)->_Data._Size() != 1) { // move final character to new string node
        _Node_str<_Elem>* _Node = static_cast<_Node_str<_Elem>*>(_Current);
        _Add_char(_Node->_Data._Del());
    }

    _Node_base* _Pos = _Current;
    if (_Pos->_Kind == _N_end_group || _Pos->_Kind == _N_end_capture) {
        _Pos = static_cast<_Node_end_group*>(_Pos)->_Back;
    }

    if (_Min == 0 && _Max == 1) { // rewrite zero-or-one quantifiers as alternations to make the
                                  // "simple loop" optimization more likely to engage
        _Node_endif* _End       = new _Node_endif;
        _Node_if* _If_expr      = new _Node_if(_End);
        _Node_if* _If_empty_str = new _Node_if(_End);
        _Node_base* _Gbegin     = new _Node_base(_N_group);
        _Node_end_group* _Gend  = new _Node_end_group(_N_end_group, _Fl_none, _Gbegin);

        _If_empty_str->_Next = _Gbegin;
        _Gbegin->_Prev       = _If_empty_str;

        _Gbegin->_Next = _Gend;
        _Gend->_Prev   = _Gbegin;

        _Gend->_Next = _End;

        _If_expr->_Child = _If_empty_str;

        _Link_node(_End);
        _Insert_node(_Pos, _If_expr);

        if (!_Greedy) {
            _Swap_adl(_If_expr->_Next->_Prev, _If_empty_str->_Next->_Prev);
            _Swap_adl(_If_expr->_Next, _If_empty_str->_Next);
        }
    } else {
        _Node_end_rep* _Node0 = new _Node_end_rep();
        _Node_rep* _Nx        = new _Node_rep(_Greedy, _Min, _Max, _Node0, _Root->_Loops++);
        _Node0->_Begin_rep    = _Nx;
        _Link_node(_Node0);
        _Insert_node(_Pos, _Nx);
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Root_node* _Builder<_FwdIt, _Elem, _RxTraits>::_End_pattern() { // wrap up
    _New_node(_N_end);
    return _Root;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Builder<_FwdIt, _Elem, _RxTraits>::_Tidy() noexcept { // free memory
    _Destroy_node(_Root);
    _Root = nullptr;
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Do_if(_Node_if* _Node) { // apply if node
    _Tgt_state_t<_It> _St = _Tgt_state;

    // look for the first match
    for (; _Node; _Node = _Node->_Child) { // process one branch of if
        _Tgt_state = _St; // rewind to where the alternation starts in input
        if (_Match_pat(_Node->_Next)) { // try to match this branch
            break;
        }
    }

    // if none of the if branches matched, fail to match
    if (!_Node) {
        return false;
    }

    // if we aren't looking for the longest match, that's it
    if (!_Longest) {
        return true;
    }

    // see if there is a longer match
    _Tgt_state_t<_It> _Final = _Tgt_state;
    auto _Final_len          = _STD distance(_St._Cur, _Tgt_state._Cur);
    for (;;) { // process one branch of if
        _Node = _Node->_Child;
        if (!_Node) {
            break;
        }

        _Tgt_state = _St;
        if (_Match_pat(_Node->_Next)) { // record match if it is longer
            const auto _Len = _STD distance(_St._Cur, _Tgt_state._Cur);
            if (_Final_len < _Len) { // memorize longest so far
                _Final     = _Tgt_state;
                _Final_len = _Len;
            }
        }
    }

    // set the input end to the longest match
    _Tgt_state = _Final;
    return true;
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Do_rep0(
    _Node_rep* _Node, bool _Greedy) { // apply repetition to loop with no nested if/do
    int _Ix               = 0;
    _Tgt_state_t<_It> _St = _Tgt_state;

    for (; _Ix < _Node->_Min; ++_Ix) { // do minimum number of reps
        _It _Cur = _Tgt_state._Cur;
        if (!_Match_pat(_Node->_Next)) { // didn't match minimum number of reps, fail
            _Tgt_state = _St;
            return false;
        } else if (_Cur == _Tgt_state._Cur) {
            _Ix = _Node->_Min - 1; // skip matches that don't change state
        }
    }

    _Tgt_state_t<_It> _Final = _Tgt_state;
    bool _Matched0           = false;
    _It _Saved_pos           = _Tgt_state._Cur;

    if (_Match_pat(_Node->_End_rep->_Next)) {
        if (!_Greedy) {
            return true; // go with current match
        }

        // record an acceptable match and continue
        _Final    = _Tgt_state;
        _Matched0 = true;
    }

    while (_Node->_Max == -1 || _Ix++ < _Node->_Max) { // try another rep/tail match
        _Tgt_state._Cur       = _Saved_pos;
        _Tgt_state._Grp_valid = _St._Grp_valid;
        if (!_Match_pat(_Node->_Next)) {
            break; // rep match failed, quit loop
        }

        _It _Mid = _Tgt_state._Cur;
        if (_Match_pat(_Node->_End_rep->_Next)) {
            if (!_Greedy) {
                return true; // go with current match
            }

            // record match and continue
            _Final    = _Tgt_state;
            _Matched0 = true;
        }

        if (_Saved_pos == _Mid) {
            break; // rep match ate no additional elements, quit loop
        }

        _Saved_pos = _Mid;
    }

    _Tgt_state = _Matched0 ? _Final : _St;
    return _Matched0;
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Do_rep(_Node_rep* _Node, bool _Greedy, int _Init_idx) {
    // apply repetition
    if (_Node->_Simple_loop == 1) {
        return _Do_rep0(_Node, _Greedy);
    }

    bool _Matched0        = false;
    _Tgt_state_t<_It> _St = _Tgt_state;
    _Loop_vals_t* _Psav   = &_Loop_vals[_Node->_Loop_number];
    int _Loop_idx_sav     = _Psav->_Loop_idx;
    _It* _Loop_iter_sav   = static_cast<_It*>(_Psav->_Loop_iter);
    _It _Cur_iter         = _Tgt_state._Cur;

    bool _Progress = _Init_idx == 0 || *_Loop_iter_sav != _Cur_iter;

    if (0 <= _Node->_Max && _Node->_Max <= _Init_idx) {
        _Matched0 = _Match_pat(_Node->_End_rep->_Next); // reps done, try tail
    } else if (_Init_idx < _Node->_Min) { // try a required rep
        if (!_Progress) {
            _Matched0 = _Match_pat(_Node->_End_rep->_Next); // empty, try tail
        } else { // try another required match
            _Psav->_Loop_idx  = _Init_idx + 1;
            _Psav->_Loop_iter = _STD addressof(_Cur_iter);
            _Matched0         = _Match_pat(_Node->_Next);
        }
    } else if (!_Greedy) { // not greedy, favor minimum number of reps
        _Matched0 = _Match_pat(_Node->_End_rep->_Next);
        if (!_Matched0 && _Progress) { // tail failed, try another rep
            _Tgt_state        = _St;
            _Psav->_Loop_idx  = _Init_idx + 1;
            _Psav->_Loop_iter = _STD addressof(_Cur_iter);
            _Matched0         = _Match_pat(_Node->_Next);
        }
    } else { // greedy, favor maximum number of reps
        if (_Progress) { // try another rep
            _Psav->_Loop_idx  = _Init_idx + 1;
            _Psav->_Loop_iter = _STD addressof(_Cur_iter);
            _Matched0         = _Match_pat(_Node->_Next);
        }

        if ((_Progress || 1 >= _Init_idx) && !_Matched0) { // rep failed, try tail
            _Psav->_Loop_idx  = _Loop_idx_sav;
            _Psav->_Loop_iter = _Loop_iter_sav;
            _Tgt_state        = _St;
            _Matched0         = _Match_pat(_Node->_End_rep->_Next);
        }
    }

    if (!_Matched0) {
        _Tgt_state = _St;
    }

    _Psav->_Loop_idx  = _Loop_idx_sav;
    _Psav->_Loop_iter = _Loop_iter_sav;
    return _Matched0;
}

template <class _BidIt1, class _BidIt2, class _Pr>
_BidIt1 _Cmp_chrange(_BidIt1 _Begin1, _BidIt1 _End1, _BidIt2 _Begin2, _BidIt2 _End2, _Pr _Pred) {
    // compare character ranges
    _BidIt1 _Res = _Begin1;
    while (_Begin1 != _End1 && _Begin2 != _End2) {
        if (!_Pred(*_Begin1++, *_Begin2++)) {
            return _Res;
        }
    }

    return _Begin2 == _End2 ? _Begin1 : _Res;
}

template <class _BidIt1, class _BidIt2, class _RxTraits>
_BidIt1 _Compare(_BidIt1 _Begin1, _BidIt1 _End1, _BidIt2 _Begin2, _BidIt2 _End2, const _RxTraits& _Traits,
    regex_constants::syntax_option_type _Sflags) { // compare character ranges
    _BidIt1 _Res = _End1;
    if (_Sflags & regex_constants::icase) {
        _Res = _Cmp_chrange(_Begin1, _End1, _Begin2, _End2, _Cmp_icase<_RxTraits>{_Traits});
    } else if (_Sflags & regex_constants::collate) {
        _Res = _Cmp_chrange(_Begin1, _End1, _Begin2, _End2, _Cmp_collate<_RxTraits>{_Traits});
    } else {
        _Res = _Cmp_chrange(_Begin1, _End1, _Begin2, _End2, _Cmp_cs<_RxTraits>{});
    }

    return _Res;
}

template <class _Elem>
bool _Lookup_range(unsigned int _Ch, const _Buf<_Elem>* _Bufptr) { // check whether _Ch is in _Buf
    using _Uelem = make_unsigned_t<_Elem>;
    for (unsigned int _Ix = 0; _Ix < _Bufptr->_Size(); _Ix += 2) { // check current position
        if (static_cast<_Uelem>(_Bufptr->_At(_Ix)) <= _Ch && _Ch <= static_cast<_Uelem>(_Bufptr->_At(_Ix + 1))) {
            return true;
        }
    }

    return false;
}

template <class _Elem, class _RxTraits>
bool _Lookup_equiv(typename _RxTraits::_Uelem _Ch, const _Sequence<_Elem>* _Eq, const _RxTraits& _Traits) {
    // check whether _Ch is in _Eq
    typename _RxTraits::string_type _Str0;
    typename _RxTraits::string_type _Str1;
    _Str1.push_back(static_cast<_Elem>(_Ch));
    _Str1 = _Traits.transform_primary(_Str1.begin(), _Str1.end());
    while (_Eq) { // look for sequence of elements that are the right size
        for (unsigned int _Ix = 0; _Ix < _Eq->_Data._Size(); _Ix += _Eq->_Sz) { // look for _Ch
            _Str0.assign(_Eq->_Data._Str() + _Ix, _Eq->_Sz);
            _Str0 = _Traits.transform_primary(_Str0.begin(), _Str0.end());
            if (_Str0 == _Str1) {
                return true;
            }
        }
        _Eq = _Eq->_Next;
    }
    return false;
}

template <class _BidIt, class _Elem>
_BidIt _Lookup_coll(_BidIt _First, _BidIt _Last, const _Sequence<_Elem>* _Eq) {
    // look for collation element [_First, _Last) in _Eq
    while (_Eq) { // look for sequence of elements that are the right size
        for (unsigned int _Ix = 0; _Ix < _Eq->_Data._Size(); _Ix += _Eq->_Sz) { // look for character range
            _BidIt _Res = _First;
            for (size_t _Jx = 0; _Jx < _Eq->_Sz; ++_Jx) { // check current character
                if (*_Res++ != *(_Eq->_Data._Str() + _Ix + _Jx)) {
                    break;
                }
            }
            if (_Res == _Last) {
                return _Last;
            }
        }
        _Eq = _Eq->_Next;
    }
    return _First;
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Do_class(_Node_base* _Nx) { // apply bracket expression
    bool _Found;
    auto _Ch = static_cast<typename _RxTraits::_Uelem>(*_Tgt_state._Cur);
    if (_Sflags & regex_constants::icase) {
        _Ch = static_cast<typename _RxTraits::_Uelem>(_Traits.translate_nocase(static_cast<_Elem>(_Ch)));
    }

    _It _Res0 = _Tgt_state._Cur;
    ++_Res0;
    _It _Resx;
    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Nx);
    if (_Node->_Coll
        && (_Resx = _Lookup_coll(_Tgt_state._Cur, _End, _Node->_Coll))
               != _Tgt_state._Cur) { // check for collation element
        _Res0  = _Resx;
        _Found = true;
    } else if (_Node->_Ranges
               && (_Lookup_range(static_cast<typename _RxTraits::_Uelem>(
                                     _Sflags & regex_constants::collate ? _Traits.translate(static_cast<_Elem>(_Ch))
                                                                        : static_cast<_Elem>(_Ch)),
                   _Node->_Ranges))) {
        _Found = true;
    } else if (_Ch < _Bmp_max) {
        _Found = _Node->_Small && _Node->_Small->_Find(_Ch);
    } else if (_Node->_Large
               && _STD find(_Node->_Large->_Str(), _Node->_Large->_Str() + _Node->_Large->_Size(), _Ch)
                      != _Node->_Large->_Str() + _Node->_Large->_Size()) {
        _Found = true;
    } else if (_Node->_Classes != 0 && _Traits.isctype(static_cast<_Elem>(_Ch), _Node->_Classes)) {
        _Found = true;
    } else if (_Node->_Equiv && _Lookup_equiv(_Ch, _Node->_Equiv, _Traits)) {
        _Found = true;
    } else {
        _Found = false;
    }

    const bool _Negated = (_Node->_Flags & _Fl_negate) != 0;

    if (_Found == _Negated) {
        return false;
    } else { // record result
        _Tgt_state._Cur = _Res0;
        return true;
    }
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Better_match() { // check for better match under UNIX rules
    for (unsigned int _Ix = 0; _Ix < _Get_ncap(); ++_Ix) { // check each capture group
        if (_Res._Grp_valid[_Ix] && _Tgt_state._Grp_valid[_Ix]) {
            if (_Res._Grps[_Ix]._Begin != _Tgt_state._Grps[_Ix]._Begin) {
                return _STD distance(_Begin, _Res._Grps[_Ix]._Begin)
                     < _STD distance(_Begin, _Tgt_state._Grps[_Ix]._Begin);
            }

            if (_Res._Grps[_Ix]._End != _Tgt_state._Grps[_Ix]._End) {
                return _STD distance(_Begin, _Res._Grps[_Ix]._End) < _STD distance(_Begin, _Tgt_state._Grps[_Ix]._End);
            }
        }
    }
    return false;
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Is_wbound() const {
    if ((_Mflags & regex_constants::match_prev_avail)
        || _Tgt_state._Cur != _Begin) { // if --_Cur is valid, check for preceding word character
        if (_Tgt_state._Cur == _End) {
            return (_Mflags & regex_constants::match_not_eow) == 0 && _Is_word(*_Prev_iter(_Tgt_state._Cur));
        } else {
            return _Is_word(*_Prev_iter(_Tgt_state._Cur)) != _Is_word(*_Tgt_state._Cur);
        }
    } else { // --_Cur is not valid
        if (_Tgt_state._Cur == _End) {
            return (_Mflags & (regex_constants::match_not_bow | regex_constants::match_not_eow)) == 0;
        } else {
            return (_Mflags & regex_constants::match_not_bow) == 0 && _Is_word(*_Tgt_state._Cur);
        }
    }
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
unsigned int _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Get_ncap() const {
    return static_cast<unsigned int>(_Ncap);
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
bool _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Match_pat(_Node_base* _Nx) { // check for match
    if (0 < _Max_stack_count && --_Max_stack_count <= 0) {
        _Xregex_error(regex_constants::error_stack);
    }

    if (0 < _Max_complexity_count && --_Max_complexity_count <= 0) {
        _Xregex_error(regex_constants::error_complexity);
    }

    bool _Failed = false;
    while (_Nx) { // match current node
        switch (_Nx->_Kind) { // handle current node's type
        case _N_nop:
            break;

        case _N_bol:
            if ((_Mflags & regex_constants::match_prev_avail)
                || _Tgt_state._Cur != _Begin) { // if --_Cur is valid, check for preceding newline
                _Failed = *_Prev_iter(_Tgt_state._Cur) != _Meta_nl;
            } else {
                _Failed = (_Mflags & regex_constants::match_not_bol) != 0;
            }

            break;

        case _N_eol:
            if (_Tgt_state._Cur == _End) {
                _Failed = (_Mflags & regex_constants::match_not_eol) != 0;
            } else {
                _Failed = *_Tgt_state._Cur != _Meta_nl;
            }

            break;

        case _N_wbound:
            _Failed = _Is_wbound() == ((_Nx->_Flags & _Fl_negate) != 0);
            break;

        case _N_dot:
            if (_Tgt_state._Cur == _End || *_Tgt_state._Cur == _Meta_nl || *_Tgt_state._Cur == _Meta_cr) {
                _Failed = true;
            } else {
                ++_Tgt_state._Cur;
            }

            break;

        case _N_str:
            { // check for string match
                _Node_str<_Elem>* _Node = static_cast<_Node_str<_Elem>*>(_Nx);
                _It _Res0;
                if ((_Res0 = _Compare(_Tgt_state._Cur, _End, _Node->_Data._Str(),
                         _Node->_Data._Str() + _Node->_Data._Size(), _Traits, _Sflags))
                    != _Tgt_state._Cur) {
                    _Tgt_state._Cur = _Res0;
                } else {
                    _Failed = true;
                }

                break;
            }

        case _N_class:
            { // check for bracket expression match
                _Failed = _Tgt_state._Cur == _End || !_Do_class(_Nx);
                break;
            }

        case _N_group:
            break;

        case _N_end_group:
            break;

        case _N_neg_assert:
        case _N_assert:
            { // check assert
                _It _Ch              = _Tgt_state._Cur;
                bool _Neg            = _Nx->_Kind == _N_neg_assert;
                _Bt_state_t<_It> _St = _Tgt_state;
                if (_Match_pat(static_cast<_Node_assert*>(_Nx)->_Child) == _Neg) {
                    // restore initial state and indicate failure
                    _Tgt_state = _St;
                    _Failed    = true;
                } else {
                    _Tgt_state._Cur = _Ch;
                }

                break;
            }

        case _N_end_assert:
            _Nx = nullptr;
            break;

        case _N_capture:
            { // record current position
                _Node_capture* _Node                 = static_cast<_Node_capture*>(_Nx);
                _Tgt_state._Grps[_Node->_Idx]._Begin = _Tgt_state._Cur;
                for (size_t _Idx = _Tgt_state._Grp_valid.size(); _Node->_Idx < _Idx;) {
                    _Tgt_state._Grp_valid[--_Idx] = false;
                }

                break;
            }

        case _N_end_capture:
            { // record successful capture
                _Node_end_group* _Node = static_cast<_Node_end_group*>(_Nx);
                _Node_capture* _Node0  = static_cast<_Node_capture*>(_Node->_Back);
                if (_Cap || _Node0->_Idx != 0) { // update capture data
                    _Tgt_state._Grp_valid[_Node0->_Idx] = true;
                    _Tgt_state._Grps[_Node0->_Idx]._End = _Tgt_state._Cur;
                }
                break;
            }

        case _N_back:
            { // check back reference
                _Node_back* _Node = static_cast<_Node_back*>(_Nx);
                if (_Tgt_state._Grp_valid[_Node->_Idx]) { // check for match
                    _It _Res0 = _Tgt_state._Cur;
                    _It _Bx   = _Tgt_state._Grps[_Node->_Idx]._Begin;
                    _It _Ex   = _Tgt_state._Grps[_Node->_Idx]._End;
                    if (_Bx != _Ex // _Bx == _Ex for zero-length match
                        && (_Res0 = _Compare(_Tgt_state._Cur, _End, _Bx, _Ex, _Traits, _Sflags)) == _Tgt_state._Cur) {
                        _Failed = true;
                    } else {
                        _Tgt_state._Cur = _Res0;
                    }
                }
                break;
            }

        case _N_if:
            if (!_Do_if(static_cast<_Node_if*>(_Nx))) {
                _Failed = true;
            }

            _Nx = nullptr;
            break;

        case _N_endif:
            break;

        case _N_rep:
            if (!_Do_rep(static_cast<_Node_rep*>(_Nx), (_Nx->_Flags & _Fl_greedy) != 0, 0)) {
                _Failed = true;
            }

            _Nx = nullptr;
            break;

        case _N_end_rep:
            {
                _Node_rep* _Nr      = static_cast<_Node_end_rep*>(_Nx)->_Begin_rep;
                _Loop_vals_t* _Psav = &_Loop_vals[_Nr->_Loop_number];

                if (_Nr->_Simple_loop == 0 && !_Do_rep(_Nr, (_Nr->_Flags & _Fl_greedy) != 0, _Psav->_Loop_idx)) {
                    _Failed = true; // recurse only if loop contains if/do
                }

                _Nx = nullptr;
                break;
            }

        case _N_begin:
            break;

        case _N_end:
            if (((_Mflags & (regex_constants::match_not_null | regex_constants::_Match_not_null))
                    && _Begin == _Tgt_state._Cur)
                || (_Full && _Tgt_state._Cur != _End)) {
                _Failed = true;
            } else if (!_Matched || _Better_match()) { // record successful match
                _Res     = _Tgt_state;
                _Matched = true;
            }
            _Nx = nullptr;
            break;

        case _N_none:
        default:
            _Xregex_error(regex_constants::error_parse);
        }

        if (_Failed) {
            _Nx = nullptr;
        } else if (_Nx) {
            _Nx = _Nx->_Next;
        }
    }

    if (0 < _Max_stack_count) {
        ++_Max_stack_count;
    }

    return !_Failed;
}

template <class _BidIt, class _Elem, class _RxTraits, class _It>
_BidIt _Matcher<_BidIt, _Elem, _RxTraits, _It>::_Skip(_BidIt _First_arg, _BidIt _Last, _Node_base* _Node_arg) {
    // skip until possible match
    // assumes --_First_arg is valid
    _Node_base* _Nx = _Node_arg ? _Node_arg : _Rep;

    while (_First_arg != _Last && _Nx) { // check current node
        switch (_Nx->_Kind) { // handle current node's type
        case _N_nop:
            break;

        case _N_bol:
            { // check for embedded newline
              // return iterator to character just after the newline; for input like "\nabc"
              // matching "^abc", _First_arg could be pointing at 'a', so we need to check
              // --_First_arg for '\n'
                if (*_Prev_iter(_First_arg) != _Meta_nl) {
                    _First_arg = _STD find(_First_arg, _Last, _Meta_nl);
                    if (_First_arg != _Last) {
                        ++_First_arg;
                    }
                }

                return _First_arg;
            }

        case _N_eol:
            return _STD find(_First_arg, _Last, _Meta_nl);

        case _N_str:
            { // check for string match
                _Node_str<_Elem>* _Node = static_cast<_Node_str<_Elem>*>(_Nx);
                for (; _First_arg != _Last; ++_First_arg) { // look for starting match
                    _BidIt _Next = _First_arg;
                    if (_Compare(_First_arg, ++_Next, _Node->_Data._Str(), _Node->_Data._Str() + 1, _Traits, _Sflags)
                        != _First_arg) {
                        break;
                    }
                }
                return _First_arg;
            }

        case _N_class:
            { // check for string match
                for (; _First_arg != _Last; ++_First_arg) { // look for starting match
                    using _Uelem = typename _RxTraits::_Uelem;
                    bool _Found;
                    auto _Ch                             = static_cast<_Uelem>(*_First_arg);
                    _Node_class<_Elem, _RxTraits>* _Node = static_cast<_Node_class<_Elem, _RxTraits>*>(_Nx);
                    _It _Next                            = _First_arg;
                    ++_Next;

                    if (_Sflags & regex_constants::icase) {
                        _Ch = static_cast<_Uelem>(_Traits.translate_nocase(static_cast<_Elem>(_Ch)));
                    }

                    if (_Node->_Coll && _Lookup_coll(_First_arg, _Next, _Node->_Coll) != _First_arg) {
                        _Found = true;
                    } else if (_Node->_Ranges
                               && (_Lookup_range(static_cast<_Uelem>(_Sflags & regex_constants::collate
                                                                         ? _Traits.translate(static_cast<_Elem>(_Ch))
                                                                         : static_cast<_Elem>(_Ch)),
                                   _Node->_Ranges))) {
                        _Found = true;
                    } else if (_Ch < _Bmp_max) {
                        _Found = _Node->_Small && _Node->_Small->_Find(_Ch);
                    } else if (_Node->_Large
                               && _STD find(_Node->_Large->_Str(), _Node->_Large->_Str() + _Node->_Large->_Size(), _Ch)
                                      != _Node->_Large->_Str() + _Node->_Large->_Size()) {
                        _Found = true;
                    } else if (_Node->_Classes && _Traits.isctype(static_cast<_Elem>(_Ch), _Node->_Classes)) {
                        _Found = true;
                    } else if (_Node->_Equiv && _Lookup_equiv(_Ch, _Node->_Equiv, _Traits)) {
                        _Found = true;
                    } else {
                        _Found = false;
                    }

                    const bool _Negated = (_Node->_Flags & _Fl_negate) != 0;

                    if (_Found != _Negated) {
                        return _First_arg;
                    }
                }
            }
            return _First_arg;

        case _N_group:
            break;

        case _N_end_group:
            break;

        case _N_end_assert:
            _Nx = nullptr;
            break;

        case _N_capture:
            break;

        case _N_end_capture:
            break;

        case _N_if:
            { // check for soonest string match
                _Node_if* _Node = static_cast<_Node_if*>(_Nx);

                for (; _First_arg != _Last && _Node; _Node = _Node->_Child) {
                    _Last = _Skip(_First_arg, _Last, _Node->_Next);
                }

                return _Last;
            }

        case _N_begin:
            break;

        case _N_end:
            _Nx = nullptr;
            break;

        case _N_none:
        case _N_wbound:
        case _N_dot:
        case _N_assert:
        case _N_neg_assert:
        case _N_back:
        case _N_endif:
        case _N_rep:
        case _N_end_rep:
        default:
            return _First_arg;
        }
        if (_Nx) {
            _Nx = _Nx->_Next;
        }
    }
    return _First_arg;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Error(regex_constants::error_type _Code) { // handle error
    _Xregex_error(_Code);
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_Is_esc() const { // assumes _Pat != _End
    _FwdIt _Ch0 = _Pat;
    return ++_Ch0 != _End
        && ((!(_L_flags & _L_nex_grp) && (*_Ch0 == _Meta_lpar || *_Ch0 == _Meta_rpar))
            || (!(_L_flags & _L_nex_rep) && (*_Ch0 == _Meta_lbr || *_Ch0 == _Meta_rbr)));
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Trans() { // map character to meta-character
    static constexpr char _Meta_map[] = {_Meta_lpar, _Meta_rpar, _Meta_dlr, _Meta_caret, _Meta_dot, _Meta_star,
        _Meta_plus, _Meta_query, _Meta_lsq, _Meta_rsq, _Meta_bar, _Meta_esc, _Meta_dash, _Meta_lbr, _Meta_rbr,
        _Meta_comma, _Meta_colon, _Meta_equal, _Meta_exc, _Meta_nl, _Meta_cr, _Meta_bsp, 0}; // array of meta chars

    if (_Pat == _End) {
        _Mchar = _Meta_eos;
        _Char  = static_cast<_Elem>(_Meta_eos);
    } else { // map current character
        _Char  = *_Pat;
        _Mchar = _CSTD strchr(_Meta_map, _Char) ? static_cast<_Meta_type>(_Char) : _Meta_chr;
    }
    switch (_Char) { // handle special cases
    case _Meta_esc:
        if (_Is_esc()) { // replace escape sequence
            _FwdIt _Ch0 = _Pat;
            _Mchar      = static_cast<_Meta_type>(_Char = *++_Ch0);
        }
        break;

    case _Meta_nl:
        if ((_L_flags & _L_alt_nl) && _Disj_count == 0) {
            _Mchar = _Meta_bar;
        }

        break;

    case _Meta_lpar:
    case _Meta_rpar:
        if (!(_L_flags & _L_nex_grp)) {
            _Mchar = _Meta_chr;
        }

        break;

    case _Meta_lbr:
    case _Meta_rbr:
        if (!(_L_flags & _L_nex_rep)) {
            _Mchar = _Meta_chr;
        }

        break;

    case _Meta_star:
        if ((_L_flags & _L_star_beg) && _Nfa._Beg_expr()) {
            _Mchar = _Meta_chr;
        }

        break;

    case _Meta_caret:
        if ((_L_flags & _L_anch_rstr) && !_Nfa._Beg_expr()) {
            _Mchar = _Meta_chr;
        }

        break;

    case _Meta_dlr:
        { // check if $ is special
            _FwdIt _Ch0 = _Pat;
            if ((_L_flags & _L_anch_rstr) && ++_Ch0 != _End && *_Ch0 != _Meta_nl) {
                _Mchar = _Meta_chr;
            }

            break;
        }

    case _Meta_plus:
    case _Meta_query:
        if (!(_L_flags & _L_ext_rep)) {
            _Mchar = _Meta_chr;
        }

        break;

    case _Meta_bar:
        if (!(_L_flags & _L_alt_pipe)) {
            _Mchar = _Meta_chr;
        }

        break;
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Next() { // advance to next input character
    if (_Pat != _End) { // advance
        if (*_Pat == _Meta_esc && _Is_esc()) {
            ++_Pat;
        }

        ++_Pat;
    }
    _Trans();
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Expect(_Meta_type _St, regex_constants::error_type _Code) {
    // check whether current meta-character is _St
    if (_Mchar != _St) {
        _Error(_Code);
    }

    _Next();
}

template <class _FwdIt, class _Elem, class _RxTraits>
int _Parser<_FwdIt, _Elem, _RxTraits>::_Do_digits(int _Base, int _Count) { // translate digits to numeric value
    int _Chv;
    _Val = 0;
    while (_Count != 0 && (_Chv = _Traits.value(_Char, _Base)) != -1) { // append next digit
        --_Count;
        _Val *= _Base;
        _Val += _Chv;
        _Next();
    }
    return _Count;
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_DecimalDigits() { // check for decimal value
    return _Do_digits(10, INT_MAX) != INT_MAX;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_HexDigits(int _Count) { // check for _Count hex digits
    if (_Do_digits(16, _Count) != 0) {
        _Error(regex_constants::error_escape);
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_OctalDigits() { // check for up to 3 octal digits
    return _Do_digits(8, 3) != 3;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Do_ex_class(
    _Meta_type _End_arg) { // handle delimited expressions within bracket expression
    regex_constants::error_type _Errtype = (_End_arg == _Meta_colon   ? regex_constants::error_ctype
                                            : _End_arg == _Meta_equal ? regex_constants::error_collate
                                            : _End_arg == _Meta_dot   ? regex_constants::error_collate
                                                                      : regex_constants::error_syntax);
    _FwdIt _Beg                          = _Pat;
    _Iter_diff_t<_FwdIt> _Diff           = 0;

    while (_Mchar != _Meta_colon && _Mchar != _Meta_equal && _Mchar != _Meta_dot && _Mchar != _Meta_eos) {
        // advance to end delimiter
        _Next();
        ++_Diff;
    }
    if (_Mchar != _End_arg) {
        _Error(_Errtype);
    } else if (_End_arg == _Meta_colon) { // handle named character class
        typename _RxTraits::char_class_type _Cls =
            _Traits.lookup_classname(_Beg, _Pat, (_Flags & regex_constants::icase) != 0);
        if (!_Cls) {
            _Error(regex_constants::error_ctype);
        }

        _Nfa._Add_named_class(_Cls);
    } else if (_End_arg == _Meta_equal) { // process =
        if (_Beg == _Pat) {
            _Error(regex_constants::error_collate);
        } else {
            _Nfa._Add_equiv(_Beg, _Pat, _Diff);
        }
    } else if (_End_arg == _Meta_dot) { // process .
        if (_Beg == _Pat) {
            _Error(regex_constants::error_collate);
        } else {
            _Nfa._Add_coll(_Beg, _Pat, _Diff);
        }
    }
    _Next();
    _Expect(_Meta_rsq, _Errtype);
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_CharacterClassEscape(bool _Addit) { // check for character class escape
    typename _RxTraits::char_class_type _Cls;
    _FwdIt _Ch0 = _Pat;
    if (_Ch0 == _End || (_Cls = _Traits.lookup_classname(_Pat, ++_Ch0, (_Flags & regex_constants::icase) != 0)) == 0) {
        return false;
    }

    if (_Addit) {
        _Nfa._Add_class();
    }

    _Nfa._Add_named_class(_Cls, _Traits.isctype(_Char, _RxTraits::_Ch_upper));
    _Next();
    return true;
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Prs_ret _Parser<_FwdIt, _Elem, _RxTraits>::_ClassEscape(bool _Addit) { // check for class escape
    if ((_L_flags & _L_esc_bsl) && _Char == _Esc_bsl) { // handle escape backslash if allowed
        _Val = _Esc_bsl;
        _Next();
        return _Prs_chr;
    } else if ((_L_flags & _L_esc_wsd) && _CharacterClassEscape(_Addit)) {
        return _Prs_set;
    } else if (_DecimalDigits()) { // check for invalid value
        if (_Val != 0) {
            _Error(regex_constants::error_escape);
        }

        return _Prs_chr;
    }
    return _CharacterEscape() ? _Prs_chr : _Prs_none;
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Prs_ret _Parser<_FwdIt, _Elem, _RxTraits>::_ClassAtom() { // check for class atom
    if (_Mchar == _Meta_esc) { // check for valid escape sequence
        _Next();
        if (_L_flags & _L_grp_esc) {
            return _ClassEscape(false);
        } else if ((_L_flags & _L_esc_ffn && _Do_ffn(_Char))
                   || (_L_flags & _L_esc_ffnx && _Do_ffnx(_Char))) { // advance to next character
            _Next();
            return _Prs_chr;
        }
        _Val = _Meta_esc;
        return _Prs_chr;
    } else if (_Mchar == _Meta_lsq) { // check for valid delimited expression
        _Next();
        if (_Mchar == _Meta_colon || _Mchar == _Meta_equal || _Mchar == _Meta_dot) { // handle delimited expression
            _Meta_type _St = _Mchar;
            _Next();
            _Do_ex_class(_St);
            return _Prs_set;
        } else { // handle ordinary [
            _Val = _Meta_lsq;
            return _Prs_chr;
        }
    } else if (_Mchar == _Meta_rsq || _Mchar == _Meta_eos) {
        return _Prs_none;
    } else { // handle ordinary character
        _Val = _Char;
        _Next();
        return _Prs_chr;
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_ClassRanges() { // check for valid class ranges
    _Prs_ret _Ret;

    for (;;) { // process characters through end of bracket expression
        if ((_Ret = _ClassAtom()) == _Prs_none) {
            return;
        }

        if (_Ret != _Prs_set) {
            if (_Val == 0 && !(_L_flags & _L_bzr_chr)) {
                _Error(regex_constants::error_escape);
            }

            if (_Mchar == _Meta_dash) { // check for valid range
                _Next();
                _Elem _Chr1 = static_cast<_Elem>(_Val);
                if ((_Ret = _ClassAtom()) == _Prs_none) { // treat - as ordinary character
                    _Nfa._Add_char_to_class(static_cast<_Elem>(_Val));
                    _Nfa._Add_char_to_class(_Meta_dash);
                    return;
                }

                if (_Ret == _Prs_set) {
                    _Error(regex_constants::error_range); // set follows dash
                }

                if (_Flags & regex_constants::collate) { // translate ends of range
                    _Val  = _Traits.translate(static_cast<_Elem>(_Val));
                    _Chr1 = _Traits.translate(_Chr1);
                }

                if (static_cast<typename _RxTraits::_Uelem>(_Val) < static_cast<typename _RxTraits::_Uelem>(_Chr1)) {
                    _Error(regex_constants::error_range);
                }

                _Nfa._Add_range(_Chr1, static_cast<_Elem>(_Val));
            } else {
                _Nfa._Add_char_to_class(static_cast<_Elem>(_Val));
            }
        }
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_CharacterClass() { // add bracket expression
    _Nfa._Add_class();
    if (_Mchar == _Meta_caret) { // negate bracket expression
        _Nfa._Negate();
        _Next();
    }
    if ((_L_flags & _L_brk_rstr) && _Mchar == _Meta_rsq) { // insert initial ] when not special
        _Nfa._Add_char_to_class(_Meta_rsq);
        _Next();
    }
    _ClassRanges();
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Do_capture_group() { // add capture group
    // if (_MAX_GRP <= ++_Grp_idx)
    //     _Error(regex_constants::error_complexity);
    _Node_base* _Pos1 = _Nfa._Begin_capture_group(++_Grp_idx);
    _Disjunction();
    _Nfa._End_group(_Pos1);
    _Finished_grps.resize(_Grp_idx + 1);
    _Finished_grps[static_cast<_Node_capture*>(_Pos1)->_Idx] = true;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Do_noncapture_group() { // add non-capture group
    _Node_base* _Pos1 = _Nfa._Begin_group();
    _Disjunction();
    _Nfa._End_group(_Pos1);
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Do_assert_group(bool _Neg) { // add assert group
    _Node_base* _Pos1 = _Nfa._Begin_assert_group(_Neg);
    _Disjunction();
    _Nfa._End_assert_group(_Pos1);
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_Wrapped_disjunction() { // add disjunction inside group
    ++_Disj_count;
    if (!(_L_flags & _L_empty_grp) && _Mchar == _Meta_rpar) {
        _Error(regex_constants::error_paren);
    } else if ((_L_flags & _L_nc_grp) && _Mchar == _Meta_query) { // check for valid ECMAScript (?x ... ) group
        _Next();
        _Meta_type _Ch = _Mchar;
        _Next();
        if (_Ch == _Meta_colon) {
            _Do_noncapture_group();
        } else if (_Ch == _Meta_exc) { // process assert group, negating
            _Do_assert_group(true);
            --_Disj_count;
            return false;
        } else if (_Ch == _Meta_equal) { // process assert group
            _Do_assert_group(false);
            --_Disj_count;
            return false;
        } else {
            _Error(regex_constants::error_syntax);
        }
    } else if (_Flags & regex_constants::nosubs) {
        _Do_noncapture_group();
    } else {
        _Do_capture_group();
    }

    --_Disj_count;
    return true;
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_IsIdentityEscape() const { // check for valid identity escape
    if (_L_flags & _L_ident_ECMA) {
        // ECMAScript identity escape characters
        switch (_Char) {
        case 'c': // ASCII control character
        case 'd': // digit
        case 'D':
        case 's': // whitespace
        case 'S':
        case 'w': // word character
        case 'W':
            return false;
        default:
            return true;
        }
    }

    switch (_Char) {
    case _Meta_dot:
    case _Meta_lsq:
    case _Meta_esc:
    case _Meta_star:
    case _Meta_bar:
    case _Meta_caret:
    case _Meta_dlr:
        // BRE, ERE, awk identity escape characters
        return true;
    case _Meta_lpar:
    case _Meta_rpar:
    case _Meta_plus:
    case _Meta_query:
    case _Meta_lbr:
    case _Meta_rbr:
        // additional ERE identity escape characters
        return (_L_flags & _L_ident_ERE) != 0;
    case '"':
    case '/':
        // additional awk identity escape characters
        return (_L_flags & _L_ident_awk) != 0;
    default:
        return false;
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_IdentityEscape() { // check whether an escape is valid, and process it if so
    if (_IsIdentityEscape()) {
        _Val = _Char;
        _Next();
        return true;
    } else {
        return false;
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_Do_ffn(_Elem _Ch) { // check for limited file format escape characters
    if (_Ch == _Esc_ctrl_f) {
        _Val = '\f';
    } else if (_Ch == _Esc_ctrl_n) {
        _Val = '\n';
    } else if (_Ch == _Esc_ctrl_r) {
        _Val = '\r';
    } else if (_Ch == _Esc_ctrl_t) {
        _Val = '\t';
    } else if (_Ch == _Esc_ctrl_v) {
        _Val = '\v';
    } else {
        return false;
    }

    return true;
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_Do_ffnx(_Elem _Ch) { // check for the remaining file format escape characters
    if (_Ch == _Esc_ctrl_a) {
        _Val = '\a';
    } else if (_Ch == _Esc_ctrl_b) {
        _Val = '\b';
    } else {
        return false;
    }

    return true;
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_CharacterEscape() { // check for valid character escape
    if (_Mchar == _Meta_eos) {
        _Error(regex_constants::error_escape);
    }

    if ((_L_flags & _L_esc_ffn && _Do_ffn(_Char)) || (_L_flags & _L_esc_ffnx && _Do_ffnx(_Char))) {
        _Next();
    } else if (_Char == _Esc_ctrl && (_L_flags & _L_esc_ctrl)) { // handle control escape sequence
        _Next();
        if (!_Traits.isctype(_Char, _RxTraits::_Ch_alpha)) {
            _Error(regex_constants::error_escape);
        }

        _Val = static_cast<char>(_Char % 32);
        _Next();
    } else if (_Char == _Esc_hex && (_L_flags & _L_esc_hex)) { // handle hexadecimal escape sequence
        _Next();
        _HexDigits(2);
    } else if (_Char == _Esc_uni && (_L_flags & _L_esc_uni)) { // handle Unicode escape sequence
        _Next();
        _HexDigits(4);
    } else if ((_L_flags & _L_esc_oct) && _OctalDigits()) { // handle octal escape sequence
        if (_Val == 0) {
            _Error(regex_constants::error_escape);
        }
    } else {
        return _IdentityEscape();
    }

    if ((numeric_limits<typename _RxTraits::_Uelem>::max)() < static_cast<unsigned int>(_Val)) {
        _Error(regex_constants::error_escape);
    }

    _Val = static_cast<_Elem>(_Val);
    return true;
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_AtomEscape() { // check for valid atom escape
    if ((_L_flags & _L_bckr) && _DecimalDigits()) { // check for valid back reference
        if (_Val == 0) { // handle \0
            if (!(_L_flags & _L_bzr_chr)) {
                _Error(regex_constants::error_escape);
            } else {
                _Nfa._Add_char(static_cast<_Elem>(_Val));
            }
        } else if (((_L_flags & _L_lim_bckr) && _BRE_MAX_GRP < static_cast<size_t>(_Val))
                   || _Grp_idx < static_cast<size_t>(_Val) || _Finished_grps.size() <= static_cast<size_t>(_Val)
                   || !_Finished_grps[static_cast<size_t>(_Val)]) {
            _Error(regex_constants::error_backref);
        } else {
            _Nfa._Add_backreference(static_cast<size_t>(_Val));
        }
    } else if (_CharacterEscape()) {
        _Nfa._Add_char(static_cast<_Elem>(_Val));
    } else if (!(_L_flags & _L_esc_wsd) || !_CharacterClassEscape(true)) {
        _Error(regex_constants::error_escape);
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Quantifier() { // check for quantifier following atom
    int _Min = 0;
    int _Max = -1;
    if (_Mchar != _Meta_star) {
        if (_Mchar == _Meta_plus) {
            _Min = 1;
        } else if (_Mchar == _Meta_query) {
            _Max = 1;
        } else if (_Mchar == _Meta_lbr) { // check for valid bracketed value
            _Next();
            if (!_DecimalDigits()) {
                _Error(regex_constants::error_badbrace);
            }

            _Min = _Val;
            if (_Mchar != _Meta_comma) {
                _Max = _Min;
            } else { // check for decimal constant following comma
                _Next();
                if (_Mchar != _Meta_rbr) {
                    if (!_DecimalDigits()) {
                        _Error(regex_constants::error_badbrace);
                    }

                    _Max = _Val;
                }
            }

            if (_Mchar != _Meta_rbr || (_Max != -1 && _Max < _Min)) {
                _Error(regex_constants::error_badbrace);
            }
        } else {
            return;
        }
    }

    _Nfa._Mark_final();
    _Next();
    const bool _Greedy = !(_L_flags & _L_ngr_rep) || _Mchar != _Meta_query;
    if (!_Greedy) { // add non-greedy repeat node
        _Next();
    }

    _Nfa._Add_rep(_Min, _Max, _Greedy);
}

template <class _FwdIt, class _Elem, class _RxTraits>
bool _Parser<_FwdIt, _Elem, _RxTraits>::_Alternative() { // check for valid alternative
    bool _Found = false;
    for (;;) { // concatenate valid elements
        bool _Quant = true;
        if (_Mchar == _Meta_eos || _Mchar == _Meta_bar || (_Mchar == _Meta_rpar && _Disj_count != 0)) {
            return _Found;
        } else if (_Mchar == _Meta_rpar && !(_L_flags & _L_paren_bal)) {
            _Error(regex_constants::error_paren);
        } else if (_Mchar == _Meta_dot) { // add dot node
            _Nfa._Add_dot();
            _Next();
        } else if (_Mchar == _Meta_esc) { // check for valid escape sequence
            _Next();
            if ((_L_flags & _L_asrt_wrd) && _Char == _Esc_word) { // add word assert
                _Nfa._Add_wbound();
                _Next();
                _Quant = false;
            } else if ((_L_flags & _L_asrt_wrd) && _Char == _Esc_not_word) { // add not-word assert
                _Nfa._Add_wbound();
                _Nfa._Negate();
                _Next();
                _Quant = false;
            } else {
                _AtomEscape();
            }
        } else if (_Mchar == _Meta_lsq) { // add bracket expression
            _Next();
            _CharacterClass();
            _Expect(_Meta_rsq, regex_constants::error_brack);
        } else if (_Mchar == _Meta_lpar) { // check for valid group
            _Next();
            _Quant = _Wrapped_disjunction();
            _Expect(_Meta_rpar, regex_constants::error_paren);
        } else if (_Mchar == _Meta_caret) { // add bol node
            _Nfa._Add_bol();
            _Next();
            _Quant = false;
        } else if (_Mchar == _Meta_dlr) { // add eol node
            _Nfa._Add_eol();
            _Next();
            _Quant = false;
        } else if (_Mchar == _Meta_star || _Mchar == _Meta_plus || _Mchar == _Meta_query || _Mchar == _Meta_lbr) {
            _Error(regex_constants::error_badrepeat);
        } else if (_Mchar == _Meta_rbr && !(_L_flags & _L_paren_bal)) {
            _Error(regex_constants::error_brace);
        } else if (_Mchar == _Meta_rsq && !(_L_flags & _L_paren_bal)) {
            _Error(regex_constants::error_brack);
        } else { // add character
            _Nfa._Add_char(_Char);
            _Next();
        }

        if (_Quant) {
            _Quantifier();
        }

        _Found = true;
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
void _Parser<_FwdIt, _Elem, _RxTraits>::_Disjunction() { // check for valid disjunction
    _Node_base* _Pos1 = _Nfa._Getmark();
    if (!_Alternative()) {
        if (_Mchar != _Meta_bar) {
            return; // zero-length alternative not followed by '|'
        }

        // zero-length leading alternative
        _Node_base* _Pos3 = _Nfa._Begin_group();
        _Nfa._End_group(_Pos3);
    }

    _Node_base* _Pos2 = _Nfa._Begin_if(_Pos1);
    while (_Mchar == _Meta_bar) { // append terms as long as we keep finding | characters
        _Next();
        if (!_Alternative()) { // zero-length trailing alternative
            _Node_base* _Pos3 = _Nfa._Begin_group();
            _Nfa._End_group(_Pos3);
        }

        _Nfa._Else_if(_Pos1, _Pos2);
    }
}

inline void _Calculate_loop_simplicity(_Node_base* _Nx, _Node_base* _Ne, _Node_rep* _Outer_rep) {
    // walks regex NFA, calculates values of _Node_rep::_Simple_loop
    for (; _Nx != _Ne && _Nx; _Nx = _Nx->_Next) {
        switch (_Nx->_Kind) {
        case _N_if:
            // _Node_if inside a _Node_rep makes the rep not simple
            if (_Outer_rep) {
                _Outer_rep->_Simple_loop = 0;
            }

            // visit each branch of the if
            for (_Node_if* _Branch = static_cast<_Node_if*>(_Nx)->_Child; _Branch; _Branch = _Branch->_Child) {
                _Calculate_loop_simplicity(_Branch->_Next, _Branch->_Endif, _Outer_rep);
            }

            break;
        case _N_assert:
        case _N_neg_assert:
            // visit the assertion body
            // note _Outer_rep being reset: the assertion regex is completely independent
            _Calculate_loop_simplicity(static_cast<_Node_assert*>(_Nx)->_Child, nullptr, nullptr);
            break;
        case _N_rep:
            // _Node_rep inside another _Node_rep makes both not simple
            if (_Outer_rep) {
                _Outer_rep->_Simple_loop                   = 0;
                static_cast<_Node_rep*>(_Nx)->_Simple_loop = 0;
            } else {
                _Outer_rep = static_cast<_Node_rep*>(_Nx);
            }
            break;
        case _N_end_rep:
            if (_Outer_rep == static_cast<_Node_end_rep*>(_Nx)->_Begin_rep) {
                // if the _Node_rep is still undetermined when we reach its end, it is simple
#pragma warning(push)
#pragma warning(disable : 6011) // Dereferencing NULL pointer
                if (_Outer_rep->_Simple_loop == -1) {
                    _Outer_rep->_Simple_loop = 1;
                }
#pragma warning(pop)
                _Outer_rep = nullptr;
            }
            break;
        case _N_none:
        case _N_nop:
        case _N_bol:
        case _N_eol:
        case _N_wbound:
        case _N_dot:
        case _N_str:
        case _N_class:
        case _N_group:
        case _N_end_group:
        case _N_end_assert:
        case _N_capture:
        case _N_end_capture:
        case _N_back:
        case _N_endif:
        case _N_begin:
        case _N_end:
        default:
            break;
        }
    }
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Root_node* _Parser<_FwdIt, _Elem, _RxTraits>::_Compile() { // compile regular expression
    _Root_node* _Res = nullptr;
    _Tidy_guard<decltype(_Nfa)> _Guard{_STD addressof(_Nfa)};
    _Node_base* _Pos1 = _Nfa._Begin_capture_group(0);
    _Disjunction();
    if (_Pat != _End) {
        _Error(regex_constants::error_syntax);
    }

    _Nfa._End_group(_Pos1);
    _Res         = _Nfa._End_pattern();
    _Res->_Fl    = _Flags;
    _Res->_Marks = _Mark_count();
    _Calculate_loop_simplicity(_Res, nullptr, nullptr);
    _Guard._Target = nullptr;
    return _Res;
}

template <class _FwdIt, class _Elem, class _RxTraits>
_Parser<_FwdIt, _Elem, _RxTraits>::_Parser(
    const _RxTraits& _Tr, _FwdIt _Pfirst, _FwdIt _Plast, regex_constants::syntax_option_type _Fx)
    : _Pat(_Pfirst), _Begin(_Pfirst), _End(_Plast), _Grp_idx(0), _Disj_count(0), _Finished_grps(0), _Nfa(_Tr, _Fx),
      _Traits(_Tr), _Flags(_Fx) {

    constexpr unsigned int _ECMA_flags = _L_ext_rep | _L_alt_pipe | _L_nex_grp | _L_nex_rep | _L_nc_grp | _L_asrt_gen
                                       | _L_asrt_wrd | _L_bckr | _L_ngr_rep | _L_esc_uni | _L_esc_hex | _L_esc_bsl
                                       | _L_esc_ffn | _L_esc_wsd | _L_esc_ctrl | _L_bzr_chr | _L_grp_esc | _L_ident_ECMA
                                       | _L_empty_grp;

    constexpr unsigned int _Basic_flags =
        _L_bckr | _L_lim_bckr | _L_anch_rstr | _L_star_beg | _L_empty_grp | _L_brk_rstr | _L_mtch_long;

    constexpr unsigned int _Grep_flags = _Basic_flags | _L_alt_nl | _L_no_nl;

    constexpr unsigned int _Extended_flags =
        _L_ext_rep | _L_alt_pipe | _L_nex_grp | _L_nex_rep | _L_ident_ERE | _L_paren_bal | _L_brk_rstr | _L_mtch_long;

    constexpr unsigned int _Awk_flags = _Extended_flags | _L_esc_oct | _L_esc_ffn | _L_esc_ffnx | _L_ident_awk;

    constexpr unsigned int _Egrep_flags = _Extended_flags | _L_alt_nl | _L_no_nl;

    const regex_constants::syntax_option_type _Masked = _Flags & regex_constants::_Gmask;

    if (_Masked == regex_constants::ECMAScript || _Masked == 0) {
        _L_flags = _ECMA_flags;
    } else if (_Masked == regex_constants::basic) {
        _L_flags = _Basic_flags;
    } else if (_Masked == regex_constants::extended) {
        _L_flags = _Extended_flags;
    } else if (_Masked == regex_constants::awk) {
        _L_flags = _Awk_flags;
    } else if (_Masked == regex_constants::grep) {
        _L_flags = _Grep_flags;
    } else if (_Masked == regex_constants::egrep) {
        _L_flags = _Egrep_flags;
    } else {
        _L_flags = 0;
    }

    if (_L_flags & _L_mtch_long) {
        _Nfa._Setlong();
    }

    _Trans();
}

#if _HAS_TR1_NAMESPACE
namespace _DEPRECATE_TR1_NAMESPACE tr1 {
    using _STD basic_regex;
    using _STD cmatch;
    using _STD cregex_iterator;
    using _STD cregex_token_iterator;
    using _STD csub_match;
    using _STD match_results;
    using _STD regex;
    using _STD regex_error;
    using _STD regex_iterator;
    using _STD regex_match;
    using _STD regex_replace;
    using _STD regex_search;
    using _STD regex_token_iterator;
    using _STD regex_traits;
    using _STD smatch;
    using _STD sregex_iterator;
    using _STD sregex_token_iterator;
    using _STD ssub_match;
    using _STD sub_match;
    using _STD swap;
    using _STD wcmatch;
    using _STD wcregex_iterator;
    using _STD wcregex_token_iterator;
    using _STD wcsub_match;
    using _STD wregex;
    using _STD wsmatch;
    using _STD wsregex_iterator;
    using _STD wsregex_token_iterator;
    using _STD wssub_match;
    namespace regex_constants {
        using namespace _STD regex_constants;
    }
} // namespace tr1
#endif // _HAS_TR1_NAMESPACE

#if _HAS_CXX17
namespace pmr {
    template <class _BidIt>
    using match_results = _STD match_results<_BidIt, polymorphic_allocator<sub_match<_BidIt>>>;

    using cmatch  = match_results<const char*>;
    using wcmatch = match_results<const wchar_t*>;
    using smatch  = match_results<string::const_iterator>;
    using wsmatch = match_results<wstring::const_iterator>;
} // namespace pmr
#endif // _HAS_CXX17
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _REGEX_