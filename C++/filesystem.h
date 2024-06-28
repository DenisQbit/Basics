// filesystem standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _FILESYSTEM_
#define _FILESYSTEM_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#if !_HAS_CXX17
#pragma message("The contents of <filesystem> are available only with C++17 or later.")
#else // ^^^ !_HAS_CXX17 / _HAS_CXX17 vvv
#include <algorithm>
#include <chrono>
#include <cwchar>
#include <iomanip>
#include <list>
#include <locale>
#include <memory>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include <xfilesystem_abi.h>
#include <xstring>

#if _HAS_CXX20
#include <compare>
#endif // _HAS_CXX20

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
namespace filesystem {
    // We would really love to use the proper way of building error_code by specializing
    // is_error_code_enum and make_error_code for __std_win_error, but because:
    //   1. We would like to keep the definition of __std_win_error in xfilesystem_abi.h
    //   2. and xfilesystem_abi.h cannot include <system_error>
    //   3. and specialization of is_error_code_enum and overload of make_error_code
    //      need to be kept together with the enum (see limerick in N4810 [temp.expl.spec]/7)
    // we resort to using this _Make_ec helper.
    _NODISCARD inline error_code _Make_ec(__std_win_error _Errno) noexcept { // make an error_code
        return {static_cast<int>(_Errno), _STD system_category()};
    }

    [[noreturn]] inline void _Throw_system_error_from_std_win_error(const __std_win_error _Errno) {
        _THROW(system_error{_Make_ec(_Errno)});
    }

    _NODISCARD inline int _Check_convert_result(const __std_fs_convert_result _Result) {
        if (_Result._Err != __std_win_error::_Success) {
            _Throw_system_error_from_std_win_error(_Result._Err);
        }

        return _Result._Len;
    }

    _NODISCARD inline wstring _Convert_narrow_to_wide(const __std_code_page _Code_page, const string_view _Input) {
        wstring _Output;

        if (!_Input.empty()) {
            if (_Input.size() > static_cast<size_t>(INT_MAX)) {
                _Throw_system_error(errc::invalid_argument);
            }

            const int _Len = _Check_convert_result(__std_fs_convert_narrow_to_wide(
                _Code_page, _Input.data(), static_cast<int>(_Input.size()), nullptr, 0));

            _Output.resize(static_cast<size_t>(_Len));

            (void) _Check_convert_result(__std_fs_convert_narrow_to_wide(
                _Code_page, _Input.data(), static_cast<int>(_Input.size()), _Output.data(), _Len));
        }

        return _Output;
    }

    template <class _Traits, class _Alloc>
    _NODISCARD basic_string<typename _Traits::char_type, _Traits, _Alloc> _Convert_wide_to_narrow(
        const __std_code_page _Code_page, const wstring_view _Input, const _Alloc& _Al) {
        basic_string<typename _Traits::char_type, _Traits, _Alloc> _Output(_Al);

        if (!_Input.empty()) {
            if (_Input.size() > static_cast<size_t>(INT_MAX)) {
                _Throw_system_error(errc::invalid_argument);
            }

            const int _Len = _Check_convert_result(__std_fs_convert_wide_to_narrow(
                _Code_page, _Input.data(), static_cast<int>(_Input.size()), nullptr, 0));

            _Output.resize(static_cast<size_t>(_Len));

            const auto _Data_as_char = reinterpret_cast<char*>(_Output.data());

            (void) _Check_convert_result(__std_fs_convert_wide_to_narrow(
                _Code_page, _Input.data(), static_cast<int>(_Input.size()), _Data_as_char, _Len));
        }

        return _Output;
    }

    // More lenient version of _Convert_wide_to_narrow: Instead of failing on non-representable characters,
    // replace them with a replacement character.
    template <class _Traits, class _Alloc>
    _NODISCARD basic_string<typename _Traits::char_type, _Traits, _Alloc> _Convert_wide_to_narrow_replace_chars(
        const __std_code_page _Code_page, const wstring_view _Input, const _Alloc& _Al) {
        basic_string<typename _Traits::char_type, _Traits, _Alloc> _Output(_Al);

        if (!_Input.empty()) {
            if (_Input.size() > static_cast<size_t>(INT_MAX)) {
                _Throw_system_error(errc::invalid_argument);
            }

            const int _Len = _Check_convert_result(__std_fs_convert_wide_to_narrow_replace_chars(
                _Code_page, _Input.data(), static_cast<int>(_Input.size()), nullptr, 0));

            _Output.resize(static_cast<size_t>(_Len));

            const auto _Data_as_char = reinterpret_cast<char*>(_Output.data());

            (void) _Check_convert_result(__std_fs_convert_wide_to_narrow_replace_chars(
                _Code_page, _Input.data(), static_cast<int>(_Input.size()), _Data_as_char, _Len));
        }

        return _Output;
    }

    _NODISCARD inline wstring _Convert_utf32_to_wide(const u32string_view _Input) {
        wstring _Output;

        _Output.reserve(_Input.size()); // ideal when surrogate pairs are uncommon

        for (const auto& _Code_point : _Input) {
            if (_Code_point <= 0xD7FFU) {
                _Output.push_back(static_cast<wchar_t>(_Code_point));
            } else if (_Code_point <= 0xDFFFU) {
                _Throw_system_error(errc::invalid_argument);
            } else if (_Code_point <= 0xFFFFU) {
                _Output.push_back(static_cast<wchar_t>(_Code_point));
            } else if (_Code_point <= 0x10FFFFU) {
                _Output.push_back(static_cast<wchar_t>(0xD7C0U + (_Code_point >> 10)));
                _Output.push_back(static_cast<wchar_t>(0xDC00U + (_Code_point & 0x3FFU)));
            } else {
                _Throw_system_error(errc::invalid_argument);
            }
        }

        return _Output;
    }

    template <class _Traits, class _Alloc>
    _NODISCARD basic_string<char32_t, _Traits, _Alloc> _Convert_wide_to_utf32(
        const wstring_view _Input, const _Alloc& _Al) {
        basic_string<char32_t, _Traits, _Alloc> _Output(_Al);

        _Output.reserve(_Input.size()); // ideal when surrogate pairs are uncommon

        const wchar_t* _First      = _Input.data();
        const wchar_t* const _Last = _First + _Input.size();

        for (; _First != _Last; ++_First) {
            if (*_First <= 0xD7FFU) {
                _Output.push_back(*_First);
            } else if (*_First <= 0xDBFFU) { // found leading surrogate
                const char32_t _Leading = *_First; // widen for later math

                ++_First;

                if (_First == _Last) { // missing trailing surrogate
                    _Throw_system_error(errc::invalid_argument);
                }

                const char32_t _Trailing = *_First; // widen for later math

                if (0xDC00U <= _Trailing && _Trailing <= 0xDFFFU) { // valid trailing surrogate
                    _Output.push_back(0xFCA02400U + (_Leading << 10) + _Trailing);
                } else { // invalid trailing surrogate
                    _Throw_system_error(errc::invalid_argument);
                }
            } else if (*_First <= 0xDFFFU) { // found trailing surrogate by itself, invalid
                _Throw_system_error(errc::invalid_argument);
            } else {
                _Output.push_back(*_First);
            }
        }

        return _Output;
    }

    template <class _Traits, class _Alloc, class _EcharT = typename _Traits::char_type>
    _NODISCARD basic_string<_EcharT, _Traits, _Alloc> _Convert_wide_to(const wstring_view _Input, const _Alloc& _Al) {
        if constexpr (is_same_v<_EcharT, char>) {
            return _Convert_wide_to_narrow<_Traits>(__std_fs_code_page(), _Input, _Al);
        }
#ifdef __cpp_char8_t
        else if constexpr (is_same_v<_EcharT, char8_t>) {
            return _Convert_wide_to_narrow<_Traits>(__std_code_page::_Utf8, _Input, _Al);
        }
#endif // __cpp_char8_t
        else if constexpr (is_same_v<_EcharT, char32_t>) {
            return _Convert_wide_to_utf32<_Traits>(_Input, _Al);
        } else { // wchar_t, char16_t
            return basic_string<_EcharT, _Traits, _Alloc>(_Input.data(), _Input.data() + _Input.size(), _Al);
        }
    }

    template <class>
    inline constexpr bool _Is_EcharT = false;
    template <>
    inline constexpr bool _Is_EcharT<char> = true;
    template <>
    inline constexpr bool _Is_EcharT<wchar_t> = true;
#ifdef __cpp_char8_t
    template <>
    inline constexpr bool _Is_EcharT<char8_t> = true;
#endif // __cpp_char8_t
    template <>
    inline constexpr bool _Is_EcharT<char16_t> = true;
    template <>
    inline constexpr bool _Is_EcharT<char32_t> = true;

    template <class _Ty, class = void>
    inline constexpr bool _Is_Source_impl = false;

    template <class _Ty>
    inline constexpr bool _Is_Source_impl<_Ty, void_t<typename iterator_traits<_Ty>::value_type>> =
        _Is_EcharT<typename iterator_traits<_Ty>::value_type>;

    template <class _Ty>
    inline constexpr bool _Is_Source = _Is_Source_impl<decay_t<_Ty>>;

    class path;

    template <>
    inline constexpr bool _Is_Source<path> = false; // to avoid constraint recursion via the converting constructor and
                                                    // iterator_traits when determining if path is copyable.

    template <class _Elem, class _Traits, class _Alloc>
    inline constexpr bool _Is_Source<basic_string<_Elem, _Traits, _Alloc>> = _Is_EcharT<_Elem>;

    template <class _Elem, class _Traits>
    inline constexpr bool _Is_Source<basic_string_view<_Elem, _Traits>> = _Is_EcharT<_Elem>;

    struct _Normal_conversion {};

    struct _Utf8_conversion {};

    // A "stringoid" is basic_string_view<_EcharT> or basic_string<_EcharT>.

    template <class _Conversion>
    _NODISCARD wstring _Convert_stringoid_to_wide(const string_view _Input, _Conversion) {
        static_assert(_Is_any_of_v<_Conversion, _Normal_conversion, _Utf8_conversion>);

        if constexpr (is_same_v<_Conversion, _Normal_conversion>) {
            return _Convert_narrow_to_wide(__std_fs_code_page(), _Input);
        } else {
            return _Convert_narrow_to_wide(__std_code_page::_Utf8, _Input);
        }
    }

    template <class _Conversion>
    _NODISCARD wstring _Convert_stringoid_to_wide(const wstring_view _Input, _Conversion) {
        static_assert(
            is_same_v<_Conversion, _Normal_conversion>, "invalid value_type, see N4810 D.17 [depr.fs.path.factory]/1");
        return wstring(_Input);
    }

#ifdef __cpp_char8_t
    template <class _Conversion>
    _NODISCARD wstring _Convert_stringoid_to_wide(const basic_string_view<char8_t> _Input, _Conversion) {
        static_assert(
            is_same_v<_Conversion, _Normal_conversion>, "invalid value_type, see N4810 D.17 [depr.fs.path.factory]/1");
        const string_view _Input_as_char{reinterpret_cast<const char*>(_Input.data()), _Input.size()};
        return _Convert_narrow_to_wide(__std_code_page::_Utf8, _Input_as_char);
    }
#endif // __cpp_char8_t

    template <class _Conversion>
    _NODISCARD wstring _Convert_stringoid_to_wide(const u16string_view _Input, _Conversion) {
        static_assert(
            is_same_v<_Conversion, _Normal_conversion>, "invalid value_type, see N4810 D.17 [depr.fs.path.factory]/1");
        return wstring(_Input.data(), _Input.data() + _Input.size());
    }

    template <class _Conversion>
    _NODISCARD wstring _Convert_stringoid_to_wide(const u32string_view _Input, _Conversion) {
        static_assert(
            is_same_v<_Conversion, _Normal_conversion>, "invalid value_type, see N4810 D.17 [depr.fs.path.factory]/1");
        return _Convert_utf32_to_wide(_Input);
    }

    template <class _EcharT, class _Traits>
    _NODISCARD auto _Stringoid_from_Source(const basic_string_view<_EcharT, _Traits>& _Source) {
        return basic_string_view<_EcharT>(_Source.data(), _Source.size()); // erase mismatching _Traits
    }

    template <class _EcharT, class _Traits, class _Alloc>
    _NODISCARD auto _Stringoid_from_Source(const basic_string<_EcharT, _Traits, _Alloc>& _Source) {
        return basic_string_view<_EcharT>(_Source.data(), _Source.size()); // erase mismatching _Traits
    }

