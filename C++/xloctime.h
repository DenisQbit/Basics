// xloctime internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XLOCTIME_
#define _XLOCTIME_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <ctime>
#include <iterator>
#include <xlocnum>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
struct _CRTIMP2_PURE_IMPORT time_base : locale::facet { // base class for time_get
    enum dateorder { // constants for different orders of date components
        no_order,
        dmy,
        mdy,
        ymd,
        ydm
    };

    __CLR_OR_THIS_CALL time_base(size_t _Refs = 0) : locale::facet(_Refs) {}

    __CLR_OR_THIS_CALL ~time_base() noexcept {}
};

template <class _Elem, class _InIt = istreambuf_iterator<_Elem, char_traits<_Elem>>>
class time_get : public time_base { // facet for converting text to encoded times
private:
    friend _Tidy_guard<time_get>;

public:
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || _Is_any_of_v<_Elem, char, wchar_t, unsigned short>,
        _FACET_SPECIALIZATION_MESSAGE);

    using char_type = _Elem;
    using iter_type = _InIt;
    using _Ctype    = ctype<_Elem>;

    __PURE_APPDOMAIN_GLOBAL static locale::id id; // unique facet id

    dateorder __CLR_OR_THIS_CALL date_order() const {
        return do_date_order();
    }

    _InIt __CLR_OR_THIS_CALL get_time(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State,
        tm* _Pt) const { // get time of day from [_First, _Last) into _Pt
        return do_get_time(_First, _Last, _Iosbase, _State, _Pt);
    }

    _InIt __CLR_OR_THIS_CALL get_date(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State,
        tm* _Pt) const { // get date from [_First, _Last) into _Pt
        return do_get_date(_First, _Last, _Iosbase, _State, _Pt);
    }

    _InIt __CLR_OR_THIS_CALL get_weekday(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State,
        tm* _Pt) const { // get weekday from [_First, _Last) into _Pt
        return do_get_weekday(_First, _Last, _Iosbase, _State, _Pt);
    }

    _InIt __CLR_OR_THIS_CALL get_monthname(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State,
        tm* _Pt) const { // get month from [_First, _Last) into _Pt
        return do_get_monthname(_First, _Last, _Iosbase, _State, _Pt);
    }

    _InIt __CLR_OR_THIS_CALL get_year(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State,
        tm* _Pt) const { // get year from [_First, _Last) into _Pt
        return do_get_year(_First, _Last, _Iosbase, _State, _Pt);
    }

    _InIt __CLR_OR_THIS_CALL get(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State, tm* _Pt,
        char _Specifier, char _Modifier = '\0') const { // get formatted time for _Specifier/_Modifier
        return do_get(_First, _Last, _Iosbase, _State, _Pt, _Specifier, _Modifier);
    }

    _InIt __CLR_OR_THIS_CALL get(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State, tm* _Pt,
        const _Elem* _Fmtfirst, const _Elem* _Fmtlast) const { // get formatted time for format string
        _Adl_verify_range(_Fmtfirst, _Fmtlast);
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        _State = ios_base::goodbit;

        for (; _Fmtfirst != _Fmtlast; ++_Fmtfirst) {
            if (_State != ios_base::goodbit) {
                // N4878 [locale.time.get.members]/8.2
                // _State is fail, eof, or bad. Do not proceed to the next fields. Return with current _State.
                break;
            } else if (_First == _Last) {
                // N4878 [locale.time.get.members]/8.3
                _State = ios_base::eofbit | ios_base::failbit;
                break;
            } else if (_Ctype_fac.narrow(*_Fmtfirst) != '%') { // match literal element
                if (_Ctype_fac.is(_Ctype::space, *_Fmtfirst)) {
                    while (_First != _Last && _Ctype_fac.is(_Ctype::space, *_First)) {
                        ++_First;
                    }
                } else if (_Ctype_fac.tolower(*_First) != _Ctype_fac.tolower(*_Fmtfirst)) { // bad literal match
                    _State |= ios_base::failbit;
                    break;
                } else {
                    ++_First;
                }
            } else if (++_Fmtfirst == _Fmtlast) {
                // N4878 [locale.time.get.members]/8.4: "If the number of elements in the range [fmt, fmtend) is not
                // sufficient to unambiguously determine whether the conversion specification is complete and valid,
                // the function evaluates err = ios_base::failbit."
                _State = ios_base::failbit;
                break;
            } else { // get specifier after %
                char _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                char _Modifier  = '\0';

                if (_Specifier == 'E' || _Specifier == 'O' || _Specifier == 'Q' || _Specifier == '#') {
                    if (++_Fmtfirst == _Fmtlast) { // no specifier
                        _State = ios_base::failbit;
                        break;
                    } else { // save both qualifier and specifier
                        _Modifier  = _Specifier;
                        _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                    }
                }

                _First = do_get(_First, _Last, _Iosbase, _State, _Pt, _Specifier, _Modifier); // convert a single field
            }
        }

        return _First;
    }

    explicit __CLR_OR_THIS_CALL time_get(size_t _Refs = 0) : time_base(_Refs) { // construct from current locale
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL time_get(const _Locinfo& _Lobj, size_t _Refs = 0) : time_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new time_get<_Elem, _InIt>(_Locinfo(_Ploc->c_str()));
        }

        return _X_TIME;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~time_get() noexcept {
        _Tidy();
    }

    __CLR_OR_THIS_CALL time_get(const char* _Locname, size_t _Refs = 0) : time_base(_Refs) {
        _BEGIN_LOCINFO(_Lobj(_Locname))
        _Init(_Lobj);
        _END_LOCINFO()
    }

    template <class _Elem2>
    void __CLR_OR_THIS_CALL _Getvals(_Elem2, const _Locinfo& _Lobj) { // get values
        _Cvt = _Lobj._Getcvt();

        if (is_same_v<_Elem2, wchar_t>) {
            _Days = reinterpret_cast<const _Elem*>(_Maklocwcs(reinterpret_cast<const wchar_t*>(_Lobj._W_Getdays())));
            _Months =
                reinterpret_cast<const _Elem*>(_Maklocwcs(reinterpret_cast<const wchar_t*>(_Lobj._W_Getmonths())));
            _Ampm = reinterpret_cast<const _Elem*>(_Maklocwcs(L":AM:am:PM:pm"));
        } else {
            _Days   = _Maklocstr(_Lobj._Getdays(), static_cast<_Elem*>(nullptr), _Cvt);
            _Months = _Maklocstr(_Lobj._Getmonths(), static_cast<_Elem*>(nullptr), _Cvt);
            _Ampm   = _Maklocstr(":AM:am:PM:pm", static_cast<_Elem*>(nullptr), _Cvt);
        }
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Days   = nullptr;
        _Months = nullptr;
        _Ampm   = nullptr;

        _Tidy_guard<time_get> _Guard{this};
        _Getvals(_Elem{}, _Lobj);
        _Dateorder     = static_cast<dateorder>(_Lobj._Getdateorder());
        _Guard._Target = nullptr;
    }

    virtual dateorder __CLR_OR_THIS_CALL do_date_order() const {
        return _Dateorder;
    }

    virtual _InIt __CLR_OR_THIS_CALL do_get_time(_InIt _First, _InIt _Last, ios_base& _Iosbase,
        ios_base::iostate& _State, tm* _Pt) const { // get time of day from [_First, _Last) into _Pt
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        _State |= _Getint(_First, _Last, 0, 23, _Pt->tm_hour, _Ctype_fac);

        if (_State != ios_base::goodbit || _Ctype_fac.narrow(*_First) != ':') {
            _State |= ios_base::failbit; // hour field is bad
        } else {
            _State |= _Getint(++_First, _Last, 0, 59, _Pt->tm_min, _Ctype_fac);
        }

        if (_State != ios_base::goodbit || _Ctype_fac.narrow(*_First) != ':') {
            _State |= ios_base::failbit; // min field is bad
        } else {
            _State |= _Getint(++_First, _Last, 0, 60, _Pt->tm_sec, _Ctype_fac);
        }

        return _First;
    }

    virtual _InIt __CLR_OR_THIS_CALL do_get_date(_InIt _First, _InIt _Last, ios_base& _Iosbase,
        ios_base::iostate& _State, tm* _Pt) const { // get date from [_First, _Last) into _Pt
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        dateorder _Dorder = date_order();
        if (_Dorder == no_order) {
            _Dorder = mdy;
        }

        if (_First != _Last) {
            if (!_Ctype_fac.is(_Ctype::digit, *_First)) { // begins with month name, assume mdy
                _First  = get_monthname(_First, _Last, _Iosbase, _State, _Pt);
                _Dorder = mdy;
            } else if (_Dorder == mdy) { // get month number
                _State |= _Getint(_First, _Last, 1, 12, _Pt->tm_mon, _Ctype_fac);
                --_Pt->tm_mon;
            } else if (_Dorder == dmy) {
                _State |= _Getint(_First, _Last, 1, 31, _Pt->tm_mday, _Ctype_fac);
            } else { // ymd or ydm
                _First = get_year(_First, _Last, _Iosbase, _State, _Pt);
            }
        }

        while (_First != _Last && _Ctype_fac.is(_Ctype::space, *_First)) {
            ++_First; // skip spaces
        }

        if (_First != _Last) { // skip [:,/]
            char _Ch = _Ctype_fac.narrow(*_First);
            if (_Ch == ':' || _Ch == ',' || _Ch == '/') {
                ++_First;
            }
        }

        while (_First != _Last && _Ctype_fac.is(_Ctype::space, *_First)) {
            ++_First; // skip spaces
        }

        if (_First != _Last) {
            if (!_Ctype_fac.is(_Ctype::digit, *_First)) {
                if (_Dorder == mdy) {
                    _State |= ios_base::failbit; // error, month already seen
                } else { // month name is second, like it or not
                    _First = get_monthname(_First, _Last, _Iosbase, _State, _Pt);
                    if (_Dorder == ydm) {
                        _Dorder = ymd;
                    }
                }
            } else if (_Dorder == dmy || _Dorder == ymd) { // get month number
                _State |= _Getint(_First, _Last, 1, 12, _Pt->tm_mon, _Ctype_fac);
                --_Pt->tm_mon;
            } else {
                _State |= _Getint(_First, _Last, 1, 31, _Pt->tm_mday, _Ctype_fac);
            }
        }

        while (_First != _Last && _Ctype_fac.is(_Ctype::space, *_First)) {
            ++_First; // skip spaces
        }

        if (_First != _Last) { // skip [:,/]
            char _Ch = _Ctype_fac.narrow(*_First);
            if (_Ch == ':' || _Ch == ',' || _Ch == '/') {
                ++_First;
            }
        }

        while (_First != _Last && _Ctype_fac.is(_Ctype::space, *_First)) {
            ++_First; // skip spaces
        }

        if (_First == _Last) {
            _State |= ios_base::failbit; // error, missing component(s)
        } else if (!_Ctype_fac.is(_Ctype::digit, *_First)) {
            if (_Dorder != ydm) {
                _State |= ios_base::failbit; // error, month out of place
            } else {
                _First = get_monthname(_First, _Last, _Iosbase, _State, _Pt);
            }
        } else if (_Dorder == ydm) { // get month number
            _State |= _Getint(_First, _Last, 1, 12, _Pt->tm_mon, _Ctype_fac);
            --_Pt->tm_mon;
        } else if (_Dorder == ymd) {
            _State |= _Getint(_First, _Last, 1, 31, _Pt->tm_mday, _Ctype_fac);
        } else { // mdy or dmy
            _First = get_year(_First, _Last, _Iosbase, _State, _Pt);
        }

        if (_First == _Last) {
            _State |= ios_base::eofbit;
        }

        return _First;
    }

    virtual _InIt __CLR_OR_THIS_CALL do_get_weekday(_InIt _First, _InIt _Last, ios_base&, ios_base::iostate& _State,
        tm* _Pt) const { // get weekday from [_First, _Last) into _Pt
        int _Num = _Getloctxt(_First, _Last, 0, _Days, _Case_sensitive::_Nope);
        if (_Num < 0) {
            _State |= ios_base::failbit;
        } else {
            _Pt->tm_wday = _Num >> 1; // set wday
        }

        return _First;
    }

    virtual _InIt __CLR_OR_THIS_CALL do_get_monthname(_InIt _First, _InIt _Last, ios_base&, ios_base::iostate& _State,
        tm* _Pt) const { // get month from [_First, _Last) into _Pt
        int _Num = _Getloctxt(_First, _Last, 0, _Months, _Case_sensitive::_Nope);

        if (_Num < 0) {
            _State |= ios_base::failbit;
        } else {
            _Pt->tm_mon = _Num >> 1; // set mon
        }

        return _First;
    }

    virtual _InIt __CLR_OR_THIS_CALL do_get_year(_InIt _First, _InIt _Last, ios_base& _Iosbase,
        ios_base::iostate& _State, tm* _Pt) const { // get year from [_First, _Last) into _Pt
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        int _Ans               = 0;
        ios_base::iostate _Res = _Getint(_First, _Last, 0, 9999, _Ans, _Ctype_fac);

        _State |= _Res; // pass on eofbit and failbit
        if (!(_Res & ios_base::failbit)) {
            if (_Ans < 69) {
                _Pt->tm_year = _Ans + 100; // [0, 68] parsed as [2000, 2068]
            } else if (_Ans < 100) {
                _Pt->tm_year = _Ans; // [69, 99] parsed as [1969, 1999]
            } else {
                _Pt->tm_year = _Ans - 1900; // [100, 9999] parsed literally
            }
        }

        return _First;
    }

    virtual _InIt __CLR_OR_THIS_CALL do_get(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State,
        tm* _Pt, char _Specifier, char = 0) const { // get formatted time for _Specifier (_Modifier ignored)
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());
        int _Ans                 = 0;

        _State = ios_base::goodbit;

        switch (_Specifier) { // process format specifier
        case 'a':
        case 'A':
            _First = get_weekday(_First, _Last, _Iosbase, _State, _Pt);
            break;

        case 'b':
        case 'B':
        case 'h':
            _First = get_monthname(_First, _Last, _Iosbase, _State, _Pt);
            break;

        case 'c':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, "%b %d %H : %M : %S %Y");
            break;

        case 'C':
            _State |= _Getint(_First, _Last, 0, 99, _Ans, _Ctype_fac);
            if (!(_State & ios_base::failbit)) {
                _Pt->tm_year = _Ans * 100 - 1900; // convert to century
            }

            break;

        case 'd':
        case 'e':
            _State |= _Getint(_First, _Last, 1, 31, _Pt->tm_mday, _Ctype_fac);
            break;

        case 'D':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, "%m / %d / %y");
            break;

        case 'H':
            _State |= _Getint(_First, _Last, 0, 23, _Pt->tm_hour, _Ctype_fac);
            break;

        case 'I':
            _State |= _Getint(_First, _Last, 1, 12, _Ans, _Ctype_fac);
            if (!(_State & ios_base::failbit)) {
                _Pt->tm_hour = _Ans == 12 ? 0 : _Ans;
            }

            break;

        case 'j':
            _State |= _Getint(_First, _Last, 1, 366, _Pt->tm_yday, _Ctype_fac);
            break;

        case 'm':
            _State |= _Getint(_First, _Last, 1, 12, _Ans, _Ctype_fac);
            if (!(_State & ios_base::failbit)) {
                _Pt->tm_mon = _Ans - 1;
            }

            break;

        case 'M':
            _State |= _Getint(_First, _Last, 0, 59, _Pt->tm_min, _Ctype_fac);
            break;

        case 'n':
        case 't':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, " ");
            break;

        case 'p':
            _Ans = _Getloctxt(_First, _Last, 0, ":AM:am:PM:pm", _Case_sensitive::_Nope);
            if (_Ans < 0) {
                _State |= ios_base::failbit;
            } else if (1 < _Ans) {
                _Pt->tm_hour += 12;
            }

            break;

        case 'r':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, "%I : %M : %S %p");
            break;

        case 'R':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, "%H : %M");
            break;

        case 'S':
            _State |= _Getint(_First, _Last, 0, 60, _Pt->tm_sec, _Ctype_fac);
            break;

        case 'T':
        case 'X':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, "%H : %M : %S");
            break;

        case 'U':
            _State |= _Getint(_First, _Last, 0, 53, _Pt->tm_yday, _Ctype_fac);
            break;

        case 'w':
            _State |= _Getint(_First, _Last, 0, 6, _Pt->tm_wday, _Ctype_fac);
            break;

        case 'W':
            _State |= _Getint(_First, _Last, 0, 53, _Pt->tm_yday, _Ctype_fac);
            break;

        case 'x':
            _First = _Getfmt(_First, _Last, _Iosbase, _State, _Pt, "%d / %m / %y");
            break;

        case 'y':
            _State |= _Getint(_First, _Last, 0, 99, _Ans, _Ctype_fac);
            if (!(_State & ios_base::failbit)) {
                _Pt->tm_year = _Ans < 69 ? _Ans + 100 : _Ans;
            }

            break;

        case 'Y':
            _First = get_year(_First, _Last, _Iosbase, _State, _Pt);
            break;

        default:
            _State |= ios_base::failbit; // unknown specifier
        }

        if (_First == _Last) {
            _State |= ios_base::eofbit;
        }

        return _First;
    }

    _InIt __CLR_OR_THIS_CALL _Getfmt(_InIt _First, _InIt _Last, ios_base& _Iosbase, ios_base::iostate& _State, tm* _Pt,
        const char* _Fmtfirst) const { // get formatted time for format NTBS
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        for (; *_Fmtfirst != '\0'; ++_Fmtfirst) {
            if (*_Fmtfirst == '%') {
                _First = do_get(_First, _Last, _Iosbase, _State, _Pt,
                    *++_Fmtfirst); // convert a single field
            } else if (*_Fmtfirst == ' ') {
                while (_First != _Last && _Ctype_fac.is(_Ctype::space, *_First)) {
                    ++_First;
                }
            } else if (_Ctype_fac.narrow(*_First) != *_Fmtfirst) { // bad literal match
                _State |= ios_base::failbit;
                break;
            } else {
                ++_First;
            }
        }

        if (_First == _Last) {
            _State |= ios_base::eofbit;
        }

        return _First;
    }

