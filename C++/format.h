// format standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// NOTE:
// The contents of this header are derived in part from libfmt under the following license:

// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// --- Optional exception to the license ---
//
// As an exception, if, as a result of your compiling your source code, portions
// of this Software are embedded into a machine-executable object form of such
// source code, you may redistribute such embedded portions in such object form
// without including the above copyright and permission notices.

#pragma once
#ifndef _FORMAT_
#define _FORMAT_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#ifndef __cpp_lib_format
#pragma message("The contents of <format> are available only with C++20 or later.")
#else // ^^^ !defined(__cpp_lib_format) / defined(__cpp_lib_format) vvv

#include <charconv>
#include <concepts>
#include <cstdint>
#include <exception>
#include <iterator>
#include <locale>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <xfilesystem_abi.h>
#include <xutility>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

extern "C" _NODISCARD __std_win_error __stdcall __std_get_cvt(__std_code_page _Codepage, _Cvtvec* _Pcvt) noexcept;

_STD_BEGIN

template <class, class>
class vector;

class format_error : public runtime_error {
    using runtime_error::runtime_error;
};

enum class _Fmt_align : uint8_t { _None, _Left, _Right, _Center };

enum class _Fmt_sign : uint8_t { _None, _Plus, _Minus, _Space };

enum class _Basic_format_arg_type : uint8_t {
    _None,
    _Int_type,
    _UInt_type,
    _Long_long_type,
    _ULong_long_type,
    _Bool_type,
    _Char_type,
    _Float_type,
    _Double_type,
    _Long_double_type,
    _Pointer_type,
    _CString_type,
    _String_type,
    _Custom_type,
};
static_assert(static_cast<int>(_Basic_format_arg_type::_Custom_type) < 16, "must fit in 4-bit bitfield");

_NODISCARD constexpr bool _Is_integral_fmt_type(_Basic_format_arg_type _Ty) {
    return _Ty > _Basic_format_arg_type::_None && _Ty <= _Basic_format_arg_type::_Char_type;
}
_NODISCARD constexpr bool _Is_arithmetic_fmt_type(_Basic_format_arg_type _Ty) {
    return _Ty > _Basic_format_arg_type::_None && _Ty <= _Basic_format_arg_type::_Long_double_type;
}

struct _Auto_id_tag {};

// clang-format off
template <class _Ty, class _CharT>
concept _Parse_arg_id_callbacks = requires(_Ty _At) {
    { _At._On_auto_id() } -> same_as<void>;
    { _At._On_manual_id(size_t{}) } -> same_as<void>;
};

template <class _Ty, class _CharT>
concept _Parse_replacement_field_callbacks = requires(_Ty _At, const _CharT* _First, const _CharT* _Last) {
    { _At._Parse_context };
    { _At._On_text(_First, _Last) } -> same_as<void>;
    { _At._On_replacement_field(size_t{}, static_cast<const _CharT*>(nullptr)) } -> same_as<void>;
    { _At._On_format_specs(size_t{}, _First, _Last) } -> same_as<const _CharT*>;
};

template <class _Ty, class _CharT>
concept _Parse_align_callbacks = requires(_Ty _At, basic_string_view<_CharT> _Sv, _Fmt_align _Aln) {
    { _At._On_fill(_Sv) } -> same_as<void>;
    { _At._On_align(_Aln) } -> same_as<void>;
};

template <class _Ty, class _CharT>
concept _Parse_width_callbacks = requires(_Ty _At) {
    { _At._On_width(int{}) } -> same_as<void>;
};

template <class _Ty, class _CharT>
concept _Parse_precision_callbacks = requires(_Ty _At) {
    { _At._On_precision(int{}) } -> same_as<void>;
};

template <class _Ty, class _CharT>
concept _Width_adapter_callbacks = requires(_Ty _At) {
    { _At._On_dynamic_width(_Auto_id_tag{}) } -> same_as<void>;
    { _At._On_dynamic_width(size_t{}) } -> same_as<void>;
};

template <class _Ty, class _CharT>
concept _Precision_adapter_callbacks = requires(_Ty _At) {
    { _At._On_dynamic_precision(_Auto_id_tag{}) } -> same_as<void>;
    { _At._On_dynamic_precision(size_t{}) } -> same_as<void>;
};

template <class _Ty, class _CharT>
concept _Parse_spec_callbacks = _Parse_align_callbacks<_Ty, _CharT>
                                && _Parse_width_callbacks<_Ty, _CharT>
                                && _Parse_precision_callbacks<_Ty, _CharT>
                                && _Width_adapter_callbacks<_Ty, _CharT>
                                && _Precision_adapter_callbacks<_Ty, _CharT>
                                && requires(_Ty _At, basic_string_view<_CharT> _Sv, _Fmt_align _Aln, _Fmt_sign _Sgn) {
                                    { _At._On_sign(_Sgn) } -> same_as<void>;
                                    { _At._On_hash() } -> same_as<void>;
                                    { _At._On_zero() } -> same_as<void>;
                                    { _At._On_localized() } -> same_as<void>;
                                    { _At._On_type(_CharT{}) } -> same_as<void>;
};
// clang-format on

template <class _Ty, class _CharT>
concept _CharT_or_bool = same_as<_Ty, _CharT> || same_as<_Ty, bool>;

template <class _CharT>
concept _Format_supported_charT = _Is_any_of_v<_CharT, char, wchar_t>;

template <class _Ty, class _Context>
concept _Has_formatter = requires(_Ty& _Val, _Context& _Ctx) {
    _STD declval<typename _Context::template formatter_type<remove_cvref_t<_Ty>>>().format(_Val, _Ctx);
};

template <class _Ty, class _Context>
concept _Has_const_formatter = _Has_formatter<const remove_reference_t<_Ty>, _Context>;

template <class _Ty, class _CharT = char>
struct formatter;

inline void _You_see_this_error_because_arg_id_is_out_of_range() noexcept {}

template <class _CharT>
class basic_format_parse_context {
public:
    using char_type      = _CharT;
    using const_iterator = typename basic_string_view<_CharT>::const_iterator;
    using iterator       = const_iterator;

    constexpr explicit basic_format_parse_context(
        const basic_string_view<_CharT> _Fmt, const size_t _Num_args_ = 0) noexcept
        : _Format_string(_Fmt), _Num_args(_Num_args_) {}

    basic_format_parse_context(const basic_format_parse_context&) = delete;
    basic_format_parse_context& operator=(const basic_format_parse_context&) = delete;

    _NODISCARD constexpr const_iterator begin() const noexcept {
        return _Format_string.begin();
    }
    _NODISCARD constexpr const_iterator end() const noexcept {
        return _Format_string.end();
    }
    _NODISCARD constexpr const _CharT* _Unchecked_begin() const noexcept {
        return _Format_string._Unchecked_begin();
    }
    _NODISCARD constexpr const _CharT* _Unchecked_end() const noexcept {
        return _Format_string._Unchecked_end();
    }

    constexpr void advance_to(const const_iterator _It) {
        _Adl_verify_range(_It, _Format_string.end());
        _Adl_verify_range(_Format_string.begin(), _It);
        const auto _Diff = static_cast<size_t>(_It._Unwrapped() - _Format_string._Unchecked_begin());
        _Format_string.remove_prefix(_Diff);
    }

    // While the standard presents an exposition-only enum value for
    // the indexing mode (manual, automatic, or unknown) we use _Next_arg_id to indicate it.
    // _Next_arg_id > 0 means automatic
    // _Next_arg_id == 0 means unknown
    // _Next_arg_id < 0 means manual
    _NODISCARD constexpr size_t next_arg_id() {
        if (_Next_arg_id < 0) {
            _THROW(format_error("Can not switch from manual to automatic indexing"));
        }

        if (_STD is_constant_evaluated()) {
            if (static_cast<size_t>(_Next_arg_id) >= _Num_args) {
                _You_see_this_error_because_arg_id_is_out_of_range();
            }
        }

        return static_cast<size_t>(_Next_arg_id++);
    }

    constexpr void check_arg_id(const size_t _Id) {
        if (_STD is_constant_evaluated()) {
            if (_Id >= _Num_args) {
                _You_see_this_error_because_arg_id_is_out_of_range();
            }
        }

        if (_Next_arg_id > 0) {
            _THROW(format_error("Can not switch from automatic to manual indexing"));
        }
        _Next_arg_id = -1;
    }

private:
    basic_string_view<_CharT> _Format_string;
    size_t _Num_args;
    // The standard says this is size_t, however we use ptrdiff_t to save some space
    // by not having to store the indexing mode. Above is a more detailed explanation
    // of how this works.
    ptrdiff_t _Next_arg_id = 0;
};

using format_parse_context  = basic_format_parse_context<char>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;

template <class _Context>
class basic_format_arg {
public:
    using _CharType = typename _Context::char_type;

    class handle {
    private:
        const void* _Ptr;
        void(__cdecl* _Format)(basic_format_parse_context<_CharType>& _Parse_ctx, _Context& _Format_ctx, const void*);
        friend basic_format_arg;

    public:
        template <class _Ty>
        explicit handle(_Ty&& _Val) noexcept
            : _Ptr(_STD addressof(_Val)),
              _Format([](basic_format_parse_context<_CharType>& _Parse_ctx, _Context& _Format_ctx, const void* _Ptr) {
                  using _Value_type = remove_cvref_t<_Ty>;
                  typename _Context::template formatter_type<_Value_type> _Formatter;
                  using _Qualified_type =
                      conditional_t<_Has_const_formatter<_Value_type, _Context>, const _Value_type, _Value_type>;
                  _Parse_ctx.advance_to(_Formatter.parse(_Parse_ctx));
                  _Format_ctx.advance_to(_Formatter.format(
                      *const_cast<_Qualified_type*>(static_cast<const _Value_type*>(_Ptr)), _Format_ctx));
              }) {
            static_assert(_Has_const_formatter<_Ty, _Context> || !is_const_v<remove_reference_t<_Ty>>);
        }

        void format(basic_format_parse_context<_CharType>& _Parse_ctx, _Context& _Format_ctx) const {
            _Format(_Parse_ctx, _Format_ctx, _Ptr);
        }
    };

    // TRANSITION, LLVM-49072
    basic_format_arg() noexcept : _Active_state(_Basic_format_arg_type::_None), _No_state() {}

    explicit basic_format_arg(const int _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Int_type), _Int_state(_Val) {}
    explicit basic_format_arg(const unsigned int _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_UInt_type), _UInt_state(_Val) {}
    explicit basic_format_arg(const long long _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Long_long_type), _Long_long_state(_Val) {}
    explicit basic_format_arg(const unsigned long long _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_ULong_long_type), _ULong_long_state(_Val) {}
    explicit basic_format_arg(const bool _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Bool_type), _Bool_state(_Val) {}
    explicit basic_format_arg(const _CharType _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Char_type), _Char_state(_Val) {}
    explicit basic_format_arg(const float _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Float_type), _Float_state(_Val) {}
    explicit basic_format_arg(const double _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Double_type), _Double_state(_Val) {}
    explicit basic_format_arg(const long double _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Long_double_type), _Long_double_state(_Val) {}
    explicit basic_format_arg(const void* _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Pointer_type), _Pointer_state(_Val) {}
    explicit basic_format_arg(const _CharType* _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_CString_type), _CString_state(_Val) {}
    explicit basic_format_arg(const basic_string_view<_CharType> _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_String_type), _String_state(_Val) {}
    explicit basic_format_arg(const handle _Val) noexcept
        : _Active_state(_Basic_format_arg_type::_Custom_type), _Custom_state(_Val) {}

    explicit operator bool() const noexcept {
        return _Active_state != _Basic_format_arg_type::_None;
    }

    _Basic_format_arg_type _Active_state = _Basic_format_arg_type::_None;
    union {
        monostate _No_state = monostate{};
        int _Int_state;
        unsigned int _UInt_state;
        long long _Long_long_state;
        unsigned long long _ULong_long_state;
        bool _Bool_state;
        _CharType _Char_state;
        float _Float_state;
        double _Double_state;
        long double _Long_double_state;
        const void* _Pointer_state;
        const _CharType* _CString_state;
        basic_string_view<_CharType> _String_state;
        handle _Custom_state;
    };
};

template <class _Visitor, class _Context>
decltype(auto) visit_format_arg(_Visitor&& _Vis, basic_format_arg<_Context> _Arg) {
    switch (_Arg._Active_state) {
    case _Basic_format_arg_type::_None:
        return _STD forward<_Visitor>(_Vis)(_Arg._No_state);
    case _Basic_format_arg_type::_Int_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Int_state);
    case _Basic_format_arg_type::_UInt_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._UInt_state);
    case _Basic_format_arg_type::_Long_long_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Long_long_state);
    case _Basic_format_arg_type::_ULong_long_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._ULong_long_state);
    case _Basic_format_arg_type::_Bool_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Bool_state);
    case _Basic_format_arg_type::_Char_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Char_state);
    case _Basic_format_arg_type::_Float_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Float_state);
    case _Basic_format_arg_type::_Double_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Double_state);
    case _Basic_format_arg_type::_Long_double_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Long_double_state);
    case _Basic_format_arg_type::_Pointer_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Pointer_state);
    case _Basic_format_arg_type::_CString_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._CString_state);
    case _Basic_format_arg_type::_String_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._String_state);
    case _Basic_format_arg_type::_Custom_type:
        return _STD forward<_Visitor>(_Vis)(_Arg._Custom_state);
    default:
        _STL_VERIFY(false, "basic_format_arg is in impossible state");
        return _STD forward<_Visitor>(_Vis)(0);
    }
}

// we need to implement this ourselves because from_chars does not work with wide characters and isn't constexpr
template <class _CharT>
_NODISCARD constexpr const _CharT* _Parse_nonnegative_integer(
    const _CharT* _First, const _CharT* _Last, unsigned int& _Value) {
    _STL_INTERNAL_CHECK(_First != _Last && '0' <= *_First && *_First <= '9');

    constexpr auto _Max_int = static_cast<unsigned int>((numeric_limits<int>::max)());
    constexpr auto _Big_int = _Max_int / 10u;

    _Value = 0;

    do {
        if (_Value > _Big_int) {
            _Value = _Max_int + 1;
            break;
        }
        _Value = _Value * 10 + static_cast<unsigned int>(*_First - '0');
        ++_First;
    } while (_First != _Last && '0' <= *_First && *_First <= '9');

    if (_Value > _Max_int) {
        _THROW(format_error("Number is too big"));
    }

    return _First;
}

template <class _CharT>
_NODISCARD constexpr const _CharT* _Parse_nonnegative_integer(const _CharT* _First, const _CharT* _Last, int& _Value) {
    unsigned int _Val_unsigned = 0;

    _First = _Parse_nonnegative_integer(_First, _Last, _Val_unsigned);
    // Never invalid because _Parse_nonnegative_integer throws an error for values that don't fit in signed integers
    _Value = static_cast<int>(_Val_unsigned);
    return _First;
}