    template <class _Src>
    _NODISCARD auto _Stringoid_from_Source(const _Src& _Source) {
        using _EcharT = _Iter_value_t<decay_t<_Src>>;
        if constexpr (is_pointer_v<_Unwrapped_unverified_t<const _Src&>>) {
            return basic_string_view<_EcharT>(_Get_unwrapped_unverified(_Source));
        } else if constexpr (is_pointer_v<_Unwrapped_t<const _Src&>>) {
            const auto _Data = _Get_unwrapped(_Source);
            auto _Next       = _Source;
            while (*_Next != _EcharT{}) {
                ++_Next;
            }

            return basic_string_view<_EcharT>(_Data, static_cast<size_t>(_Get_unwrapped(_Next) - _Data));
        } else {
            basic_string<_EcharT> _Str;
            for (auto _Next = _Source; *_Next != _EcharT{}; ++_Next) {
                _Str.push_back(*_Next);
            }

            return _Str;
        }
    }

#if _ITERATOR_DEBUG_LEVEL == 2
    template <class _EcharT, size_t _SourceSize>
    _NODISCARD basic_string_view<_EcharT> _Stringoid_from_Source(const _EcharT (&_Src)[_SourceSize]) {
        for (size_t _Idx = 0;; ++_Idx) {
            _STL_VERIFY(_Idx < _SourceSize, "path input not null terminated");
            if (_Src[_Idx] == _EcharT{}) {
                return basic_string_view<_EcharT>(_Src, _Idx);
            }
        }
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2

    template <class _Src, class _Conversion = _Normal_conversion>
    _NODISCARD wstring _Convert_Source_to_wide(const _Src& _Source, _Conversion _Tag = {}) {
        return _Convert_stringoid_to_wide(_Stringoid_from_Source(_Source), _Tag);
    }

    template <class _InIt>
    _NODISCARD auto _Stringoid_from_range(_InIt _First, _InIt _Last) {
        _Adl_verify_range(_First, _Last);
        const auto _UFirst = _Get_unwrapped(_First);
        const auto _ULast  = _Get_unwrapped(_Last);

        if constexpr (is_pointer_v<decltype(_UFirst)>) {
            return basic_string_view<_Iter_value_t<_InIt>>(_UFirst, static_cast<size_t>(_ULast - _UFirst));
        } else {
            return basic_string<_Iter_value_t<_InIt>>(_UFirst, _ULast);
        }
    }

    template <class _InIt, class _Conversion = _Normal_conversion>
    _NODISCARD wstring _Convert_range_to_wide(_InIt _First, _InIt _Last, _Conversion _Tag = {}) {
        return _Convert_stringoid_to_wide(_Stringoid_from_range(_First, _Last), _Tag);
    }

    _NODISCARD inline wstring _Convert_stringoid_with_locale_to_wide(const string_view _Input, const locale& _Loc) {
        const auto& _Facet = _STD use_facet<codecvt<wchar_t, char, mbstate_t>>(_Loc);

        wstring _Output(_Input.size(), L'\0'); // almost always sufficient

        for (;;) {
            mbstate_t _State{};
            const char* const _From_begin = _Input.data();
            const char* const _From_end   = _From_begin + _Input.size();
            const char* _From_next        = nullptr;
            wchar_t* const _To_begin      = _Output.data();
            wchar_t* const _To_end        = _To_begin + _Output.size();
            wchar_t* _To_next             = nullptr;

            const auto _Result = _Facet.in(_State, _From_begin, _From_end, _From_next, _To_begin, _To_end, _To_next);

            if (_From_next < _From_begin || _From_next > _From_end || _To_next < _To_begin || _To_next > _To_end) {
                _Throw_system_error(errc::invalid_argument);
            }

            switch (_Result) {
            case codecvt_base::ok:
                _Output.resize(static_cast<size_t>(_To_next - _To_begin));
                return _Output;

            case codecvt_base::partial:
                // N4810 28.4.1.4.2 [locale.codecvt.virtuals]/5:
                // "A return value of partial, if (from_next == from_end), indicates that either the
                // destination sequence has not absorbed all the available destination elements,
                // or that additional source elements are needed before another destination element can be produced."
                if ((_From_next == _From_end && _To_next != _To_end) || _Output.size() > _Output.max_size() / 2) {
                    _Throw_system_error(errc::invalid_argument);
                }

                _Output.resize(_Output.size() * 2);
                break; // out of switch, keep looping

            case codecvt_base::error:
            case codecvt_base::noconv:
            default:
                _Throw_system_error(errc::invalid_argument);
            }
        }
    }

    template <class _Ty>
    _NODISCARD _Ty _Unaligned_load(const void* _Ptr) { // load a _Ty from _Ptr
        static_assert(is_trivial_v<_Ty>, "Unaligned loads require trivial types");
        _Ty _Tmp;
        _CSTD memcpy(&_Tmp, _Ptr, sizeof(_Tmp));
        return _Tmp;
    }

    _NODISCARD inline bool _Is_drive_prefix(const wchar_t* const _First) {
        // test if _First points to a prefix of the form X:
        // pre: _First points to at least 2 wchar_t instances
        // pre: Little endian
        auto _Value = _Unaligned_load<unsigned int>(_First);
        _Value &= 0xFFFF'FFDFu; // transform lowercase drive letters into uppercase ones
        _Value -= (static_cast<unsigned int>(L':') << (sizeof(wchar_t) * CHAR_BIT)) | L'A';
        return _Value < 26;
    }

    _NODISCARD inline bool _Has_drive_letter_prefix(const wchar_t* const _First, const wchar_t* const _Last) {
        // test if [_First, _Last) has a prefix of the form X:
        return _Last - _First >= 2 && _Is_drive_prefix(_First);
    }

    _NODISCARD inline const wchar_t* _Find_root_name_end(const wchar_t* const _First, const wchar_t* const _Last) {
        // attempt to parse [_First, _Last) as a path and return the end of root-name if it exists; otherwise, _First

        // This is the place in the generic grammar where library implementations have the most freedom.
        // Below are example Windows paths, and what we've decided to do with them:
        // * X:DriveRelative, X:\DosAbsolute
        //   We parse X: as root-name, if and only if \ is present we consider that root-directory
        // * \RootRelative
        //   We parse no root-name, and \ as root-directory
        // * \\server\share
        //   We parse \\server as root-name, \ as root-directory, and share as the first element in relative-path.
        //   Technically, Windows considers all of \\server\share the logical "root", but for purposes
        //   of decomposition we want those split, so that path(R"(\\server\share)").replace_filename("other_share")
        //   is \\server\other_share
        // * \\?\device
        // * \??\device
        // * \\.\device
        //   CreateFile appears to treat these as the same thing; we will set the first three characters as root-name
        //   and the first \ as root-directory. Support for these prefixes varies by particular Windows version, but
        //   for the purposes of path decomposition we don't need to worry about that.
        // * \\?\UNC\server\share
        //   MSDN explicitly documents the \\?\UNC syntax as a special case. What actually happens is that the device
        //   Mup, or "Multiple UNC provider", owns the path \\?\UNC in the NT namespace, and is responsible for the
        //   network file access. When the user says \\server\share, CreateFile translates that into
        //   \\?\UNC\server\share to get the remote server access behavior. Because NT treats this like any other
        //   device, we have chosen to treat this as the \\?\ case above.
        if (_Last - _First < 2) {
            return _First;
        }

        if (_Has_drive_letter_prefix(_First, _Last)) { // check for X: first because it's the most common root-name
            return _First + 2;
        }

        if (!_Is_slash(_First[0])) { // all the other root-names start with a slash; check that first because
                                     // we expect paths without a leading slash to be very common
            return _First;
        }

        // $ means anything other than a slash, including potentially the end of the input
        if (_Last - _First >= 4 && _Is_slash(_First[3]) && (_Last - _First == 4 || !_Is_slash(_First[4])) // \xx\$
            && ((_Is_slash(_First[1]) && (_First[2] == L'?' || _First[2] == L'.')) // \\?\$ or \\.\$
                || (_First[1] == L'?' && _First[2] == L'?'))) { // \??\$
            return _First + 3;
        }

        if (_Last - _First >= 3 && _Is_slash(_First[1]) && !_Is_slash(_First[2])) { // \\server
            return _STD find_if(_First + 3, _Last, _Is_slash);
        }

        // no match
        return _First;
    }

    _NODISCARD inline wstring_view _Parse_root_name(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the root-name if it exists; otherwise, an empty view
        const auto _First = _Str.data();
        const auto _Last  = _First + _Str.size();
        return wstring_view(_First, static_cast<size_t>(_Find_root_name_end(_First, _Last) - _First));
    }

    _NODISCARD inline const wchar_t* _Find_relative_path(const wchar_t* const _First, const wchar_t* const _Last) {
        // attempt to parse [_First, _Last) as a path and return the start of relative-path
        return _STD find_if_not(_Find_root_name_end(_First, _Last), _Last, _Is_slash);
    }

    _NODISCARD inline wstring_view _Parse_root_directory(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the root-directory if it exists; otherwise, an empty view
        const auto _First         = _Str.data();
        const auto _Last          = _First + _Str.size();
        const auto _Root_name_end = _Find_root_name_end(_First, _Last);
        const auto _Relative_path = _STD find_if_not(_Root_name_end, _Last, _Is_slash);
        return wstring_view(_Root_name_end, static_cast<size_t>(_Relative_path - _Root_name_end));
    }

    _NODISCARD inline wstring_view _Parse_root_path(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the root-path if it exists; otherwise, an empty view
        const auto _First = _Str.data();
        const auto _Last  = _First + _Str.size();
        return wstring_view(_First, static_cast<size_t>(_Find_relative_path(_First, _Last) - _First));
    }

    _NODISCARD inline wstring_view _Parse_relative_path(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the relative-path if it exists; otherwise, an empty view
        const auto _First         = _Str.data();
        const auto _Last          = _First + _Str.size();
        const auto _Relative_path = _Find_relative_path(_First, _Last);
        return wstring_view(_Relative_path, static_cast<size_t>(_Last - _Relative_path));
    }

    _NODISCARD inline wstring_view _Parse_parent_path(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the parent_path if it exists; otherwise, an empty view
        const auto _First         = _Str.data();
        auto _Last                = _First + _Str.size();
        const auto _Relative_path = _Find_relative_path(_First, _Last);
        // case 1: relative-path ends in a directory-separator, remove the separator to remove "magic empty path"
        //  for example: R"(/cat/dog/\//\)"
        // case 2: relative-path doesn't end in a directory-separator, remove the filename and last directory-separator
        //  to prevent creation of a "magic empty path"
        //  for example: "/cat/dog"
        while (_Relative_path != _Last && !_Is_slash(_Last[-1])) {
            // handle case 2 by removing trailing filename, puts us into case 1
            --_Last;
        }

        while (_Relative_path != _Last && _Is_slash(_Last[-1])) { // handle case 1 by removing trailing slashes
            --_Last;
        }

        return wstring_view(_First, static_cast<size_t>(_Last - _First));
    }

    _NODISCARD inline const wchar_t* _Find_filename(const wchar_t* const _First, const wchar_t* _Last) {
        // attempt to parse [_First, _Last) as a path and return the start of filename if it exists; otherwise, _Last
        const auto _Relative_path = _Find_relative_path(_First, _Last);
        while (_Relative_path != _Last && !_Is_slash(_Last[-1])) {
            --_Last;
        }

        return _Last;
    }

    _NODISCARD inline wstring_view _Parse_filename(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the filename if it exists; otherwise, an empty view
        const auto _First    = _Str.data();
        const auto _Last     = _First + _Str.size();
        const auto _Filename = _Find_filename(_First, _Last);
        return wstring_view(_Filename, static_cast<size_t>(_Last - _Filename));
    }

    _NODISCARD constexpr const wchar_t* _Find_extension(const wchar_t* const _Filename, const wchar_t* const _Ads) {
        // find dividing point between stem and extension in a generic format filename consisting of [_Filename, _Ads)
        auto _Extension = _Ads;
        if (_Filename == _Extension) { // empty path
            return _Ads;
        }

        --_Extension;
        if (_Filename == _Extension) {
            // path is length 1 and either dot, or has no dots; either way, extension() is empty
            return _Ads;
        }

        if (*_Extension == L'.') { // we might have found the end of stem
            if (_Filename == _Extension - 1 && _Extension[-1] == L'.') { // dotdot special case
                return _Ads;
            } else { // x.
                return _Extension;
            }
        }

        while (_Filename != --_Extension) {
            if (*_Extension == L'.') { // found a dot which is not in first position, so it starts extension()
                return _Extension;
            }
        }

        // if we got here, either there are no dots, in which case extension is empty, or the first element
        // is a dot, in which case we have the leading single dot special case, which also makes extension empty
        return _Ads;
    }

    _NODISCARD inline wstring_view _Parse_stem(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the stem if it exists; otherwise, an empty view
        const auto _First    = _Str.data();
        const auto _Last     = _First + _Str.size();
        const auto _Filename = _Find_filename(_First, _Last);
        const auto _Ads =
            _STD find(_Filename, _Last, L':'); // strip alternate data streams in intra-filename decomposition
        const auto _Extension = _Find_extension(_Filename, _Ads);
        return wstring_view(_Filename, static_cast<size_t>(_Extension - _Filename));
    }

    _NODISCARD inline wstring_view _Parse_extension(const wstring_view _Str) {
        // attempt to parse _Str as a path and return the extension if it exists; otherwise, an empty view
        const auto _First    = _Str.data();
        const auto _Last     = _First + _Str.size();
        const auto _Filename = _Find_filename(_First, _Last);
        const auto _Ads =
            _STD find(_Filename, _Last, L':'); // strip alternate data streams in intra-filename decomposition
        const auto _Extension = _Find_extension(_Filename, _Ads);
        return wstring_view(_Extension, static_cast<size_t>(_Ads - _Extension));
    }

    _NODISCARD inline int _Range_compare(const wchar_t* const _Lfirst, const wchar_t* const _Llast,
        const wchar_t* const _Rfirst, const wchar_t* const _Rlast) {
        // 3 way compare [_Lfirst, _Llast) with [_Rfirst, _Rlast)
        return _Traits_compare<char_traits<wchar_t>>(
            _Lfirst, static_cast<size_t>(_Llast - _Lfirst), _Rfirst, static_cast<size_t>(_Rlast - _Rfirst));
    }

    _NODISCARD inline bool _Is_drive_prefix_with_slash_slash_question(const wstring_view _Text) {
        // test if _Text starts with a \\?\X: prefix
        return _Text.size() >= 6 && _Text._Starts_with(LR"(\\?\)"sv) && _Is_drive_prefix(_Text.data() + 4);
    }

    _NODISCARD inline bool _Is_dot_or_dotdot(const __std_fs_find_data& _Data) {
        // tests if _File_name of __std_fs_find_data is . or ..
        if (_Data._File_name[0] != L'.') {
            return false;
        }

        const auto _Second_char = _Data._File_name[1];
        if (_Second_char == 0) {
            return true;
        }

        if (_Second_char != L'.') {
            return false;
        }

        return _Data._File_name[2] == 0;
    }

    struct _Find_file_handle {
        __std_fs_dir_handle _Handle = __std_fs_dir_handle::_Invalid;

        _Find_file_handle() noexcept = default;
        _Find_file_handle(_Find_file_handle&& _Rhs) noexcept
            : _Handle(_STD exchange(_Rhs._Handle, __std_fs_dir_handle::_Invalid)) {}

        _Find_file_handle& operator=(_Find_file_handle&& _Rhs) noexcept {
            auto _Tmp = _STD exchange(_Rhs._Handle, __std_fs_dir_handle::_Invalid);
            _Tmp      = _STD exchange(_Handle, _Tmp);
            __std_fs_directory_iterator_close(_Tmp);
            return *this;
        }

        _NODISCARD __std_win_error _Open(const wchar_t* _Path_spec, __std_fs_find_data* _Results) noexcept {
            return __std_fs_directory_iterator_open(_Path_spec, &_Handle, _Results);
        }

        ~_Find_file_handle() noexcept {
            __std_fs_directory_iterator_close(_Handle);
        }

        explicit operator bool() const noexcept {
            return _Handle != __std_fs_dir_handle::_Invalid;
        }
    };

    template <class _Base_iter>
    class _Path_iterator;

    class path {
        template <class _Base_iter>
        friend class _Path_iterator;
        friend inline __std_win_error _Absolute(path& _Result, const wstring& _Text);
        friend inline __std_win_error _Canonical(path& _Result, const wstring& _Text);
        friend inline path temp_directory_path(error_code& _Ec);
        friend inline __std_win_error _Current_path(path& _Result) noexcept;
        friend inline void current_path(const path& _To);
        friend inline void current_path(const path& _To, error_code& _Ec) noexcept;
        friend inline __std_win_error _Read_symlink(const path& _Symlink_path, path& _Result) noexcept;

    public:
        using value_type  = wchar_t;
        using string_type = _STD wstring;

        static constexpr wchar_t preferred_separator = L'\\';

        enum format { auto_format, native_format, generic_format };

        path()            = default;
        path(const path&) = default;
        path(path&&)      = default;
        ~path()           = default;
        path& operator=(const path&) = default;
        path& operator=(path&&) noexcept = default;

        path(string_type&& _Source) : _Text(_STD move(_Source)) {}

        path(string_type&& _Source, format) : _Text(_STD move(_Source)) {
            // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path(const _Src& _Source, format = auto_format) : _Text(_Convert_Source_to_wide(_Source)) {
            // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
        }

        template <class _InIt>
        path(_InIt _First, _InIt _Last, format = auto_format) : _Text(_Convert_range_to_wide(_First, _Last)) {
            // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
            static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path(const _Src& _Source, const locale& _Loc, format = auto_format)
            : _Text(_Convert_stringoid_with_locale_to_wide(_Stringoid_from_Source(_Source), _Loc)) {
            // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
            using _Stringoid = decltype(_Stringoid_from_Source(_Source));
            static_assert(is_same_v<typename _Stringoid::value_type, char>,
                "invalid value_type, see N4810 29.11.7.4.1 [fs.path.construct]/6");
        }

        template <class _InIt>
        path(_InIt _First, _InIt _Last, const locale& _Loc, format = auto_format)
            : _Text(_Convert_stringoid_with_locale_to_wide(_Stringoid_from_range(_First, _Last), _Loc)) {
            // format has no meaning for this implementation, as the generic grammar is acceptable as a native path
            static_assert(is_same_v<_Iter_value_t<_InIt>, char>,
                "invalid value_type, see N4810 29.11.7.4.1 [fs.path.construct]/6");
        }

        path& operator=(string_type&& _Source) noexcept /* strengthened */ {
            // set native() to _Source
            _Text = _STD move(_Source);
            return *this;
        }

        path& assign(string_type&& _Source) noexcept /* strengthened */ {
            // set native() to _Source
            _Text = _STD move(_Source);
            return *this;
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path& operator=(const _Src& _Source) { // set native() to _Source
            _Text = _Convert_Source_to_wide(_Source);
            return *this;
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path& assign(const _Src& _Source) { // set native() to _Source
            _Text = _Convert_Source_to_wide(_Source);
            return *this;
        }

        template <class _InIt>
        path& assign(_InIt _First, _InIt _Last) { // set native() to [_First, _Last)
            static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
            _Text = _Convert_range_to_wide(_First, _Last);
            return *this;
        }

        path& operator/=(const path& _Other) {
            // set *this to the path lexically resolved by _Other relative to *this
            // examples:
            // path("cat") / "c:/dog"; // yields "c:/dog"
            // path("cat") / "c:"; // yields "c:"
            // path("c:") / ""; // yields "c:"
            // path("c:cat") / "/dog"; // yields "c:/dog"
            // path("c:cat") / "c:dog"; // yields "c:cat/dog"
            // path("c:cat") / "d:dog"; // yields "d:dog"
            // several places herein quote the standard, but the standard's variable p is replaced with _Other

            if (_Other.is_absolute()) { // if _Other.is_absolute(), then op=(_Other)
                return operator=(_Other);
            }

            const auto _My_first            = _Text.data();
            const auto _My_last             = _My_first + _Text.size();
            const auto _Other_first         = _Other._Text.data();
            const auto _Other_last          = _Other_first + _Other._Text.size();
            const auto _My_root_name_end    = _Find_root_name_end(_My_first, _My_last);
            const auto _Other_root_name_end = _Find_root_name_end(_Other_first, _Other_last);
            if (_Other_first != _Other_root_name_end
                && _Range_compare(_My_first, _My_root_name_end, _Other_first, _Other_root_name_end) != 0) {
                // if _Other.has_root_name() && _Other.root_name() != root_name(), then op=(_Other)
                return operator=(_Other);
            }

            if (_Other_root_name_end != _Other_last && _Is_slash(*_Other_root_name_end)) {
                // If _Other.has_root_directory() removes any root directory and relative-path from *this
                _Text.erase(static_cast<size_t>(_My_root_name_end - _My_first));
            } else {
                // Otherwise, if (!has_root_directory && is_absolute) || has_filename appends path::preferred_separator
                if (_My_root_name_end == _My_last) {
                    // Here, !has_root_directory && !has_filename
                    // Going through our root_name kinds:
                    // X: can't be absolute here, since those paths are absolute only when has_root_directory
                    // \\?\ can't exist without has_root_directory
                    // \\server can be absolute here
                    if (_My_root_name_end - _My_first >= 3) {
                        _Text.push_back(preferred_separator);
                    }
                } else {
                    // Here, has_root_directory || has_filename
                    // If there is a trailing slash, the trailing slash might be part of root_directory.
                    // If it is, has_root_directory && !has_filename, so the test fails.
                    // If there is a trailing slash not part of root_directory, then !has_filename, so only
                    // (!has_root_directory && is_absolute) remains
                    // Going through our root_name kinds:
                    // X:cat\ needs a root_directory to be absolute
                    // \\server\cat must have a root_directory to exist with a relative_path
                    // \\?\ must have a root_directory to exist
                    // As a result, the test fails if there is a trailing slash.
                    // If there is no trailing slash, then has_filename, so the test passes.
                    // Therefore, the test passes if and only if there is no trailing slash.
                    if (!_Is_slash(_My_last[-1])) {
                        _Text.push_back(preferred_separator);
                    }
                }
            }

            // Then appends the native format pathname of _Other, omitting any root-name from its generic format
            // pathname, to the native format pathname.
            _Text.append(_Other_root_name_end, static_cast<size_t>(_Other_last - _Other_root_name_end));
            return *this;
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path& operator/=(const _Src& _Source) {
            return operator/=(path(_Source));
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path& append(const _Src& _Source) {
            return operator/=(path(_Source));
        }

        template <class _InIt>
        path& append(_InIt _First, _InIt _Last) {
            static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
            return operator/=(path(_First, _Last));
        }

        path& operator+=(const path& _Added) { // concat _Added to native()
            return operator+=(_Added._Text);
        }

        path& operator+=(const string_type& _Added) { // concat _Added to native()
            _Text._Orphan_all();
            _Text += _Added;
            return *this;
        }

        path& operator+=(const wstring_view _Added) { // concat _Added to native()
            _Text._Orphan_all();
            _Text += _Added;
            return *this;
        }

        path& operator+=(const value_type* const _Added) { // concat _Added to native()
            _Text._Orphan_all();
            _Text += _Added;
            return *this;
        }

        path& operator+=(const value_type _Added) { // concat _Added to native()
            _Text._Orphan_all();
            _Text += _Added;
            return *this;
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path& operator+=(const _Src& _Added) { // concat _Added to native()
            return operator+=(path(_Added)._Text);
        }

        template <class _EcharT, enable_if_t<_Is_EcharT<_EcharT>, int> = 0>
        path& operator+=(const _EcharT _Added) { // concat _Added to native()
            return operator+=(path(&_Added, &_Added + 1)._Text);
        }

        template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
        path& concat(const _Src& _Added) { // concat _Added to native()
            return operator+=(path(_Added)._Text);
        }

        template <class _InIt>
        path& concat(_InIt _First, _InIt _Last) { // concat [_First, _Last) to native()
            static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
            return operator+=(path(_First, _Last)._Text);
        }

        void clear() noexcept { // set *this to the empty path
            _Text._Orphan_all();
            _Text.clear();
        }

        path& make_preferred() noexcept /* strengthened */ {
            // transform each fallback-separator into preferred-separator
            _Text._Orphan_all();
            _STD replace(_Text.begin(), _Text.end(), L'/', L'\\');
            return *this;
        }

        path& remove_filename() noexcept /* strengthened */ {
            // remove any filename from *this
            const auto _First    = _Text.data();
            const auto _Last     = _First + _Text.size();
            const auto _Filename = _Find_filename(_First, _Last);
            _Text._Orphan_all();
            _Text.erase(static_cast<size_t>(_Filename - _First));
            return *this;
        }

        void _Remove_filename_and_separator() noexcept { // remove filename and preceding non-root directory-separator
            const auto _First         = _Text.data();
            const auto _Last          = _First + _Text.size();
            const auto _Root_name_end = _Find_root_name_end(_First, _Last);
            const auto _Root_dir_end =
                (_Root_name_end != _Last && _Is_slash(*_Root_name_end)) ? _Root_name_end + 1 : _Root_name_end;

            using _Reverse_iter = reverse_iterator<const wchar_t*>;

            const _Reverse_iter _Rbegin{_Last};
            const _Reverse_iter _Rend{_Root_dir_end};

            const auto _Rslash_first = _STD find_if(_Rbegin, _Rend, _Is_slash);
            const auto _Rslash_last  = _STD find_if_not(_Rslash_first, _Rend, _Is_slash);

            const _Reverse_iter _Rlast{_First};

            _Text._Orphan_all();
            _Text.erase(static_cast<size_t>(_Rlast - _Rslash_last));
        }

        path& replace_filename(const path& _Replacement) { // remove any filename from *this and append _Replacement
            remove_filename();
            return operator/=(_Replacement);
        }

        path& replace_extension(/* const path& _Replacement = {} */) noexcept /* strengthened */ {
            // remove any extension() (and alternate data stream references) from *this's filename()
            const wchar_t* _First = _Text.data();
            const auto _Last      = _First + _Text.size();
            const auto _Filename  = _Find_filename(_First, _Last);
            const auto _Ads       = _STD find(_Filename, _Last, L':');
            const auto _Extension = _Find_extension(_Filename, _Ads);
            _Text._Orphan_all();
            _Text.erase(static_cast<size_t>(_Extension - _First));
            return *this;
        }

        path& replace_extension(const path& _Replacement) {
            // remove any extension() (and alternate data stream references) from *this's filename(), and concatenate
            // _Replacement
            replace_extension();
            if (!_Replacement.empty() && _Replacement._Text[0] != L'.') {
                _Text.push_back(L'.');
            }

            return operator+=(_Replacement._Text);
        }

        void swap(path& _Rhs) noexcept {
            _Text.swap(_Rhs._Text);
        }

        _NODISCARD const string_type& native() const noexcept {
            // return a reference to the internally stored wstring in the native format
            return _Text;
        }

        _NODISCARD const value_type* c_str() const noexcept {
            // return a NTCTS to the internally stored path in the native format
            return _Text.c_str();
        }

        operator string_type() const { // implicitly convert *this into a string containing the native format
            return _Text;
        }

        template <class _EcharT, class _Traits = char_traits<_EcharT>, class _Alloc = allocator<_EcharT>,
            enable_if_t<_Is_EcharT<_EcharT>, int> = 0>
        _NODISCARD basic_string<_EcharT, _Traits, _Alloc> string(const _Alloc& _Al = _Alloc()) const {
            // convert the native path from this instance into a basic_string
            return _Convert_wide_to<_Traits>(_Text, _Al);
        }

        _NODISCARD _STD string string() const { // convert the native path from this instance into a string
            return string<char>();
        }

        _NODISCARD _STD wstring wstring() const { // copy the native path from this instance into a wstring
            return _Text;
        }

        _NODISCARD auto u8string() const { // convert the native path from this instance into a UTF-8 string
#ifdef __cpp_lib_char8_t
            using _U8Ty = char8_t;
#else // ^^^ __cpp_lib_char8_t / !__cpp_lib_char8_t vvv
            using _U8Ty = char;
#endif // __cpp_lib_char8_t
            return _Convert_wide_to_narrow<char_traits<_U8Ty>>(__std_code_page::_Utf8, _Text, allocator<_U8Ty>{});
        }

        _NODISCARD _STD u16string u16string() const { // convert the native path from this instance into a u16string
            return string<char16_t>();
        }

        _NODISCARD _STD u32string u32string() const { // convert the native path from this instance into a u32string
            return string<char32_t>();
        }

        template <class _EcharT, class _Traits = char_traits<_EcharT>, class _Alloc = allocator<_EcharT>,
            enable_if_t<_Is_EcharT<_EcharT>, int> = 0>
        _NODISCARD basic_string<_EcharT, _Traits, _Alloc> generic_string(const _Alloc& _Al = _Alloc()) const {
            // convert the native path from this instance into a generic basic_string
            using _Alwide = _Rebind_alloc_t<_Alloc, wchar_t>;
            _Alwide _Al_wchar(_Al);
            basic_string<wchar_t, char_traits<wchar_t>, _Alwide> _Generic_str(_Al_wchar);
            _Generic_str.resize(_Text.size());
            _STD replace_copy(_Text.begin(), _Text.end(), _Generic_str.begin(), L'\\', L'/');
            return _Convert_wide_to<_Traits>(_Generic_str, _Al);
        }

        _NODISCARD _STD string generic_string() const {
            // convert the native path from this instance into a generic string
            return generic_string<char>();
        }

        _NODISCARD _STD wstring generic_wstring() const {
            // convert the current native path into a copy of it in the generic format
            // note: intra-filename() observers stem() and extension() strip alternate data
            // streams, but filenames with alternate data streams inside can serve as
            // perfectly valid values of filename in the generic format, so in the interest of
            // destroying less information we have preserved them here.
            _STD wstring _Result;
            _Result.resize(_Text.size());
            _STD replace_copy(_Text.begin(), _Text.end(), _Result.begin(), L'\\', L'/');
            return _Result;
        }

        _NODISCARD auto generic_u8string() const {
            // convert the native path from this instance into a generic UTF-8 string
            auto _Result = u8string();
            using _U8Ty  = decltype(_Result)::value_type;
            _STD replace(_Result.begin(), _Result.end(), _U8Ty{u8'\\'}, _U8Ty{u8'/'});
            return _Result;
        }

        _NODISCARD _STD u16string generic_u16string() const {
            // convert the native path from this instance into a generic u16string
            _STD u16string _Result = u16string();
            _STD replace(_Result.begin(), _Result.end(), u'\\', u'/');
            return _Result;
        }

        _NODISCARD _STD u32string generic_u32string() const {
            // convert the native path from this instance into a generic u32string
            _STD u32string _Result = u32string();
            _STD replace(_Result.begin(), _Result.end(), U'\\', U'/');
            return _Result;
        }

        _NODISCARD int compare(const path& _Other) const noexcept { // compare *this with _Other
            return compare(static_cast<wstring_view>(_Other._Text));
        }

        _NODISCARD int compare(const string_type& _Other) const noexcept /* strengthened */ {
            // compare *this with _Other
            return compare(static_cast<wstring_view>(_Other));
        }

        _NODISCARD int compare(const basic_string_view<value_type> _Other) const noexcept /* strengthened */ {
            // compare *this with _Other
            // several places herein quote the standard, but the standard's variable p is replaced with _Other
            const auto _My_first            = _Text.data();
            const auto _My_last             = _My_first + _Text.size();
            const auto _My_root_name_end    = _Find_root_name_end(_My_first, _My_last);
            const auto _Other_first         = _Other.data();
            const auto _Other_last          = _Other_first + _Other.size();
            const auto _Other_root_name_end = _Find_root_name_end(_Other_first, _Other_last);

            // Let rootNameComparison be the result of this->root_name().native().compare(_Other.root_name().native())
            const int _Root_cmp = _Range_compare(_My_first, _My_root_name_end, _Other_first, _Other_root_name_end);
            if (_Root_cmp != 0) { // If rootNameComparison is not 0, rootNameComparison
                return _Root_cmp;
            }

            auto _My_relative               = _STD find_if_not(_My_root_name_end, _My_last, _Is_slash);
            auto _Other_relative            = _STD find_if_not(_Other_root_name_end, _Other_last, _Is_slash);
            const bool _My_has_root_name    = _My_root_name_end != _My_relative;
            const bool _Other_has_root_name = _Other_root_name_end != _Other_relative;
            // If !this->has_root_directory() and _Other.has_root_directory(), a value less than 0
            // If this->has_root_directory() and !_Other.has_root_directory(), a value greater than 0
            const int _Root_name_cmp = _My_has_root_name - _Other_has_root_name;
            if (_Root_name_cmp != 0) {
                return _Root_name_cmp;
            }

            // Otherwise, lexicographic by element
            for (;;) {
                const bool _My_empty    = _My_relative == _My_last;
                const bool _Other_empty = _Other_relative == _Other_last;
                const int _Empty_cmp    = _Other_empty - _My_empty;
                if (_My_empty || _Empty_cmp != 0) {
                    return _Empty_cmp;
                }

                const bool _My_slash    = _Is_slash(*_My_relative);
                const bool _Other_slash = _Is_slash(*_Other_relative);
                const int _Slash_cmp    = _Other_slash - _My_slash;
                if (_Slash_cmp != 0) {
                    return _Slash_cmp;
                }

                if (_My_slash) { // comparing directory-separator
                    _My_relative    = _STD find_if_not(_My_relative + 1, _My_last, _Is_slash);
                    _Other_relative = _STD find_if_not(_Other_relative + 1, _Other_last, _Is_slash);
                    continue;
                }

                const int _Cmp = *_My_relative - *_Other_relative;
                if (_Cmp != 0) {
                    return _Cmp;
                }

                ++_My_relative;
                ++_Other_relative;
            }
        }

        _NODISCARD int compare(const value_type* const _Other) const noexcept /* strengthened */ {
            // compare *this with _Other
            return compare(static_cast<wstring_view>(_Other));
        }

        _NODISCARD path root_name() const {
            // parse the root-name from *this and return a copy if present; otherwise, return the empty path
            return _Parse_root_name(_Text);
        }

        _NODISCARD path root_directory() const {
            // parse the root-directory from *this and return a copy if present; otherwise, return the empty path
            return _Parse_root_directory(_Text);
        }

        _NODISCARD path root_path() const {
            // parse the root-path from *this and return a copy if present; otherwise, return the empty path
            return _Parse_root_path(_Text);
        }

        _NODISCARD path relative_path() const {
            // parse the relative-path from *this and return a copy if present; otherwise, return the empty path
            return _Parse_relative_path(_Text);
        }

        _NODISCARD path parent_path() const {
            // parse the parent-path from *this and return a copy if present; otherwise, return the empty path
            return _Parse_parent_path(_Text);
        }

        _NODISCARD path filename() const {
            // parse the filename from *this and return a copy if present; otherwise, return the empty path
            return _Parse_filename(_Text);
        }

        _NODISCARD path stem() const {
            // parse the stem from *this and return a copy if present; otherwise, return the empty path
            return _Parse_stem(_Text);
        }

        _NODISCARD path extension() const {
            // parse the extension from *this and return a copy if present; otherwise, return the empty path
            return _Parse_extension(_Text);
        }

        _NODISCARD bool empty() const noexcept {
            return _Text.empty();
        }

        _NODISCARD bool has_root_name() const noexcept /* strengthened */ {
            // parse the root-name from *this and return whether it exists
            return !_Parse_root_name(_Text).empty();
        }

        _NODISCARD bool has_root_directory() const noexcept /* strengthened */ {
            // parse the root-directory from *this and return whether it exists
            return !_Parse_root_directory(_Text).empty();
        }

        _NODISCARD bool has_root_path() const noexcept /* strengthened */ {
            // parse the root-path from *this and return whether it exists
            return !_Parse_root_path(_Text).empty();
        }

        _NODISCARD bool has_relative_path() const noexcept /* strengthened */ {
            // parse the relative-path from *this and return whether it exists
            return !_Parse_relative_path(_Text).empty();
        }

        _NODISCARD bool has_parent_path() const noexcept /* strengthened */ {
            // parse the parent-path from *this and return whether it exists
            return !_Parse_parent_path(_Text).empty();
        }

        _NODISCARD bool has_filename() const noexcept /* strengthened */ {
            // parse the filename from *this and return whether it exists
            return !_Parse_filename(_Text).empty();
        }

        _NODISCARD bool has_stem() const noexcept /* strengthened */ {
            // parse the stem from *this and return whether it exists
            return !_Parse_stem(_Text).empty();
        }

        _NODISCARD bool has_extension() const noexcept /* strengthened */ {
            // parse the extension from *this and return whether it exists
            return !_Parse_extension(_Text).empty();
        }

        _NODISCARD bool is_absolute() const noexcept /* strengthened */ {
            // paths with a root-name that is a drive letter and no root-directory are drive relative, such as x:example
            // paths with no root-name or root-directory are relative, such as example
            // paths with no root-name but a root-directory are root relative, such as \example
            // all other paths are absolute
            const auto _First = _Text.data();
            const auto _Last  = _First + _Text.size();
            if (_Has_drive_letter_prefix(_First, _Last)) { // test for X:\ but not X:cat
                return _Last - _First >= 3 && _Is_slash(_First[2]);
            }

            // if root-name is otherwise nonempty, then it must be one of the always-absolute prefixes like
            // \\?\ or \\server, so the path is absolute. Otherwise it is relative.
            return _First != _Find_root_name_end(_First, _Last);
        }

        _NODISCARD bool is_relative() const noexcept /* strengthened */ {
            // test if *this is a relative path
            return !is_absolute();
        }

        _NODISCARD path lexically_normal() const {
            constexpr wstring_view _Dot     = L"."sv;
            constexpr wstring_view _Dot_dot = L".."sv;

            // N4810 29.11.7.1 [fs.path.generic]/6:
            // "Normalization of a generic format pathname means:"

            // "1. If the path is empty, stop."
            if (empty()) {
                return {};
            }

            // "2. Replace each slash character in the root-name with a preferred-separator."
            const auto _First         = _Text.data();
            const auto _Last          = _First + _Text.size();
            const auto _Root_name_end = _Find_root_name_end(_First, _Last);
            string_type _Normalized(_First, _Root_name_end);
            _STD replace(_Normalized.begin(), _Normalized.end(), L'/', L'\\');

            // "3. Replace each directory-separator with a preferred-separator.
            // [ Note: The generic pathname grammar (29.11.7.1) defines directory-separator
            // as one or more slashes and preferred-separators. -end note ]"
            list<wstring_view> _Lst; // Empty wstring_view means directory-separator
                                     // that will be normalized to a preferred-separator.
                                     // Non-empty wstring_view means filename.
            for (auto _Next = _Root_name_end; _Next != _Last;) {
                if (_Is_slash(*_Next)) {
                    if (_Lst.empty() || !_Lst.back().empty()) {
                        // collapse one or more slashes and preferred-separators to one empty wstring_view
                        _Lst.emplace_back();
                    }

                    ++_Next;
                } else {
                    const auto _Filename_end = _STD find_if(_Next + 1, _Last, _Is_slash);
                    _Lst.emplace_back(_Next, static_cast<size_t>(_Filename_end - _Next));
                    _Next = _Filename_end;
                }
            }

            // "4. Remove each dot filename and any immediately following directory-separator."
            for (auto _Next = _Lst.begin(); _Next != _Lst.end();) {
                if (*_Next == _Dot) {
                    _Next = _Lst.erase(_Next); // erase dot filename

                    if (_Next != _Lst.end()) {
                        _Next = _Lst.erase(_Next); // erase immediately following directory-separator
                    }
                } else {
                    ++_Next;
                }
            }

            // "5. As long as any appear, remove a non-dot-dot filename immediately followed by a
            // directory-separator and a dot-dot filename, along with any immediately following directory-separator."
            for (auto _Next = _Lst.begin(); _Next != _Lst.end();) {
                auto _Prev = _Next;

                ++_Next; // If we aren't going to erase, keep advancing.
                         // If we're going to erase, _Next now points past the dot-dot filename.

                if (*_Prev == _Dot_dot && _Prev != _Lst.begin() && --_Prev != _Lst.begin() && *--_Prev != _Dot_dot) {
                    if (_Next != _Lst.end()) { // dot-dot filename has an immediately following directory-separator
                        ++_Next;
                    }

                    _Lst.erase(_Prev, _Next); // _Next remains valid
                }
            }

            // "6. If there is a root-directory, remove all dot-dot filenames
            // and any directory-separators immediately following them.
            // [ Note: These dot-dot filenames attempt to refer to nonexistent parent directories. -end note ]"
            if (!_Lst.empty() && _Lst.front().empty()) { // we have a root-directory
                for (auto _Next = _Lst.begin(); _Next != _Lst.end();) {
                    if (*_Next == _Dot_dot) {
                        _Next = _Lst.erase(_Next); // erase dot-dot filename

                        if (_Next != _Lst.end()) {
                            _Next = _Lst.erase(_Next); // erase immediately following directory-separator
                        }
                    } else {
                        ++_Next;
                    }
                }
            }

            // "7. If the last filename is dot-dot, remove any trailing directory-separator."
            if (_Lst.size() >= 2 && _Lst.back().empty() && *(_STD prev(_Lst.end(), 2)) == _Dot_dot) {
                _Lst.pop_back();
            }

            // Build up _Normalized by flattening _Lst.
            for (const auto& _Elem : _Lst) {
                if (_Elem.empty()) {
                    _Normalized += preferred_separator;
                } else {
                    _Normalized += _Elem;
                }
            }

            // "8. If the path is empty, add a dot."
            if (_Normalized.empty()) {
                _Normalized = _Dot;
            }

            // "The result of normalization is a path in normal form, which is said to be normalized."
            path _Result(_STD move(_Normalized));

            return _Result;
        }

        _NODISCARD inline path lexically_relative(const path& _Base) const;

        _NODISCARD path lexically_proximate(const path& _Base) const {
            path _Result = lexically_relative(_Base);

            if (_Result.empty()) {
                _Result = *this;
            }

            return _Result;
        }

        using iterator       = _Path_iterator<string_type::const_iterator>;
        using const_iterator = iterator;

        _NODISCARD inline iterator begin() const;
        _NODISCARD inline iterator end() const noexcept; // strengthened

        template <class _Elem, class _Traits>
        friend _STD basic_ostream<_Elem, _Traits>& operator<<( // TRANSITION, VSO-570323
            _STD basic_ostream<_Elem, _Traits>& _Ostr, const path& _Path) { // TRANSITION, VSO-570323
            // insert a path into a stream
            return _Ostr << _STD quoted(_Path.string<_Elem, _Traits>());
        }

        template <class _Elem, class _Traits>
        friend _STD basic_istream<_Elem, _Traits>& operator>>( // TRANSITION, VSO-570323
            _STD basic_istream<_Elem, _Traits>& _Istr, path& _Path) { // TRANSITION, VSO-570323
            // extract a path from a stream
            basic_string<_Elem, _Traits> _Tmp;
            _Istr >> _STD quoted(_Tmp);
            _Path = _STD move(_Tmp); // obvious optimization not depicted in N4810 29.11.7.6 [fs.path.io]/3
            return _Istr;
        }

        _NODISCARD_FRIEND bool operator==(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right._Text) == 0;
        }

#if _HAS_CXX20
        _NODISCARD_FRIEND strong_ordering operator<=>(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right._Text) <=> 0;
        }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
        _NODISCARD_FRIEND bool operator!=(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right) != 0;
        }

        _NODISCARD_FRIEND bool operator<(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right) < 0;
        }

        _NODISCARD_FRIEND bool operator>(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right) > 0;
        }

        _NODISCARD_FRIEND bool operator<=(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right) <= 0;
        }

        _NODISCARD_FRIEND bool operator>=(const path& _Left, const path& _Right) noexcept {
            return _Left.compare(_Right) >= 0;
        }
#endif // !_HAS_CXX20

        _NODISCARD_FRIEND path operator/(const path& _Left, const path& _Right) { // append a pair of paths together
            return path(_Left) /= _Right;
        }

    private:
        string_type _Text;
    };

    template <class _Src, enable_if_t<_Is_Source<_Src>, int> = 0>
    _CXX20_DEPRECATE_U8PATH _NODISCARD path u8path(const _Src& _Source) { // construct a path from UTF-8 _Source
        return path(_Convert_Source_to_wide(_Source, _Utf8_conversion{}));
    }

    template <class _InIt>
    _CXX20_DEPRECATE_U8PATH _NODISCARD path u8path(_InIt _First, _InIt _Last) {
        // construct a path from UTF-8 [_First, _Last)
        static_assert(_Is_EcharT<_Iter_value_t<_InIt>>, "invalid value_type, see N4810 29.11.4 [fs.req]/3");
        return path(_Convert_range_to_wide(_First, _Last, _Utf8_conversion{}));
    }

    template <class _Base_iter>
    class _Path_iterator { // almost bidirectional iterator for path
    public:
        // Note that the path::iterator can be decremented and can be dereferenced multiple times,
        // but doesn't actually meet the forward iterator requirements. See N4810 29.11.7.5 [fs.path.itr]/2:
        //  ... for dereferenceable iterators a and b of type path::iterator with a == b, there is no requirement
        //  that *a and *b are bound to the same object
        // This means that you can't give path::iterator to std::reverse_iterator, as operator* there ends
        // up returning a reference to a destroyed temporary. Other algorithms requiring bidirectional
        // iterators may be similarly affected, so we've marked input for now.
        // Also note, in the vector<path>(p.begin(), p.end()) case, the user actually probably wants
        // input behavior, as distance()-ing a path is fairly expensive.
        using iterator_category = input_iterator_tag;
        using value_type        = path;
        using difference_type   = ptrdiff_t;
        using pointer           = const path*;
        using reference         = const path&;

        _Path_iterator() = default;

        _Path_iterator(const _Base_iter& _Position_, const path* _Mypath_) noexcept
            : _Position(_Position_), _Element(), _Mypath(_Mypath_) {}

        _Path_iterator(const _Base_iter& _Position_, wstring_view _Element_text, const path* _Mypath_)
            : _Position(_Position_), _Element(_Element_text), _Mypath(_Mypath_) {}

        _Path_iterator(const _Path_iterator&) = default;
        _Path_iterator(_Path_iterator&&)      = default;
        _Path_iterator& operator=(const _Path_iterator&) = default;
        _Path_iterator& operator=(_Path_iterator&&) = default;

        _NODISCARD reference operator*() const noexcept {
            return _Element;
        }

        _NODISCARD pointer operator->() const noexcept {
            return _STD addressof(_Element);
        }

        _Path_iterator& operator++() {
            const auto& _Text = _Mypath->native();
            const auto _Size  = _Element.native().size();
            _Adl_verify_range(_Text.begin(), _Position); // engaged when *this is checked
            const auto _Begin = _Text.data();
            const auto _End   = _Begin + _Text.size();
            _Adl_verify_range(_Begin, _Position); // engaged when *this is unchecked
            if (_Begin == _Get_unwrapped(_Position)) { // test if the next element will be root-directory
                _Position += static_cast<ptrdiff_t>(_Size);
                const auto _First              = _Text.data();
                const auto _Last               = _First + _Text.size();
                const auto _Root_name_end      = _Find_root_name_end(_First, _Last);
                const auto _Root_directory_end = _STD find_if_not(_Root_name_end, _Last, _Is_slash);
                if (_First != _Root_name_end && _Root_name_end != _Root_directory_end) {
                    // current element is root-name, root-directory exists, so next is root-directory
                    _Element._Text.assign(_Root_name_end, _Root_directory_end);
                    return *this;
                }

                // If we get here, either there is no root-name, and by !_Is_slash(*_Position), no root-directory,
                // or, current element is root-name, and root-directory doesn't exist.
                // Either way, the next element is the first of relative-path
            } else if (_Is_slash(*_Position)) { // current element is root-directory, or the "magic empty path"
                if (_Size == 0) { // current element was "magic empty path", become end()
                    ++_Position;
                    return *this;
                }

                // current element was root-directory, advance to relative-path
                _Position += static_cast<ptrdiff_t>(_Size);
            } else { // current element is one of relative-path
                _Position += static_cast<ptrdiff_t>(_Size);
            }

            if (_Get_unwrapped(_Position) == _End) {
                _Element.clear();
                return *this;
            }

            // at this point, the next element is a standard filename from relative_path(), and _Position
            // points at the preferred-separator or fallback-separator after the previous element
            while (_Is_slash(*_Position)) { // advance to the start of the following path element
                if (_Get_unwrapped(++_Position) == _End) { // "magic" empty element selected
                    --_Position;
                    _Element.clear();
                    return *this;
                }
            }

            _Element._Text.assign(_Get_unwrapped(_Position), _STD find_if(_Get_unwrapped(_Position), _End, _Is_slash));
            return *this;
        }

        _Path_iterator operator++(int) {
            _Path_iterator _Tmp = *this;
            ++*this;
            return _Tmp;
        }

        _Path_iterator& operator--() {
            const auto& _Text = _Mypath->native();
            _Adl_verify_range(_Text.begin(), _Position); // engaged when *this is checked
            const auto _First = _Text.data();
            _Adl_verify_range(_First, _Position); // engaged when *this is unchecked
            const auto _Last                   = _First + _Text.size();
            const auto _Root_name_end_ptr      = _Find_root_name_end(_First, _Last);
            const auto _Root_directory_end_ptr = _STD find_if_not(_Root_name_end_ptr, _Last, _Is_slash);

            if (_Root_name_end_ptr != _Root_directory_end_ptr && _Get_unwrapped(_Position) == _Root_directory_end_ptr) {
                // current element is the first of relative-path, and the prev element is root-directory
                _Seek_wrapped(_Position, _Root_name_end_ptr);
                _Element._Text.assign(
                    _Root_name_end_ptr, static_cast<size_t>(_Root_directory_end_ptr - _Root_name_end_ptr));
                return *this;
            }

            if (_First != _Root_name_end_ptr && _Get_unwrapped(_Position) == _Root_name_end_ptr) {
                // current element is root-directory or, if that doesn't exist, first
                // element of relative-path prev element therefore is root-name
                _Seek_wrapped(_Position, _First);
                _Element._Text.assign(_First, static_cast<size_t>(_Root_name_end_ptr - _First));
                return *this;
            }

            // from here, the result will be somewhere in relative-path
            if (_Get_unwrapped(_Position) == _Last && _Is_slash(_Position[-1])) { // target is "magic empty path"
                --_Position;
                _Element.clear();
                return *this;
            }

            while (_Root_directory_end_ptr != _Get_unwrapped(_Position) && _Is_slash(_Position[-1])) {
                --_Position;
            }

            const auto _New_end = _Position;
            while (_Root_directory_end_ptr != _Get_unwrapped(_Position) && !_Is_slash(_Position[-1])) {
                --_Position;
            }

            _Element._Text.assign(_Position, _New_end);
            return *this;
        }

        _Path_iterator operator--(int) {
            _Path_iterator _Tmp = *this;
            --*this;
            return _Tmp;
        }

        _NODISCARD_FRIEND bool operator==(const _Path_iterator& _Lhs, const _Path_iterator& _Rhs) {
            return _Lhs._Position == _Rhs._Position;
        }

#if !_HAS_CXX20
        _NODISCARD_FRIEND bool operator!=(const _Path_iterator& _Lhs, const _Path_iterator& _Rhs) {
            return _Lhs._Position != _Rhs._Position;
        }
#endif // !_HAS_CXX20

#if _ITERATOR_DEBUG_LEVEL != 0
        friend void _Verify_range(const _Path_iterator& _Lhs, const _Path_iterator& _Rhs) {
            _Verify_range(_Lhs._Position, _Rhs._Position);
        }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        using _Prevent_inheriting_unwrap = _Path_iterator;

        template <class _Iter2 = _Base_iter, enable_if_t<_Unwrappable_v<const _Iter2&>, int> = 0>
        _NODISCARD _Path_iterator<_Unwrapped_t<const _Iter2&>> _Unwrapped() const {
            return {_Position._Unwrapped(), _Element.native(), _Mypath};
        }

        static constexpr bool _Unwrap_when_unverified = _Do_unwrap_when_unverified_v<_Base_iter>;

        template <class _Other>
        friend class _Path_iterator;

        template <class _Other, enable_if_t<_Wrapped_seekable_v<_Base_iter, const _Other&>, int> = 0>
        constexpr void _Seek_to(const _Path_iterator<_Other>& _It) {
            _Position._Seek_to(_It._Position);
            _Element = _It._Element;
            // _Mypath intentionally unchanged
        }

    private:
        // if the current element is the "magic empty path",
        //   points to preferred-separator or fallback-separator immediately before
        // otherwise, points to the first element of _Element
        _Base_iter _Position{};
        path _Element{};
        const path* _Mypath{};
    };

    inline void swap(path& _Left, path& _Right) noexcept {
        _Left.swap(_Right);
    }

    _NODISCARD inline size_t hash_value(const path& _Path) noexcept {
        // calculate a hash value for _Path
        // See path::compare; we effectively decompose the path with special handling for root_name, root_directory.
        // Examples:
        // c:\cat\dog =>  {"c:", true , "cat", "dog"}
        // c:cat\dog =>   {"c:", false, "cat", "dog"}
        // \cat\dog =>    {""  , true , "cat", "dog"}
        // cat\dog =>     {""  , false, "cat", "dog"}
        // c:\cat\dog\ => {"c:", true , "cat", "dog", ""}
        size_t _Val       = _FNV_offset_basis;
        const auto& _Text = _Path.native();
        const auto _First = _Text.data();
        const auto _Last  = _First + _Text.size();

        // First, like compare, examine the raw root_name directly
        auto _Next = _Find_root_name_end(_First, _Last);
        _Val       = _Fnv1a_append_range(_Val, _First, _Next);

        // The remaining path elements, including root_directory, are effectively hashed by normalizing each
        // directory-separator into a single preferred-separator when that goes into the hash function.
        // path::compare has special handling for root_directory to ensure c:\cat sorts before c:cat, but hash only
        // cares about equality, so no special case is necessary.
        bool _Slash_inserted = false;
        for (; _Next != _Last; ++_Next) {
            if (_Is_slash(*_Next)) {
                if (!_Slash_inserted) {
                    _Val            = _Fnv1a_append_value(_Val, path::preferred_separator);
                    _Slash_inserted = true;
                }
            } else {
                _Val            = _Fnv1a_append_value(_Val, *_Next);
                _Slash_inserted = false;
            }
        }

        return _Val;
    }

    _NODISCARD inline bool _Relative_path_contains_root_name(const path& _Path) {
        for (const auto& _File_name : _Path.relative_path()) {
            if (!_Parse_root_name(_File_name.native()).empty()) {
                return true;
            }
        }
        return false;
    }

    _NODISCARD inline path path::lexically_relative(const path& _Base) const {
        constexpr wstring_view _Dot     = L"."sv;
        constexpr wstring_view _Dot_dot = L".."sv;

        path _Result;

        if (root_name() != _Base.root_name() || is_absolute() != _Base.is_absolute()
            || (!has_root_directory() && _Base.has_root_directory()) || _Relative_path_contains_root_name(*this)
            || _Relative_path_contains_root_name(_Base)) {
            return _Result;
        }

        const iterator _This_end   = end();
        const iterator _Base_begin = _Base.begin();
        const iterator _Base_end   = _Base.end();

        auto _Mismatched  = _STD mismatch(begin(), _This_end, _Base_begin, _Base_end);
        iterator& _A_iter = _Mismatched.first;
        iterator& _B_iter = _Mismatched.second;

        if (_A_iter == _This_end && _B_iter == _Base_end) {
            _Result = _Dot;
            return _Result;
        }

        { // Skip root-name and root-directory elements, N4810 29.11.7.5 [fs.path.itr]/4.1, 4.2
            ptrdiff_t _B_dist = _STD distance(_Base_begin, _B_iter);

            const ptrdiff_t _Base_root_dist =
                static_cast<ptrdiff_t>(_Base.has_root_name()) + static_cast<ptrdiff_t>(_Base.has_root_directory());

            while (_B_dist < _Base_root_dist) {
                ++_B_iter;
                ++_B_dist;
            }
        }

        ptrdiff_t _Num = 0;

        for (; _B_iter != _Base_end; ++_B_iter) {
            const path& _Elem = *_B_iter;

            if (_Elem.empty()) { // skip empty element, N4810 29.11.7.5 [fs.path.itr]/4.4
            } else if (_Elem == _Dot) { // skip filename elements that are dot, N4810 29.11.7.4.11 [fs.path.gen]/4.2
            } else if (_Elem == _Dot_dot) {
                --_Num;
            } else {
                ++_Num;
            }
        }

        if (_Num < 0) {
            return _Result;
        }

        if (_Num == 0 && (_A_iter == _This_end || _A_iter->empty())) {
            _Result = _Dot;
            return _Result;
        }

        for (; _Num > 0; --_Num) {
            _Result /= _Dot_dot;
        }

        for (; _A_iter != _This_end; ++_A_iter) {
            _Result /= *_A_iter;
        }

        return _Result;
    }

    _NODISCARD inline path::iterator path::begin() const {
        const auto _First         = _Text.data();
        const auto _Last          = _First + _Text.size();
        const auto _Root_name_end = _Find_root_name_end(_First, _Last);
        const wchar_t* _First_end;
        if (_First == _Root_name_end) { // first element isn't root-name
            auto _Root_directory_end = _STD find_if_not(_Root_name_end, _Last, _Is_slash);
            if (_First == _Root_directory_end) { // first element is first relative-path entry
                _First_end = _STD find_if(_Root_directory_end, _Last, _Is_slash);
            } else { // first element is root-directory
                _First_end = _Root_directory_end;
            }
        } else { // first element is root-name
            _First_end = _Root_name_end;
        }

        return iterator(_Text.cbegin(), wstring_view(_First, static_cast<size_t>(_First_end - _First)), this);
    }

    _NODISCARD inline path::iterator path::end() const noexcept /* strengthened */ {
        return iterator(_Text.cend(), this);
    }

    class filesystem_error : public system_error { // base of all filesystem-error exceptions
    public:
        filesystem_error(const string& _Message, const error_code _Errcode)
            : system_error(_Errcode, _Message), _What(runtime_error::what()) {}

        filesystem_error(const string& _Message, const path& _Path1_arg, const error_code _Errcode)
            : system_error(_Errcode, _Message), _Path1(_Path1_arg), _Path2(),
              _What(_Pretty_message(runtime_error::what(), _Path1_arg)) {}

        filesystem_error(
            const string& _Message, const path& _Path1_arg, const path& _Path2_arg, const error_code _Errcode)
            : system_error(_Errcode, _Message), _Path1(_Path1_arg), _Path2(_Path2_arg),
              _What(_Pretty_message(runtime_error::what(), _Path1_arg, _Path2_arg)) {}

        _NODISCARD const path& path1() const noexcept {
            return _Path1;
        }

        _NODISCARD const path& path2() const noexcept {
            return _Path2;
        }

        _NODISCARD virtual const char* what() const noexcept override {
            return _What.c_str();
        }

    private:
        static string _Pretty_message(const string_view _Op, const path& _Path1, const path& _Path2 = {}) {
            string _Result;
            // Convert the paths to narrow encoding in a way that gracefully handles non-encodable characters
            const auto _Code_page   = __std_fs_code_page();
            const string _Path1_str = _Convert_wide_to_narrow_replace_chars<char_traits<char>>(
                _Code_page, _Path1.native(), allocator<char>{});
            const string _Path2_str = _Convert_wide_to_narrow_replace_chars<char_traits<char>>(
                _Code_page, _Path2.native(), allocator<char>{});
            _Result.reserve(_Op.size() + (_Path2_str.empty() ? 4 : 8) + _Path1_str.size() + _Path2_str.size());
            _Result += _Op;
            _Result += R"(: ")"sv; // 3 chars
            _Result += _Path1_str;
            if (!_Path2_str.empty()) {
                _Result += R"(", ")"sv; // 4 chars
                _Result += _Path2_str;
            }
            _Result += '"'; // 1 char
            return _Result;
        }

        path _Path1;
        path _Path2;
        string _What;

#if !_HAS_EXCEPTIONS
    protected:
        virtual void _Doraise() const override { // perform class-specific exception handling
            _RAISE(*this);
        }
#endif // !_HAS_EXCEPTIONS
    };