private:
    ios_base::iostate __CLRCALL_OR_CDECL _Getint(_InIt& _First, _InIt& _Last, int _Lo, int _Hi, int& _Val,
        const _Ctype& _Ctype_fac) const { // get integer in range [_Lo, _Hi] from [_First, _Last)
        _STL_INTERNAL_CHECK(0 <= _Hi && _Hi <= 9999);
        const int _Hi_digits = (_Hi <= 9 ? 1 : _Hi <= 99 ? 2 : _Hi <= 999 ? 3 : 4);
        char _Ac[_MAX_INT_DIG];
        char* _Ep;
        char* _Ptr = _Ac;
        char _Ch;

        int _Digits_seen = 0;

        while (_First != _Last && _Digits_seen < _Hi_digits && _Ctype_fac.is(ctype_base::space, *_First)) {
            ++_First;
            ++_Digits_seen;
        }

        if (_First != _Last && _Digits_seen < _Hi_digits) {
            if ((_Ch = _Ctype_fac.narrow(*_First)) == '+') { // copy plus sign
                *_Ptr++ = '+';
                ++_First;
            } else if (_Ch == '-') { // copy minus sign
                *_Ptr++ = '-';
                ++_First;
            }
        }

        for (; _First != _Last && _Digits_seen < _Hi_digits && _Ctype_fac.narrow(*_First) == '0';
             ++_First) { // strip leading zeros
            ++_Digits_seen;
        }

        if (_Digits_seen > 0) {
            *_Ptr++ = '0'; // replace one or more with single zero
        }

        for (char* const _Pe = &_Ac[_MAX_INT_DIG - 1];
             _First != _Last && '0' <= (_Ch = _Ctype_fac.narrow(*_First)) && _Ch <= '9' && _Digits_seen < _Hi_digits;
             ++_Digits_seen, (void) ++_First) { // copy digits
            *_Ptr = _Ch;
            if (_Ptr < _Pe) {
                ++_Ptr; // drop trailing digits if already too large
            }
        }

        if (_Digits_seen == 0) {
            _Ptr = _Ac;
        }

        *_Ptr                    = '\0';
        int _Errno               = 0;
        const long _Ans          = _CSTD _Stolx(_Ac, &_Ep, 10, &_Errno);
        ios_base::iostate _State = ios_base::goodbit;

        if (_First == _Last) {
            _State |= ios_base::eofbit;
        }

        if (_Ep == _Ac || _Errno != 0 || _Ans < _Lo || _Hi < _Ans) {
            _State |= ios_base::failbit; // bad conversion
        } else {
            _Val = _Ans; // store valid result
        }

        return _State;
    }

    void __CLR_OR_THIS_CALL _Tidy() noexcept { // free all storage
        _CSTD free(const_cast<_Elem*>(_Days));
        _CSTD free(const_cast<_Elem*>(_Months));
        _CSTD free(const_cast<_Elem*>(_Ampm));
    }

    const _Elem* _Days; // ":Sun:Sunday:Mon:Monday..." for example
    const _Elem* _Months; // "Jan:January:Feb:February..." for example
    const _Elem* _Ampm; // ":AM:am:PM:pm"
    dateorder _Dateorder;
    _Locinfo::_Cvtvec _Cvt; // conversion information
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdllimport-static-field-def"
#endif // __clang__