template <class _CharT, _Parse_arg_id_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_arg_id(
    const _CharT* _First, const _CharT* _Last, _Callbacks_type&& _Callbacks) {
    _STL_INTERNAL_CHECK(_First != _Last);
    _CharT _Ch = *_First;
    // No id provided, format string is using automatic indexing.
    if (_Ch == '}' || _Ch == ':') {
        _Callbacks._On_auto_id();
        return _First;
    }

    if (_Ch >= '0' && _Ch <= '9') {
        unsigned int _Index = 0;
        // arg_id is not allowed to have any leading zeros, but is allowed to be
        // equal to zero (but not '00'). So if _Ch is zero we skip the parsing, leave
        // _Index set to zero and let the validity checks below ensure that the arg_id
        // wasn't something like "00", or "023".
        if (_Ch == '0') {
            ++_First;
        } else {
            _First = _Parse_nonnegative_integer(_First, _Last, _Index);
        }

        // The format string shouldn't end right after the index number.
        // The only things permitted after the index are the end of the replacement field ('}')
        // or the beginning of the format spec (':').
        if (_First == _Last || (*_First != '}' && *_First != ':')) {
            _THROW(format_error("Invalid format string."));
        }

        _Callbacks._On_manual_id(_Index);
        return _First;
    }
    // This is where we would parse named arg ids if std::format were to support them.
    _THROW(format_error("Invalid format string."));
}

_NODISCARD constexpr bool _Is_execution_charset_self_synchronizing() {
#ifdef _MSVC_EXECUTION_CHARACTER_SET
    // This is a list of charset codes (as reported by _MSVC_EXECUTION_CHARACTER_SET) that are
    // self-synchronizing (and "narrow" so 1200 (utf-16) and 1201 (unicodeFFFE) are not listed).
    // We care about this because if a charset is self-synchronizing then we can search through it
    // for formatting control characters _without_ generally decoding the format string.
    switch (_MSVC_EXECUTION_CHARACTER_SET) {
    // See: https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
    case 874: // Thai (Windows)
    case 1250: // ANSI Central European; Central European (Windows)
    case 1251: // ANSI Cyrillic; Cyrillic (Windows)
    case 1252: // ANSI Latin 1; Western European (Windows)
    case 1253: // ANSI Greek; Greek (Windows)
    case 1254: // ANSI Turkish; Turkish (Windows)
    case 1255: // ANSI Hebrew; Hebrew (Windows)
    case 1256: // ANSI Arabic; Arabic (Windows)
    case 1257: // ANSI Baltic; Baltic (Windows)
    case 1258: // ANSI/OEM Vietnamese; Vietnamese (Windows)
    case 65001: // Unicode (UTF-8)
        return true;
    default:
        return false;
    }
#elif defined(__clang__) // ^^^ no workaround / Clang workaround vvv
    return true; // clang-cl always uses UTF-8
#else // ^^^ Clang workaround / EDG workaround vvv
    return false; // TRANSITION, VSO-1468747 (EDG) - safe fallback response
#endif // ^^^ EDG workaround ^^^
}

inline constexpr char16_t _Width_estimate_low_intervals[] = { // Per N4885 [format.string.std]/11
    0x1100u, 0x1160u, 0x2329u, 0x232Bu, 0x2E80u, 0x303Fu, 0x3040u, 0xA4D0u, 0xAC00u, 0xD7A4u, 0xF900u, 0xFB00u, 0xFE10u,
    0xFE1Au, 0xFE30u, 0xFE70u, 0xFF00u, 0xFF61u, 0xFFE0u, 0xFFE7u};

inline constexpr char32_t _Width_estimate_high_intervals[] = { // Per N4885 [format.string.std]/11
    0x1F300u, 0x1F650u, 0x1F900u, 0x1FA00u, 0x20000u, 0x2FFFEu, 0x30000u, 0x3FFFEu};

template <auto& _Bounds>
_NODISCARD constexpr int _Unicode_width_estimate(const char32_t _Ch) noexcept {
    // Computes the width estimation for Unicode characters from N4885 [format.string.std]/11
    int _Result = 1;
    for (const auto& _Bound : _Bounds) {
        if (_Ch < _Bound) {
            return _Result;
        }
        _Result ^= 0b11u; // Flip between 1 and 2 on each iteration
    }

    return 1;
}

template <class _CharT, bool _Statically_Utf8 = _Is_execution_charset_self_synchronizing()>
class _Fmt_codec;

template <bool _Statically_Utf8>
class _Fmt_codec_base {};

template <>
class _Fmt_codec_base<false> {
protected:
    _Cvtvec _Cvt;

    _NODISCARD int _Double_byte_encoding_code_units_in_next_character(
        const char* const _First, const char* const _Last) const {
        // Returns a count of the number of code units that compose the first encoded character in [_First, _Last),
        // or -1 if [_First, _Last) doesn't contain an entire encoded character or *_First is not a valid lead byte.
        wchar_t _Wide;
        mbstate_t _St{};
        const auto _Len   = static_cast<size_t>(_Last - _First);
        const int _Result = _Mbrtowc(&_Wide, _First, _Len, &_St, &_Cvt);
        if (_Result > 0) {
            return _Result;
        } else if (_Result < 0) { // invalid or incomplete encoded character
            return -1;
        } else { // next code unit is '\0'
            return 1;
        }
    }

    _Fmt_codec_base() {
#ifdef _MSVC_EXECUTION_CHARACTER_SET
        constexpr __std_code_page _Format_codepage{_MSVC_EXECUTION_CHARACTER_SET};
#else // ^^^ no workaround / workaround vvv
      // TRANSITION, Clang 14 - only utf-8 is supported
      // TRANSITION, VSO-1468747 (EDG) - constructor isn't constexpr, so value is unimportant
        constexpr __std_code_page _Format_codepage{65001};
#endif // ^^^ workaround ^^^
        [[maybe_unused]] const __std_win_error _Result = __std_get_cvt(_Format_codepage, &_Cvt);
        _STL_INTERNAL_CHECK(_Result == __std_win_error::_Success);
    }
};