    [[noreturn]] inline void _Throw_fs_error(const char* _Op, __std_win_error _Error) {
        _THROW(filesystem_error(_Op, _Make_ec(_Error)));
    }

    [[noreturn]] inline void _Throw_fs_error(const char* _Op, __std_win_error _Error, const path& _Path1) {
        _THROW(filesystem_error(_Op, _Path1, _Make_ec(_Error)));
    }

    [[noreturn]] inline void _Throw_fs_error(
        const char* _Op, __std_win_error _Error, const path& _Path1, const path& _Path2) {
        _THROW(filesystem_error(_Op, _Path1, _Path2, _Make_ec(_Error)));
    }

    [[noreturn]] inline void _Throw_fs_error(const char* _Op, const error_code& _Error, const path& _Path1) {
        _THROW(filesystem_error(_Op, _Path1, _Error));
    }

    [[noreturn]] inline void _Throw_fs_error(
        const char* _Op, const error_code& _Error, const path& _Path1, const path& _Path2) {
        _THROW(filesystem_error(_Op, _Path1, _Path2, _Error));
    }

    enum class file_type {
        none,
        not_found,
        regular,
        directory,
        symlink,

        block, // not used on Windows
        character, // not used in this implementation; theoretically some special files like CON
                   // might qualify, but querying for this is extremely expensive and unlikely
                   // to be useful in practice