template <class _Elem, class _InIt>
__PURE_APPDOMAIN_GLOBAL locale::id time_get<_Elem, _InIt>::id;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

template <class _Elem, class _InIt = istreambuf_iterator<_Elem, char_traits<_Elem>>>
class time_get_byname : public time_get<_Elem, _InIt> { // time_get for named locale
public:
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || _Is_any_of_v<_Elem, char, wchar_t>, _FACET_SPECIALIZATION_MESSAGE);

    explicit time_get_byname(const char* _Locname, size_t _Refs = 0)
        : time_get<_Elem, _InIt>(_Locname, _Refs) {} // construct for named locale

    explicit time_get_byname(const string& _Str, size_t _Refs = 0)
        : time_get<_Elem, _InIt>(_Locinfo(_Str.c_str()), _Refs) {} // construct for named locale

protected:
    virtual __CLR_OR_THIS_CALL ~time_get_byname() noexcept {}
};

template <class _Elem, class _OutIt = ostreambuf_iterator<_Elem, char_traits<_Elem>>>
class time_put : public locale::facet { // facet for converting encoded times to text
public:
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || _Is_any_of_v<_Elem, char, wchar_t, unsigned short>,
        _FACET_SPECIALIZATION_MESSAGE);

    using char_type = _Elem;
    using iter_type = _OutIt;
    using _Ctype    = ctype<_Elem>;

    _OutIt __CLR_OR_THIS_CALL put(_OutIt _Dest, ios_base& _Iosbase, _Elem _Fill, const tm* _Pt, const _Elem* _Fmtfirst,
        const _Elem* _Fmtlast) const { // put formatted time from _Pt to _Dest for [_Fmtfirst, _Fmtlast)
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        for (; _Fmtfirst != _Fmtlast; ++_Fmtfirst) {
            if (_Ctype_fac.narrow(*_Fmtfirst) != '%') {
                *_Dest++ = *_Fmtfirst; // copy literal element
            } else if (++_Fmtfirst == _Fmtlast) { // treat trailing % as %%
                *_Dest++ = _Fmtfirst[-1];
                break;
            } else { // get specifier after %
                char _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                char _Modifier  = '\0';
                _Elem _Percent  = _Fmtfirst[-1];

                if (_Specifier == 'E' || _Specifier == 'O' || _Specifier == 'Q' || _Specifier == '#') {
                    if (++_Fmtfirst == _Fmtlast) { // no specifier, copy %[E0Q#] as literal elements
                        *_Dest++ = _Percent;
                        *_Dest++ = _Specifier;
                        break;
                    }

                    // save both qualifier and specifier
                    _Modifier  = _Specifier;
                    _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                }

                _Dest = do_put(_Dest, _Iosbase, _Fill, _Pt, _Specifier, _Modifier); // convert a single field
            }
        }

        return _Dest;
    }

    _OutIt __CLR_OR_THIS_CALL put(_OutIt _Dest, ios_base& _Iosbase, _Elem _Fill, const tm* _Pt, char _Specifier,
        char _Modifier = '\0') const { // put formatted time from _Pt to _Dest for _Specifier/_Modifier
        return do_put(_Dest, _Iosbase, _Fill, _Pt, _Specifier, _Modifier);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id; // unique facet id

    explicit __CLR_OR_THIS_CALL time_put(size_t _Refs = 0) : locale::facet(_Refs) { // construct from current locale
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL time_put(const _Locinfo& _Lobj, size_t _Refs = 0) : locale::facet(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new time_put<_Elem, _OutIt>(_Locinfo(_Ploc->c_str()));
        }

        return _X_TIME;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~time_put() noexcept {}

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Tnames = _Lobj._Gettnames();
    }

    virtual _OutIt __CLR_OR_THIS_CALL do_put(_OutIt _Dest, ios_base&, _Elem, const tm* _Pt, char _Specifier,
        char _Modifier = '\0') const { // put formatted time from _Pt to _Dest for [_Fmtfirst, _Fmtlast)
        char _Fmt[5] = "!%x\0"; // '!' for nonzero count, null for modifier
        size_t _Count;
        size_t _Num;
        string _Str;

        if (_Modifier == '\0') {
            _Fmt[2] = _Specifier;
        } else { // add both modifier and specifier
            _Fmt[2] = _Modifier;
            _Fmt[3] = _Specifier;
        }

        for (_Num = 16;; _Num *= 2) { // convert into ever larger string buffer until success
            _Str.append(_Num, '\0');
            if (0 < (_Count = _Strftime(&_Str[0], _Str.size(), _Fmt, _Pt, _Tnames._Getptr()))) {
                break;
            }
        }

        return _STD copy(&_Str[1], &_Str[_Count], _Dest);
    }

private:
    _Locinfo::_Timevec _Tnames; // locale-specific stuff for _Strftime
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdllimport-static-field-def"
#endif // __clang__

template <class _Elem, class _OutIt>
__PURE_APPDOMAIN_GLOBAL locale::id time_put<_Elem, _OutIt>::id;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

template <class _OutIt>
class _CRTIMP2_PURE_IMPORT time_put<wchar_t, _OutIt>
    : public locale::facet { // facet for converting encoded times to wchar_t text
public:
    using _Elem     = wchar_t;
    using char_type = _Elem;
    using iter_type = _OutIt;
    using _Ctype    = ctype<_Elem>;

    _OutIt __CLR_OR_THIS_CALL put(_OutIt _Dest, ios_base& _Iosbase, _Elem _Fill, const tm* _Pt, const _Elem* _Fmtfirst,
        const _Elem* _Fmtlast) const { // put formatted time from _Pt to _Dest for [_Fmtfirst, _Fmtlast)
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        for (; _Fmtfirst != _Fmtlast; ++_Fmtfirst) {
            if (_Ctype_fac.narrow(*_Fmtfirst) != '%') {
                *_Dest++ = *_Fmtfirst; // copy literal element
            } else if (++_Fmtfirst == _Fmtlast) { // treat trailing % as %%
                *_Dest++ = _Fmtfirst[-1];
                break;
            } else { // get specifier after %
                _Elem _Raw      = *_Fmtfirst;
                char _Specifier = _Ctype_fac.narrow(_Raw);
                char _Modifier  = '\0';
                _Elem _Percent  = _Fmtfirst[-1];

                if (_Specifier == 'E' || _Specifier == 'O' || _Specifier == 'Q' || _Specifier == '#') {
                    if (++_Fmtfirst == _Fmtlast) { // no specifier, copy %[E0Q#] as literal elements
                        *_Dest++ = _Percent;
                        *_Dest++ = _Raw;
                        break;
                    }

                    // save both qualifier and specifier
                    _Modifier  = _Specifier;
                    _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                }

                _Dest = do_put(_Dest, _Iosbase, _Fill, _Pt, _Specifier, _Modifier); // convert a single field
            }
        }

        return _Dest;
    }

    _OutIt __CLR_OR_THIS_CALL put(_OutIt _Dest, ios_base& _Iosbase, _Elem _Fill, const tm* _Pt, char _Specifier,
        char _Modifier = '\0') const { // put formatted time from _Pt to _Dest for _Specifier/_Modifier
        return do_put(_Dest, _Iosbase, _Fill, _Pt, _Specifier, _Modifier);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id; // unique facet id

    explicit __CLR_OR_THIS_CALL time_put(size_t _Refs = 0) : locale::facet(_Refs) { // construct from current locale
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL time_put(const _Locinfo& _Lobj, size_t _Refs = 0) : locale::facet(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new time_put<_Elem, _OutIt>(_Locinfo(_Ploc->c_str()));
        }

        return _X_TIME;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~time_put() noexcept {}

    __CLR_OR_THIS_CALL time_put(const char* _Locname, size_t _Refs = 0) : locale::facet(_Refs) {
        _BEGIN_LOCINFO(_Lobj(_Locname))
        _Init(_Lobj);
        _END_LOCINFO()
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Tnames = _Lobj._W_Gettnames();
    }

    virtual _OutIt __CLR_OR_THIS_CALL do_put(_OutIt _Dest, ios_base&, _Elem, const tm* _Pt, char _Specifier,
        char _Modifier = '\0') const { // put formatted time from _Pt to _Dest for [_Fmtfirst, _Fmtlast)
        wchar_t _Fmt[5] = L"!%x\0"; // ! for nonzero count, null for modifier
        size_t _Count;
        size_t _Num;
        wstring _Str;

        if (_Modifier == '\0') {
            _Fmt[2] = static_cast<_Elem>(_Specifier); // conversion rule unspecified
        } else { // add both modifier and specifier
            _Fmt[2] = static_cast<_Elem>(_Modifier);
            _Fmt[3] = static_cast<_Elem>(_Specifier);
        }

        for (_Num = 16;; _Num *= 2) { // convert into ever larger string buffer until success
            _Str.append(_Num, '\0');
            if (0 < (_Count = _Wcsftime(&_Str[0], _Str.size(), _Fmt, _Pt, _Tnames._Getptr()))) {
                break;
            }
        }
        return _STD copy(&_Str[1], &_Str[_Count], _Dest);
    }

private:
    _Locinfo::_Timevec _Tnames; // locale-specific stuff for _Wcsftime
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdllimport-static-field-def"
#endif // __clang__

template <class _OutIt>
__PURE_APPDOMAIN_GLOBAL locale::id time_put<wchar_t, _OutIt>::id;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

#if defined(_CRTBLD)

template <class _OutIt>
class _CRTIMP2_PURE_IMPORT time_put<unsigned short, _OutIt>
    : public locale::facet { // facet for converting encoded times to unsigned short text
public:
    using _Elem     = unsigned short;
    using char_type = _Elem;
    using iter_type = _OutIt;
    using _Ctype    = ctype<_Elem>;

    _OutIt __CLR_OR_THIS_CALL put(_OutIt _Dest, ios_base& _Iosbase, _Elem _Fill, const tm* _Pt, const _Elem* _Fmtfirst,
        const _Elem* _Fmtlast) const { // put formatted time from _Pt to _Dest for [_Fmtfirst, _Fmtlast)
        const _Ctype& _Ctype_fac = _STD use_facet<_Ctype>(_Iosbase.getloc());

        for (; _Fmtfirst != _Fmtlast; ++_Fmtfirst) {
            if (_Ctype_fac.narrow(*_Fmtfirst) != '%') {
                *_Dest++ = *_Fmtfirst; // copy literal element
            } else if (++_Fmtfirst == _Fmtlast) { // treat trailing % as %%
                *_Dest++ = _Fmtfirst[-1];
                break;
            } else { // get specifier after %
                char _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                char _Modifier  = '\0';
                _Elem _Percent  = _Fmtfirst[-1];

                if (_Specifier == 'E' || _Specifier == 'O' || _Specifier == 'Q' || _Specifier == '#') {
                    if (++_Fmtfirst == _Fmtlast) { // no specifier, copy %[E0Q#] as literal elements
                        *_Dest++ = _Percent;
                        *_Dest++ = _Specifier;
                        break;
                    }

                    // save both qualifier and specifier
                    _Modifier  = _Specifier;
                    _Specifier = _Ctype_fac.narrow(*_Fmtfirst);
                }

                _Dest = do_put(_Dest, _Iosbase, _Fill, _Pt, _Specifier, _Modifier); // convert a single field
            }
        }

        return _Dest;
    }

    _OutIt __CLR_OR_THIS_CALL put(_OutIt _Dest, ios_base& _Iosbase, _Elem _Fill, const tm* _Pt, char _Specifier,
        char _Modifier = '\0') const { // put formatted time from _Pt to _Dest for _Specifier/_Modifier
        return do_put(_Dest, _Iosbase, _Fill, _Pt, _Specifier, _Modifier);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id; // unique facet id

    explicit __CLR_OR_THIS_CALL time_put(size_t _Refs = 0) : locale::facet(_Refs) { // construct from current locale
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL time_put(const _Locinfo& _Lobj, size_t _Refs = 0) : locale::facet(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new time_put<_Elem, _OutIt>(_Locinfo(_Ploc->c_str()));
        }

        return _X_TIME;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~time_put() noexcept {}

    __CLR_OR_THIS_CALL time_put(const char* _Locname, size_t _Refs = 0) : locale::facet(_Refs) {
        _BEGIN_LOCINFO(_Lobj(_Locname))
        _Init(_Lobj);
        _END_LOCINFO()
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Tnames = _Lobj._W_Gettnames();
    }

    virtual _OutIt __CLR_OR_THIS_CALL do_put(_OutIt _Dest, ios_base&, _Elem, const tm* _Pt, char _Specifier,
        char _Modifier = '\0') const { // put formatted time from _Pt to _Dest for [_Fmtfirst, _Fmtlast)
        wchar_t _Fmt[5] = L"!%x\0"; // ! for nonzero count, null for modifier
        size_t _Count;
        size_t _Num;
        wstring _Str;

        if (_Modifier == '\0') {
            _Fmt[2] = static_cast<_Elem>(_Specifier); // conversion rule unspecified
        } else { // add both modifier and specifier
            _Fmt[2] = static_cast<_Elem>(_Modifier);
            _Fmt[3] = static_cast<_Elem>(_Specifier);
        }

        for (_Num = 16;; _Num *= 2) { // convert into ever larger string buffer until success
            _Str.append(_Num, '\0');
            if (0 < (_Count = _Wcsftime(&_Str[0], _Str.size(), _Fmt, _Pt, _Tnames._Getptr()))) {
                break;
            }
        }

        return _STD copy(&_Str[1], &_Str[_Count], _Dest);
    }

private:
    _Locinfo::_Timevec _Tnames; // locale-specific stuff for _Wcsftime
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdllimport-static-field-def"
#endif // __clang__

template <class _OutIt>
__PURE_APPDOMAIN_GLOBAL locale::id time_put<unsigned short, _OutIt>::id;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
#endif // defined(_CRTBLD)

template <class _Elem, class _OutIt = ostreambuf_iterator<_Elem, char_traits<_Elem>>>
class time_put_byname : public time_put<_Elem, _OutIt> { // time_put for named locale
public:
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || _Is_any_of_v<_Elem, char, wchar_t>, _FACET_SPECIALIZATION_MESSAGE);

    explicit time_put_byname(const char* _Locname, size_t _Refs = 0)
        : time_put<_Elem, _OutIt>(_Locname, _Refs) {} // construct for named locale

    explicit time_put_byname(const string& _Str, size_t _Refs = 0)
        : time_put<_Elem, _OutIt>(_Str.c_str(), _Refs) {} // construct for named locale

protected:
    virtual __CLR_OR_THIS_CALL ~time_put_byname() noexcept {}
};

#if defined(_DLL_CPPLIB)

#if !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
template class _CRTIMP2_PURE_IMPORT time_get<char, istreambuf_iterator<char, char_traits<char>>>;
template class _CRTIMP2_PURE_IMPORT time_put<char, ostreambuf_iterator<char, char_traits<char>>>;
template _CRTIMP2_PURE void __CLR_OR_THIS_CALL time_get<char, istreambuf_iterator<char, char_traits<char>>>::_Getvals(
    wchar_t, const _Locinfo&);

template class _CRTIMP2_PURE_IMPORT time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t>>>;
template class _CRTIMP2_PURE_IMPORT time_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t>>>;
template _CRTIMP2_PURE void __CLR_OR_THIS_CALL
    time_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t>>>::_Getvals(wchar_t, const _Locinfo&);
#endif // !defined(_CRTBLD) || defined(__FORCE_INSTANCE)

#ifdef __FORCE_INSTANCE
template class _CRTIMP2_PURE_IMPORT
    time_get<unsigned short, istreambuf_iterator<unsigned short, char_traits<unsigned short>>>;
template class _CRTIMP2_PURE_IMPORT
    time_put<unsigned short, ostreambuf_iterator<unsigned short, char_traits<unsigned short>>>;
template _CRTIMP2_PURE void __CLR_OR_THIS_CALL
    time_get<unsigned short, istreambuf_iterator<unsigned short, char_traits<unsigned short>>>::_Getvals(
        wchar_t, const _Locinfo&);
#endif // __FORCE_INSTANCE
#endif // defined(_DLL_CPPLIB)
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XLOCTIME_