template <bool _Statically_Utf8>
class _Fmt_codec<char, _Statically_Utf8> : private _Fmt_codec_base<_Statically_Utf8> {
private:
    _NODISCARD static constexpr int _Utf8_code_units_in_next_character(
        const char* const _First, const char* const _Last) noexcept {
        // Returns a count of the number of UTF-8 code units that compose the first encoded character in [_First,
        // _Last), or -1 if [_First, _Last) doesn't contain an entire encoded character or *_First is not a valid lead
        // byte.
        const auto _Ch = static_cast<unsigned char>(*_First);
        if (_Ch < 0b1000'0000u) {
            return 1;
        }

        const auto _Len = static_cast<size_t>(_Last - _First);

        if (_Ch < 0b1110'0000u) {
            // check for non-lead byte or partial 2-byte encoded character
            return (_Ch >= 0b1100'0000u && _Len >= 2) ? 2 : -1;
        }

        if (_Ch < 0b1111'0000u) {
            // check for partial 3-byte encoded character
            return (_Len >= 3) ? 3 : -1;
        }

        // check for partial 4-byte encoded character
        return (_Len >= 4) ? 4 : -1;
    }

    _NODISCARD static constexpr int _Estimate_utf8_character_width(const char* const _Ptr, const int _Units) noexcept {
        // Return an estimate for the width of the character composed of _Units code units,
        // whose first code unit is denoted by _Ptr.
        auto _Ch = static_cast<char32_t>(*_Ptr);
        switch (_Units) {
        default:
        case 1:
        case 2:
            return 1;
        case 3:
            _Ch &= 0b1111u;
            break;
        case 4:
            _Ch &= 0b111u;
            break;
        }

        for (int _Idx = 1; _Idx < _Units; ++_Idx) {
            _Ch = _Ch << 6 | (_Ptr[_Idx] & 0b11'1111u);
        }

        if (_Units == 3) {
            return _Unicode_width_estimate<_Width_estimate_low_intervals>(_Ch);
        }

        return _Unicode_width_estimate<_Width_estimate_high_intervals>(_Ch);
    }

public:
    _NODISCARD constexpr int _Units_in_next_character(
        const char* const _First, const char* const _Last) const noexcept {
        // Returns a count of the number of code units that compose the first encoded character in
        // [_First, _Last), or -1 if [_First, _Last) doesn't contain an entire encoded character or
        // *_First is not a valid lead byte.
        _STL_INTERNAL_CHECK(_First < _Last);

        if constexpr (_Statically_Utf8) {
            return _Utf8_code_units_in_next_character(_First, _Last);
        } else {
            switch (this->_Cvt._Mbcurmax) {
            default:
                _STL_INTERNAL_CHECK(!"Bad number of encoding units for this code page");
                [[fallthrough]];
            case 1:
                return 1; // all characters have only one code unit

            case 2:
                return this->_Double_byte_encoding_code_units_in_next_character(_First, _Last);

            case 4: // Assume UTF-8 (as does _Mbrtowc)
                return _Utf8_code_units_in_next_character(_First, _Last);
            }
        }
    }

    _NODISCARD constexpr const char* _Find_encoded(const char* _First, const char* const _Last, const char _Val) const {
        // Returns the first occurrence of _Val as an encoded character (and not, for example, as a
        // continuation byte) in [_First, _Last).
        if constexpr (_Statically_Utf8) {
            return _Find_unchecked(_First, _Last, _Val);
        } else {
            if (this->_Cvt._Mbcurmax == 1 || this->_Cvt._Mbcurmax == 4) {
                // As above and in _Mbrtowc, assume 4-byte encodings are UTF-8
                return _Find_unchecked(_First, _Last, _Val);
            }

            while (_First != _Last && *_First != _Val) {
                const int _Units = _Units_in_next_character(_First, _Last);
                if (_Units < 0) {
                    _THROW(format_error("Invalid encoded character in format string."));
                }
                _First += _Units;
            }

            return _First;
        }
    }

    _NODISCARD constexpr int _Estimate_width(const char* const _Ptr, const int _Units) const {
        // Return an estimate for the width of the character composed of _Units code units,
        // whose first code unit is denoted by _Ptr.
        if constexpr (_Statically_Utf8) {
            return _Estimate_utf8_character_width(_Ptr, _Units);
        } else {
            if (this->_Cvt._Mbcurmax != 4) {
                // not a Unicode encoding; estimate width == number of code units
                return _Units;
            }

            // assume UTF-8
            return _Estimate_utf8_character_width(_Ptr, _Units);
        }
    }
};

template <bool _Statically_Utf8>
class _Fmt_codec<wchar_t, _Statically_Utf8> {
public:
    _NODISCARD constexpr int _Units_in_next_character(
        const wchar_t* _First, const wchar_t* const _Last) const noexcept {
        // Returns a count of the number of code units that compose the first encoded character in
        // [_First, _Last), or -1 if [_First, _Last) doesn't contain an entire encoded character or
        // *_First is an unpaired surrogate.
        _STL_INTERNAL_CHECK(_First < _Last);

        if (*_First < 0xD800u || *_First >= 0xE000u) {
            return 1;
        }

        if (*_First >= 0xDC00u) { // unpaired low surrogate
            return -1;
        }

        if (++_First == _Last || *_First < 0xDC00u || *_First >= 0xE000u) { // unpaired high surrogate
            return -1;
        }

        return 2; // surrogate pair
    }

    _NODISCARD constexpr const wchar_t* _Find_encoded(
        const wchar_t* const _First, const wchar_t* const _Last, const wchar_t _Val) const {
        return _Find_unchecked(_First, _Last, _Val);
    }

    _NODISCARD constexpr int _Estimate_width(const wchar_t* const _Ptr, const int _Units) const {
        // Return an estimate for the width of the character composed of _Units code units,
        // whose first code unit is denoted by _Ptr.
        auto _Ch = static_cast<char32_t>(*_Ptr);
        if (_Units == 1) {
            return _Unicode_width_estimate<_Width_estimate_low_intervals>(_Ch);
        }

        // surrogate pair
        _Ch = (_Ch - 0xD800u) << 10;
        _Ch += static_cast<char32_t>(_Ptr[1]) - 0xDC00u;
        _Ch += 0x10000u;
        return _Unicode_width_estimate<_Width_estimate_high_intervals>(_Ch);
    }
};

template <class _CharT>
_NODISCARD constexpr _Fmt_codec<_CharT> _Get_fmt_codec() {
    return {};
}

// clang-format off
template <class _CharT>
    requires (is_same_v<_CharT, char> && !_Is_execution_charset_self_synchronizing())
_NODISCARD const _Fmt_codec<_CharT>& _Get_fmt_codec() {
    // clang-format on
    using _CodecType = _Fmt_codec<_CharT>;

    static once_flag _Flag;
    alignas(_CodecType) static unsigned char _Storage[sizeof(_CodecType)];

    _STD call_once(_Flag, [] { _STD construct_at(reinterpret_cast<_CodecType*>(&_Storage)); });

    return *_STD launder(reinterpret_cast<const _CodecType*>(&_Storage));
}

template <class _CharT, _Parse_align_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_align(
    const _CharT* _First, const _CharT* _Last, _Callbacks_type&& _Callbacks) {
    // align and fill
    _STL_INTERNAL_CHECK(_First != _Last && *_First != '}');
    auto _Parsed_align = _Fmt_align::_None;

    const int _Units = _Get_fmt_codec<_CharT>()._Units_in_next_character(_First, _Last);
    if (_Units < 0) { // invalid fill character encoding
        _THROW(format_error("Invalid format string."));
    }
    auto _Align_pt = _First + _Units;

    if (_Align_pt == _Last) {
        _Align_pt = _First;
    }

    for (;;) {
        switch (*_Align_pt) {
        case '<':
            _Parsed_align = _Fmt_align::_Left;
            break;
        case '>':
            _Parsed_align = _Fmt_align::_Right;
            break;
        case '^':
            _Parsed_align = _Fmt_align::_Center;
            break;
        }

        if (_Parsed_align != _Fmt_align::_None) {
            if (_Align_pt != _First) {
                if (*_First == '{') {
                    _THROW(format_error("invalid fill character '{'"));
                }
                _Callbacks._On_fill({_First, static_cast<size_t>(_Align_pt - _First)});
                _First = _Align_pt + 1;
            } else {
                ++_First;
            }
            _Callbacks._On_align(_Parsed_align);
            break;
        } else if (_Align_pt == _First) {
            break;
        }
        _Align_pt = _First;
    }

    return _First;
}

// Adapts a type modeling _Width_adapter_callbacks to model _Parse_arg_id_callbacks.
// Used in _Parse_width so that _Parse_arg_id can be used to parse dynamic widths.
template <class _CharT, _Width_adapter_callbacks<_CharT> _Callbacks_type>
struct _Width_adapter {
    _Callbacks_type& _Callbacks;

    constexpr explicit _Width_adapter(_Callbacks_type& _Handler) : _Callbacks(_Handler) {}

    constexpr void _On_auto_id() {
        _Callbacks._On_dynamic_width(_Auto_id_tag{});
    }
    constexpr void _On_manual_id(const size_t _Id) {
        _Callbacks._On_dynamic_width(_Id);
    }
};

// Adapts a type modeling _Precision_adapter_callbacks to model _Parse_arg_id_callbacks.
// Used in _Parse_precision so that _Parse_arg_id can be used to parse dynamic precisions.
template <class _CharT, _Precision_adapter_callbacks<_CharT> _Callbacks_type>
struct _Precision_adapter {
    _Callbacks_type& _Callbacks;

    constexpr explicit _Precision_adapter(_Callbacks_type& _Handler) : _Callbacks(_Handler) {}

    constexpr void _On_auto_id() {
        _Callbacks._On_dynamic_precision(_Auto_id_tag{});
    }
    constexpr void _On_manual_id(const size_t _Id) {
        _Callbacks._On_dynamic_precision(_Id);
    }
};

// _Parse_arg_id expects a handler when it finds an argument id, however
// _Parse_replacement_field actually needs to know the value of that argument ID to pass on
// to _Handler._On_replacement_field or _Handler._On_format_specs. This _Parse_arg_id wrapper
// stores the value of the arg id for later use, so _Parse_replacement_field has access to it.
template <class _CharT>
struct _Id_adapter {
    basic_format_parse_context<_CharT>& _Parse_context;
    size_t _Arg_id = static_cast<size_t>(-1);

    constexpr void _On_auto_id() {
        _Arg_id = _Parse_context.next_arg_id();
        _STL_INTERNAL_CHECK(_Arg_id != static_cast<size_t>(-1));
    }
    constexpr void _On_manual_id(const size_t _Id) {
        _Parse_context.check_arg_id(_Id);
        _Arg_id = _Id;
        _STL_INTERNAL_CHECK(_Arg_id != static_cast<size_t>(-1));
    }
};

template <class _CharT, _Parse_width_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_width(
    const _CharT* _First, const _CharT* _Last, _Callbacks_type&& _Callbacks) {
    _STL_INTERNAL_CHECK(_First != _Last);
    if ('1' <= *_First && *_First <= '9') {
        int _Value = 0;
        _First     = _Parse_nonnegative_integer(_First, _Last, _Value);
        _Callbacks._On_width(_Value);
    } else if (*_First == '{') {
        ++_First;
        if (_First != _Last) {
            _First = _Parse_arg_id(_First, _Last, _Width_adapter<_CharT, _Callbacks_type>{_Callbacks});
        }

        if (_First == _Last || *_First != '}') {
            _THROW(format_error("Invalid format string."));
        }
        ++_First;
    }
    return _First;
}

template <class _CharT, _Parse_precision_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_precision(
    const _CharT* _First, const _CharT* _Last, _Callbacks_type&& _Callbacks) {
    ++_First;
    _CharT _Ch = '\0';
    if (_First != _Last) {
        _Ch = *_First;
    }

    if ('0' <= _Ch && _Ch <= '9') {
        int _Precision = 0;
        _First         = _Parse_nonnegative_integer(_First, _Last, _Precision);
        _Callbacks._On_precision(_Precision);
    } else if (_Ch == '{') {
        ++_First;
        if (_First != _Last) {
            _First = _Parse_arg_id(_First, _Last, _Precision_adapter<_CharT, _Callbacks_type>{_Callbacks});
        }

        if (_First == _Last || *_First != '}') {
            _THROW(format_error("Invalid format string."));
        }
        ++_First;
    } else {
        _THROW(format_error("Missing precision specifier."));
    }

    return _First;
}

template <class _CharT, _Parse_spec_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_format_specs(
    const _CharT* _First, const _CharT* _Last, _Callbacks_type&& _Callbacks) {
    if (_First == _Last || *_First == '}') {
        return _First;
    }

    _First = _Parse_align(_First, _Last, _Callbacks);
    if (_First == _Last) {
        return _First;
    }

    switch (*_First) {
    case '+':
        _Callbacks._On_sign(_Fmt_sign::_Plus);
        ++_First;
        break;
    case '-':
        _Callbacks._On_sign(_Fmt_sign::_Minus);
        ++_First;
        break;
    case ' ':
        _Callbacks._On_sign(_Fmt_sign::_Space);
        ++_First;
        break;
    default:
        break;
    }

    if (_First == _Last) {
        return _First;
    }

    if (*_First == '#') {
        _Callbacks._On_hash();
        if (++_First == _Last) {
            return _First;
        }
    }

    if (*_First == '0') {
        _Callbacks._On_zero();
        if (++_First == _Last) {
            return _First;
        }
    }

    _First = _Parse_width(_First, _Last, _Callbacks);
    if (_First == _Last) {
        return _First;
    }

    if (*_First == '.') {
        _First = _Parse_precision(_First, _Last, _Callbacks);
        if (_First == _Last) {
            return _First;
        }
    }

    if (*_First == 'L') {
        _Callbacks._On_localized();
        if (++_First == _Last) {
            return _First;
        }
    }

    // If there's anything remaining we assume it's a type.
    if (*_First != '}') {
        _Callbacks._On_type(*_First);
        ++_First;
    } else {
        // call the type callback so it gets a default type, this is required
        // since _Specs_checker needs to be able to tell that it got a default type
        // to raise an error for default formatted bools with a sign modifier
        _Callbacks._On_type(static_cast<_CharT>('\0'));
    }

    return _First;
}

template <class _CharT, _Parse_replacement_field_callbacks<_CharT> _HandlerT>
_NODISCARD constexpr const _CharT* _Parse_replacement_field(
    const _CharT* _First, const _CharT* _Last, _HandlerT&& _Handler) {
    ++_First;
    if (_First == _Last) {
        _THROW(format_error("Invalid format string."));
    }

    if (*_First == '}') {
        // string was "{}", and we have a replacement field
        _Handler._On_replacement_field(_Handler._Parse_context.next_arg_id(), _First);
    } else if (*_First == '{') {
        // string was "{{", so we have a literal "{" to print
        _Handler._On_text(_First, _First + 1);
    } else {
        _Id_adapter<_CharT> _Adapter{_Handler._Parse_context};
        _First     = _Parse_arg_id(_First, _Last, _Adapter);
        _CharT _Ch = _CharT{};
        if (_First != _Last) {
            _Ch = *_First;
        }

        if (_Ch == '}') {
            _Handler._On_replacement_field(_Adapter._Arg_id, _First);
        } else if (_Ch == ':') {
            _First = _Handler._On_format_specs(_Adapter._Arg_id, _First + 1, _Last);
            if (_First == _Last || *_First != '}') {
                _THROW(format_error("Unknown format specifier."));
            }
        } else {
            _THROW(format_error("Missing '}' in format string."));
        }
    }

    return _First + 1;
}

template <class _CharT, _Parse_replacement_field_callbacks<_CharT> _HandlerT>
constexpr void _Parse_format_string(basic_string_view<_CharT> _Format_str, _HandlerT&& _Handler) {
    auto _First        = _Format_str.data();
    auto _Last         = _First + _Format_str.size();
    const auto& _Codec = _Get_fmt_codec<_CharT>();

    while (_First != _Last) {
        const _CharT* _OpeningCurl = _First;
        if (*_First != '{') {
            _OpeningCurl = _Codec._Find_encoded(_First, _Last, _CharT{'{'});

            for (;;) {
                const _CharT* _ClosingCurl = _Codec._Find_encoded(_First, _OpeningCurl, _CharT{'}'});

                // In this case there are neither closing nor opening curls in [_First, _OpeningCurl)
                // Write the whole thing out.
                if (_ClosingCurl == _OpeningCurl) {
                    _Handler._On_text(_First, _OpeningCurl);
                    break;
                }
                // We know _ClosingCurl isn't past the end because
                // the above condition was not met.
                ++_ClosingCurl;
                if (_ClosingCurl == _OpeningCurl || *_ClosingCurl != '}') {
                    _THROW(format_error("Unmatched '}' in format string."));
                }
                // We found two closing curls, so output only one of them
                _Handler._On_text(_First, _ClosingCurl);

                // skip over the second closing curl
                _First = _ClosingCurl + 1;
            }

            // We are done, there were no replacement fields.
            if (_OpeningCurl == _Last) {
                return;
            }
        }
        // Parse the replacement field starting at _OpeningCurl and ending sometime before _Last.
        _First = _Parse_replacement_field(_OpeningCurl, _Last, _Handler);
    }
}

template <class _CharT>
struct _Basic_format_specs {
    int _Width            = 0;
    int _Precision        = -1;
    char _Type            = '\0';
    _Fmt_align _Alignment = _Fmt_align::_None;
    _Fmt_sign _Sgn        = _Fmt_sign::_None;
    bool _Alt             = false;
    bool _Localized       = false;
    bool _Leading_zero    = false;
    uint8_t _Fill_length  = 1;
    // At most one codepoint (so one char32_t or four utf-8 char8_t).
    _CharT _Fill[4 / sizeof(_CharT)] = {_CharT{' '}};
};

// Adds width and precision references to _Basic_format_specs.
// This is required for std::formatter implementations because we must
// parse the format specs without having access to the format args (via a format context).
template <class _CharT>
struct _Dynamic_format_specs : _Basic_format_specs<_CharT> {
    int _Dynamic_width_index     = -1;
    int _Dynamic_precision_index = -1;
};

// Model of _Parse_spec_callbacks that fills a _Basic_format_specs with the parsed data.
template <class _CharT>
class _Specs_setter {
public:
    constexpr explicit _Specs_setter(_Basic_format_specs<_CharT>& _Specs_) : _Specs(_Specs_) {}

    constexpr void _On_align(const _Fmt_align _Aln) {
        _Specs._Alignment = _Aln;
    }

    constexpr void _On_fill(const basic_string_view<_CharT> _Sv) {
        if (_Sv.size() > _STD size(_Specs._Fill)) {
            _THROW(format_error("Invalid fill (too long)."));
        }

        const auto _Pos = _STD _Copy_unchecked(_Sv._Unchecked_begin(), _Sv._Unchecked_end(), _Specs._Fill);
        _STD fill(_Pos, _STD end(_Specs._Fill), _CharT{});
        _Specs._Fill_length = static_cast<uint8_t>(_Sv.size());
    }

    constexpr void _On_sign(const _Fmt_sign _Sgn) {
        _Specs._Sgn = _Sgn;
    }

    constexpr void _On_hash() {
        _Specs._Alt = true;
    }

    constexpr void _On_zero() {
        _Specs._Leading_zero = true;
    }

    constexpr void _On_width(const int _Width) {
        _Specs._Width = _Width;
    }

    constexpr void _On_precision(const int _Precision) {
        _Specs._Precision = _Precision;
    }

    constexpr void _On_localized() {
        _Specs._Localized = true;
    }

    constexpr void _On_type(const _CharT _Type) {
        _Specs._Type = static_cast<char>(_Type);
    }

protected:
    _Basic_format_specs<_CharT>& _Specs;
};

template <class _Context>
_NODISCARD constexpr basic_format_arg<_Context> _Get_arg(const _Context& _Ctx, const size_t _Arg_id) {
    // note: while this is parameterized on the _Arg_id type in libfmt we don't
    // need to do that in std::format because it's only called with either an integer
    // id or a named id (which we do not support in std::format)
    auto _Arg = _Ctx.arg(_Arg_id);
    if (!_Arg) {
        _THROW(format_error("Argument not found."));
    }

    return _Arg;
}

// Checks that the type and value of an argument associated with a dynamic
// width specifier are valid.
class _Width_checker {
public:
    template <class _Ty>
    _NODISCARD constexpr unsigned long long operator()(const _Ty _Value) const {
        if constexpr (is_integral_v<_Ty>) {
            bool _Positive;
            if constexpr (same_as<_Ty, bool>) { // avoid "bool > 0", which triggers C4804
                _Positive = _Value != 0;
            } else {
                _Positive = _Value > 0;
            }

            if (!_Positive) {
                _THROW(format_error("width is not positive."));
            }
            return static_cast<unsigned long long>(_Value);
        } else {
            _THROW(format_error("Width is not an integer."));
        }
    }
};

// Checks that the type and value of an argument associated with a dynamic
// precision specifier are valid.
class _Precision_checker {
public:
    template <class _Ty>
    _NODISCARD constexpr unsigned long long operator()(const _Ty _Value) const {
        if constexpr (is_integral_v<_Ty>) {
            if constexpr (is_signed_v<_Ty>) {
                if (_Value < 0) {
                    _THROW(format_error("Negative precision."));
                }
            }
            return static_cast<unsigned long long>(_Value);
        } else {
            _THROW(format_error("Precision is not an integer."));
        }
    }
};

// Fetch the value of an argument associated with a dynamic
// width or precision specifier. This will be called with either
// _Width_checker or _Precision_checker as "_Handler".
template <class _Handler, class _FormatArg>
_NODISCARD constexpr int _Get_dynamic_specs(const _FormatArg _Arg) {
    _STL_INTERNAL_STATIC_ASSERT(_Is_any_of_v<_Handler, _Width_checker, _Precision_checker>);
    const unsigned long long _Val = _STD visit_format_arg(_Handler{}, _Arg);
    if (_Val > static_cast<unsigned long long>((numeric_limits<int>::max)())) {
        _THROW(format_error("Number is too big."));
    }

    return static_cast<int>(_Val);
}

// Parses standard format specs into a _Basic_format_specs using _Specs_setter, and
// additionally handles dynamic width and precision. This is separate from _Specs_setter
// because it needs to know about the current basic_format_parse_context and basic_format_context
// in order to fetch the width from the arguments.
template <class _ParseContext, class _Context>
class _Specs_handler : public _Specs_setter<typename _Context::char_type> {
public:
    using _CharT = typename _Context::char_type;

    constexpr _Specs_handler(_Basic_format_specs<_CharT>& _Specs_, _ParseContext& _Parse_ctx_, _Context& _Ctx_)
        : _Specs_setter<_CharT>(_Specs_), _Parse_ctx(_Parse_ctx_), _Ctx(_Ctx_) {}

    template <class _Id>
    constexpr void _On_dynamic_width(const _Id _Arg_id) {
        this->_Specs._Width = _Get_dynamic_specs<_Width_checker>(_Get_arg(_Arg_id));
    }

    template <class _Id>
    constexpr void _On_dynamic_precision(const _Id _Arg_id) {
        this->_Specs._Precision = _Get_dynamic_specs<_Precision_checker>(_Get_arg(_Arg_id));
    }

private:
    _ParseContext& _Parse_ctx;
    _Context& _Ctx;

    _NODISCARD constexpr basic_format_arg<_Context> _Get_arg(_Auto_id_tag) {
        return _STD _Get_arg(_Ctx, _Parse_ctx.next_arg_id());
    }

    _NODISCARD constexpr basic_format_arg<_Context> _Get_arg(const size_t _Arg_id) {
        _Parse_ctx.check_arg_id(_Arg_id);
        return _STD _Get_arg(_Ctx, _Arg_id);
    }
};

template <class _ParseContext>
class _Dynamic_specs_handler : public _Specs_setter<typename _ParseContext::char_type> {
public:
    using _CharT = typename _ParseContext::char_type;

    constexpr _Dynamic_specs_handler(_Dynamic_format_specs<_CharT>& _Specs_, _ParseContext& _Parse_ctx_)
        : _Specs_setter<_CharT>(_Specs_), _Dynamic_specs(_Specs_), _Parse_ctx(_Parse_ctx_) {}

    constexpr void _On_dynamic_width(const size_t _Arg_id) {
        _Parse_ctx.check_arg_id(_Arg_id);
        _Dynamic_specs._Dynamic_width_index = _Verify_dynamic_arg_index_in_range(_Arg_id);
    }

    constexpr void _On_dynamic_width(const _Auto_id_tag) {
        _Dynamic_specs._Dynamic_width_index = _Verify_dynamic_arg_index_in_range(_Parse_ctx.next_arg_id());
    }

    constexpr void _On_dynamic_precision(const size_t _Arg_id) {
        _Parse_ctx.check_arg_id(_Arg_id);
        _Dynamic_specs._Dynamic_precision_index = _Verify_dynamic_arg_index_in_range(_Arg_id);
    }

    constexpr void _On_dynamic_precision(const _Auto_id_tag) {
        _Dynamic_specs._Dynamic_precision_index = _Verify_dynamic_arg_index_in_range(_Parse_ctx.next_arg_id());
    }

private:
    _Dynamic_format_specs<_CharT>& _Dynamic_specs;
    _ParseContext& _Parse_ctx;

    _NODISCARD static constexpr int _Verify_dynamic_arg_index_in_range(const size_t _Idx) {
        if (_Idx > static_cast<size_t>((numeric_limits<int>::max)())) {
            _THROW(format_error("Dynamic width or precision index too large."));
        }

        return static_cast<int>(_Idx);
    }
};

// Checks that the type of the argument printed by a replacement
// field with format specs actually satisfies the requirements for
// that format spec. If the requirements are met then calls the base class
// handler method.
template <class _Handler>
class _Specs_checker : public _Handler {
private:
    _Basic_format_arg_type _Arg_type;
    // we'll see this if we get a modifier that requires an integer presentation type
    // for types that can have either integer or non-integer presentation types (charT or bool)
    bool _Need_arithmetic_presentation_type = false;

public:
    constexpr explicit _Specs_checker(const _Handler& _Handler_inst, const _Basic_format_arg_type _Arg_type_)
        : _Handler(_Handler_inst), _Arg_type(_Arg_type_) {}

    constexpr void _Require_numeric_argument() const {
        if (!_Is_arithmetic_fmt_type(_Arg_type)) {
            _THROW(format_error("Format specifier requires numeric argument."));
        }
    }

    constexpr void _Check_precision() const {
        if (_Is_integral_fmt_type(_Arg_type) || _Arg_type == _Basic_format_arg_type::_Pointer_type) {
            _THROW(format_error("Precision not allowed for this argument type."));
        }
    }

    // _On_align has no checking, since we don't implement numeric alignments.

    constexpr void _On_localized() {
        _Require_numeric_argument();
        _Handler::_On_localized();
    }

    constexpr void _On_hash() {
        _Need_arithmetic_presentation_type = true;
        _Require_numeric_argument();
        _Handler::_On_hash();
    }

    constexpr void _On_sign(_Fmt_sign _Sgn) {
        _Need_arithmetic_presentation_type = true;
        _Require_numeric_argument();
        _Handler::_On_sign(_Sgn);
    }

    constexpr void _On_zero() {
        _Need_arithmetic_presentation_type = true;
        _Require_numeric_argument();
        _Handler::_On_zero();
    }

    constexpr void _On_precision(int _Precision) {
        _Check_precision();
        _Handler::_On_precision(_Precision);
    }

    template <class _Ty>
    constexpr void _On_dynamic_precision(const _Ty _Val) {
        _Check_precision();
        _Handler::_On_dynamic_precision(_Val);
    }

    template <class _CharT>
    constexpr void _On_type(_CharT _Type) {
        if (_Type < 0 || _Type > (numeric_limits<signed char>::max)()) {
            _THROW(format_error("Invalid type specification."));
        }
        const char _Narrow_type = static_cast<char>(_Type);
        enum class _Presentation_type_category { _Default, _Integer, _Floating, _String, _Pointer, _Char };
        auto _Cat = _Presentation_type_category::_Default;
        switch (_Narrow_type) {
        case '\0':
            break;
        case 's':
            _Cat = _Presentation_type_category::_String;
            break;
        case 'c':
            _Cat = _Presentation_type_category::_Char;
            break;
        case 'd':
        case 'B':
        case 'b':
        case 'X':
        case 'x':
        case 'o':
            _Cat = _Presentation_type_category::_Integer;
            break;
        case 'A':
        case 'a':
        case 'E':
        case 'e':
        case 'F':
        case 'f':
        case 'G':
        case 'g':
            _Cat = _Presentation_type_category::_Floating;
            break;
        case 'p':
            _Cat = _Presentation_type_category::_Pointer;
            break;
        default:
            _THROW(format_error("Invalid presentation type specifier"));
        }

        switch (_Arg_type) {
        case _Basic_format_arg_type::_None:
            _STL_INTERNAL_CHECK(!"Invalid argument type.");
            break;
        case _Basic_format_arg_type::_Bool_type:
            if (_Cat == _Presentation_type_category::_Default) {
                _Cat = _Presentation_type_category::_String;
            }
            // note, we don't get a call if there isn't a type, but none is valid for everything.
            if (_Cat != _Presentation_type_category::_String && _Cat != _Presentation_type_category::_Integer) {
                _THROW(format_error("Invalid presentation type for bool"));
            }
            break;
        case _Basic_format_arg_type::_Char_type:
            if (_Cat == _Presentation_type_category::_Default) {
                _Cat = _Presentation_type_category::_Char;
            }

            if (_Cat != _Presentation_type_category::_Char && _Cat != _Presentation_type_category::_Integer) {
                _THROW(format_error("Invalid presentation type for char/wchar_t"));
            }
            break;
        case _Basic_format_arg_type::_Int_type:
        case _Basic_format_arg_type::_UInt_type:
        case _Basic_format_arg_type::_Long_long_type:
        case _Basic_format_arg_type::_ULong_long_type:
            if (_Cat == _Presentation_type_category::_Default) {
                _Cat = _Presentation_type_category::_Integer;
            }

            if (_Cat != _Presentation_type_category::_Integer && _Cat != _Presentation_type_category::_Char) {
                _THROW(format_error("Invalid presentation type for integer"));
            }
            break;
        case _Basic_format_arg_type::_Float_type:
        case _Basic_format_arg_type::_Double_type:
        case _Basic_format_arg_type::_Long_double_type:
            if (_Cat == _Presentation_type_category::_Default) {
                _Cat = _Presentation_type_category::_Floating;
            }

            if (_Cat != _Presentation_type_category::_Floating) {
                _THROW(format_error("Invalid presentation type for floating-point"));
            }
            break;
        case _Basic_format_arg_type::_CString_type:
        case _Basic_format_arg_type::_String_type:
            if (_Cat == _Presentation_type_category::_Default) {
                _Cat = _Presentation_type_category::_String;
            }

            if (_Cat != _Presentation_type_category::_String) {
                _THROW(format_error("Invalid presentation type for string"));
            }
            break;
        case _Basic_format_arg_type::_Pointer_type:
            if (_Cat == _Presentation_type_category::_Default) {
                _Cat = _Presentation_type_category::_Pointer;
            }

            if (_Cat != _Presentation_type_category::_Pointer) {
                _THROW(format_error("Invalid presentation type for pointer"));
            }
            break;
        case _Basic_format_arg_type::_Custom_type:
            // there's no checking we can do here for custom types
            // (however if a custom type uses a standard formatter
            // to do its spec parsing it should get the above checks)
            break;
        }

        if (_Need_arithmetic_presentation_type && _Cat != _Presentation_type_category::_Integer
            && _Cat != _Presentation_type_category::_Floating) {
            _THROW(format_error("Modifier requires an integer presentation type for bool"));
        }
        _Handler::_On_type(_Type);
    }
};

template <class _Context>
struct _Format_arg_traits {
    using _Char_type = typename _Context::char_type;

    // These overloads mirror the exposition-only single-argument constructor
    // set of basic_format_arg (N4885 [format.arg]). They determine the mapping
    // from "raw" to "erased" argument type for _Format_arg_store.
    template <_Has_formatter<_Context> _Ty>
    static auto _Phony_basic_format_arg_constructor(_Ty&&) {
        // per the proposed resolution of LWG-3631
        using _Td = remove_cvref_t<_Ty>;
        // See N4885 [format.arg]/5
        if constexpr (is_same_v<_Td, bool>) {
            return bool{};
        } else if constexpr (is_same_v<_Td, _Char_type>) {
            return _Char_type{};
        } else if constexpr (is_same_v<_Td, char> && is_same_v<_Char_type, wchar_t>) {
            return _Char_type{};
        } else if constexpr (signed_integral<_Td> && sizeof(_Td) <= sizeof(int)) {
            return int{};
        } else if constexpr (unsigned_integral<_Td> && sizeof(_Td) <= sizeof(unsigned int)) {
            return static_cast<unsigned int>(42);
        } else if constexpr (signed_integral<_Td> && sizeof(_Td) <= sizeof(long long)) {
            return static_cast<long long>(42);
        } else if constexpr (unsigned_integral<_Td> && sizeof(_Td) <= sizeof(unsigned long long)) {
            return static_cast<unsigned long long>(42);
        } else {
            return typename basic_format_arg<_Context>::handle{42};
        }
    }

    static auto _Phony_basic_format_arg_constructor(float) -> float; // not defined
    static auto _Phony_basic_format_arg_constructor(double) -> double; // not defined
    static auto _Phony_basic_format_arg_constructor(long double) -> long double; // not defined

    static auto _Phony_basic_format_arg_constructor(const _Char_type*) -> const _Char_type*; // not defined

    template <class _Traits>
    static auto _Phony_basic_format_arg_constructor(basic_string_view<_Char_type, _Traits>)
        -> basic_string_view<_Char_type>; // not defined

    template <class _Traits, class _Alloc>
    static auto _Phony_basic_format_arg_constructor(const basic_string<_Char_type, _Traits, _Alloc>&)
        -> basic_string_view<_Char_type>; // not defined

    static auto _Phony_basic_format_arg_constructor(nullptr_t) -> const void*; // not defined

    // clang-format off
    template <class _Ty>
        requires is_void_v<_Ty>
    static auto _Phony_basic_format_arg_constructor(_Ty*) -> const void*; // not defined
    // clang-format on

    template <class _Ty>
    using _Storage_type = decltype(_Phony_basic_format_arg_constructor(_STD declval<_Ty&>()));

    template <class _Ty>
    static constexpr size_t _Storage_size = sizeof(_Storage_type<_Ty>);
};

struct _Format_arg_index {
    // TRANSITION, Should be templated on number of arguments for even less storage

    constexpr _Format_arg_index() = default;
    constexpr explicit _Format_arg_index(const size_t _Index_) noexcept : _Index(_Index_) {
        _Type(_Basic_format_arg_type::_None);
    }

    _NODISCARD constexpr _Basic_format_arg_type _Type() const noexcept {
        return static_cast<_Basic_format_arg_type>(_Type_);
    }

    constexpr void _Type(_Basic_format_arg_type _Val) noexcept {
        _Type_ = static_cast<size_t>(_Val);
    }

    size_t _Index : (sizeof(size_t) * 8 - 4);
    size_t _Type_ : 4;
};

template <class _Context>
class basic_format_args;

template <class _Context, class... _Args>
class _Format_arg_store {
private:
    using _CharType = typename _Context::char_type;
    using _Traits   = _Format_arg_traits<_Context>;

    friend basic_format_args<_Context>;

    static constexpr size_t _Num_args       = sizeof...(_Args);
    static constexpr size_t _Storage_length = (_Traits::template _Storage_size<_Args> + ...);

    // The actual storage representation: _Num_args offsets into _Storage, followed immediately by the untyped
    // _Storage which holds copies of the object representations of arguments (with no regard for alignment).
    // These must be allocated consecutively, since basic_format_args thinks it can store a pointer to
    // _Index_array and use arithmetic to access the bytes of _Storage.
    _Format_arg_index _Index_array[_Num_args];
    unsigned char _Storage[_Storage_length];

    template <class _Ty>
    void _Store_impl(
        const size_t _Arg_index, const _Basic_format_arg_type _Arg_type, const type_identity_t<_Ty>& _Val) noexcept {
        _STL_INTERNAL_CHECK(_Arg_index < _Num_args);

        const auto _Store_index = _Index_array[_Arg_index]._Index;

        _CSTD memcpy(_Storage + _Store_index, _STD addressof(_Val), sizeof(_Ty));
        _Index_array[_Arg_index]._Type(_Arg_type);
        if (_Arg_index + 1 < _Num_args) {
            // Set the starting index of the next arg, as that is dynamic, must be called with increasing index
#pragma warning(suppress : 6386) // Buffer overrun while writing to '%s' ...
            _Index_array[_Arg_index + 1] = _Format_arg_index{_Store_index + sizeof(_Ty)};
        }
    }

    template <class _Ty>
    void _Store(const size_t _Arg_index, _Ty&& _Val) noexcept {
        using _Erased_type = typename _Traits::template _Storage_type<_Ty>;

        _Basic_format_arg_type _Arg_type;
        if constexpr (is_same_v<_Erased_type, bool>) {
            _Arg_type = _Basic_format_arg_type::_Bool_type;
        } else if constexpr (is_same_v<_Erased_type, _CharType>) {
            _Arg_type = _Basic_format_arg_type::_Char_type;
        } else if constexpr (is_same_v<_Erased_type, int>) {
            _Arg_type = _Basic_format_arg_type::_Int_type;
        } else if constexpr (is_same_v<_Erased_type, unsigned int>) {
            _Arg_type = _Basic_format_arg_type::_UInt_type;
        } else if constexpr (is_same_v<_Erased_type, long long>) {
            _Arg_type = _Basic_format_arg_type::_Long_long_type;
        } else if constexpr (is_same_v<_Erased_type, unsigned long long>) {
            _Arg_type = _Basic_format_arg_type::_ULong_long_type;
        } else if constexpr (is_same_v<_Erased_type, float>) {
            _Arg_type = _Basic_format_arg_type::_Float_type;
        } else if constexpr (is_same_v<_Erased_type, double>) {
            _Arg_type = _Basic_format_arg_type::_Double_type;
        } else if constexpr (is_same_v<_Erased_type, long double>) {
            _Arg_type = _Basic_format_arg_type::_Long_double_type;
        } else if constexpr (is_same_v<_Erased_type, const void*>) {
            _Arg_type = _Basic_format_arg_type::_Pointer_type;
        } else if constexpr (is_same_v<_Erased_type, const _CharType*>) {
            _Arg_type = _Basic_format_arg_type::_CString_type;
        } else if constexpr (is_same_v<_Erased_type, basic_string_view<_CharType>>) {
            _Arg_type = _Basic_format_arg_type::_String_type;
        } else {
            _STL_INTERNAL_STATIC_ASSERT(is_same_v<_Erased_type, typename basic_format_arg<_Context>::handle>);
            _Arg_type = _Basic_format_arg_type::_Custom_type;
        }

        _Store_impl<_Erased_type>(_Arg_index, _Arg_type, static_cast<_Erased_type>(_Val));
    }

public:
    _Format_arg_store(_Args&... _Vals) noexcept {
        _Index_array[0]   = {};
        size_t _Arg_index = 0;
        (_Store(_Arg_index++, _Vals), ...);
    }
};

template <class _Context>
class _Format_arg_store<_Context> {};

template <class _Context>
class basic_format_args {
public:
    basic_format_args() noexcept = default;

    basic_format_args(const _Format_arg_store<_Context>&) noexcept {}

    template <class... _Args>
    basic_format_args(const _Format_arg_store<_Context, _Args...>& _Store) noexcept
        : _Num_args(sizeof...(_Args)), _Index_array(_Store._Index_array) {}

    _NODISCARD basic_format_arg<_Context> get(const size_t _Index) const noexcept {
        if (_Index >= _Num_args) {
            return basic_format_arg<_Context>{};
        }

        using _CharType = typename _Context::char_type;
        // The explanatory comment in _Format_arg_store explains how the following works.
        const auto _Packed_index = _Index_array[_Index];
        const auto _Arg_storage =
            reinterpret_cast<const unsigned char*>(_Index_array + _Num_args) + _Packed_index._Index;

        switch (_Packed_index._Type()) {
        case _Basic_format_arg_type::_None:
        default:
            _STL_ASSERT(false, "Invalid basic_format_arg type");
            return basic_format_arg<_Context>{};
        case _Basic_format_arg_type::_Int_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<int>(_Arg_storage)};
        case _Basic_format_arg_type::_UInt_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<unsigned int>(_Arg_storage)};
        case _Basic_format_arg_type::_Long_long_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<long long>(_Arg_storage)};
        case _Basic_format_arg_type::_ULong_long_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<unsigned long long>(_Arg_storage)};
        case _Basic_format_arg_type::_Bool_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<bool>(_Arg_storage)};
        case _Basic_format_arg_type::_Char_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<_CharType>(_Arg_storage)};
        case _Basic_format_arg_type::_Float_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<float>(_Arg_storage)};
        case _Basic_format_arg_type::_Double_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<double>(_Arg_storage)};
        case _Basic_format_arg_type::_Long_double_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<long double>(_Arg_storage)};
        case _Basic_format_arg_type::_Pointer_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<const void*>(_Arg_storage)};
        case _Basic_format_arg_type::_CString_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<const _CharType*>(_Arg_storage)};
        case _Basic_format_arg_type::_String_type:
            return basic_format_arg<_Context>{_Get_value_from_memory<basic_string_view<_CharType>>(_Arg_storage)};
        case _Basic_format_arg_type::_Custom_type:
            return basic_format_arg<_Context>{
                _Get_value_from_memory<typename basic_format_arg<_Context>::handle>(_Arg_storage)};
        }
    }

    _NODISCARD size_t _Estimate_required_capacity() const noexcept {
        using _CharT        = typename _Context::char_type;
        size_t _Result      = 0;
        const auto _Visitor = [&_Result]<class _ArgTy>(const _ArgTy _Arg) noexcept {
            if constexpr (is_same_v<_ArgTy, basic_string_view<_CharT>>) {
                _Result += _Arg.size();
            } else if constexpr (is_same_v<_ArgTy, const _CharT*>) {
                _Result += 32; // estimate for length of null-terminated strings
            } else {
                _Result += 8; // estimate for length of all other arguments
            }
        };
        for (size_t _Idx = 0; _Idx < _Num_args; ++_Idx) {
            _STD visit_format_arg(_Visitor, get(_Idx));
        }
        return _Result;
    }