        fifo, // not used on Windows (\\.\pipe named pipes don't behave exactly like POSIX fifos)
        socket, // not used on Windows
        unknown,

        junction // implementation-defined value indicating an NT junction
    };

    enum class perms {
        none = 0,

        owner_read  = 0400,
        owner_write = 0200,
        owner_exec  = 0100,
        owner_all   = 0700,

        group_read  = 0040,
        group_write = 0020,
        group_exec  = 0010,
        group_all   = 0070,

        others_read  = 0004,
        others_write = 0002,
        others_exec  = 0001,
        others_all   = 0007,

        all        = 0777, // returned for all files without FILE_ATTRIBUTE_READONLY
        set_uid    = 04000,
        set_gid    = 02000,
        sticky_bit = 01000,
        mask       = 07777,
        unknown    = 0xFFFF,

        _All_write               = owner_write | group_write | others_write,
        _File_attribute_readonly = all & ~_All_write // returned for files with FILE_ATTRIBUTE_READONLY
    };

    _BITMASK_OPS(perms)

    enum class copy_options {
        none = static_cast<int>(__std_fs_copy_options::_None),

        _Existing_mask     = static_cast<int>(__std_fs_copy_options::_Existing_mask),
        skip_existing      = static_cast<int>(__std_fs_copy_options::_Skip_existing),
        overwrite_existing = static_cast<int>(__std_fs_copy_options::_Overwrite_existing),
        update_existing    = static_cast<int>(__std_fs_copy_options::_Update_existing),

        recursive = 0x10,

        _Symlinks_mask = 0xF00,
        copy_symlinks  = 0x100,
        skip_symlinks  = 0x200,

        _Copy_form_mask   = 0xF000,
        directories_only  = 0x1000,
        create_symlinks   = 0x2000,
        create_hard_links = 0x4000,

        _Unspecified_copy_prevention_tag = 0x10000 // to be removed by LWG-3057
    };

    _BITMASK_OPS(copy_options)

    class file_status {
    public:
        // [fs.file_status.cons], constructors and destructor
        file_status() noexcept = default;
        explicit file_status(file_type _Ft, perms _Perms = perms::unknown) noexcept : _Myftype(_Ft), _Myperms(_Perms) {}
        file_status(const file_status&) noexcept = default;
        file_status(file_status&&) noexcept      = default;
        ~file_status() noexcept                  = default;

        // assignments
        file_status& operator=(const file_status&) noexcept = default;
        file_status& operator=(file_status&&) noexcept = default;

        // [fs.file_status.mods], modifiers
        void type(file_type _Ft) noexcept {
            _Myftype = _Ft;
        }

        void permissions(perms _Perms) noexcept {
            _Myperms = _Perms;
        }

        // [fs.file_status.obs], observers
        _NODISCARD file_type type() const noexcept {
            return _Myftype;
        }

        _NODISCARD perms permissions() const noexcept {
            return _Myperms;
        }

#if _HAS_CXX20
        _NODISCARD_FRIEND bool operator==(const file_status& _Lhs, const file_status& _Rhs) noexcept {
            return _Lhs._Myftype == _Rhs._Myftype && _Lhs._Myperms == _Rhs._Myperms;
        }
#endif // _HAS_CXX20

        void _Refresh(const __std_win_error _Error, const __std_fs_stats& _Stats) noexcept {
            if (_Error == __std_win_error::_Success) {
                const auto _Attrs = _Stats._Attributes;

                if (_Bitmask_includes(_Attrs, __std_fs_file_attr::_Readonly)) {
                    this->permissions(perms::_File_attribute_readonly);
                } else {
                    this->permissions(perms::all);
                }

                if (_Bitmask_includes(_Attrs, __std_fs_file_attr::_Reparse_point)) {
                    if (_Stats._Reparse_point_tag == __std_fs_reparse_tag::_Symlink) {
                        this->type(file_type::symlink);
                        return;
                    }

                    if (_Stats._Reparse_point_tag == __std_fs_reparse_tag::_Mount_point) {
                        this->type(file_type::junction);
                        return;
                    }

                    // All other reparse points considered ordinary files or directories
                }

                if (_Bitmask_includes(_Attrs, __std_fs_file_attr::_Directory)) {
                    this->type(file_type::directory);
                } else {
                    this->type(file_type::regular);
                }

                return;
            }

            this->permissions(perms::unknown);
            this->type(__std_is_file_not_found(_Error) ? file_type::not_found : file_type::none);
        }

    private:
        file_type _Myftype = file_type::none;
        perms _Myperms     = perms::unknown;
    };

    _NODISCARD inline bool exists(
        const file_status _Status) noexcept { // tests whether _Status indicates an existing file
        switch (_Status.type()) {
        case file_type::none:
        case file_type::not_found:
            return false;
        case file_type::regular:
        case file_type::directory:
        case file_type::symlink:
        case file_type::block:
        case file_type::character:
        case file_type::fifo:
        case file_type::socket:
        case file_type::unknown:
        case file_type::junction:
        default:
            return true;
        }
    }

    _NODISCARD inline bool is_block_file(const file_status _Status) noexcept {
        // tests whether _Status indicates a block file
        return _Status.type() == file_type::block;
    }

    _NODISCARD inline bool is_character_file(const file_status _Status) noexcept {
        // tests whether _Status indicates a character file
        return _Status.type() == file_type::character;
    }

    _NODISCARD inline bool is_directory(const file_status _Status) noexcept {
        // tests whether _Status indicates a directory
        return _Status.type() == file_type::directory;
    }

    _NODISCARD inline bool is_fifo(const file_status _Status) noexcept {
        // tests whether _Status indicates a fifo
        return _Status.type() == file_type::fifo;
    }

    _NODISCARD inline bool is_other(const file_status _Status) noexcept {
        // tests whether _Status indicates other file types
        switch (_Status.type()) {
        case file_type::none:
        case file_type::not_found:
        case file_type::regular:
        case file_type::directory:
        case file_type::symlink:
            return false;
        case file_type::block:
        case file_type::character:
        case file_type::fifo:
        case file_type::socket:
        case file_type::unknown:
        case file_type::junction:
        default:
            return true;
        }
    }

    _NODISCARD inline bool is_regular_file(const file_status _Status) noexcept {
        // tests whether _Status indicates a regular file
        return _Status.type() == file_type::regular;
    }

    _NODISCARD inline bool is_socket(const file_status _Status) noexcept {
        // tests whether _Status indicates a socket
        return _Status.type() == file_type::socket;
    }

    _NODISCARD inline bool is_symlink(const file_status _Status) noexcept {
        // tests whether _Status indicates a symlink
        return _Status.type() == file_type::symlink;
    }

    struct _File_status_and_error {
        file_status _Status;
        __std_win_error _Error;

        _NODISCARD bool _Not_good() const noexcept {
            // [fs.op.status]/Throws: result values of file_status(file_type::not_found) and
            // file_status(file_type::unknown) are not considered failures and do not cause an exception to be thrown.
            return _Error != __std_win_error::_Success && _Status.type() != file_type::not_found
                && _Status.type() != file_type::unknown;
        }
    };

    inline constexpr auto _Status_stats_flags =
        __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Follow_symlinks;