private:
    template <class _Ty>
    _NODISCARD static auto _Get_value_from_memory(const unsigned char* const _Val) noexcept {
        auto& _Temp = *reinterpret_cast<const unsigned char(*)[sizeof(_Ty)]>(_Val);
        return _Bit_cast<_Ty>(_Temp);
    }

    size_t _Num_args                      = 0;
    const _Format_arg_index* _Index_array = nullptr;
};

// _Lazy_locale is used instead of a std::locale so that the locale is only
// constructed when needed, and is never constructed if the format string does not
// contain locale-sensitive format specifiers. Note that this means that a new locale
// will be constructed for _every_ locale-sensitive format specifier in the format string,
// making that case slower than if we had stored a "real" locale in the basic_format_context.
class _Lazy_locale {
private:
    const locale* _Loc = nullptr;

public:
    _Lazy_locale() = default;

    explicit _Lazy_locale(const locale& _Loc_) : _Loc(&_Loc_) {}

    explicit _Lazy_locale(const locale&&) = delete;

    _NODISCARD locale _Get() const {
        if (_Loc) {
            return *_Loc;
        } else {
            return locale{};
        }
    }
};

// clang-format off
template <class _Out, class _CharT>
    requires output_iterator<_Out, const _CharT&>
class basic_format_context {
    // clang-format on
private:
    _Out _OutputIt;
    basic_format_args<basic_format_context> _Args;
    _Lazy_locale _Loc;

public:
    using iterator  = _Out;
    using char_type = _CharT;

    template <class _Ty>
    using formatter_type = formatter<_Ty, _CharT>;

    constexpr basic_format_context(_Out _OutputIt_, basic_format_args<basic_format_context> _Ctx_args)
        : _OutputIt(_STD move(_OutputIt_)), _Args(_Ctx_args) {}

    constexpr basic_format_context(
        _Out _OutputIt_, basic_format_args<basic_format_context> _Ctx_args, const _Lazy_locale& _Loc_)
        : _OutputIt(_STD move(_OutputIt_)), _Args(_Ctx_args), _Loc(_Loc_) {}

    _NODISCARD basic_format_arg<basic_format_context> arg(size_t _Id) const noexcept {
        return _Args.get(_Id);
    }
    _NODISCARD locale locale() {
        return _Loc._Get();
    }
    _NODISCARD iterator out() {
        return _STD move(_OutputIt);
    }
    void advance_to(iterator _It) {
        _OutputIt = _STD move(_It);
    }

    _NODISCARD const basic_format_args<basic_format_context>& _Get_args() const noexcept {
        return _Args;
    }
    _NODISCARD _Lazy_locale _Get_lazy_locale() const {
        return _Loc;
    }
};

template <class _Ty>
class _Fmt_buffer {
private:
    _Ty* _Ptr_        = nullptr;
    size_t _Size_     = 0;
    size_t _Capacity_ = 0;

protected:
    explicit _Fmt_buffer(const size_t _Size) noexcept : _Size_(_Size), _Capacity_(_Size) {}

    ~_Fmt_buffer() = default;

    _Fmt_buffer(_Ty* _Data, const size_t _Size, const size_t _Capacity) noexcept
        : _Ptr_(_Data), _Size_(_Size), _Capacity_(_Capacity) {}

    void _Set(_Ty* _Buf_data, const size_t _Buf_capacity) noexcept {
        _Ptr_      = _Buf_data;
        _Capacity_ = _Buf_capacity;
    }

    virtual void _Grow(size_t _Capacity) = 0;

public:
    using value_type = _Ty;

    _Fmt_buffer(const _Fmt_buffer&) = delete;
    void operator=(const _Fmt_buffer&) = delete;

    _NODISCARD _Ty* begin() noexcept {
        return _Ptr_;
    }

    _NODISCARD _Ty* end() noexcept {
        return _Ptr_ + _Size_;
    }

    _NODISCARD size_t _Size() const noexcept {
        return _Size_;
    }

    _NODISCARD size_t _Capacity() const noexcept {
        return _Capacity_;
    }

    void _Clear() noexcept {
        _Size_ = 0;
    }

    void _Try_resize(const size_t _Count) {
        _Try_reserve(_Count);
        _Size_ = _Count <= _Capacity_ ? _Count : _Capacity_;
    }

    void _Try_reserve(const size_t _New_capacity) {
        if (_New_capacity > _Capacity_) {
            _Grow(_New_capacity);
        }
    }

    void push_back(const _Ty _Value) {
        _Try_reserve(_Size_ + 1);
        _Ptr_[_Size_++] = _Value;
    }
};

struct _Fmt_buffer_traits {
    explicit _Fmt_buffer_traits(ptrdiff_t) {}

    _NODISCARD size_t _Count() const noexcept {
        return 0;
    }

    _NODISCARD size_t _Limit(const size_t _Size) noexcept {
        return _Size;
    }
};

class _Fmt_fixed_buffer_traits {
private:
    ptrdiff_t _Count_ = 0;
    ptrdiff_t _Limit_;

public:
    explicit _Fmt_fixed_buffer_traits(const ptrdiff_t _Limit) noexcept : _Limit_(_Limit) {}

    _NODISCARD size_t _Count() const noexcept {
        return static_cast<size_t>(_Count_);
    }

    _NODISCARD size_t _Limit(const size_t _Size) noexcept {
        size_t _Avail = static_cast<size_t>(_Limit_ > _Count_ ? _Limit_ - _Count_ : 0);
        _Count_ += _Size;
        return _Size < _Avail ? _Size : _Avail;
    }
};

inline constexpr size_t _Fmt_buffer_size = 256;

template <class _Iterator>
struct _Back_insert_iterator_container_type {
    using type = void;
};

template <class _Container>
struct _Back_insert_iterator_container_type<back_insert_iterator<_Container>> {
    using type = _Container;
};

template <class _Container>
struct _Back_insert_iterator_container_access : back_insert_iterator<_Container> {
    explicit _Back_insert_iterator_container_access(back_insert_iterator<_Container> _Iter)
        : back_insert_iterator<_Container>(_Iter) {}

    using back_insert_iterator<_Container>::container;
};

template <class _OutputIt, class _Ty, class _Traits = _Fmt_buffer_traits>
class _Fmt_iterator_buffer final : public _Traits, public _Fmt_buffer<_Ty> {
private:
    _OutputIt _Output;
    _Ty _Data[_Fmt_buffer_size];

    void _Grow(size_t) final {
        if (this->_Size() == _Fmt_buffer_size) {
            _Flush();
        }
    }

    void _Flush() {
        auto _Size = this->_Size();
        this->_Clear();
        const auto _End = _Data + this->_Limit(_Size);

        // extracts back_insert_iterator's underlying container type, or void if not.
        using _Container = typename _Back_insert_iterator_container_type<_OutputIt>::type;
        if constexpr (_Is_specialization_v<_Container, basic_string> || _Is_specialization_v<_Container, vector>) {
            auto& _Cont = *_Back_insert_iterator_container_access<_Container>{_Output}.container;
            _Cont.insert(_Cont.end(), _Data, _End);
        } else {
            _Output = _STD _Copy_unchecked(_Data, _End, _STD move(_Output));
        }
    }

public:
    explicit _Fmt_iterator_buffer(_OutputIt _Out, ptrdiff_t _Size = _Fmt_buffer_size)
        : _Traits(_Size), _Fmt_buffer<_Ty>(_Data, 0, _Fmt_buffer_size), _Output(_STD move(_Out)) {}

    ~_Fmt_iterator_buffer() {
        if (this->_Size() != 0) {
            _Flush();
        }
    }

    _NODISCARD _OutputIt _Out() {
        _Flush();
        return _STD move(_Output);
    }

    _NODISCARD ptrdiff_t _Count() const noexcept {
        return static_cast<ptrdiff_t>(_Traits::_Count() + this->_Size());
    }
};

template <class _Ty>
class _Fmt_iterator_buffer<_Ty*, _Ty> final : public _Fmt_buffer<_Ty> {
private:
    void _Grow(size_t) final {}

public:
    explicit _Fmt_iterator_buffer(_Ty* _Out, ptrdiff_t = 0) : _Fmt_buffer<_Ty>(_Out, 0, ~size_t{}) {}

    _NODISCARD _Ty* _Out() noexcept {
        return this->end();
    }
};

template <class _Ty>
class _Fmt_counting_buffer final : public _Fmt_buffer<_Ty> {
private:
    _Ty _Data[_Fmt_buffer_size];
    size_t _Count_ = 0;

    void _Grow(size_t) final {
        if (this->_Size() != _Fmt_buffer_size) {
            return;
        }
        _Count_ += this->_Size();
        this->_Clear();
    }

public:
    _Fmt_counting_buffer() : _Fmt_buffer<_Ty>(_Data, 0, _Fmt_buffer_size) {}

    _NODISCARD size_t _Count() const noexcept {
        return _Count_ + this->_Size();
    }
};

using _Fmt_it  = back_insert_iterator<_Fmt_buffer<char>>;
using _Fmt_wit = back_insert_iterator<_Fmt_buffer<wchar_t>>;

using format_context  = basic_format_context<_Fmt_it, char>;
using wformat_context = basic_format_context<_Fmt_wit, wchar_t>;

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, monostate) {
    _STL_INTERNAL_CHECK(false);
    return _Out;
}

// This size is derived from the maximum length of an arithmetic type. The contenders for widest are:
// (a) long long has a max length of 20 characters: LLONG_MIN is "-9223372036854775807".
// (b) unsigned long long has a max length of 20 characters: ULLONG_MAX is "18446744073709551615".
// (c) double has a max length of 24 characters: -DBL_MAX is "-1.7976931348623158e+308".
// That's 17 characters for numeric_limits<double>::max_digits10,
// plus 1 character for the sign,
// plus 1 character for the decimal point,
// plus 1 character for 'e',
// plus 1 character for the exponent's sign,
// plus 3 characters for the max exponent.
inline constexpr size_t _Format_min_buffer_length = 24;

// clang-format off
template <class _CharT, class _OutputIt, class _Arithmetic>
    requires (is_arithmetic_v<_Arithmetic> && !_CharT_or_bool<_Arithmetic, _CharT>)
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, _Arithmetic _Value);
// clang-format on

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, bool _Value);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, _CharT _Value);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const void* _Value);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const _CharT* _Value);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, basic_string_view<_CharT> _Value);

#pragma warning(push)
#pragma warning(disable : 4365) // 'argument': conversion from 'char' to 'const wchar_t', signed/unsigned mismatch
// clang-format off
template <class _CharT, class _OutputIt, class _Arithmetic>
    requires (is_arithmetic_v<_Arithmetic> && !_CharT_or_bool<_Arithmetic, _CharT>)
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const _Arithmetic _Value) {
    // clang-format on
    // TRANSITION, Reusable buffer
    char _Buffer[_Format_min_buffer_length];
    const auto [_End, _Ec] = _STD to_chars(_Buffer, _STD end(_Buffer), _Value);
    _STL_ASSERT(_Ec == errc{}, "to_chars failed");
    return _RANGES _Copy_unchecked(_Buffer, _End, _STD move(_Out)).out;
}
#pragma warning(pop)

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const bool _Value) {
    if constexpr (is_same_v<_CharT, wchar_t>) {
        return _Fmt_write(_STD move(_Out), _Value ? L"true" : L"false");
    } else {
        return _Fmt_write(_STD move(_Out), _Value ? "true" : "false");
    }
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const _CharT _Value) {
    *_Out++ = _Value;
    return _Out;
}

#pragma warning(push)
#pragma warning(disable : 4365) // 'argument': conversion from 'char' to 'const wchar_t', signed/unsigned mismatch
template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const void* const _Value) {
    // TRANSITION, Reusable buffer
    char _Buffer[_Format_min_buffer_length];
    const auto [_End, _Ec] = _STD to_chars(_Buffer, _STD end(_Buffer), reinterpret_cast<uintptr_t>(_Value), 16);
    _STL_ASSERT(_Ec == errc{}, "to_chars failed");
    *_Out++ = '0';
    *_Out++ = 'x';
    return _RANGES _Copy_unchecked(_Buffer, _End, _STD move(_Out)).out;
}
#pragma warning(pop)

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const _CharT* _Value) {
    if (!_Value) {
        _THROW(format_error("String pointer is null."));
    }

    while (*_Value) {
        *_Out++ = *_Value++;
    }

    return _Out;
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, const basic_string_view<_CharT> _Value) {
    return _RANGES copy(_Value, _STD move(_Out)).out;
}

template <class _OutputIt, class _Specs_type, class _Func>
_NODISCARD _OutputIt _Write_aligned(
    _OutputIt _Out, const int _Width, const _Specs_type& _Specs, const _Fmt_align _Default_align, _Func&& _Fn) {
    int _Fill_left  = 0;
    int _Fill_right = 0;
    auto _Alignment = _Specs._Alignment;

    if (_Alignment == _Fmt_align::_None) {
        _Alignment = _Default_align;
    }

    if (_Width < _Specs._Width) {
        switch (_Alignment) {
        case _Fmt_align::_Left:
            _Fill_right = _Specs._Width - _Width;
            break;
        case _Fmt_align::_Right:
            _Fill_left = _Specs._Width - _Width;
            break;
        case _Fmt_align::_Center:
            _Fill_left  = (_Specs._Width - _Width) / 2;
            _Fill_right = _Specs._Width - _Width - _Fill_left;
            break;
        case _Fmt_align::_None:
            _STL_ASSERT(false, "Invalid alignment");
            break;
        }
    }

    const basic_string_view _Fill_char{_Specs._Fill, _Specs._Fill_length};
    for (; _Fill_left > 0; --_Fill_left) {
        _Out = _RANGES copy(_Fill_char, _STD move(_Out)).out;
    }

    _Out = _Fn(_STD move(_Out));

    for (; _Fill_right > 0; --_Fill_right) {
        _Out = _RANGES copy(_Fill_char, _STD move(_Out)).out;
    }

    return _Out;
}