    inline constexpr auto _Symlink_status_stats_flags =
        __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Reparse_tag;

#if _HAS_CXX20
    using file_time_type = _CHRONO time_point<_CHRONO file_clock>;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    using file_time_type = _CHRONO time_point<filesystem::_File_time_clock>;
#endif // ^^^ !_HAS_CXX20

    class directory_entry {
    public:
        // [fs.dir.entry.cons], constructors and destructor
        directory_entry() noexcept : _Cached_data{}, _Path() {}

        directory_entry(const directory_entry&)     = default;
        directory_entry(directory_entry&&) noexcept = default;
        explicit directory_entry(const filesystem::path& _Path_arg) : _Cached_data{}, _Path(_Path_arg) {
            (void) _Refresh(_Cached_data, _Path);
        }

        directory_entry(const filesystem::path& _Path_arg, error_code& _Ec) : _Cached_data{}, _Path(_Path_arg) {
            _Ec.clear();
            (void) _Refresh(_Cached_data, _Path);
        }

        ~directory_entry() = default;

        // assignments
        directory_entry& operator=(const directory_entry&) = default;
        directory_entry& operator=(directory_entry&&) noexcept = default;

        // [fs.dir.entry.mods], modifiers
        void assign(const filesystem::path& _Path_arg) {
            _Path.assign(_Path_arg);
            (void) _Refresh(_Cached_data, _Path);
        }

        void assign(const filesystem::path& _Path_arg, error_code& _Ec) {
            _Ec.clear(); // for exception safety
            _Path.assign(_Path_arg);
            (void) _Refresh(_Cached_data, _Path);
        }

        void replace_filename(const filesystem::path& _Path_arg) {
            _Path.replace_filename(_Path_arg);
            (void) _Refresh(_Cached_data, _Path);
        }

        void replace_filename(const filesystem::path& _Path_arg, error_code& _Ec) {
            _Ec.clear(); // for exception safety
            _Path.replace_filename(_Path_arg);
            (void) _Refresh(_Cached_data, _Path);
        }

        void refresh() {
            const auto _Error = _Refresh(_Cached_data, _Path);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_entry::refresh", _Error, _Path);
            }
        }

        void refresh(error_code& _Ec) noexcept {
            _Ec = _Make_ec(_Refresh(_Cached_data, _Path));
        }

#if _HAS_CXX20
        void clear_cache() noexcept {
            _Cached_data._Available = __std_fs_stats_flags::_None;
        }
#endif // _HAS_CXX20

    private:
        _NODISCARD bool _Has_cached_attribute(const __std_fs_file_attr _Attrs) const noexcept {
            return _Bitmask_includes(_Cached_data._Attributes, _Attrs);
        }

    public:
        // [fs.dir.entry.obs], observers
        _NODISCARD const filesystem::path& path() const noexcept {
            return _Path;
        }

        operator const filesystem::path&() const noexcept {
            return _Path;
        }

        _NODISCARD bool exists() const {
            const auto _Type = this->status().type();
            return _Type != file_type::not_found && _Type != file_type::none;
        }

        _NODISCARD bool exists(error_code& _Ec) const noexcept {
            const auto _Type = this->status(_Ec).type();
            return _Type != file_type::not_found && _Type != file_type::none;
        }

        _NODISCARD bool is_block_file() const {
            return false;
        }

        _NODISCARD bool is_block_file(error_code& _Ec) const noexcept {
            _Ec.clear();
            return false;
        }

        _NODISCARD bool is_character_file() const {
            return false;
        }

        _NODISCARD bool is_character_file(error_code& _Ec) const noexcept {
            _Ec.clear();
            return false;
        }

        _NODISCARD bool is_directory() const {
            return _STD filesystem::is_directory(this->status());
        }

        _NODISCARD bool is_directory(error_code& _Ec) const noexcept {
            return _STD filesystem::is_directory(this->status(_Ec));
        }

        _NODISCARD bool is_fifo() const {
            return false;
        }

        _NODISCARD bool is_fifo(error_code& _Ec) const noexcept {
            _Ec.clear();
            return false;
        }

        _NODISCARD bool is_other() const {
            return _STD filesystem::is_other(this->status());
        }

        _NODISCARD bool is_other(error_code& _Ec) const noexcept {
            return _STD filesystem::is_other(this->status(_Ec));
        }

        _NODISCARD bool is_regular_file() const {
            return _STD filesystem::is_regular_file(this->status());
        }

        _NODISCARD bool is_regular_file(error_code& _Ec) const noexcept {
            return _STD filesystem::is_regular_file(this->status(_Ec));
        }

        _NODISCARD bool is_socket() const {
            return false;
        }

        _NODISCARD bool is_socket(error_code& _Ec) const noexcept {
            _Ec.clear();
            return false;
        }

        _NODISCARD bool _Is_symlink_or_junction() const noexcept {
            return _Has_cached_attribute(__std_fs_file_attr::_Reparse_point)
                && (_Cached_data._Reparse_point_tag == __std_fs_reparse_tag::_Symlink
                    || _Cached_data._Reparse_point_tag == __std_fs_reparse_tag::_Mount_point);
        }

        _NODISCARD bool is_symlink() const {
            return _STD filesystem::is_symlink(this->symlink_status());
        }

        _NODISCARD bool is_symlink(error_code& _Ec) const noexcept {
            return _STD filesystem::is_symlink(this->symlink_status(_Ec));
        }

        _NODISCARD bool _Available(const __std_fs_stats_flags _Flags) const noexcept {
            return _Bitmask_includes(_Cached_data._Available, _Flags);
        }

    private:
        _NODISCARD __std_win_error _File_size(uintmax_t& _Result) const noexcept {
            if (_Available(__std_fs_stats_flags::_File_size)) {
                _Result = _Cached_data._File_size;
                return __std_win_error::_Success;
            }

            __std_fs_stats _Stats;
            constexpr auto _Flags = __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Follow_symlinks;
            const auto _Error =
                __std_fs_get_stats(_Path.c_str(), &_Stats, _Flags, _Cached_data._Symlink_hint_attributes());
            if (_Error == __std_win_error::_Success) {
                _Result = _Stats._File_size;
            } else {
                _Result = static_cast<uintmax_t>(-1);
            }

            return _Error;
        }

    public:
        _NODISCARD uintmax_t file_size() const {
            uintmax_t _Result;
            const auto _Error = _File_size(_Result);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_entry::file_size", _Error, _Path);
            }

            return _Result;
        }

        _NODISCARD uintmax_t file_size(error_code& _Ec) const noexcept {
            uintmax_t _Result;
            _Ec = _Make_ec(_File_size(_Result));
            return _Result;
        }

    private:
        _NODISCARD __std_win_error _Hard_link_count(uintmax_t& _Result) const noexcept {
            if (_Available(__std_fs_stats_flags::_Link_count)) {
                _Result = _Cached_data._Link_count;
                return __std_win_error::_Success;
            }

            __std_fs_stats _Stats;
            constexpr auto _Flags = __std_fs_stats_flags::_Link_count | __std_fs_stats_flags::_Follow_symlinks;
            const auto _Error =
                __std_fs_get_stats(_Path.c_str(), &_Stats, _Flags, _Cached_data._Symlink_hint_attributes());
            if (_Error == __std_win_error::_Success) {
                _Result = _Stats._Link_count;
            } else {
                _Result = static_cast<uintmax_t>(-1);
            }

            return _Error;
        }

    public:
        _NODISCARD uintmax_t hard_link_count() const {
            uintmax_t _Result;
            const auto _Error = _Hard_link_count(_Result);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_entry::hard_link_count", _Error, _Path);
            }

            return _Result;
        }

        _NODISCARD uintmax_t hard_link_count(error_code& _Ec) const noexcept {
            uintmax_t _Result;
            _Ec = _Make_ec(_Hard_link_count(_Result));
            return _Result;
        }

    private:
        _NODISCARD __std_win_error _Last_write_time(file_time_type& _Result) const noexcept {
            if (_Available(__std_fs_stats_flags::_Last_write_time)) {
                _Result = file_time_type{file_time_type::duration{_Cached_data._Last_write_time}};
                return __std_win_error::_Success;
            }

            __std_fs_stats _Stats;
            constexpr auto _Flags = __std_fs_stats_flags::_Last_write_time | __std_fs_stats_flags::_Follow_symlinks;
            const auto _Error =
                __std_fs_get_stats(_Path.c_str(), &_Stats, _Flags, _Cached_data._Symlink_hint_attributes());
            if (_Error == __std_win_error::_Success) {
                _Result = file_time_type{file_time_type::duration{_Stats._Last_write_time}};
            } else {
                _Result = file_time_type{file_time_type::duration{LLONG_MIN}};
            }

            return _Error;
        }

    public:
        _NODISCARD file_time_type last_write_time() const {
            file_time_type _Result;
            const auto _Error = _Last_write_time(_Result);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_entry::last_write_time", _Error, _Path);
            }

            return _Result;
        }

        _NODISCARD file_time_type last_write_time(error_code& _Ec) const noexcept {
            file_time_type _Result;
            _Ec = _Make_ec(_Last_write_time(_Result));
            return _Result;
        }

    private:
        _NODISCARD _File_status_and_error _Get_any_status(const __std_fs_stats_flags _Flags) const noexcept {
            _File_status_and_error _Result;
            __std_fs_stats _Stats;

            if (_Bitmask_includes_all(_Cached_data._Available, _Flags)) {
                _Result._Error = __std_win_error::_Success;
                _Result._Status._Refresh(__std_win_error::_Success, _Cached_data);
            } else {
                const auto _Error =
                    __std_fs_get_stats(_Path.c_str(), &_Stats, _Flags, _Cached_data._Symlink_hint_attributes());
                _Result._Error = _Error;
                _Result._Status._Refresh(_Error, _Stats);
            }

            return _Result;
        }

    public:
        _NODISCARD file_status status() const {
            const auto _Result = _Get_any_status(_Status_stats_flags);
            if (_Result._Not_good()) {
                _Throw_fs_error("directory_entry::status", _Result._Error, _Path);
            }

            return _Result._Status;
        }

        _NODISCARD file_status status(error_code& _Ec) const noexcept {
            const auto _Result = _Get_any_status(_Status_stats_flags);
            _Ec                = _Make_ec(_Result._Error);
            return _Result._Status;
        }

        _NODISCARD file_status symlink_status() const {
            const auto _Result = _Get_any_status(_Symlink_status_stats_flags);
            if (_Result._Not_good()) {
                _Throw_fs_error("directory_entry::symlink_status", _Result._Error, _Path);
            }

            return _Result._Status;
        }

        _NODISCARD file_status symlink_status(error_code& _Ec) const noexcept {
            const auto _Result = _Get_any_status(_Symlink_status_stats_flags);
            _Ec                = _Make_ec(_Result._Error);
            return _Result._Status;
        }

        _NODISCARD bool operator==(const directory_entry& _Rhs) const noexcept {
            return _Path == _Rhs._Path;
        }

#if _HAS_CXX20
        _NODISCARD strong_ordering operator<=>(const directory_entry& _Rhs) const noexcept {
            return _Path <=> _Rhs._Path;
        }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
        _NODISCARD bool operator!=(const directory_entry& _Rhs) const noexcept {
            return _Path != _Rhs._Path;
        }

        _NODISCARD bool operator<(const directory_entry& _Rhs) const noexcept {
            return _Path < _Rhs._Path;
        }

        _NODISCARD bool operator<=(const directory_entry& _Rhs) const noexcept {
            return _Path <= _Rhs._Path;
        }

        _NODISCARD bool operator>(const directory_entry& _Rhs) const noexcept {
            return _Path > _Rhs._Path;
        }

        _NODISCARD bool operator>=(const directory_entry& _Rhs) const noexcept {
            return _Path >= _Rhs._Path;
        }
#endif // !_HAS_CXX20

        // [fs.dir.entry.io], inserter
        template <class _Elem, class _Traits>
        friend _STD basic_ostream<_Elem, _Traits>& operator<<( // TRANSITION, VSO-570323
            _STD basic_ostream<_Elem, _Traits>& _Ostr, const directory_entry& _Entry) { // TRANSITION, VSO-570323
            // insert a directory_entry into a stream
            return _Ostr << _Entry.path();
        }

    private:
        void _Refresh(const __std_fs_find_data& _Data) noexcept {
            _Cached_data._Attributes        = _Data._Attributes;
            _Cached_data._Reparse_point_tag = _Data._Reparse_point_tag;
            _Cached_data._Available         = __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Reparse_tag;
            if (!_Bitmask_includes(_Data._Attributes, __std_fs_file_attr::_Reparse_point)) {
                _Cached_data._File_size = (static_cast<uintmax_t>(_Data._File_size_high) << 32)
                                        + static_cast<uintmax_t>(_Data._File_size_low);
                _CSTD memcpy(
                    &_Cached_data._Last_write_time, &_Data._Last_write_time, sizeof(_Cached_data._Last_write_time));
                _Cached_data._Available |= __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Last_write_time;
            }
        }

        _NODISCARD static __std_win_error _Refresh(__std_fs_stats& _Stats, const filesystem::path& _Path) noexcept {
            const auto _Error = __std_fs_get_stats(_Path.c_str(), &_Stats, __std_fs_stats_flags::_All_data);
            if (_Error == __std_win_error::_Success) {
                if (_Bitmask_includes(_Stats._Attributes, __std_fs_file_attr::_Reparse_point)) {
                    _Stats._Available = __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Reparse_tag;
                } else {
                    _Stats._Available = __std_fs_stats_flags::_All_data;
                }
            } else {
                _Stats._Available = __std_fs_stats_flags::_None;
            }

            return _Error;
        }

        friend struct _Dir_enum_impl;
        friend struct _Recursive_dir_enum_impl;
        friend void _Copy_impl(
            const directory_entry& _From, const _STD filesystem::path& _To, copy_options _Options, error_code& _Ec);

        __std_fs_stats _Cached_data;
        filesystem::path _Path;
    };

    enum class directory_options { none = 0, follow_directory_symlink = 1, skip_permission_denied = 2 };
    _BITMASK_OPS(directory_options)

    struct _Dir_enum_impl {
        _NODISCARD static __std_win_error _Advance_and_reset_if_no_more_files(shared_ptr<_Dir_enum_impl>& _Ptr) {
            auto& _Impl = *_Ptr;
            __std_fs_find_data _Data;
            do {
                const auto _Error = __std_fs_directory_iterator_advance(_Impl._Dir._Handle, &_Data);
                if (_Error == __std_win_error::_No_more_files) {
                    _Ptr.reset();
                    return __std_win_error::_Success;
                }

                if (_Error != __std_win_error::_Success) {
                    return _Error;
                }
            } while (_Is_dot_or_dotdot(_Data));
            _Impl._Refresh(_Data); // can throw
            return __std_win_error::_Success;
        }

        _NODISCARD static __std_win_error _Skip_dots(
            __std_fs_dir_handle _Dir_handle, __std_fs_find_data& _Data) noexcept {
            while (_Is_dot_or_dotdot(_Data)) {
                const auto _Error = __std_fs_directory_iterator_advance(_Dir_handle, &_Data);
                if (_Error != __std_win_error::_Success) {
                    return _Error;
                }
            }

            return __std_win_error::_Success;
        }

        _NODISCARD static __std_win_error _Open_dir(
            path& _Path, const directory_options _Options_arg, _Find_file_handle& _Dir, __std_fs_find_data& _Data) {
            const size_t _Null_term_len = _CSTD wcslen(_Path.c_str());
            if (_Null_term_len == 0 || _Null_term_len != _Path.native().size()) {
                return __std_win_error::_File_not_found;
            }

            _Path /= L"*"sv;
            auto _Error = _Dir._Open(_Path.c_str(), &_Data);
            if (_Error == __std_win_error::_Success) {
                return _Skip_dots(_Dir._Handle, _Data);
            }

            if (_Error == __std_win_error::_Access_denied
                && _Bitmask_includes(_Options_arg, directory_options::skip_permission_denied)) {
                _Error = __std_win_error::_No_more_files;
            }

            return _Error;
        }

        struct _Creator { // factored out part common to recursive and non-recursive implementation
            path _Path;
            _Find_file_handle _Dir;
            __std_fs_find_data _Find_data;

            struct _Create_status {
                bool _Should_create_impl;
                __std_win_error _Error;
            };

            _Create_status _Status;

            _Creator(const path& _Path_arg, const directory_options _Options) : _Path(_Path_arg) {
                const auto _Error = _Open_dir(_Path, _Options, _Dir, _Find_data);
                if (_Error == __std_win_error::_Success) {
                    _Status = {true, __std_win_error::_Success};
                } else if (_Error == __std_win_error::_No_more_files) {
                    _Status = {false, __std_win_error::_Success};
                } else {
                    _Status = {false, _Error};
                }
            }
        };

        void _Refresh(const __std_fs_find_data& _Data) {
            _Entry._Refresh(_Data);
            _Entry._Path.replace_filename(wstring_view{_Data._File_name});
        }

        explicit _Dir_enum_impl(_Creator&& _Create_data, const directory_options = {})
            : _Dir(_STD move(_Create_data._Dir)) {
            // directory_options provided, but unused to keep signature identical to recursive_directory_iterator
            _Entry._Path = _STD move(_Create_data._Path);
            _Refresh(_Create_data._Find_data);
        }

        template <class _Dir_enum_kind>
        _NODISCARD static __std_win_error _Initialize_dir_enum(
            shared_ptr<_Dir_enum_kind>& _Impl, const path& _Path, const directory_options _Options = {}) {
            _Creator _Create_data(_Path, _Options);
            if (_Create_data._Status._Should_create_impl) {
                _Impl = _STD make_shared<_Dir_enum_kind>(_STD move(_Create_data), _Options);
            }
            return _Create_data._Status._Error;
        }

        directory_entry _Entry;
        _Find_file_handle _Dir;
    };

    class directory_iterator;
    class recursive_directory_iterator;

    struct _Directory_entry_proxy {
        _NODISCARD directory_entry operator*() && noexcept {
            return _STD move(_Entry);
        }

    private:
        friend directory_iterator;
        friend recursive_directory_iterator;

        explicit _Directory_entry_proxy(const directory_entry& _Entry_arg) : _Entry(_Entry_arg) {}

        directory_entry _Entry;
    };

    class directory_iterator {
    public:
        using iterator_category = input_iterator_tag;
        using value_type        = directory_entry;
        using difference_type   = ptrdiff_t;
        using pointer           = const directory_entry*;
        using reference         = const directory_entry&;

        // [fs.dir.itr.members], member functions
        directory_iterator() noexcept = default;
        explicit directory_iterator(const path& _Path) {
            const auto _Error = _Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_iterator::directory_iterator", _Error, _Path);
            }
        }

        directory_iterator(const path& _Path, const directory_options _Options) {
            const auto _Error = _Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path, _Options);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_iterator::directory_iterator", _Error, _Path);
            }
        }

        directory_iterator(const path& _Path, error_code& _Ec) {
            _Ec = _Make_ec(_Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path));
        }

        directory_iterator(const path& _Path, const directory_options _Options, error_code& _Ec) {
            _Ec = _Make_ec(_Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path, _Options));
        }

        directory_iterator(const directory_iterator&) noexcept = default; // strengthened
        directory_iterator(directory_iterator&&) noexcept      = default;
        ~directory_iterator() noexcept                         = default;

        directory_iterator& operator=(const directory_iterator&) noexcept = default; // strengthened
        directory_iterator& operator=(directory_iterator&&) noexcept = default;

        _NODISCARD const directory_entry& operator*() const noexcept /* strengthened */ {
            return _Impl->_Entry;
        }

        _NODISCARD const directory_entry* operator->() const noexcept /* strengthened */ {
            return &**this;
        }

        directory_iterator& operator++() {
            const auto _Error = _Dir_enum_impl::_Advance_and_reset_if_no_more_files(_Impl);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("directory_iterator::operator++", _Error);
            }

            return *this;
        }

        directory_iterator& increment(error_code& _Ec) {
            _Ec = _Make_ec(_Dir_enum_impl::_Advance_and_reset_if_no_more_files(_Impl));
            return *this;
        }

        // other members as required by [input.iterators]:
        _NODISCARD bool operator==(const directory_iterator& _Rhs) const noexcept /* strengthened */ {
            return _Impl == _Rhs._Impl;
        }

#if !_HAS_CXX20
        _NODISCARD bool operator!=(const directory_iterator& _Rhs) const noexcept /* strengthened */ {
            return _Impl != _Rhs._Impl;
        }
#endif // !_HAS_CXX20

        _Directory_entry_proxy operator++(int) {
            _Directory_entry_proxy _Proxy(**this);
            ++*this;
            return _Proxy;
        }

        _NODISCARD bool _At_end() const noexcept {
            return !_Impl;
        }

    private:
        shared_ptr<_Dir_enum_impl> _Impl;
    };

    _NODISCARD inline directory_iterator begin(directory_iterator _Iter) noexcept {
        return _Iter;
    }

    _NODISCARD inline directory_iterator end(directory_iterator) noexcept {
        return {};
    }

    struct _Should_recurse_result {
        bool _Should_recurse;
        __std_win_error _Error;
    };

    struct _Recursive_dir_enum_impl : _Dir_enum_impl {
        vector<_Find_file_handle> _Stack;
        directory_options _Options = {};
        bool _Recursion_pending    = true;

        _NODISCARD _Should_recurse_result _Should_recurse() const noexcept {
            bool _Should_recurse   = false;
            __std_win_error _Error = __std_win_error::_Success;
            if (_Recursion_pending) {
                if (_Entry._Is_symlink_or_junction()) {
                    if (_Bitmask_includes(_Options, directory_options::follow_directory_symlink)) {
                        // check for broken symlink/junction
                        __std_fs_stats _Target_stats;
                        constexpr auto _Flags =
                            __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Follow_symlinks;
                        _Error = __std_fs_get_stats(
                            _Entry._Path.c_str(), &_Target_stats, _Flags, _Entry._Cached_data._Attributes);
                        if (_Error == __std_win_error::_Success) {
                            _Should_recurse =
                                _Bitmask_includes(_Target_stats._Attributes, __std_fs_file_attr::_Directory);
                        } else if (__std_is_file_not_found(_Error)
                                   || (_Error == __std_win_error::_Access_denied
                                       && _Bitmask_includes(_Options, directory_options::skip_permission_denied))) {
                            // skip broken symlinks and permission denied (when configured)
                            _Error = __std_win_error::_Success;
                        }
                    }
                } else {
                    _Should_recurse = _Entry._Has_cached_attribute(__std_fs_file_attr::_Directory);
                }
            }

            return {_Should_recurse, _Error};
        }

        _NODISCARD __std_win_error _Advance_and_skip_dots(__std_fs_find_data& _Data) noexcept {
            const auto _Error = __std_fs_directory_iterator_advance(_Dir._Handle, &_Data);
            if (_Error != __std_win_error::_Success) {
                return _Error;
            }

            return _Skip_dots(_Dir._Handle, _Data);
        }

        _NODISCARD static __std_win_error _Pop_and_reset_if_no_more_files(shared_ptr<_Recursive_dir_enum_impl>& _Ptr) {
            __std_win_error _Error;
            auto& _Impl = *_Ptr;
            __std_fs_find_data _Data;

            _Impl._Recursion_pending = true;

            do {
                if (_Impl._Stack.empty()) {
                    _Error = __std_win_error::_Success;
                    break;
                }

                _Impl._Dir = _STD move(_Impl._Stack.back());
                _Impl._Stack.pop_back();
                _Impl._Entry._Path._Remove_filename_and_separator();
                _Error = _Impl._Advance_and_skip_dots(_Data);

                if (_Error == __std_win_error::_Success) {
                    _Impl._Refresh(_Data);
                    return __std_win_error::_Success;
                }
            } while (_Error == __std_win_error::_No_more_files);

            _Ptr.reset();
            return _Error;
        }

        _NODISCARD static __std_win_error _Advance_and_reset_if_no_more_files(
            shared_ptr<_Recursive_dir_enum_impl>& _Ptr) {
            auto& _Impl = *_Ptr;
            __std_fs_find_data _Data;
            auto [_Should_recurse, _Error] = _Impl._Should_recurse();
            if (_Error != __std_win_error::_Success) {
                _Ptr.reset();
                return _Error;
            }

            if (_Should_recurse) {
                _Impl._Stack.push_back(_STD move(_Impl._Dir));
                _Error = _Open_dir(_Impl._Entry._Path, _Impl._Options, _Impl._Dir, _Data);
            } else {
                _Error = _Impl._Advance_and_skip_dots(_Data);
            }

            _Impl._Recursion_pending = true;
            for (;; _Error = _Impl._Advance_and_skip_dots(_Data)) {
                if (_Error == __std_win_error::_Success) {
                    _Impl._Refresh(_Data);
                    return __std_win_error::_Success;
                }

                if (_Error != __std_win_error::_No_more_files) {
                    break;
                }

                // no more files at this level, see if we can pop
                if (_Impl._Stack.empty()) { // nothing to pop, clear the error, reset and return
                    _Error = __std_win_error::_Success;
                    break;
                }

                _Impl._Dir = _STD move(_Impl._Stack.back());
                _Impl._Stack.pop_back();
                _Impl._Entry._Path._Remove_filename_and_separator();
            }

            _Ptr.reset();
            return _Error;
        }

        _Recursive_dir_enum_impl(_Dir_enum_impl::_Creator&& _Create_data, const directory_options _Options_arg)
            : _Dir_enum_impl(_STD move(_Create_data)), _Options(_Options_arg) {}
    };

    class recursive_directory_iterator {
    public:
        using iterator_category = input_iterator_tag;
        using value_type        = directory_entry;
        using difference_type   = ptrdiff_t;
        using pointer           = const directory_entry*;
        using reference         = const directory_entry&;

        // [fs.rec.dir.itr.members], constructors and destructor

        recursive_directory_iterator() noexcept = default;
        explicit recursive_directory_iterator(const path& _Path) {
            const auto _Error = _Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("recursive_directory_iterator::recursive_directory_iterator", _Error, _Path);
            }
        }

        recursive_directory_iterator(const path& _Path, const directory_options _Options) {
            const auto _Error = _Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path, _Options);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("recursive_directory_iterator::recursive_directory_iterator", _Error, _Path);
            }
        }

        recursive_directory_iterator(const path& _Path, const directory_options _Options, error_code& _Ec) {
            _Ec = _Make_ec(_Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path, _Options));
        }

        recursive_directory_iterator(const path& _Path, error_code& _Ec) {
            _Ec = _Make_ec(_Dir_enum_impl::_Initialize_dir_enum(_Impl, _Path));
        }

        recursive_directory_iterator(const recursive_directory_iterator&) noexcept = default; // strengthened
        recursive_directory_iterator(recursive_directory_iterator&&) noexcept      = default;
        ~recursive_directory_iterator() noexcept                                   = default;

        // [fs.rec.dir.itr.members], observers
        _NODISCARD directory_options options() const noexcept /* strengthened */ {
            return _Impl->_Options;
        }
        _NODISCARD int depth() const noexcept /* strengthened */ {
            // NT uses uint32_t to store the length of the path
            // that allows us 2^31 wchar_t per path.
            // A directory name should be at least 1 character, otherwise
            // adjacent directory separators will be treated as one.
            // Hence, we can only get to 2^30 entries in the stack.
            return static_cast<int>(_Impl->_Stack.size());
        }
        _NODISCARD bool recursion_pending() const noexcept /* strengthened */ {
            return _Impl->_Recursion_pending;
        }

        _NODISCARD const directory_entry& operator*() const noexcept /* strengthened */ {
            return _Impl->_Entry;
        }

        _NODISCARD const directory_entry* operator->() const noexcept /* strengthened */ {
            return &**this;
        }

        // [fs.rec.dir.itr.members], modifiers
        recursive_directory_iterator& operator=(recursive_directory_iterator&&) noexcept = default;
        recursive_directory_iterator& operator=(const recursive_directory_iterator&) noexcept = default; // strengthened

        recursive_directory_iterator& operator++() {
            const auto _Error = _Recursive_dir_enum_impl::_Advance_and_reset_if_no_more_files(_Impl);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("recursive_directory_iterator::operator++", _Error);
            }
            return *this;
        }

        recursive_directory_iterator& increment(error_code& _Ec) {
            _Ec = _Make_ec(_Recursive_dir_enum_impl::_Advance_and_reset_if_no_more_files(_Impl));
            return *this;
        }

        void pop() {
            const auto _Error = _Recursive_dir_enum_impl::_Pop_and_reset_if_no_more_files(_Impl);
            if (_Error != __std_win_error::_Success) {
                _Throw_fs_error("recursive_directory_iterator::pop", _Error);
            }
        }

        void pop(error_code& _Ec) {
            _Ec = _Make_ec(_Recursive_dir_enum_impl::_Pop_and_reset_if_no_more_files(_Impl));
        }

        void disable_recursion_pending() noexcept {
            _Impl->_Recursion_pending = false;
        }

        // other members as required by [input.iterators]:
        _NODISCARD bool operator==(const recursive_directory_iterator& _Rhs) const noexcept {
            return _Impl == _Rhs._Impl;
        }

#if !_HAS_CXX20
        _NODISCARD bool operator!=(const recursive_directory_iterator& _Rhs) const noexcept {
            return _Impl != _Rhs._Impl;
        }