template <integral _Integral>
_NODISCARD constexpr string_view _Get_integral_prefix(const char _Type, const _Integral _Value) noexcept {
    switch (_Type) {
    case 'b':
        return "0b"sv;
    case 'B':
        return "0B"sv;
    case 'x':
        return "0x"sv;
    case 'X':
        return "0X"sv;
    case 'o':
        if (_Value != _Integral{0}) {
            return "0"sv;
        }
        return {};
    default:
        return {};
    }
}

template <class _OutputIt>
_NODISCARD _OutputIt _Write_sign(_OutputIt _Out, const _Fmt_sign _Sgn, const bool _Is_negative) {
    if (_Is_negative) {
        *_Out++ = '-';
    } else {
        switch (_Sgn) {
        case _Fmt_sign::_Plus:
            *_Out++ = '+';
            break;
        case _Fmt_sign::_Space:
            *_Out++ = ' ';
            break;
        case _Fmt_sign::_None:
        case _Fmt_sign::_Minus:
            break;
        }
    }
    return _Out;
}

inline void _Buffer_to_uppercase(char* _First, const char* _Last) {
    for (; _First != _Last; ++_First) {
        *_First = static_cast<char>(_CSTD toupper(*_First));
    }
}

template <class _Ty>
using _Make_standard_integer = conditional_t<is_signed_v<_Ty>, make_signed_t<_Ty>, make_unsigned_t<_Ty>>;

template <class _CharT, integral _Ty>
_NODISCARD constexpr bool _In_bounds(const _Ty _Value) {
    return _STD in_range<_Make_standard_integer<_CharT>>(static_cast<_Make_standard_integer<_Ty>>(_Value));
}

_NODISCARD inline int _Count_separators(size_t _Digits, const string_view _Groups) {
    if (_Groups.empty()) {
        return 0;
    }

    // Calculate the amount of separators that are going to be inserted based on the groupings of the locale.
    int _Separators = 0;
    auto _Group_it  = _Groups.begin();
    while (_Digits > static_cast<size_t>(*_Group_it)) {
        _Digits -= static_cast<size_t>(*_Group_it);
        ++_Separators;
        if (_Group_it + 1 != _Groups.end()) {
            ++_Group_it;
        }
    }

    return _Separators;
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Write_separated_integer(const char* _First, const char* const _Last, const string_view _Groups,
    const _CharT _Separator, int _Separators, _OutputIt _Out) {
    auto _Group_it = _Groups.begin();
    auto _Repeats  = 0;
    auto _Grouped  = 0;

    for (int _Section = 0; _Section < _Separators; ++_Section) {
        _Grouped += *_Group_it;
        if (_Group_it + 1 != _Groups.end()) {
            ++_Group_it;
        } else {
            ++_Repeats;
        }
    }
    _Out   = _RANGES _Copy_unchecked(_First, _Last - _Grouped, _STD move(_Out)).out;
    _First = _Last - _Grouped;

    for (; _Separators > 0; --_Separators) {
        if (_Repeats > 0) {
            --_Repeats;
        } else {
            --_Group_it;
        }

        *_Out++ = _Separator;
        _Out    = _RANGES _Copy_unchecked(_First, _First + *_Group_it, _STD move(_Out)).out;
        _First += *_Group_it;
    }
    _STL_INTERNAL_CHECK(_First == _Last);
    return _Out;
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, monostate, const _Basic_format_specs<_CharT>&, _Lazy_locale) {
    _STL_INTERNAL_CHECK(false);
    return _Out;
}

template <class _CharT, class _OutputIt, integral _Integral>
_NODISCARD _OutputIt _Write_integral(
    _OutputIt _Out, _Integral _Value, _Basic_format_specs<_CharT> _Specs, _Lazy_locale _Locale);

// clang-format off
template <class _CharT, class _OutputIt, integral _Integral>
    requires (!_CharT_or_bool<_Integral, _CharT>)
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, _Integral _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale _Locale);
// clang-format on

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(_OutputIt _Out, bool _Value, _Basic_format_specs<_CharT> _Specs, _Lazy_locale _Locale);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, _CharT _Value, _Basic_format_specs<_CharT> _Specs, _Lazy_locale _Locale);


template <class _CharT, class _OutputIt, floating_point _Float>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, _Float _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale _Locale);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const void* _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const _CharT* _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale _Locale);

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, basic_string_view<_CharT> _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale);

#pragma warning(push)
#pragma warning(disable : 4365) // 'argument': conversion from 'char' to 'const wchar_t', signed/unsigned mismatch
template <class _CharT, class _OutputIt, integral _Integral>
_NODISCARD _OutputIt _Write_integral(
    _OutputIt _Out, const _Integral _Value, _Basic_format_specs<_CharT> _Specs, _Lazy_locale _Locale) {
    if (_Specs._Type == 'c') {
        if (!_In_bounds<_CharT>(_Value)) {
            _THROW(format_error("integral cannot be stored in charT"));
        }
        _Specs._Alt = false;
        return _Fmt_write(_STD move(_Out), static_cast<_CharT>(_Value), _Specs, _Locale);
    }

    _STL_INTERNAL_CHECK(_Specs._Precision == -1);

    if (_Specs._Sgn == _Fmt_sign::_None) {
        _Specs._Sgn = _Fmt_sign::_Minus;
    }

    int _Base      = 10;
    bool _To_upper = false;

    switch (_Specs._Type) {
    case 'B':
        _To_upper = true;
        [[fallthrough]];
    case 'b':
        _Base = 2;
        break;
    case 'X':
        _To_upper = true;
        [[fallthrough]];
    case 'x':
        _Base = 16;
        break;
    case 'o':
        _Base = 8;
        break;
    }

    // long long -1 representation in binary is 64 bits + sign
    char _Buffer[65];
    const auto [_End, _Ec] = _STD to_chars(_Buffer, _STD end(_Buffer), _Value, _Base);
    _STL_ASSERT(_Ec == errc{}, "to_chars failed");

    auto _Buffer_start = _Buffer;
    auto _Width        = static_cast<int>(_End - _Buffer_start);

    if (_Value >= _Integral{0}) {
        if (_Specs._Sgn != _Fmt_sign::_Minus) {
            _Width += 1;
        }
    } else {
        // Remove the '-', it will be dealt with directly
        _Buffer_start += 1;
    }

    if (_To_upper) {
        _Buffer_to_uppercase(_Buffer_start, _End);
    }

    string_view _Prefix;
    if (_Specs._Alt) {
        _Prefix = _Get_integral_prefix(_Specs._Type, _Value);
        _Width += static_cast<int>(_Prefix.size());
    }

    auto _Separators = 0;
    string _Groups;
    if (_Specs._Localized) {
        _Groups     = _STD use_facet<numpunct<_CharT>>(_Locale._Get()).grouping();
        _Separators = _Count_separators(static_cast<size_t>(_End - _Buffer_start), _Groups);
        // TRANSITION, separators may be wider for wide chars
        _Width += _Separators;
    }

    const bool _Write_leading_zeroes = _Specs._Leading_zero && _Specs._Alignment == _Fmt_align::_None;
    auto _Writer                     = [&, _End = _End](_OutputIt _Out) {
#pragma warning(push)
#pragma warning(disable : 4296) // '<': expression is always false
        _Out = _Write_sign(_STD move(_Out), _Specs._Sgn, _Value < _Integral{0});
#pragma warning(pop)
        _Out = _RANGES _Copy_unchecked(_Prefix.begin(), _Prefix.end(), _STD move(_Out)).out;
        if (_Write_leading_zeroes && _Width < _Specs._Width) {
            _Out = _RANGES fill_n(_STD move(_Out), _Specs._Width - _Width, '0');
        }

        if (_Separators > 0) {
            return _Write_separated_integer(_Buffer_start, _End, _Groups,
                                    _STD use_facet<numpunct<_CharT>>(_Locale._Get()).thousands_sep(), _Separators, _STD move(_Out));
        }
        return _RANGES _Copy_unchecked(_Buffer_start, _End, _STD move(_Out)).out;
    };

    if (_Write_leading_zeroes) {
        return _Writer(_STD move(_Out));
    }

    return _Write_aligned(_STD move(_Out), _Width, _Specs, _Fmt_align::_Right, _Writer);
}
#pragma warning(pop)

// clang-format off
template <class _CharT, class _OutputIt, integral _Integral>
    requires (!_CharT_or_bool<_Integral, _CharT>)
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const _Integral _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale _Locale) {
    // clang-format on
    return _Write_integral(_STD move(_Out), _Value, _Specs, _Locale);
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const bool _Value, _Basic_format_specs<_CharT> _Specs, _Lazy_locale _Locale) {
    if (_Specs._Type != '\0' && _Specs._Type != 's') {
        return _Write_integral(_STD move(_Out), static_cast<unsigned char>(_Value), _Specs, _Locale);
    }

    _STL_INTERNAL_CHECK(_Specs._Precision == -1);

    if (_Specs._Localized) {
        _Specs._Localized  = false;
        const auto& _Facet = _STD use_facet<numpunct<_CharT>>(_Locale._Get());
        return _Fmt_write(_STD move(_Out),
            _Value ? static_cast<basic_string_view<_CharT>>(_Facet.truename())
                   : static_cast<basic_string_view<_CharT>>(_Facet.falsename()),
            _Specs, _Locale);
    }

    if constexpr (is_same_v<_CharT, wchar_t>) {
        return _Fmt_write(_STD move(_Out), _Value ? L"true" : L"false", _Specs, _Locale);
    } else {
        return _Fmt_write(_STD move(_Out), _Value ? "true" : "false", _Specs, _Locale);
    }
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const _CharT _Value, _Basic_format_specs<_CharT> _Specs, _Lazy_locale _Locale) {
    if (_Specs._Type != '\0' && _Specs._Type != 'c') {
        return _Write_integral(_STD move(_Out), _Value, _Specs, _Locale);
    }

    _STL_INTERNAL_CHECK(_Specs._Precision == -1);

    return _Fmt_write(_STD move(_Out), basic_string_view<_CharT>{&_Value, 1}, _Specs, _Locale);
}

#pragma warning(push)
#pragma warning(disable : 4365) // 'argument': conversion from 'char' to 'const wchar_t', signed/unsigned mismatch
template <class _CharT, class _OutputIt, floating_point _Float>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const _Float _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale _Locale) {
    auto _Sgn = _Specs._Sgn;
    if (_Sgn == _Fmt_sign::_None) {
        _Sgn = _Fmt_sign::_Minus;
    }

    auto _To_upper  = false;
    auto _Format    = chars_format::general;
    auto _Exponent  = '\0';
    auto _Precision = _Specs._Precision;

    switch (_Specs._Type) {
    case 'A':
        _To_upper = true;
        [[fallthrough]];
    case 'a':
        _Format   = chars_format::hex;
        _Exponent = 'p';
        break;
    case 'E':
        _To_upper = true;
        [[fallthrough]];
    case 'e':
        if (_Precision == -1) {
            _Precision = 6;
        }
        _Format   = chars_format::scientific;
        _Exponent = 'e';
        break;
    case 'F':
        _To_upper = true;
        [[fallthrough]];
    case 'f':
        if (_Precision == -1) {
            _Precision = 6;
        }
        _Format = chars_format::fixed;
        break;
    case 'G':
        _To_upper = true;
        [[fallthrough]];
    case 'g':
        if (_Precision == -1) {
            _Precision = 6;
        }
        _Format   = chars_format::general;
        _Exponent = 'e';
        break;
    }

    // Consider the powers of 2 in decimal:
    // 2^-1 = 0.5
    // 2^-2 = 0.25
    // 2^-3 = 0.125
    // 2^-4 = 0.0625
    // Each power of 2 consumes one more decimal digit. This is because:
    // 2^-N * 5^-N = 10^-N
    // 2^-N = 10^-N * 5^N
    // Example: 2^-4 = 10^-4 * 5^4 = 0.0001 * 625
    // Therefore, the min subnormal 2^-1074 consumes 1074 digits of precision (digits after the decimal point).
    // We need 3 more characters for a potential negative sign, the zero integer part, and the decimal point.
    // Therefore, the precision can be clamped to 1074.
    // The largest number consumes 309 digits before the decimal point. With a precision of 1074, and it being
    // negative, it would use a buffer of size 1074+309+2. We need to add an additional number to the max
    // exponent to accommodate the ones place.
    constexpr auto _Max_precision = 1074;
    constexpr auto _Buffer_size   = _Max_precision + DBL_MAX_10_EXP + 3;
    char _Buffer[_Buffer_size];
    to_chars_result _Result;

    auto _Extra_precision = 0;
    if (_Precision > _Max_precision) {
        _Extra_precision = _Precision - _Max_precision;
        _Precision       = _Max_precision;
    }

    if (_Precision == -1) {
        _Result = _STD to_chars(_Buffer, _STD end(_Buffer), _Value, _Format);
    } else {
        _Result = _STD to_chars(_Buffer, _STD end(_Buffer), _Value, _Format, _Precision);
    }

    _STL_ASSERT(_Result.ec == errc{}, "to_chars failed");

    auto _Buffer_start = _Buffer;
    auto _Width        = static_cast<int>(_Result.ptr - _Buffer_start);

    const auto _Is_negative = (_STD signbit)(_Value);

    if (_Is_negative) {
        // Remove the '-', it will be dealt with directly
        _Buffer_start += 1;
    } else {
        if (_Sgn != _Fmt_sign::_Minus) {
            _Width += 1;
        }
    }

    if (_To_upper) {
        _Buffer_to_uppercase(_Buffer_start, _Result.ptr);
        _Exponent = static_cast<char>(_CSTD toupper(_Exponent));
    }

    const auto _Is_finite = (_STD isfinite)(_Value);

    auto _Append_decimal   = false;
    auto _Exponent_start   = _Result.ptr;
    auto _Radix_point      = _Result.ptr;
    auto _Integral_end     = _Result.ptr;
    auto _Zeroes_to_append = 0;
    auto _Separators       = 0;
    string _Groups;

    if (_Is_finite) {
        if (_Specs._Alt || _Specs._Localized) {
            for (auto _It = _Buffer_start; _It < _Result.ptr; ++_It) {
                if (*_It == '.') {
                    _Radix_point = _It;
                } else if (*_It == _Exponent) {
                    _Exponent_start = _It;
                }
            }
            _Integral_end = (_STD min)(_Radix_point, _Exponent_start);

            if (_Specs._Alt && _Radix_point == _Result.ptr) {
                // TRANSITION, decimal point may be wider
                ++_Width;
                _Append_decimal = true;
            }

            if (_Specs._Localized) {
                _Groups     = _STD use_facet<numpunct<_CharT>>(_Locale._Get()).grouping();
                _Separators = _Count_separators(static_cast<size_t>(_Integral_end - _Buffer_start), _Groups);
            }
        }

        switch (_Format) {
        case chars_format::hex:
        case chars_format::scientific:
            if (_Extra_precision != 0) {
                // Trailing zeroes are in front of the exponent
                while (*--_Exponent_start != _Exponent) {
                }
            }
            [[fallthrough]];
        case chars_format::fixed:
            _Zeroes_to_append = _Extra_precision;
            break;
        case chars_format::general:
            if (_Specs._Alt) {
                auto _Digits = static_cast<int>(_Exponent_start - _Buffer_start);

                if (!_Append_decimal) {
                    --_Digits;
                }

                _Zeroes_to_append = _Extra_precision + _Precision - _Digits;

                // Leading zeroes are not significant if we used fixed point notation.
                if (_Exponent_start == _Result.ptr && _STD abs(_Value) < 1.0 && _Value != 0.0) {
                    for (auto _It = _Buffer_start; _It < _Result.ptr; ++_It) {
                        if (*_It == '0') {
                            ++_Zeroes_to_append;
                        } else if (*_It != '.') {
                            break;
                        }
                    }
                }
            }
            break;
        default:
            _STL_UNREACHABLE;
        }
    }

    _Width += _Zeroes_to_append;

    const bool _Write_leading_zeroes = _Specs._Leading_zero && _Specs._Alignment == _Fmt_align::_None && _Is_finite;

    auto _Writer = [&](_OutputIt _Out) {
        _Out = _Write_sign(_STD move(_Out), _Sgn, _Is_negative);

        if (_Write_leading_zeroes && _Width < _Specs._Width) {
            _Out = _RANGES fill_n(_STD move(_Out), _Specs._Width - _Width, '0');
        }

        if (_Specs._Localized) {
            const auto& _Facet = _STD use_facet<numpunct<_CharT>>(_Locale._Get());
            _Out               = _Write_separated_integer(
                              _Buffer_start, _Integral_end, _Groups, _Facet.thousands_sep(), _Separators, _STD move(_Out));
            if (_Radix_point != _Result.ptr || _Append_decimal) {
                *_Out++         = _Facet.decimal_point();
                _Append_decimal = false;
            }
            _Buffer_start = _Integral_end;
            if (_Radix_point != _Result.ptr) {
                ++_Buffer_start;
            }
        }

        _Out = _RANGES _Copy_unchecked(_Buffer_start, _Exponent_start, _STD move(_Out)).out;
        if (_Specs._Alt && _Append_decimal) {
            *_Out++ = '.';
        }

        for (; _Zeroes_to_append > 0; --_Zeroes_to_append) {
            *_Out++ = '0';
        }

        return _RANGES _Copy_unchecked(_Exponent_start, _Result.ptr, _STD move(_Out)).out;
    };

    if (_Write_leading_zeroes) {
        return _Writer(_STD move(_Out));
    }

    return _Write_aligned(_STD move(_Out), _Width, _Specs, _Fmt_align::_Right, _Writer);
}
#pragma warning(pop)

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const void* const _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale) {
    _STL_INTERNAL_CHECK(_Specs._Type == '\0' || _Specs._Type == 'p');
    _STL_INTERNAL_CHECK(_Specs._Sgn == _Fmt_sign::_None);
    _STL_INTERNAL_CHECK(!_Specs._Alt);
    _STL_INTERNAL_CHECK(_Specs._Precision == -1);
    _STL_INTERNAL_CHECK(!_Specs._Leading_zero);
    _STL_INTERNAL_CHECK(!_Specs._Localized);

    // Since the bit width of 0 is 0x0, special-case it instead of complicating the math even more.
    int _Width = 3;
    if (_Value != nullptr) {
        // Compute the bit width of the pointer (i.e. how many bits it takes to be represented).
        // Add 3 to the bit width so we always round up on the division.
        // Divide that by the amount of bits a hexit represents (log2(16) = log2(2^4) = 4).
        // Add 2 for the 0x prefix.
        _Width = static_cast<int>(2 + (_STD bit_width(reinterpret_cast<uintptr_t>(_Value)) + 3) / 4);
    }

    return _Write_aligned(_STD move(_Out), _Width, _Specs, _Fmt_align::_Right,
        [=](_OutputIt _Out) { return _Fmt_write<_CharT>(_STD move(_Out), _Value); });
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const _CharT* _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale _Locale) {
    return _Fmt_write(_STD move(_Out), basic_string_view<_CharT>{_Value}, _Specs, _Locale);
}

template <class _CharT>
_NODISCARD const _CharT* _Measure_string_prefix(const basic_string_view<_CharT> _Value, int& _Width) {
    // Returns a pointer past-the-end of the largest prefix of _Value that fits in _Width, or all
    // of _Value if _Width is negative. Updates _Width to the estimated width of that prefix.
    const int _Max_width    = _Width;
    auto _Pos               = _Value.data();
    const auto _Last        = _Pos + _Value.size();
    int _Estimated_width    = 0; // the estimated width of [_Value.data(), _Pos)
    const auto& _Codec      = _Get_fmt_codec<_CharT>();
    constexpr auto _Max_int = (numeric_limits<int>::max)();

    while (_Pos != _Last) {
        if (_Estimated_width == _Max_width && _Max_width >= 0) {
            // We're at our maximum length
            break;
        }

        // TRANSITION, extended grapheme clustering
        const int _Units           = _Codec._Units_in_next_character(_Pos, _Last);
        const int _Character_width = _Codec._Estimate_width(_Pos, _Units);

        if (_Max_int - _Character_width < _Estimated_width) { // avoid overflow
            // Either _Max_width isn't set, or adding this character will exceed it.
            if (_Max_width < 0) { // unset; saturate width estimate and take all characters
                _Estimated_width = _Max_int;
                _Pos             = _Last;
            }
            break;
        }

        _Estimated_width += _Character_width;
        if (_Estimated_width > _Max_width && _Max_width >= 0) {
            // with this character, we exceed the maximum length
            _Estimated_width -= _Character_width;
            break;
        }

        _Pos += _Units;
    }

    _Width = _Estimated_width;
    return _Pos;
}

template <class _CharT, class _OutputIt>
_NODISCARD _OutputIt _Fmt_write(
    _OutputIt _Out, const basic_string_view<_CharT> _Value, const _Basic_format_specs<_CharT>& _Specs, _Lazy_locale) {
    _STL_INTERNAL_CHECK(_Specs._Type == '\0' || _Specs._Type == 'c' || _Specs._Type == 's');
    _STL_INTERNAL_CHECK(_Specs._Sgn == _Fmt_sign::_None);
    _STL_INTERNAL_CHECK(!_Specs._Alt);
    _STL_INTERNAL_CHECK(!_Specs._Leading_zero);

    if (_Specs._Precision < 0 && _Specs._Width <= 0) {
        return _Fmt_write(_STD move(_Out), _Value);
    }

    int _Width          = _Specs._Precision;
    const _CharT* _Last = _Measure_string_prefix(_Value, _Width);

    return _Write_aligned(_STD move(_Out), _Width, _Specs, _Fmt_align::_Left, [=](_OutputIt _Out) {
        return _Fmt_write(_STD move(_Out), basic_string_view<_CharT>{_Value.data(), _Last});
    });
}

// This is the visitor that's used for "simple" replacement fields.
// It could be a generic lambda, but that's bad for throughput.
// A simple replacement field is a replacement field that's just "{}",
// without any format specs.
template <class _OutputIt, class _CharT>
struct _Default_arg_formatter {
    using _Context = basic_format_context<_OutputIt, _CharT>;

    _OutputIt _Out;
    basic_format_args<_Context> _Args;
    _Lazy_locale _Loc;

    template <class _Ty>
    _OutputIt operator()(_Ty _Val) && {
        return _Fmt_write<_CharT>(_STD move(_Out), _Val);
    }

    _OutputIt operator()(typename basic_format_arg<_Context>::handle _Handle) && {
        basic_format_parse_context<_CharT> _Parse_ctx({});
        basic_format_context<_OutputIt, _CharT> _Format_ctx(_STD move(_Out), _Args, _Loc);
        _Handle.format(_Parse_ctx, _Format_ctx);
        return _Format_ctx.out();
    }
};

// Visitor used for replacement fields that contain specs
template <class _OutputIt, class _CharT>
struct _Arg_formatter {
    using _Context = basic_format_context<_OutputIt, _CharT>;

    _Context* _Ctx                      = nullptr;
    _Basic_format_specs<_CharT>* _Specs = nullptr;

    _OutputIt operator()(typename basic_format_arg<_Context>::handle) {
        _STL_VERIFY(false, "The custom handler should be structurally unreachable for _Arg_formatter");
        _STL_INTERNAL_CHECK(_Ctx);
        return _Ctx->out();
    }

    template <class _Ty>
    _OutputIt operator()(_Ty _Val) {
        _STL_INTERNAL_CHECK(_Specs);
        _STL_INTERNAL_CHECK(_Ctx);
        return _Fmt_write(_Ctx->out(), _Val, *_Specs, _Ctx->_Get_lazy_locale());
    }
};

// Special compile time version of _Parse_format_specs. This version is parameterized on
// the type of the argument associated with the format specifier, since we don't really
// care about avoiding code bloat for code that never runs at runtime, and we can't form
// the erased basic_format_args structure at compile time.
template <class _Ty, class _ParseContext>
consteval typename _ParseContext::iterator _Compile_time_parse_format_specs(_ParseContext& _Pc) {
    using _CharT                = typename _ParseContext::char_type;
    using _Context              = basic_format_context<back_insert_iterator<_Fmt_buffer<_CharT>>, _CharT>;
    using _ArgTraits            = _Format_arg_traits<_Context>;
    using _FormattedTypeMapping = typename _ArgTraits::template _Storage_type<_Ty>;
    // If the type is going to use a custom formatter we should just use that,
    // instead of trying to instantiate a custom formatter for its erased handle
    // type
    using _FormattedType = conditional_t<is_same_v<_FormattedTypeMapping, typename basic_format_arg<_Context>::handle>,
        _Ty, _FormattedTypeMapping>;
    formatter<_FormattedType, _CharT> _Formatter{};
    return _Formatter.parse(_Pc);
}

// set of format parsing actions that only checks for validity
template <class _CharT, class... _Args>
struct _Format_checker {
    using _ParseContext = basic_format_parse_context<_CharT>;
    using _ParseFunc    = typename _ParseContext::iterator (*)(_ParseContext&);

    static constexpr size_t _Num_args = sizeof...(_Args);
    _ParseContext _Parse_context;
    _ParseFunc _Parse_funcs[_Num_args > 0 ? _Num_args : 1];

    consteval explicit _Format_checker(basic_string_view<_CharT> _Fmt) noexcept
        : _Parse_context(_Fmt, _Num_args), _Parse_funcs{&_Compile_time_parse_format_specs<_Args, _ParseContext>...} {}
    constexpr void _On_text(const _CharT*, const _CharT*) const noexcept {}
    constexpr void _On_replacement_field(size_t, const _CharT*) const noexcept {}
    constexpr const _CharT* _On_format_specs(const size_t _Id, const _CharT* _First, const _CharT*) {
        _Parse_context.advance_to(_Parse_context.begin() + (_First - _Parse_context.begin()._Unwrapped()));
        if (_Id < _Num_args) {
            auto _Iter = _Parse_funcs[_Id](_Parse_context); // TRANSITION, VSO-1451773 (workaround: named variable)
            return _Iter._Unwrapped();
        } else {
            return _First;
        }
    }
};

// The top level set of parsing "actions".
template <class _CharT>
struct _Format_handler {
    using _OutputIt = back_insert_iterator<_Fmt_buffer<_CharT>>;
    using _Context  = basic_format_context<_OutputIt, _CharT>;

    basic_format_parse_context<_CharT> _Parse_context;
    _Context _Ctx;

    explicit _Format_handler(_OutputIt _Out, basic_string_view<_CharT> _Str, basic_format_args<_Context> _Format_args)
        : _Parse_context(_Str), _Ctx(_STD move(_Out), _Format_args) {}

    explicit _Format_handler(_OutputIt _Out, basic_string_view<_CharT> _Str, basic_format_args<_Context> _Format_args,
        const _Lazy_locale& _Loc)
        : _Parse_context(_Str), _Ctx(_STD move(_Out), _Format_args, _Loc) {}

    void _On_text(const _CharT* _First, const _CharT* _Last) {
        _Ctx.advance_to(_RANGES _Copy_unchecked(_First, _Last, _Ctx.out()).out);
    }

    void _On_replacement_field(const size_t _Id, const _CharT*) {
        auto _Arg = _Get_arg(_Ctx, _Id);
        _Ctx.advance_to(_STD visit_format_arg(
            _Default_arg_formatter<_OutputIt, _CharT>{_Ctx.out(), _Ctx._Get_args(), _Ctx._Get_lazy_locale()}, _Arg));
    }

    const _CharT* _On_format_specs(const size_t _Id, const _CharT* _First, const _CharT* _Last) {
        _Parse_context.advance_to(_Parse_context.begin() + (_First - &*_Parse_context.begin()));
        auto _Arg = _Get_arg(_Ctx, _Id);
        if (_Arg._Active_state == _Basic_format_arg_type::_Custom_type) {
            _Arg._Custom_state.format(_Parse_context, _Ctx);
            return _Parse_context.begin()._Unwrapped();
        }

        _Basic_format_specs<_CharT> _Specs;
        _Specs_checker<_Specs_handler<basic_format_parse_context<_CharT>, _Context>> _Handler(
            _Specs_handler<basic_format_parse_context<_CharT>, _Context>{_Specs, _Parse_context, _Ctx},
            _Arg._Active_state);
        _First = _Parse_format_specs(_First, _Last, _Handler);
        if (_First == _Last || *_First != '}') {
            _THROW(format_error("Missing '}' in format string."));
        }

        _Ctx.advance_to(_STD visit_format_arg(
            _Arg_formatter<_OutputIt, _CharT>{._Ctx = _STD addressof(_Ctx), ._Specs = _STD addressof(_Specs)}, _Arg));
        return _First;
    }
};

// Generic formatter definition, the deleted default constructor
// makes it "disabled" as per N4885 [format.formatter.spec]/5
template <class _Ty, class _CharT>
struct formatter {
    formatter()                 = delete;
    formatter(const formatter&) = delete;
    formatter operator=(const formatter&) = delete;
};

template <class _Ty, class _CharT, _Basic_format_arg_type _ArgType>
struct _Formatter_base {
    using _Pc = basic_format_parse_context<_CharT>;

    constexpr typename _Pc::iterator parse(_Pc& _ParseCtx) {
        _Specs_checker<_Dynamic_specs_handler<_Pc>> _Handler(_Dynamic_specs_handler<_Pc>{_Specs, _ParseCtx}, _ArgType);
        const auto _It = _Parse_format_specs(_ParseCtx._Unchecked_begin(), _ParseCtx._Unchecked_end(), _Handler);
        if (_It != _ParseCtx._Unchecked_end() && *_It != '}') {
            _THROW(format_error("Missing '}' in format string."));
        }
        return _ParseCtx.begin() + (_It - _ParseCtx._Unchecked_begin());
    }