#endif // !_HAS_CXX20

        _Directory_entry_proxy operator++(int) {
            _Directory_entry_proxy _Proxy(**this);
            ++*this;
            return _Proxy;
        }

    private:
        shared_ptr<_Recursive_dir_enum_impl> _Impl;
    };

    _NODISCARD inline recursive_directory_iterator begin(recursive_directory_iterator _Iter) noexcept {
        return _Iter;
    }

    _NODISCARD inline recursive_directory_iterator end(recursive_directory_iterator) noexcept {
        return {};
    }

    _NODISCARD inline __std_win_error _Absolute(path& _Result, const wstring& _Text) { // pre: _Result.empty()
        if (_Text.empty()) {
            return __std_win_error::_Success;
        }

        _Result._Text.resize(__std_fs_max_path);
        __std_win_error _Err;
        for (;;) {
            const auto _Requested_size = static_cast<unsigned long>(_Result._Text.size());
            const auto _Full_path_result =
                __std_fs_get_full_path_name(_Text.c_str(), _Requested_size, _Result._Text.data());
            _Err = _Full_path_result._Error;
            _Result._Text.resize(_Full_path_result._Size);
            if (_Full_path_result._Size < _Requested_size) {
                break;
            }
        }

        return _Err;
    }

    _NODISCARD inline path absolute(const path& _Input) {
        // normalize path according to system semantics, without touching the disk
        // calls GetFullPathNameW
        path _Result;
        const auto _Err = _Absolute(_Result, _Input.native());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("absolute", _Err, _Input);
        }

        return _Result;
    }

    _NODISCARD inline path absolute(const path& _Input, error_code& _Ec) {
        // normalize path according to system semantics, without touching the disk
        // calls GetFullPathNameW
        _Ec.clear(); // for exception safety
        path _Result;
        _Ec = _Make_ec(_Absolute(_Result, _Input.native()));
        return _Result;
    }

    _NODISCARD inline __std_win_error _Canonical(path& _Result, const wstring& _Text) { // pre: _Result.empty()
        if (_Text.empty()) {
            return __std_win_error::_Success;
        }

        auto _Name_kind = __std_fs_volume_name_kind::_Dos;
        {
            __std_win_error _Err;
            const _Fs_file _Handle(_Text.c_str(), __std_access_rights::_File_read_attributes,
                __std_fs_file_flags::_Backup_semantics, &_Err);
            if (_Err != __std_win_error::_Success) {
                return _Err;
            }

            _Result._Text.resize(__std_fs_max_path);
            for (;;) {
                const auto _Requested_size    = static_cast<unsigned long>(_Result._Text.size());
                const auto _Final_path_result = __std_fs_get_final_path_name_by_handle(
                    _Handle._Raw, _Result._Text.data(), _Requested_size, _Name_kind);
                _Err = _Final_path_result._Error;
                if (_Final_path_result._Size == 0) {
                    if (_Err == __std_win_error::_Path_not_found && _Name_kind == __std_fs_volume_name_kind::_Dos) {
                        // maybe there is no DOS name for the supplied path, retry with NT path
                        _Name_kind = __std_fs_volume_name_kind::_Nt;
                        continue;
                    }

                    _Result._Text.clear();
                    return _Err;
                }

                _Result._Text.resize(_Final_path_result._Size);
                if (_Final_path_result._Size < _Requested_size) {
                    break;
                }
            }
        } // close _Handle

        if (_Name_kind == __std_fs_volume_name_kind::_Dos) {
            if (_Is_drive_prefix_with_slash_slash_question(_Result._Text)) {
                // the result contains a \\?\ prefix but is just a drive letter, strip the \\?\ prefix
                _Result._Text.erase(0, 4);
            } else if (_Result._Text._Starts_with(LR"(\\?\UNC\)"sv)) {
                // the result contains a \\?\UNC\ prefix, replace with the simpler \\ prefix
                _Result._Text.erase(2, 6); // chop out ?\UNC\ leaving two preferred-separators
            }
        } else { // result is in the NT namespace, so apply the DOS to NT namespace prefix
            _Result._Text.insert(0, LR"(\\?\GLOBALROOT)"sv);
        }

        return __std_win_error::_Success;
    }

    _NODISCARD inline path canonical(const path& _Input) {
        // resolve the final path according to system semantics, by opening the file
        // calls GetFinalPathNameByHandleW
        path _Result;
        const auto _Err = _Canonical(_Result, _Input.native());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("canonical", _Err, _Input);
        }

        return _Result;
    }

    _NODISCARD inline path canonical(const path& _Input, error_code& _Ec) {
        // resolve the final path according to system semantics, by opening the file
        // calls GetFinalPathNameByHandleW
        _Ec.clear(); // for exception safety
        path _Result;
        _Ec = _Make_ec(_Canonical(_Result, _Input.native()));
        return _Result;
    }

    _NODISCARD inline unique_ptr<wchar_t[]> _Get_cleaned_symlink_target(const path& _To) noexcept {
        // transforms /s in the root-name to \s, and all other directory-separators into single \s
        // example: a/\/b -> a\b
        // example: //server/a////////b////////c////////d -> \\server\a\b\c\d
        const auto& _To_str = _To.native();
        // protected from overflow by wstring's max_size cap:
        unique_ptr<wchar_t[]> _Cleaned_link(::new (nothrow) wchar_t[_To_str.size() + 1]);
        if (!_Cleaned_link) {
            return _Cleaned_link;
        }

        const auto _First = _To_str.c_str();
        const auto _Last  = _First + _To_str.size();
        auto _Next        = _Find_root_name_end(_First, _Last);
        auto _Dest        = _STD replace_copy_if(_First, _Next, _Cleaned_link.get(), _Is_slash, L'\\');
        for (;;) {
            const wchar_t _Ch = *_Next;
            if (_Is_slash(_Ch)) {
                *_Dest = L'\\';
                do {
                    ++_Next;
                } while (_Is_slash(*_Next));
            } else {
                *_Dest = _Ch;
                if (_Ch == L'\0') {
                    break;
                }

                ++_Next;
            }

            ++_Dest;
        }

        return _Cleaned_link;
    }

    inline void create_directory_symlink(const path& _To, const path& _New_symlink) {
        // create a symlink for a directory, _New_symlink -> _To
        const auto _Cleaned = _Get_cleaned_symlink_target(_To);
        if (!_Cleaned) {
            _Xbad_alloc();
        }

        // note reversed parameter order:
        const auto _Err = __std_fs_create_directory_symbolic_link(_New_symlink.c_str(), _Cleaned.get());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("create_directory_symlink", _Err, _To, _New_symlink);
        }
    }

    inline void create_directory_symlink(const path& _To, const path& _New_symlink, error_code& _Ec) noexcept {
        // create a symlink for a directory, _New_symlink -> _To
        const auto _Cleaned = _Get_cleaned_symlink_target(_To);
        if (_Cleaned) {
            // note reversed parameter order:
            _Ec = _Make_ec(__std_fs_create_directory_symbolic_link(_New_symlink.c_str(), _Cleaned.get()));
        } else {
            _Ec = _STD make_error_code(errc::not_enough_memory);
        }
    }

    inline void create_hard_link(const path& _To, const path& _New_hard_link) {
        // create a hard link for a file, _New_hard_link -> _To
        // note reversed parameter order:
        const auto _Err = __std_fs_create_hard_link(_New_hard_link.c_str(), _To.c_str());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("create_hard_link", _Err, _To, _New_hard_link);
        }
    }

    inline void create_hard_link(const path& _To, const path& _New_hard_link, error_code& _Ec) noexcept {
        // create a hard link for a file, _New_hard_link -> _To
        // note reversed parameter order:
        _Ec = _Make_ec(__std_fs_create_hard_link(_New_hard_link.c_str(), _To.c_str()));
    }

    inline void create_symlink(const path& _To, const path& _New_symlink) {
        // create a symlink for a file, _New_symlink -> _To
        const auto _Cleaned = _Get_cleaned_symlink_target(_To);
        if (!_Cleaned) {
            _Xbad_alloc();
        }

        // note reversed parameter order:
        const auto _Err = __std_fs_create_symbolic_link(_New_symlink.c_str(), _Cleaned.get());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("create_symlink", _Err, _To, _New_symlink);
        }
    }

    inline void create_symlink(const path& _To, const path& _New_symlink, error_code& _Ec) noexcept {
        // create a symlink for a file, _New_symlink -> _To
        const auto _Cleaned = _Get_cleaned_symlink_target(_To);
        if (_Cleaned) {
            // note reversed parameter order:
            _Ec = _Make_ec(__std_fs_create_symbolic_link(_New_symlink.c_str(), _Cleaned.get()));
        } else {
            _Ec = _STD make_error_code(errc::not_enough_memory);
        }
    }

    _NODISCARD inline __std_win_error _Read_symlink_reparse_data(
        const _Fs_file& _Handle, unique_ptr<char[]>& _Buffer_unique_ptr) noexcept {
        constexpr auto _Buffer_size = 16 * 1024 + sizeof(wchar_t); // MAXIMUM_REPARSE_DATA_BUFFER_SIZE + sizeof(wchar_t)

        _Buffer_unique_ptr.reset(::new (nothrow) char[_Buffer_size]);
        if (!_Buffer_unique_ptr) {
            return __std_win_error::_Not_enough_memory;
        }

        const auto _Buffer         = reinterpret_cast<__std_fs_reparse_data_buffer*>(_Buffer_unique_ptr.get());
        const __std_win_error _Err = __std_fs_read_reparse_data_buffer(_Handle._Raw, _Buffer, _Buffer_size);
        if (_Err != __std_win_error::_Success) {
            return _Err;
        }

        return __std_win_error::_Success;
    }

    _NODISCARD inline __std_win_error _Read_symlink(const path& _Symlink_path, path& _Result) noexcept {
        __std_win_error _Err;
        unique_ptr<char[]> _Buffer_unique_ptr;
        {
            const _Fs_file _Handle(_Symlink_path.c_str(), __std_access_rights::_File_read_attributes,
                __std_fs_file_flags::_Backup_semantics | __std_fs_file_flags::_Open_reparse_point, &_Err);
            if (_Err != __std_win_error::_Success) {
                return _Err;
            }

            _Err = _Read_symlink_reparse_data(_Handle, _Buffer_unique_ptr);
            if (_Err != __std_win_error::_Success) {
                return _Err;
            }
        } // Close _Handle

        const auto _Buffer = reinterpret_cast<__std_fs_reparse_data_buffer*>(_Buffer_unique_ptr.get());
        unsigned short _Length;
        wchar_t* _Offset;
        _Err = __std_fs_read_name_from_reparse_data_buffer(_Buffer, &_Offset, &_Length);
        if (_Err != __std_win_error::_Success) {
            return _Err;
        }

        _Result._Text.resize(_Length);
        _CSTD memcpy(_Result._Text.data(), _Offset, _Length * sizeof(wchar_t));

        return __std_win_error::_Success;
    }

    _NODISCARD inline path read_symlink(const path& _Symlink_path, error_code& _Ec) {
        _Ec.clear();
        path _Result;
        _Ec = _Make_ec(_Read_symlink(_Symlink_path, _Result));
        return _Result;
    }

    _NODISCARD inline path read_symlink(const path& _Symlink_path) {
        path _Result;
        const auto _Err = _Read_symlink(_Symlink_path, _Result);
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("read_symlink", _Err, _Symlink_path);
        }
        return _Result;
    }

    _NODISCARD inline __std_win_error _Copy_symlink(const path& _Symlink, const path& _New_symlink) noexcept {
        __std_win_error _Err;
        unique_ptr<char[]> _Buffer_unique_ptr;
        bool _Is_directory;
        {
            const _Fs_file _Handle(_Symlink.c_str(), __std_access_rights::_File_read_attributes,
                __std_fs_file_flags::_Backup_semantics | __std_fs_file_flags::_Open_reparse_point, &_Err);
            if (_Err != __std_win_error::_Success) {
                return _Err;
            }

            _Err = _Read_symlink_reparse_data(_Handle, _Buffer_unique_ptr);
            if (_Err != __std_win_error::_Success) {
                return _Err;
            }

            unsigned long _File_attributes;
            _Err = __std_fs_get_file_attributes_by_handle(_Handle._Raw, &_File_attributes);
            if (_Err != __std_win_error::_Success) {
                return _Err;
            }

            _Is_directory = (_File_attributes & static_cast<unsigned long>(__std_fs_file_attr::_Directory)) != 0;
        } // Close _Handle

        const auto _Buffer = reinterpret_cast<__std_fs_reparse_data_buffer*>(_Buffer_unique_ptr.get());
        unsigned short _Length;
        wchar_t* _Offset;
        _Err = __std_fs_read_name_from_reparse_data_buffer(_Buffer, &_Offset, &_Length);
        if (_Err != __std_win_error::_Success) {
            return _Err;
        }

        _Offset[_Length] = L'\0';

        if (_Is_directory) {
            _Err = __std_fs_create_directory_symbolic_link(_New_symlink.c_str(), _Offset);
        } else {
            _Err = __std_fs_create_symbolic_link(_New_symlink.c_str(), _Offset);
        }

        return _Err;
    }

    inline void copy_symlink(const path& _Symlink, const path& _New_symlink, error_code& _Ec) {
        _Ec = _Make_ec(_Copy_symlink(_Symlink, _New_symlink));
    }

    inline void copy_symlink(const path& _Symlink, const path& _New_symlink) {
        const auto _Err = _Copy_symlink(_Symlink, _New_symlink);
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("copy_symlink", _Err, _Symlink, _New_symlink);
        }
    }

    inline bool copy_file(const path& _From, const path& _To, const copy_options _Options, error_code& _Ec) noexcept
    /* strengthened */ {
        // copy a file _From -> _To according to _Options
        const auto _Result =
            __std_fs_copy_file(_From.c_str(), _To.c_str(), static_cast<__std_fs_copy_options>(_Options));
        _Ec = _Make_ec(_Result._Error);
        return _Result._Copied;
    }

    inline bool copy_file(const path& _From, const path& _To, const copy_options _Options) {
        // copy a file _From -> _To according to _Options
        const auto _Result =
            __std_fs_copy_file(_From.c_str(), _To.c_str(), static_cast<__std_fs_copy_options>(_Options));
        if (_Result._Error != __std_win_error::_Success) {
            _Throw_fs_error("copy_file", _Result._Error, _From, _To);
        }

        return _Result._Copied;
    }

    inline bool copy_file(const path& _From, const path& _To, error_code& _Ec) noexcept /* strengthened */ {
        // copy a file _From -> _To, failing if the destination exists
        return _STD filesystem::copy_file(_From, _To, copy_options::none, _Ec);
    }

    inline bool copy_file(const path& _From, const path& _To) {
        // copy a file _From -> _To, failing if the destination exists
        return _STD filesystem::copy_file(_From, _To, copy_options::none);
    }

    _NODISCARD inline pair<__std_win_error, bool> _Equivalent(
        const wchar_t* const _Lhs, const wchar_t* const _Rhs) noexcept {
        __std_fs_file_id _Left_id;
        __std_fs_file_id _Right_id;
        auto _Last_error = __std_fs_get_file_id(&_Left_id, _Lhs);
        if (_Last_error != __std_win_error::_Success) {
            return {_Last_error, false};
        }

        _Last_error = __std_fs_get_file_id(&_Right_id, _Rhs);
        if (_Last_error != __std_win_error::_Success) {
            return {_Last_error, false};
        }

        return {__std_win_error::_Success, _CSTD memcmp(&_Left_id, &_Right_id, sizeof(__std_fs_file_id)) == 0};
    }

    _NODISCARD inline bool equivalent(const path& _Lhs, const path& _Rhs) {
        // test if the paths _Lhs and _Rhs refer to the same file
        const auto _Result = _Equivalent(_Lhs.c_str(), _Rhs.c_str());
        if (_Result.first != __std_win_error::_Success) {
            _Throw_fs_error("equivalent", _Result.first, _Lhs, _Rhs);
        }

        return _Result.second;
    }

    _NODISCARD inline bool equivalent(const path& _Lhs, const path& _Rhs, error_code& _Ec) noexcept {
        // test if the paths _Lhs and _Rhs refer to the same file
        const auto _Result = _Equivalent(_Lhs.c_str(), _Rhs.c_str());
        _Ec                = _Make_ec(_Result.first);
        return _Result.second;
    }

    _NODISCARD inline file_status status(const path& _Path);
    _NODISCARD inline file_status status(const path& _Path, error_code& _Ec) noexcept;
    _NODISCARD inline file_status symlink_status(const path& _Path);
    _NODISCARD inline file_status symlink_status(const path& _Path, error_code& _Ec) noexcept;

    _NODISCARD inline bool exists(const path& _Target, error_code& _Ec) noexcept {
        const auto _Type = _STD filesystem::status(_Target, _Ec).type();
        if (_Type != file_type::none) {
            _Ec.clear();
            return _Type != file_type::not_found;
        }

        return false;
    }

    _NODISCARD inline bool exists(const path& _Target) {
        error_code _Ec;
        const bool _Result = _STD filesystem::exists(_Target, _Ec);
        if (_Ec) {
            _Throw_fs_error("exists", _Ec, _Target);
        }

        return _Result;
    }

    _NODISCARD inline __std_win_error _File_size(const path& _Path, uintmax_t& _Result) noexcept {
        __std_fs_stats _Stats;
        const auto _Error = __std_fs_get_stats(
            _Path.c_str(), &_Stats, __std_fs_stats_flags::_Follow_symlinks | __std_fs_stats_flags::_File_size);
        if (_Error == __std_win_error::_Success) {
            _Result = _Stats._File_size;
        } else {
            _Result = static_cast<uintmax_t>(-1);
        }

        return _Error;
    }

    _NODISCARD inline uintmax_t file_size(const path& _Path) {
        uintmax_t _Result;
        const auto _Error = _File_size(_Path, _Result);
        if (_Error != __std_win_error::_Success) {
            _Throw_fs_error("file_size", _Error, _Path);
        }

        return _Result;
    }

    _NODISCARD inline uintmax_t file_size(const path& _Path, error_code& _Ec) noexcept {
        uintmax_t _Result;
        _Ec = _Make_ec(_File_size(_Path, _Result));
        return _Result;
    }

    _NODISCARD inline __std_win_error _Hard_link_count(const path& _Path, uintmax_t& _Result) noexcept {
        __std_fs_stats _Stats;
        const auto _Error = __std_fs_get_stats(
            _Path.c_str(), &_Stats, __std_fs_stats_flags::_Follow_symlinks | __std_fs_stats_flags::_Link_count);
        if (_Error == __std_win_error::_Success) {
            _Result = _Stats._Link_count;
        } else {
            _Result = static_cast<uintmax_t>(-1);
        }

        return _Error;
    }

    _NODISCARD inline uintmax_t hard_link_count(const path& _Target) { // get the number of hard links to _Target
        uintmax_t _Result;
        const auto _Err = _Hard_link_count(_Target.c_str(), _Result);
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("hard_link_count", _Err, _Target);
        }

        return _Result;
    }

    _NODISCARD inline uintmax_t hard_link_count(const path& _Target, error_code& _Ec) noexcept {
        // get the number of hard links to _Target
        uintmax_t _Result;
        _Ec = _Make_ec(_Hard_link_count(_Target.c_str(), _Result));
        return _Result;
    }

    _NODISCARD inline bool is_block_file(const path&) noexcept /* strengthened */ {
        // tests whether the input path is a block special file (never on Windows)
        return false;
    }

    _NODISCARD inline bool is_block_file(const path& _Path, error_code& _Ec) noexcept {
        // tests whether the input path is a block special file (never on Windows)
        (void) _STD filesystem::status(_Path, _Ec);
        return false; // note status sets _Ec to an error on nonexistent input
    }

    _NODISCARD inline bool is_character_file(const path&) noexcept /* strengthened */ {
        // tests whether the input path is a character special file (never on Windows)
        return false;
    }

    _NODISCARD inline bool is_character_file(const path& _Path, error_code& _Ec) noexcept {
        // tests whether the input path is a character special file (never on Windows)
        (void) _STD filesystem::status(_Path, _Ec);
        return false; // note status sets _Ec to an error on nonexistent input
    }

    _NODISCARD inline bool is_directory(const path& _Path) { // tests whether _Path is a directory
        return _STD filesystem::is_directory(_STD filesystem::status(_Path));
    }

    _NODISCARD inline bool is_directory(const path& _Path, error_code& _Ec) noexcept {
        // tests whether _Path is a directory
        return _STD filesystem::is_directory(_STD filesystem::status(_Path, _Ec));
    }

    _NODISCARD inline bool is_empty(const path& _Path, error_code& _Ec) {
        // test whether _Path refers to a zero sized file or empty directory
        constexpr auto _Flags = __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_File_size
                              | __std_fs_stats_flags::_Follow_symlinks;
        __std_fs_stats _Stats;
        const auto _Error = __std_fs_get_stats(_Path.c_str(), &_Stats, _Flags);
        _Ec               = _Make_ec(_Error);
        if (_Error != __std_win_error::_Success) {
            return false;
        }

        if ((_Stats._Attributes & __std_fs_file_attr::_Directory) == __std_fs_file_attr{}) {
            return _Stats._File_size == 0;
        } else {
            directory_iterator _Iter(_Path, _Ec);
            return !_Ec && _Iter._At_end();
        }
    }

    _NODISCARD inline bool is_empty(const path& _Path) {
        // test whether _Path refers to a zero sized file or empty directory
        error_code _Ec;
        const bool _Result = is_empty(_Path, _Ec);
        if (_Ec) {
            _Throw_fs_error("is_empty", _Ec, _Path);
        }

        return _Result;
    }

    _NODISCARD inline bool is_fifo(const path&) noexcept /* strengthened */ {
        // tests whether the input path is a fifo (never on Windows)
        return false;
    }

    _NODISCARD inline bool is_fifo(const path& _Path, error_code& _Ec) noexcept {
        // tests whether the input path is a fifo (never on Windows)
        (void) _STD filesystem::status(_Path, _Ec);
        return false; // note status sets _Ec to an error on nonexistent input
    }

    _NODISCARD inline bool is_other(const path& _Path) { // tests whether _Path is an other file (such as a junction)
        return _STD filesystem::is_other(_STD filesystem::status(_Path));
    }

    _NODISCARD inline bool is_other(const path& _Path, error_code& _Ec) noexcept {
        // tests whether _Path is an other file (such as a junction)
        return _STD filesystem::is_other(_STD filesystem::status(_Path, _Ec));
    }

    _NODISCARD inline bool is_regular_file(const path& _Path) { // tests whether _Path is a regular file
        return _STD filesystem::is_regular_file(_STD filesystem::status(_Path));
    }

    _NODISCARD inline bool is_regular_file(const path& _Path, error_code& _Ec) noexcept {
        // tests whether _Path is a regular file
        return _STD filesystem::is_regular_file(_STD filesystem::status(_Path, _Ec));
    }

    _NODISCARD inline bool is_socket(const path&) noexcept /* strengthened */ {
        // tests whether the input path is a socket (never on Windows)
        return false;
    }

    _NODISCARD inline bool is_socket(const path& _Path, error_code& _Ec) noexcept {
        // tests whether the input path is a socket (never on Windows)
        (void) _STD filesystem::status(_Path, _Ec);
        return false; // note status sets _Ec to an error on nonexistent input
    }

    _NODISCARD inline bool is_symlink(const path& _Path) { // tests whether _Path is a symlink
        return _STD filesystem::is_symlink(_STD filesystem::symlink_status(_Path));
    }

    _NODISCARD inline bool is_symlink(const path& _Path, error_code& _Ec) noexcept { // tests whether _Path is a symlink
        return _STD filesystem::is_symlink(_STD filesystem::symlink_status(_Path, _Ec));
    }

    inline bool remove(const path& _Target) {
        // remove file _Target (even if it is a directory); returns whether the file was removed
        // note !exists(_Target) is not an error, and merely returns false
        const auto _Result = __std_fs_remove(_Target.c_str());
        if (_Result._Error != __std_win_error::_Success) {
            _Throw_fs_error("remove", _Result._Error, _Target);
        }

        return _Result._Removed;
    }

    inline bool remove(const path& _Target, error_code& _Ec) noexcept {
        // remove file _Target (even if it is a directory); returns whether the file was removed
        // note !exists(_Target) is not an error, and merely returns false
        const auto _Result = __std_fs_remove(_Target.c_str());
        _Ec                = _Make_ec(_Result._Error);
        return _Result._Removed;
    }

    inline void rename(const path& _Old_p, const path& _New_p) {
        // rename _Old_p to _New_p, overwriting _New_p if it is an existing non-directory file
        // the standard explicitly allows an implementation to not replace _New_p if it is a directory,
        // and we take advantage of that here to be able to use MoveFileEx(... MOVEFILE_REPLACE_EXISTING)
        const auto _Err = __std_fs_rename(_Old_p.c_str(), _New_p.c_str());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("rename", _Err, _Old_p, _New_p);
        }
    }

    inline void rename(const path& _Old_p, const path& _New_p, error_code& _Ec) noexcept {
        // rename _Old_p to _New_p, overwriting _New_p if it is an existing non-directory file
        // the standard explicitly allows an implementation to not replace _New_p if it is a directory,
        // and we take advantage of that here to be able to use MoveFileEx(... MOVEFILE_REPLACE_EXISTING)
        _Ec = _Make_ec(__std_fs_rename(_Old_p.c_str(), _New_p.c_str()));
    }

    inline void resize_file(const path& _Target, const uintmax_t _New_size) {
        // set the size of _Target to _New_size
        const auto _Err = __std_fs_resize_file(_Target.c_str(), _New_size);
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("resize_file", _Err, _Target);
        }
    }

    inline void resize_file(const path& _Target, const uintmax_t _New_size, error_code& _Ec) noexcept {
        // set the size of _Target to _New_size
        _Ec = _Make_ec(__std_fs_resize_file(_Target.c_str(), _New_size));
    }

    struct space_info {
        uintmax_t capacity;
        uintmax_t free;
        uintmax_t available;

#if _HAS_CXX20
        _NODISCARD_FRIEND constexpr bool operator==(const space_info&, const space_info&) noexcept = default;
#endif // _HAS_CXX20
    };

    _NODISCARD inline space_info space(const path& _Target) {
        // get capacity information for the volume on which the file _Target resides
        space_info _Result;
        const auto _Last_error = __std_fs_space(_Target.c_str(), &_Result.available, &_Result.capacity, &_Result.free);
        if (_Last_error != __std_win_error::_Success) {
            _Throw_fs_error("space", _Last_error, _Target);
        }

        return _Result;
    }

    _NODISCARD inline space_info space(const path& _Target, error_code& _Ec) noexcept {
        // get capacity information for the volume on which the file _Target resides
        space_info _Result;
        _Ec = _Make_ec(__std_fs_space(_Target.c_str(), &_Result.available, &_Result.capacity, &_Result.free));
        return _Result;
    }

    _NODISCARD inline bool status_known(const file_status _Status) noexcept {
        return _Status.type() != file_type::none;
    }

    _NODISCARD inline _File_status_and_error _Get_any_status(
        const path& _Path, const __std_fs_stats_flags _Flags) noexcept {
        _File_status_and_error _Result;
        __std_fs_stats _Stats;

        const auto _Error = __std_fs_get_stats(_Path.c_str(), &_Stats, _Flags);
        _Result._Error    = _Error;
        _Result._Status._Refresh(_Error, _Stats);

        return _Result;
    }

    _NODISCARD inline file_status status(const path& _Path, error_code& _Ec) noexcept {
        const auto _Result = _Get_any_status(_Path, _Status_stats_flags);
        _Ec                = _Make_ec(_Result._Error);
        return _Result._Status;
    }

    _NODISCARD inline file_status status(const path& _Path) {
        const auto _Result = _Get_any_status(_Path, _Status_stats_flags);
        if (_Result._Not_good()) {
            _Throw_fs_error("status", _Result._Error, _Path);
        }
        return _Result._Status;
    }

    _NODISCARD inline file_status symlink_status(const path& _Path, error_code& _Ec) noexcept {
        const auto _Result = _Get_any_status(_Path, _Symlink_status_stats_flags);
        _Ec                = _Make_ec(_Result._Error);
        return _Result._Status;
    }

    _NODISCARD inline file_status symlink_status(const path& _Path) {
        const auto _Result = _Get_any_status(_Path, _Symlink_status_stats_flags);
        if (_Result._Not_good()) {
            _Throw_fs_error("symlink_status", _Result._Error, _Path);
        }

        return _Result._Status;
    }

    inline bool create_directory(const path& _Path) {
        const auto _Result = __std_fs_create_directory(_Path.c_str());
        if (_Result._Error != __std_win_error::_Success) {
            _Throw_fs_error("create_directory", _Result._Error, _Path);
        }

        return _Result._Created;
    }

    inline bool create_directory(const path& _Path, error_code& _Ec) noexcept {
        const auto _Result = __std_fs_create_directory(_Path.c_str());
        _Ec                = _Make_ec(_Result._Error);
        return _Result._Created;
    }

    inline bool create_directory(const path& _Path, const path& _Existing_p) {
        const auto _Result = __std_fs_create_directory(_Path.c_str());
        if (_Result._Error != __std_win_error::_Success) {
            _Throw_fs_error("create_directory", _Result._Error, _Path, _Existing_p);
        }

        return _Result._Created;
    }

    inline bool create_directory(const path& _Path, const path&, error_code& _Ec) noexcept {
        const auto _Result = __std_fs_create_directory(_Path.c_str());
        _Ec                = _Make_ec(_Result._Error);
        return _Result._Created;
    }

    inline bool create_directories(const path& _Path, error_code& _Ec) {
        if (_Path.empty()) {
            _Ec = _Make_ec(__std_win_error::_Path_not_found);
            return false;
        }

        _Ec.clear(); // for exception safety
        const wstring& _Text = _Path.native();
        wstring _Tmp;
        _Tmp.reserve(_Text.size());
        auto _Cursor        = _Text.data();
        const auto _End     = _Text.data() + _Text.size();
        auto _Root_path_end = _Find_relative_path(_Cursor, _End);
        if (_Root_path_end != _Cursor && _End - _Root_path_end >= 3 && _Is_drive_prefix(_Root_path_end)
            && _Is_slash(_Root_path_end[2])) {
            // \\?\ prefixes may have a drive letter suffix Windows will reject, strip
            _Root_path_end += 2;
        }

        _Tmp.append(_Cursor, _Root_path_end);
        _Cursor = _Root_path_end;

        // When creating directories, sometimes we get error reports on earlier directories.
        // Consider a case like X:\cat\dog\elk, where we get the following errors:
        // X: ERROR_ACCESS_DENIED
        // X:\cat ERROR_ALREADY_EXISTS
        // X:\cat\dog ERROR_ACCESS_DENIED
        // X:\cat\dog\elk ERROR_FILE_NOT_FOUND
        // Here, the previous access denied error prevented us from creating a parent directory,
        // and the subsequent ERROR_FILE_NOT_FOUND is not the interesting error for the user.
        // Therefore:
        //   If the last directory creation reports success, we return success.
        //   If the last directory creation fails, we return the most recent non-file-not-found error.
        //   If there is no such non-file-not-found error, we return the most recent error.

        bool _Created_last                              = false;
        __std_win_error _Error                          = __std_win_error::_Success;
        __std_win_error _Most_recent_not_file_not_found = __std_win_error::_Success;
        while (_Cursor != _End) {
            const auto _Added_end = _STD find_if(_STD find_if_not(_Cursor, _End, _Is_slash), _End, _Is_slash);
            _Tmp.append(_Cursor, _Added_end);
            const auto _Create_result = __std_fs_create_directory(_Tmp.c_str());
            _Error                    = _Create_result._Error;
            _Created_last             = _Create_result._Created;
            if (_Error != __std_win_error::_Success && !__std_is_file_not_found(_Error)) {
                _Most_recent_not_file_not_found = _Error;
            }

            _Cursor = _Added_end;
        }

        if (_Error != __std_win_error::_Success && _Most_recent_not_file_not_found != __std_win_error::_Success) {
            _Error = _Most_recent_not_file_not_found;
        }

        _Ec = _Make_ec(_Error);
        return _Created_last;
    }

    inline bool create_directories(const path& _Path) {
        error_code _Ec;
        const bool _Result = _STD filesystem::create_directories(_Path, _Ec);
        if (_Ec) {
            _Throw_fs_error("create_directories", _Ec, _Path);
        }

        return _Result;
    }

    inline constexpr int _Remove_all_retry_count = 10;

    inline void _Remove_all_dir(const path& _Path, error_code& _Ec, uintmax_t& _Removed_count) {
        // remove _Path, including any contents
        for (directory_iterator _It(_Path, _Ec);; _It.increment(_Ec)) { // remove nonempty directory contents
            if (_Ec) {
                if (_Ec.category() != _STD system_category()
                    || !__std_is_file_not_found(static_cast<__std_win_error>(_Ec.value()))) {
                    return;
                }

                break;
            }

            if (_It._At_end()) {
                break;
            }

            const auto& _Subpath  = _It->path();
            const auto _Substatus = _It->symlink_status(_Ec);
            if (_Ec) {
                return;
            }

            if (_Substatus.type() == file_type::directory) {
                _Remove_all_dir(_Subpath, _Ec, _Removed_count);
            } else {
                _Removed_count += _STD filesystem::remove(_Subpath, _Ec);
            }

            if (_Ec) {
                return;
            }
        }

        for (int _Retry = 0; _Retry < _Remove_all_retry_count; ++_Retry) {
            // retry up to _Remove_all_retry_count for resilience against
            // A/V tools, search indexers, backup tools, etc.
            const auto _Path_remove_result = __std_fs_remove(_Path.c_str());
            _Removed_count += _Path_remove_result._Removed;
            _Ec = _Make_ec(_Path_remove_result._Error);
            if (_Path_remove_result._Error != __std_win_error::_Directory_not_empty
                && _Path_remove_result._Error != __std_win_error::_Access_denied) {
                // ERROR_DIRECTORY_NOT_EMPTY if we're waiting for handles to children to be closed,
                // ERROR_ACCESS_DENIED if the directory we're targeting itself is marked for deletion.
                return;
            }
        }
    }

    inline uintmax_t remove_all(const path& _Path, error_code& _Ec) { // remove _Path, including any contents
        _Ec.clear(); // for exception safety
        const auto _First_remove_result = __std_fs_remove(_Path.c_str());
        uintmax_t _Removed_count        = _First_remove_result._Removed;
        _Ec                             = _Make_ec(_First_remove_result._Error);
        if (_First_remove_result._Error == __std_win_error::_Directory_not_empty) {
            _Remove_all_dir(_Path, _Ec, _Removed_count);
        }

        if (_Ec) {
            _Removed_count = static_cast<uintmax_t>(-1);
        }

        return _Removed_count;
    }

    inline uintmax_t remove_all(const path& _Path) {
        error_code _Ec;
        const auto _Removed_count = _STD filesystem::remove_all(_Path, _Ec);
        if (_Ec) {
            _Throw_fs_error("remove_all", _Ec, _Path);
        }

        return _Removed_count;
    }

    _NODISCARD inline __std_win_error _Last_write_time(const path& _Path, file_time_type& _Result) noexcept {
        __std_fs_stats _Stats;
        const auto _Error = __std_fs_get_stats(
            _Path.c_str(), &_Stats, __std_fs_stats_flags::_Follow_symlinks | __std_fs_stats_flags::_Last_write_time);
        if (_Error == __std_win_error::_Success) {
            _Result = file_time_type{file_time_type::duration{_Stats._Last_write_time}};
        } else {
            _Result = (file_time_type::min)();
        }

        return _Error;
    }

    _NODISCARD inline file_time_type last_write_time(const path& _Path) {
        file_time_type _Result;
        const auto _Error = _Last_write_time(_Path, _Result);
        if (_Error != __std_win_error::_Success) {
            _Throw_fs_error("last_write_time", _Error, _Path);
        }

        return _Result;
    }

    _NODISCARD inline file_time_type last_write_time(const path& _Path, error_code& _Ec) noexcept {
        file_time_type _Result;
        _Ec = _Make_ec(_Last_write_time(_Path, _Result));
        return _Result;
    }

    inline void last_write_time(const path& _Target, const file_time_type _New_time) {
        // set the last write time of _Target to _New_time
        const auto _Err = __std_fs_set_last_write_time(_New_time.time_since_epoch().count(), _Target.c_str());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("last_write_time", _Err, _Target);
        }
    }

    inline void last_write_time(const path& _Target, const file_time_type _New_time, error_code& _Ec) noexcept {
        // set the last write time of _Target to _New_time
        _Ec = _Make_ec(__std_fs_set_last_write_time(_New_time.time_since_epoch().count(), _Target.c_str()));
    }

    enum class perm_options { replace = 0x1, add = 0x2, remove = 0x4, nofollow = 0x8 };

    _BITMASK_OPS(perm_options)

    _NODISCARD inline __std_win_error _Permissions(
        const path& _Target, const perms _Perms, perm_options _Options) noexcept {
        bool _Readonly;
        const bool _Follow_symlinks = (_Options & perm_options::nofollow) == perm_options{};
        _Options &= ~perm_options::nofollow;
        const auto _Write_perms = _Perms & perms::_All_write;
        switch (_Options) {
        case perm_options::replace:
            // always apply FILE_ATTRIBUTE_READONLY according to _Perms
            _Readonly = _Write_perms == perms::none;
            break;
        case perm_options::add:
            if (_Write_perms == perms::none) {
                // if we aren't adding any write bits, then we won't change
                // FILE_ATTRIBUTE_READONLY, so there's nothing to do
                return __std_win_error::_Success;
            }

            _Readonly = false;
            break;
        case perm_options::remove:
            if (_Write_perms != perms::_All_write) {
                // if we aren't removing all write bits, then we won't change
                // FILE_ATTRIBUTE_READONLY, so there's nothing to do
                return __std_win_error::_Success;
            }

            _Readonly = true;
            break;
        case perm_options::nofollow: // avoid C4061
        default:
            return __std_win_error::_Invalid_parameter;
        }

        return __std_fs_change_permissions(_Target.c_str(), _Follow_symlinks, _Readonly);
    }

    inline void permissions(
        const path& _Target, const perms _Perms, const perm_options _Options = perm_options::replace) {
        const auto _Err = _Permissions(_Target, _Perms, _Options);
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("permissions", _Err, _Target);
        }
    }

    inline void permissions(
        const path& _Target, const perms _Perms, const perm_options _Options, error_code& _Ec) noexcept {
        _Ec = _Make_ec(_Permissions(_Target, _Perms, _Options));
    }

    inline void permissions(const path& _Target, const perms _Perms, error_code& _Ec) noexcept {
        _STD filesystem::permissions(_Target, _Perms, perm_options::replace, _Ec);
    }

    _NODISCARD inline path temp_directory_path(error_code& _Ec) {
        // get a location suitable for temporary storage, and verify that it is a directory
        _Ec.clear(); // for exception safety
        path _Result;
        _Result._Text.resize(__std_fs_temp_path_max);
        const auto _Temp_result = __std_fs_get_temp_path(_Result._Text.data());
        _Result._Text.resize(_Temp_result._Size);
        if (_Temp_result._Error == __std_win_error::_Max) { // path could be retrieved, but was not a directory
            _Ec = _STD make_error_code(errc::not_a_directory);
        } else {
            _Ec = _Make_ec(_Temp_result._Error);
        }

        return _Result;
    }

    _NODISCARD inline path temp_directory_path() {
        // get a location suitable for temporary storage, and verify that it is a directory
        error_code _Ec; // unusual arrangement to allow thrown error_code to have generic_category()
        path _Result(_STD filesystem::temp_directory_path(_Ec));
        if (_Ec) {
            _Throw_fs_error("temp_directory_path", _Ec, _Result);
        }
        return _Result;
    }

    _NODISCARD inline __std_win_error _Current_path(path& _Result) noexcept {
        _Result._Text.resize(__std_fs_max_path);
        for (;;) {
            const auto _Requested_size = static_cast<unsigned long>(_Result._Text.size());
            const auto _Temp_result    = __std_fs_get_current_path(_Requested_size, _Result._Text.data());
            _Result._Text.resize(_Temp_result._Size);
            if (_Temp_result._Size < _Requested_size) {
                return _Temp_result._Error;
            }
        }
    }

    _NODISCARD inline path current_path(error_code& _Ec) {
        _Ec.clear();
        path _Result;
        _Ec = _Make_ec(_Current_path(_Result));
        return _Result;
    }

    _NODISCARD inline path current_path() {
        path _Result;
        const auto _Err = _Current_path(_Result);
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("current_path()", _Err);
        }
        return _Result;
    }

    inline void current_path(const path& _To, error_code& _Ec) noexcept { // set the current path
        _Ec = _Make_ec(__std_fs_set_current_path(_To._Text.data()));
    }

    inline void current_path(const path& _To) { // set the current path
        const auto _Err = __std_fs_set_current_path(_To._Text.data());
        if (_Err != __std_win_error::_Success) {
            _Throw_fs_error("current_path(const path&)", _Err, _To);
        }
    }

    _NODISCARD inline path weakly_canonical(const path& _Input, error_code& _Ec) {
        // eventually calls GetFinalPathNameByHandleW
        _Ec.clear(); // for exception safety

        path _Temp;

        {
            const auto _Err = _Canonical(_Temp, _Input.native());

            if (_Err == __std_win_error::_Success) {
                return _Temp;
            }

            if (!__std_is_file_not_found(_Err)) {
                _Ec = _Make_ec(_Err);
                return {};
            }
        }

        const path _Normalized = _Input.lexically_normal();

        path _Result = _Normalized.root_path();

        const path _Normalized_relative = _Normalized.relative_path();

        bool _Call_canonical = true;

        for (const auto& _Elem : _Normalized_relative) {
            _Result /= _Elem;

            if (_Call_canonical) {
                _Temp.clear();

                const auto _Err = _Canonical(_Temp, _Result.native());

                if (_Err == __std_win_error::_Success) {
                    _Result = _STD move(_Temp);
                } else if (__std_is_file_not_found(_Err)) {
                    _Call_canonical = false;
                } else {
                    _Ec = _Make_ec(_Err);
                    return {};
                }
            }
        }

        return _Result;
    }

    _NODISCARD inline path weakly_canonical(const path& _Input) {
        // eventually calls GetFinalPathNameByHandleW
        error_code _Ec;

        path _Result = _STD filesystem::weakly_canonical(_Input, _Ec);

        if (_Ec) {
            _Throw_fs_error("weakly_canonical", _Ec, _Input);
        }

        return _Result;
    }

    _NODISCARD inline path proximate(const path& _Path, const path& _Base = _STD filesystem::current_path()) {
        // eventually calls GetFinalPathNameByHandleW
        const path _Weakly_canonical_path = _STD filesystem::weakly_canonical(_Path);
        const path _Weakly_canonical_base = _STD filesystem::weakly_canonical(_Base);
        return _Weakly_canonical_path.lexically_proximate(_Weakly_canonical_base);
    }

    _NODISCARD inline path proximate(const path& _Path, const path& _Base, error_code& _Ec) {
        // eventually calls GetFinalPathNameByHandleW
        const path _Weakly_canonical_path = _STD filesystem::weakly_canonical(_Path, _Ec);

        if (_Ec) {
            return {};
        }

        const path _Weakly_canonical_base = _STD filesystem::weakly_canonical(_Base, _Ec);

        if (_Ec) {
            return {};
        }

        return _Weakly_canonical_path.lexically_proximate(_Weakly_canonical_base);
    }

    _NODISCARD inline path proximate(const path& _Path, error_code& _Ec) {
        // eventually calls GetFinalPathNameByHandleW
        const path _Base = _STD filesystem::current_path(_Ec);
        // N4810 29.11.14.27 [fs.op.proximate]/1 incorrectly calls current_path()
        if (_Ec) {
            return {};
        }

        return _STD filesystem::proximate(_Path, _Base, _Ec);
    }

    _NODISCARD inline path relative(const path& _Path, const path& _Base = _STD filesystem::current_path()) {
        // eventually calls GetFinalPathNameByHandleW
        const path _Weakly_canonical_path = _STD filesystem::weakly_canonical(_Path);
        const path _Weakly_canonical_base = _STD filesystem::weakly_canonical(_Base);
        return _Weakly_canonical_path.lexically_relative(_Weakly_canonical_base);
    }

    _NODISCARD inline path relative(const path& _Path, const path& _Base, error_code& _Ec) {
        // eventually calls GetFinalPathNameByHandleW
        const path _Weakly_canonical_path = _STD filesystem::weakly_canonical(_Path, _Ec);

        if (_Ec) {
            return {};
        }

        const path _Weakly_canonical_base = _STD filesystem::weakly_canonical(_Base, _Ec);

        if (_Ec) {
            return {};
        }

        return _Weakly_canonical_path.lexically_relative(_Weakly_canonical_base);
    }

    _NODISCARD inline path relative(const path& _Path, error_code& _Ec) {
        // eventually calls GetFinalPathNameByHandleW
        const path _Base = _STD filesystem::current_path(_Ec);
        // N4810 29.11.14.29 [fs.op.relative]/1 incorrectly calls current_path()
        if (_Ec) {
            return {};
        }

        return _STD filesystem::relative(_Path, _Base, _Ec);
    }

    inline void _Copy_impl(
        const directory_entry& _From, const path& _To, const copy_options _Options, error_code& _Ec) {
        // implement copy, does not clear _Ec for callers
        // Standard quotes herein are relative to N4810
        // The following parts of LWG-3057 are implemented:
        // * guarding equivalent() from nonexistent to
        // * replacing unspecified recursion prevention tag with copy_options::directories_only
        // Other parts of LWG-3057 remain under discussion in the committee and are not yet implemented.
        //  (In particular, changes to existing destination flags, and error handling).
        const bool _Flink = (_Options & (copy_options::skip_symlinks | copy_options::copy_symlinks))
                         != copy_options::none; // create_symlinks intentionally removed by LWG-3057
        const auto _Fstat = _From._Get_any_status(_Flink ? _Symlink_status_stats_flags : _Status_stats_flags);
        if (_Fstat._Error != __std_win_error::_Success) { // report an error if exists(f) is false
            _Ec = _Make_ec(_Fstat._Error);
            return;
        }

        const bool _Tlink =
            (_Options & (copy_options::create_symlinks | copy_options::skip_symlinks)) != copy_options::none;
        const auto _Tstat = _Get_any_status(_To, _Tlink ? _Symlink_status_stats_flags : _Status_stats_flags);
        if (_Tstat._Not_good()) {
            _Ec = _Make_ec(_Tstat._Error);
            return;
        }

        if (_STD filesystem::exists(_Tstat._Status)) {
            if (_STD filesystem::equivalent(_From, _To, _Ec)) { // report an error if equivalent(from, to) is true
                _Ec = _STD make_error_code(errc::file_exists);
                return;
            }

            if (_Ec) {
                return;
            }
        }

        if (_STD filesystem::is_other(_Fstat._Status) || _STD filesystem::is_other(_Tstat._Status)) {
            // report an error if is_other(f) || is_other(t) is true
            _Ec = _STD make_error_code(errc::operation_not_supported);
            return;
        }

        if (_STD filesystem::is_directory(_Fstat._Status) && _STD filesystem::is_regular_file(_Tstat._Status)) {
            // report an error if is_directory(f) && is_regular_file(t) is true
            _Ec = _STD make_error_code(errc::file_exists);
            return;
        }

        if (_STD filesystem::is_symlink(_Fstat._Status)) {
            if ((_Options & copy_options::skip_symlinks) != copy_options::none) {
                return;
            }

            if (!_STD filesystem::exists(_Tstat._Status)
                && (_Options & copy_options::copy_symlinks) != copy_options::none) {
                // if (condition) then copy_symlink(from, to)
                _STD filesystem::copy_symlink(_From, _To, _Ec);
                return;
            }

            // otherwise report an error
            _Ec = _STD make_error_code(errc::operation_not_supported);
            return;
        }

        if (_STD filesystem::is_regular_file(_Fstat._Status)) {
            if ((_Options & copy_options::directories_only) != copy_options::none) {
                return;
            }

            if ((_Options & copy_options::create_symlinks) != copy_options::none) {
                // Otherwise, if (condition) then create a symbolic link to the source file
                _STD filesystem::create_symlink(_From, _To, _Ec);
                return;
            }

            if ((_Options & copy_options::create_hard_links) != copy_options::none) {
                // Otherwise, if (condition) then create a hard link to the source file
                _STD filesystem::create_hard_link(_From, _To, _Ec);
                return;
            }

            if (_STD filesystem::is_directory(_Tstat._Status)) {
                // Otherwise, if is_directory(t), then copy_file(from, to / from.filename(), options)
                _STD filesystem::copy_file(_From, _To / _From.path().filename(), _Options, _Ec);
                return;
            }

            // Otherwise, copy_file(_From, _To, _Options)
            _STD filesystem::copy_file(_From, _To, _Options, _Ec);
            return;
        }

        // The following condition modified by LWG-3057:
        if (_STD filesystem::is_directory(_Fstat._Status)) {
            if ((_Options & copy_options::create_symlinks) != copy_options::none) {
                _Ec = _STD make_error_code(errc::is_a_directory);
                return;
            }

            _STD filesystem::create_directory(_To, _From, _Ec);
            if (_Ec) {
                return;
            }

            // Note LWG-3057 uses directories_only as the flag, instead of an unspecified copy_options value:
            if ((_Options & copy_options::recursive) != copy_options::none
                || (_Options & copy_options::directories_only) == copy_options::none) {
                for (directory_iterator _It(_From, _Ec);; _It.increment(_Ec)) {
                    if (_Ec || _It._At_end()) {
                        return;
                    }

                    // if ((options & copy_options::recursive) != copy_options::none ||
                    //      !is_directory(linkf ? symlink_status(x.path()) : status(x.path())))
                    //        copy(x.path(), to/x.path().filename(), options);
                    bool _Recurse = (_Options & copy_options::recursive) != copy_options::none;
                    if (!_Recurse) {
                        const auto _Child_status_result =
                            _It->_Get_any_status(_Flink ? _Symlink_status_stats_flags : _Status_stats_flags);
                        if (_Child_status_result._Error != __std_win_error::_Success) {
                            _Ec = _Make_ec(_Child_status_result._Error);
                            return;
                        }

                        _Recurse = !_STD filesystem::is_directory(_Child_status_result._Status);
                    }

                    if (_Recurse) {
                        _Copy_impl(*_It, _To / _It->path().filename(), _Options, _Ec);
                        if (_Ec) {
                            return;
                        }
                    }
                }
            }
        }

        // Otherwise, no effects.
    }

    inline void copy(const path& _From, const path& _To, const copy_options _Options, error_code& _Ec) {
        const directory_entry _From_dir(_From, _Ec);
        if (_Ec) { // report an error if exists(f) is false
            return;
        }

        _Copy_impl(_From_dir, _To, _Options, _Ec);
    }

    inline void copy(const path& _From, const path& _To, const copy_options _Options) {
        error_code _Ec;
        _STD filesystem::copy(_From, _To, _Options, _Ec);
        if (_Ec) {
            _Throw_fs_error("copy", _Ec, _From, _To);
        }
    }

    inline void copy(const path& _From, const path& _To, error_code& _Ec) {
        return _STD filesystem::copy(_From, _To, copy_options::none, _Ec);
    }

    inline void copy(const path& _From, const path& _To) {
        return _STD filesystem::copy(_From, _To, copy_options::none);
    }
} // namespace filesystem

#ifdef __cpp_lib_concepts
namespace ranges {
    template <>
    inline constexpr bool enable_borrowed_range<filesystem::directory_iterator> = true;
    template <>
    inline constexpr bool enable_borrowed_range<filesystem::recursive_directory_iterator> = true;
    template <>
    inline constexpr bool enable_view<filesystem::directory_iterator> = true;
    template <>
    inline constexpr bool enable_view<filesystem::recursive_directory_iterator> = true;
} // namespace ranges
#endif // __cpp_lib_concepts

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _HAS_CXX17
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _FILESYSTEM_