    template <class _FormatContext>
    typename _FormatContext::iterator format(const _Ty& _Val, _FormatContext& _FormatCtx) const {
        _Dynamic_format_specs<_CharT> _Format_specs = _Specs;
        if (_Specs._Dynamic_width_index >= 0) {
            _Format_specs._Width =
                _Get_dynamic_specs<_Width_checker>(_FormatCtx.arg(static_cast<size_t>(_Specs._Dynamic_width_index)));
        }

        if (_Specs._Dynamic_precision_index >= 0) {
            _Format_specs._Precision = _Get_dynamic_specs<_Precision_checker>(
                _FormatCtx.arg(static_cast<size_t>(_Specs._Dynamic_precision_index)));
        }

        return _STD visit_format_arg(
            _Arg_formatter<typename _FormatContext::iterator, _CharT>{
                ._Ctx = _STD addressof(_FormatCtx), ._Specs = _STD addressof(_Format_specs)},
            basic_format_arg<_FormatContext>{_Val});
    }

private:
    _Dynamic_format_specs<_CharT> _Specs;
};

#define _FORMAT_SPECIALIZE_FOR(_Type, _ArgType) \
    template <_Format_supported_charT _CharT>   \
    struct formatter<_Type, _CharT> : _Formatter_base<_Type, _CharT, _ArgType> {}

_FORMAT_SPECIALIZE_FOR(int, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned int, _Basic_format_arg_type::_UInt_type);
_FORMAT_SPECIALIZE_FOR(long long, _Basic_format_arg_type::_Long_long_type);
_FORMAT_SPECIALIZE_FOR(unsigned long long, _Basic_format_arg_type::_ULong_long_type);
_FORMAT_SPECIALIZE_FOR(bool, _Basic_format_arg_type::_Bool_type);
_FORMAT_SPECIALIZE_FOR(float, _Basic_format_arg_type::_Float_type);
_FORMAT_SPECIALIZE_FOR(double, _Basic_format_arg_type::_Double_type);
_FORMAT_SPECIALIZE_FOR(long double, _Basic_format_arg_type::_Long_double_type);
_FORMAT_SPECIALIZE_FOR(nullptr_t, _Basic_format_arg_type::_Pointer_type);
_FORMAT_SPECIALIZE_FOR(void*, _Basic_format_arg_type::_Pointer_type);
_FORMAT_SPECIALIZE_FOR(const void*, _Basic_format_arg_type::_Pointer_type);
_FORMAT_SPECIALIZE_FOR(short, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned short, _Basic_format_arg_type::_UInt_type);
_FORMAT_SPECIALIZE_FOR(long, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned long, _Basic_format_arg_type::_UInt_type);
_FORMAT_SPECIALIZE_FOR(char, _Basic_format_arg_type::_Char_type);
_FORMAT_SPECIALIZE_FOR(signed char, _Basic_format_arg_type::_Int_type);
_FORMAT_SPECIALIZE_FOR(unsigned char, _Basic_format_arg_type::_UInt_type);

#undef _FORMAT_SPECIALIZE_FOR

// not using the macro because we'd like to avoid the formatter<wchar_t, char> specialization
template <>
struct formatter<wchar_t, wchar_t> : _Formatter_base<wchar_t, wchar_t, _Basic_format_arg_type::_Char_type> {};

// We could use the macro for these specializations, but it's confusing to refer to symbols that are defined
// inside the macro in the macro's "call".
template <_Format_supported_charT _CharT>
struct formatter<_CharT*, _CharT> : _Formatter_base<_CharT*, _CharT, _Basic_format_arg_type::_CString_type> {};

template <_Format_supported_charT _CharT>
struct formatter<const _CharT*, _CharT>
    : _Formatter_base<const _CharT*, _CharT, _Basic_format_arg_type::_CString_type> {};

template <_Format_supported_charT _CharT, size_t _Nx>
struct formatter<const _CharT[_Nx], _CharT>
    : _Formatter_base<const _CharT[_Nx], _CharT, _Basic_format_arg_type::_CString_type> {};

template <_Format_supported_charT _CharT, class _Traits, class _Allocator>
struct formatter<basic_string<_CharT, _Traits, _Allocator>, _CharT>
    : _Formatter_base<basic_string<_CharT, _Traits, _Allocator>, _CharT, _Basic_format_arg_type::_String_type> {};

template <_Format_supported_charT _CharT, class _Traits>
struct formatter<basic_string_view<_CharT, _Traits>, _CharT>
    : _Formatter_base<basic_string_view<_CharT, _Traits>, _CharT, _Basic_format_arg_type::_String_type> {};

template <class _CharT, class... _Args>
struct _Basic_format_string {
    basic_string_view<_CharT> _Str;

    template <class _Ty>
    requires convertible_to<const _Ty&, basic_string_view<_CharT>>
    consteval _Basic_format_string(const _Ty& _Str_val) : _Str(_Str_val) {
        if (_Is_execution_charset_self_synchronizing()) {
            _Parse_format_string(_Str, _Format_checker<_CharT, remove_cvref_t<_Args>...>{_Str});
        }
    }
};

template <class... _Args>
using _Fmt_string = _Basic_format_string<char, type_identity_t<_Args>...>;

template <class... _Args>
using _Fmt_wstring = _Basic_format_string<wchar_t, type_identity_t<_Args>...>;

using format_args  = basic_format_args<format_context>;
using wformat_args = basic_format_args<wformat_context>;

template <class _Context = format_context, class... _Args>
_NODISCARD auto make_format_args(_Args&&... _Vals) {
    return _Format_arg_store<_Context, _Args...>{_Vals...};
}

template <class... _Args>
_NODISCARD auto make_wformat_args(_Args&&... _Vals) {
    return _Format_arg_store<wformat_context, _Args...>{_Vals...};
}

template <output_iterator<const char&> _OutputIt>
_OutputIt vformat_to(_OutputIt _Out, const string_view _Fmt, const format_args _Args) {
    if constexpr (is_same_v<_OutputIt, _Fmt_it>) {
        _Format_handler<char> _Handler(_Out, _Fmt, _Args);
        _Parse_format_string(_Fmt, _Handler);
        return _Out;
    } else {
        _Fmt_iterator_buffer<_OutputIt, char> _Buf(_STD move(_Out));
        _Format_handler<char> _Handler(_Fmt_it{_Buf}, _Fmt, _Args);
        _Parse_format_string(_Fmt, _Handler);
        return _Buf._Out();
    }
}

template <output_iterator<const wchar_t&> _OutputIt>
_OutputIt vformat_to(_OutputIt _Out, const wstring_view _Fmt, const wformat_args _Args) {
    if constexpr (is_same_v<_OutputIt, _Fmt_wit>) {
        _Format_handler<wchar_t> _Handler(_Out, _Fmt, _Args);
        _Parse_format_string(_Fmt, _Handler);
        return _Out;
    } else {
        _Fmt_iterator_buffer<_OutputIt, wchar_t> _Buf(_STD move(_Out));
        _Format_handler<wchar_t> _Handler(_Fmt_wit{_Buf}, _Fmt, _Args);
        _Parse_format_string(_Fmt, _Handler);
        return _Buf._Out();
    }
}

template <output_iterator<const char&> _OutputIt>
_OutputIt vformat_to(_OutputIt _Out, const locale& _Loc, const string_view _Fmt, const format_args _Args) {
    if constexpr (is_same_v<_OutputIt, _Fmt_it>) {
        _Format_handler<char> _Handler(_Out, _Fmt, _Args, _Lazy_locale{_Loc});
        _Parse_format_string(_Fmt, _Handler);
        return _Out;
    } else {
        _Fmt_iterator_buffer<_OutputIt, char> _Buf(_STD move(_Out));
        _Format_handler<char> _Handler(_Fmt_it{_Buf}, _Fmt, _Args, _Lazy_locale{_Loc});
        _Parse_format_string(_Fmt, _Handler);
        return _Buf._Out();
    }
}

template <output_iterator<const wchar_t&> _OutputIt>
_OutputIt vformat_to(_OutputIt _Out, const locale& _Loc, const wstring_view _Fmt, const wformat_args _Args) {
    if constexpr (is_same_v<_OutputIt, _Fmt_wit>) {
        _Format_handler<wchar_t> _Handler(_Out, _Fmt, _Args, _Lazy_locale{_Loc});
        _Parse_format_string(_Fmt, _Handler);
        return _Out;
    } else {
        _Fmt_iterator_buffer<_OutputIt, wchar_t> _Buf(_STD move(_Out));
        _Format_handler<wchar_t> _Handler(_Fmt_wit{_Buf}, _Fmt, _Args, _Lazy_locale{_Loc});
        _Parse_format_string(_Fmt, _Handler);
        return _Buf._Out();
    }
}

template <output_iterator<const char&> _OutputIt, class... _Types>
_OutputIt format_to(_OutputIt _Out, const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat_to(_STD move(_Out), _Fmt._Str, _STD make_format_args(_Args...));
}

template <output_iterator<const wchar_t&> _OutputIt, class... _Types>
_OutputIt format_to(_OutputIt _Out, const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat_to(_STD move(_Out), _Fmt._Str, _STD make_wformat_args(_Args...));
}

template <output_iterator<const char&> _OutputIt, class... _Types>
_OutputIt format_to(_OutputIt _Out, const locale& _Loc, const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat_to(_STD move(_Out), _Loc, _Fmt._Str, _STD make_format_args(_Args...));
}

template <output_iterator<const wchar_t&> _OutputIt, class... _Types>
_OutputIt format_to(_OutputIt _Out, const locale& _Loc, const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat_to(_STD move(_Out), _Loc, _Fmt._Str, _STD make_wformat_args(_Args...));
}

#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-1433873
#define _TEMPLATE_INT_0_NODISCARD                             \
    template <int = 0> /* improves throughput, see GH-2329 */ \
    _NODISCARD
#else // ^^^ no workaround / workaround vvv
#define _TEMPLATE_INT_0_NODISCARD _NODISCARD inline
#endif // ^^^ workaround ^^^

_TEMPLATE_INT_0_NODISCARD
string vformat(const string_view _Fmt, const format_args _Args) {
    string _Str;
    _Str.reserve(_Fmt.size() + _Args._Estimate_required_capacity());
    _STD vformat_to(back_insert_iterator{_Str}, _Fmt, _Args);
    return _Str;
}

_TEMPLATE_INT_0_NODISCARD
wstring vformat(const wstring_view _Fmt, const wformat_args _Args) {
    wstring _Str;
    _Str.reserve(_Fmt.size() + _Args._Estimate_required_capacity());
    _STD vformat_to(back_insert_iterator{_Str}, _Fmt, _Args);
    return _Str;
}

_TEMPLATE_INT_0_NODISCARD
string vformat(const locale& _Loc, const string_view _Fmt, const format_args _Args) {
    string _Str;
    _Str.reserve(_Fmt.size() + _Args._Estimate_required_capacity());
    _STD vformat_to(back_insert_iterator{_Str}, _Loc, _Fmt, _Args);
    return _Str;
}

_TEMPLATE_INT_0_NODISCARD
wstring vformat(const locale& _Loc, const wstring_view _Fmt, const wformat_args _Args) {
    wstring _Str;
    _Str.reserve(_Fmt.size() + _Args._Estimate_required_capacity());
    _STD vformat_to(back_insert_iterator{_Str}, _Loc, _Fmt, _Args);
    return _Str;
}

#undef _TEMPLATE_INT_0_NODISCARD // TRANSITION, VSO-1433873

template <class... _Types>
_NODISCARD string format(const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat(_Fmt._Str, _STD make_format_args(_Args...));
}

template <class... _Types>
_NODISCARD wstring format(const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat(_Fmt._Str, _STD make_wformat_args(_Args...));
}

template <class... _Types>
_NODISCARD string format(const locale& _Loc, const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat(_Loc, _Fmt._Str, _STD make_format_args(_Args...));
}

template <class... _Types>
_NODISCARD wstring format(const locale& _Loc, const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    return _STD vformat(_Loc, _Fmt._Str, _STD make_wformat_args(_Args...));
}

template <class _OutputIt>
struct format_to_n_result {
    _OutputIt out;
    iter_difference_t<_OutputIt> size;
};

template <output_iterator<const char&> _OutputIt, class... _Types>
format_to_n_result<_OutputIt> format_to_n(
    _OutputIt _Out, const iter_difference_t<_OutputIt> _Max, const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_iterator_buffer<_OutputIt, char, _Fmt_fixed_buffer_traits> _Buf(_STD move(_Out), _Max);
    _STD vformat_to(_Fmt_it{_Buf}, _Fmt._Str, _STD make_format_args(_Args...));
    return {.out = _Buf._Out(), .size = _Buf._Count()};
}

template <output_iterator<const wchar_t&> _OutputIt, class... _Types>
format_to_n_result<_OutputIt> format_to_n(
    _OutputIt _Out, const iter_difference_t<_OutputIt> _Max, const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_iterator_buffer<_OutputIt, wchar_t, _Fmt_fixed_buffer_traits> _Buf(_STD move(_Out), _Max);
    _STD vformat_to(_Fmt_wit{_Buf}, _Fmt._Str, _STD make_wformat_args(_Args...));
    return {.out = _Buf._Out(), .size = _Buf._Count()};
}

template <output_iterator<const char&> _OutputIt, class... _Types>
format_to_n_result<_OutputIt> format_to_n(_OutputIt _Out, const iter_difference_t<_OutputIt> _Max, const locale& _Loc,
    const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_iterator_buffer<_OutputIt, char, _Fmt_fixed_buffer_traits> _Buf(_STD move(_Out), _Max);
    _STD vformat_to(_Fmt_it{_Buf}, _Loc, _Fmt._Str, _STD make_format_args(_Args...));
    return {.out = _Buf._Out(), .size = _Buf._Count()};
}

template <output_iterator<const wchar_t&> _OutputIt, class... _Types>
format_to_n_result<_OutputIt> format_to_n(_OutputIt _Out, const iter_difference_t<_OutputIt> _Max, const locale& _Loc,
    const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_iterator_buffer<_OutputIt, wchar_t, _Fmt_fixed_buffer_traits> _Buf(_STD move(_Out), _Max);
    _STD vformat_to(_Fmt_wit{_Buf}, _Loc, _Fmt._Str, _STD make_wformat_args(_Args...));
    return {.out = _Buf._Out(), .size = _Buf._Count()};
}

template <class... _Types>
_NODISCARD size_t formatted_size(const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_counting_buffer<char> _Buf;
    _STD vformat_to(_Fmt_it{_Buf}, _Fmt._Str, _STD make_format_args(_Args...));
    return _Buf._Count();
}

template <class... _Types>
_NODISCARD size_t formatted_size(const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_counting_buffer<wchar_t> _Buf;
    _STD vformat_to(_Fmt_wit{_Buf}, _Fmt._Str, _STD make_wformat_args(_Args...));
    return _Buf._Count();
}

template <class... _Types>
_NODISCARD size_t formatted_size(const locale& _Loc, const _Fmt_string<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_counting_buffer<char> _Buf;
    _STD vformat_to(_Fmt_it{_Buf}, _Loc, _Fmt._Str, _STD make_format_args(_Args...));
    return _Buf._Count();
}

template <class... _Types>
_NODISCARD size_t formatted_size(const locale& _Loc, const _Fmt_wstring<_Types...> _Fmt, _Types&&... _Args) {
    _Fmt_counting_buffer<wchar_t> _Buf;
    _STD vformat_to(_Fmt_wit{_Buf}, _Loc, _Fmt._Str, _STD make_wformat_args(_Args...));
    return _Buf._Count();
}

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // __cpp_lib_format
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _FORMAT_
