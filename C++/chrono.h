// chrono standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _CHRONO_
#define _CHRONO_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <__msvc_chrono.hpp>

#if _HAS_CXX20
#include <__msvc_tzdb.hpp>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <compare>
#include <forward_list>
#include <istream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <xloctime>
#include <xthreads.h>

#ifdef __cpp_lib_concepts
#include <concepts>
#endif // defined(__cpp_lib_concepts)

#ifdef __cpp_lib_format
#include <format>
#include <iomanip>
#endif // defined(__cpp_lib_format)
#endif // _HAS_CXX20

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
#if _HAS_CXX20
namespace chrono {
    // [time.duration.io]

#define _IF_PERIOD_RETURN_SUFFIX_ELSE(_TYPE, _SUFFIX) \
    if constexpr (is_same_v<_Period, _TYPE>) {        \
        if constexpr (is_same_v<_CharT, char>) {      \
            return _SUFFIX;                           \
        } else {                                      \
            return L##_SUFFIX;                        \
        }                                             \
    } else

    template <class _CharT, class _Period>
    _NODISCARD constexpr const _CharT* _Get_literal_unit_suffix() {
        _IF_PERIOD_RETURN_SUFFIX_ELSE(atto, "as")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(femto, "fs")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(pico, "ps")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(nano, "ns")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(micro, "us")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(milli, "ms")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(centi, "cs")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(deci, "ds")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(seconds::period, "s")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(deca, "das")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(hecto, "hs")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(kilo, "ks")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(mega, "Ms")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(giga, "Gs")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(tera, "Ts")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(peta, "Ps")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(exa, "Es")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(minutes::period, "min")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(hours::period, "h")
        _IF_PERIOD_RETURN_SUFFIX_ELSE(ratio<86400>, "d")

        {
            return nullptr;
        }
    }

#undef _IF_PERIOD_RETURN_SUFFIX_ELSE

    template <class _CharT>
    _NODISCARD _CharT* _Get_general_unit_suffix(_CharT* _Rnext, const intmax_t _Num, const intmax_t _Den) {
        // Returns the head pointer of the string, built in reverse.
        _STL_INTERNAL_CHECK(_Num > 0 && _Den > 0);
        *--_Rnext = '\0';
        *--_Rnext = 's';
        *--_Rnext = ']';
        if (_Den != 1) {
            _Rnext    = _UIntegral_to_buff(_Rnext, static_cast<uintmax_t>(_Den));
            *--_Rnext = '/';
        }

        _Rnext    = _UIntegral_to_buff(_Rnext, static_cast<uintmax_t>(_Num));
        *--_Rnext = '[';
        return _Rnext;
    }

    template <class _Period, class _CharT, class _Traits>
    void _Write_unit_suffix(basic_ostream<_CharT, _Traits>& _Os) {
        constexpr auto _Suffix = _Get_literal_unit_suffix<_CharT, _Period>();
        if constexpr (_Suffix == nullptr) {
            _CharT _Buffer[2 * (numeric_limits<intmax_t>::digits10 + 1) + 5] = {}; // 2 numbers + "[/]s\0"
            const _CharT* const _Begin =
                _Get_general_unit_suffix<_CharT>(_STD end(_Buffer), _Period::num, _Period::den);
            _Os << _Begin;
        } else {
            _Os << _Suffix;
        }
    }

    template <class _CharT, class _Traits, class _Rep, class _Period>
    basic_ostream<_CharT, _Traits>& operator<<(
        basic_ostream<_CharT, _Traits>& _Os, const duration<_Rep, _Period>& _Dur) {
        basic_ostringstream<_CharT, _Traits> _Sstr;
        _Sstr.flags(_Os.flags());
        _Sstr.imbue(_Os.getloc());
        _Sstr.precision(_Os.precision());
        _Sstr << _Dur.count();
        _Write_unit_suffix<_Period>(_Sstr);

        return _Os << _Sstr.str();
    }

    struct local_t {};

    template <class _Duration>
    using local_time    = time_point<local_t, _Duration>;
    using local_seconds = local_time<seconds>;
    using local_days    = local_time<days>;

    struct last_spec {
        explicit last_spec() = default;
    };
    inline constexpr last_spec last{};

    class day {
    public:
        day() = default;
        constexpr explicit day(unsigned int _Val) noexcept : _Day{static_cast<unsigned char>(_Val)} {}

        constexpr day& operator++() noexcept {
            ++_Day;
            return *this;
        }
        constexpr day operator++(int) noexcept {
            return day{_Day++};
        }
        constexpr day& operator--() noexcept {
            --_Day;
            return *this;
        }
        constexpr day operator--(int) noexcept {
            return day{_Day--};
        }

        constexpr day& operator+=(const days& _Days) noexcept {
            _Day += static_cast<unsigned char>(_Days.count());
            return *this;
        }
        constexpr day& operator-=(const days& _Days) noexcept {
            _Day -= static_cast<unsigned char>(_Days.count());
            return *this;
        }

        _NODISCARD constexpr explicit operator unsigned int() const noexcept {
            return _Day;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Day >= 1 && _Day <= 31;
        }

    private:
        unsigned char _Day;
    };

    _NODISCARD constexpr bool operator==(const day& _Left, const day& _Right) noexcept {
        return static_cast<unsigned int>(_Left) == static_cast<unsigned int>(_Right);
    }
    _NODISCARD constexpr strong_ordering operator<=>(const day& _Left, const day& _Right) noexcept {
        return static_cast<unsigned int>(_Left) <=> static_cast<unsigned int>(_Right);
    }

    _NODISCARD constexpr day operator+(const day& _Left, const days& _Right) noexcept {
        return day{static_cast<unsigned int>(_Left) + _Right.count()};
    }
    _NODISCARD constexpr day operator+(const days& _Left, const day& _Right) noexcept {
        return _Right + _Left;
    }
    _NODISCARD constexpr day operator-(const day& _Left, const days& _Right) noexcept {
        return day{static_cast<unsigned int>(_Left) - _Right.count()};
    }
    _NODISCARD constexpr days operator-(const day& _Left, const day& _Right) noexcept {
        return days{
            static_cast<int>(static_cast<unsigned int>(_Left)) - static_cast<int>(static_cast<unsigned int>(_Right))};
    }

    class month {
    public:
        month() = default;
        constexpr explicit month(unsigned int _Val) noexcept : _Month{static_cast<unsigned char>(_Val)} {}

        constexpr month& operator++() noexcept {
            *this += months{1};
            return *this;
        }
        constexpr month operator++(int) noexcept {
            month _Temp{*this};
            ++*this;
            return _Temp;
        }
        constexpr month& operator--() noexcept {
            *this -= months{1};
            return *this;
        }
        constexpr month operator--(int) noexcept {
            month _Temp{*this};
            --*this;
            return _Temp;
        }

        constexpr month& operator+=(const months& _Months) noexcept;
        constexpr month& operator-=(const months& _Months) noexcept;

        _NODISCARD constexpr explicit operator unsigned int() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Month >= 1 && _Month <= 12;
        }

    private:
        unsigned char _Month;
    };

    _NODISCARD constexpr bool operator==(const month& _Left, const month& _Right) noexcept {
        return static_cast<unsigned int>(_Left) == static_cast<unsigned int>(_Right);
    }
    _NODISCARD constexpr strong_ordering operator<=>(const month& _Left, const month& _Right) noexcept {
        return static_cast<unsigned int>(_Left) <=> static_cast<unsigned int>(_Right);
    }

    _NODISCARD constexpr month operator+(const month& _Left, const months& _Right) noexcept {
        const auto _Mo  = static_cast<long long>(static_cast<unsigned int>(_Left)) + (_Right.count() - 1);
        const auto _Div = (_Mo >= 0 ? _Mo : _Mo - 11) / 12;
        return month{static_cast<unsigned int>(_Mo - _Div * 12 + 1)};
    }
    _NODISCARD constexpr month operator+(const months& _Left, const month& _Right) noexcept {
        return _Right + _Left;
    }
    _NODISCARD constexpr month operator-(const month& _Left, const months& _Right) noexcept {
        return _Left + -_Right;
    }
    _NODISCARD constexpr months operator-(const month& _Left, const month& _Right) noexcept {
        const auto _Mo = static_cast<unsigned int>(_Left) - static_cast<unsigned int>(_Right);
        return months{_Mo <= 11 ? _Mo : _Mo + 12};
    }

    constexpr month& month::operator+=(const months& _Months) noexcept {
        *this = *this + _Months;
        return *this;
    }
    constexpr month& month::operator-=(const months& _Months) noexcept {
        *this = *this - _Months;
        return *this;
    }

    class year {
    public:
        year() = default;
        constexpr explicit year(int _Val) noexcept : _Year{static_cast<short>(_Val)} {}

        constexpr year& operator++() noexcept {
            ++_Year;
            return *this;
        }
        constexpr year operator++(int) noexcept {
            return year{_Year++};
        }
        constexpr year& operator--() noexcept {
            --_Year;
            return *this;
        }
        constexpr year operator--(int) noexcept {
            return year{_Year--};
        }

        constexpr year& operator+=(const years& _Years) noexcept {
#ifdef __EDG__ // TRANSITION, VSO-1271098
            _Year = static_cast<short>(_Year + _Years.count());
#else // ^^^ workaround / no workaround vvv
            _Year += static_cast<short>(_Years.count());
#endif // ^^^ no workaround ^^^
            return *this;
        }
        constexpr year& operator-=(const years& _Years) noexcept {
#ifdef __EDG__ // TRANSITION, VSO-1271098
            _Year = static_cast<short>(_Year - _Years.count());
#else // ^^^ workaround / no workaround vvv
            _Year -= static_cast<short>(_Years.count());
#endif // ^^^ no workaround ^^^
            return *this;
        }

        _NODISCARD constexpr year operator+() const noexcept {
            return *this;
        }
        _NODISCARD constexpr year operator-() const noexcept {
            return year{-_Year};
        }

        _NODISCARD constexpr bool is_leap() const noexcept {
            return _Year % 4 == 0 && (_Year % 100 != 0 || _Year % 400 == 0);
        }

        _NODISCARD constexpr explicit operator int() const noexcept {
            return _Year;
        }

        _NODISCARD constexpr bool ok() const noexcept {
            return _Year_min <= _Year && _Year <= _Year_max;
        }

        _NODISCARD static constexpr year(min)() noexcept {
            return year{_Year_min};
        }
        _NODISCARD static constexpr year(max)() noexcept {
            return year{_Year_max};
        }

    private:
        short _Year;
        static constexpr int _Year_min = -32767;
        static constexpr int _Year_max = 32767;
    };

    _NODISCARD constexpr bool operator==(const year& _Left, const year& _Right) noexcept {
        return static_cast<int>(_Left) == static_cast<int>(_Right);
    }
    _NODISCARD constexpr strong_ordering operator<=>(const year& _Left, const year& _Right) noexcept {
        return static_cast<int>(_Left) <=> static_cast<int>(_Right);
    }

    _NODISCARD constexpr year operator+(const year& _Left, const years& _Right) noexcept {
        return year{static_cast<int>(_Left) + _Right.count()};
    }
    _NODISCARD constexpr year operator+(const years& _Left, const year& _Right) noexcept {
        return _Right + _Left;
    }
    _NODISCARD constexpr year operator-(const year& _Left, const years& _Right) noexcept {
        return _Left + -_Right;
    }
    _NODISCARD constexpr years operator-(const year& _Left, const year& _Right) noexcept {
        return years{static_cast<int>(_Left) - static_cast<int>(_Right)};
    }

    class weekday_indexed;
    class weekday_last;

    class weekday {
    public:
        weekday() = default;
        constexpr explicit weekday(unsigned int _Val) noexcept
            : _Weekday{static_cast<unsigned char>(_Val == 7 ? 0 : _Val)} {}
        constexpr weekday(const sys_days& _Sys_day) noexcept
            : _Weekday{static_cast<unsigned char>(_Weekday_from_days(_Sys_day.time_since_epoch().count()))} {}
        constexpr explicit weekday(const local_days& _Local_day) noexcept
            : _Weekday{static_cast<unsigned char>(_Weekday_from_days(_Local_day.time_since_epoch().count()))} {}

        constexpr weekday& operator++() noexcept {
            return *this += days{1};
        }
        constexpr weekday operator++(int) noexcept {
            weekday _Temp{*this};
            ++*this;
            return _Temp;
        }
        constexpr weekday& operator--() noexcept {
            return *this -= days{1};
        }
        constexpr weekday operator--(int) noexcept {
            weekday _Temp{*this};
            --*this;
            return _Temp;
        }

        constexpr weekday& operator+=(const days& _Days) noexcept;
        constexpr weekday& operator-=(const days& _Days) noexcept;

        _NODISCARD constexpr unsigned int c_encoding() const noexcept {
            return _Weekday;
        }
        _NODISCARD constexpr unsigned int iso_encoding() const noexcept {
            return _Weekday == 0u ? 7u : _Weekday;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Weekday <= 6;
        }

        _NODISCARD constexpr weekday_indexed operator[](unsigned int _Index) const noexcept;
        _NODISCARD constexpr weekday_last operator[](last_spec) const noexcept;

    private:
        unsigned char _Weekday;

        // courtesy of Howard Hinnant
        // https://howardhinnant.github.io/date_algorithms.html#weekday_from_days
        _NODISCARD static constexpr unsigned int _Weekday_from_days(int _Tp) noexcept {
            return static_cast<unsigned int>(_Tp >= -4 ? (_Tp + 4) % 7 : (_Tp + 5) % 7 + 6);
        }
    };

    _NODISCARD constexpr bool operator==(const weekday& _Left, const weekday& _Right) noexcept {
        return _Left.c_encoding() == _Right.c_encoding();
    }

    _NODISCARD constexpr weekday operator+(const weekday& _Left, const days& _Right) noexcept {
        const auto _Wd  = static_cast<long long>(_Left.c_encoding()) + _Right.count();
        const auto _Div = (_Wd >= 0 ? _Wd : _Wd - 6) / 7;
        return weekday{static_cast<unsigned int>(_Wd - _Div * 7)};
    }
    _NODISCARD constexpr weekday operator+(const days& _Left, const weekday& _Right) noexcept {
        return _Right + _Left;
    }
    _NODISCARD constexpr weekday operator-(const weekday& _Left, const days& _Right) noexcept {
        return _Left + -_Right;
    }
    _NODISCARD constexpr days operator-(const weekday& _Left, const weekday& _Right) noexcept {
        const auto _Wd = _Left.c_encoding() - _Right.c_encoding();
        const auto _Wk = _Wd <= 6 ? _Wd : _Wd + 7;
        return days{_Wk};
    }

    constexpr weekday& weekday::operator+=(const days& _Days) noexcept {
        *this = *this + _Days;
        return *this;
    }
    constexpr weekday& weekday::operator-=(const days& _Days) noexcept {
        *this = *this - _Days;
        return *this;
    }

    class weekday_indexed {
    public:
        weekday_indexed() = default;
        constexpr weekday_indexed(const weekday& _Wd, unsigned int _Idx) noexcept
            : _Weekday{_Wd}, _Index{static_cast<unsigned char>(_Idx)} {}

        _NODISCARD constexpr weekday weekday() const noexcept {
            return _Weekday;
        }
        _NODISCARD constexpr unsigned int index() const noexcept {
            return _Index;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Weekday.ok() && _Index >= 1 && _Index <= 5;
        }

    private:
        _CHRONO weekday _Weekday;
        unsigned char _Index;
    };

    _NODISCARD constexpr bool operator==(const weekday_indexed& _Left, const weekday_indexed& _Right) noexcept {
        return _Left.weekday() == _Right.weekday() && _Left.index() == _Right.index();
    }

    class weekday_last {
    public:
        constexpr explicit weekday_last(const weekday& _Wd) noexcept : _Weekday{_Wd} {}

        _NODISCARD constexpr weekday weekday() const noexcept {
            return _Weekday;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Weekday.ok();
        }

    private:
        _CHRONO weekday _Weekday;
    };

    _NODISCARD constexpr bool operator==(const weekday_last& _Left, const weekday_last& _Right) noexcept {
        return _Left.weekday() == _Right.weekday();
    }

    _NODISCARD constexpr weekday_indexed weekday::operator[](unsigned int _Index) const noexcept {
        return {*this, _Index};
    }
    _NODISCARD constexpr weekday_last weekday::operator[](last_spec) const noexcept {
        return weekday_last{*this};
    }

    class month_day {
    public:
        month_day() = default;
        constexpr month_day(const month& _Month_, const day& _Day_) noexcept : _Month{_Month_}, _Day{_Day_} {}

        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr day day() const noexcept {
            return _Day;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            if (!_Month.ok() || !_Day.ok()) {
                return false;
            }

            const auto _Da = static_cast<unsigned int>(_Day);
            const auto _Mo = static_cast<unsigned int>(_Month);
            if (_Mo == 2) {
                return _Da <= 29;
            }

            if (_Mo == 4 || _Mo == 6 || _Mo == 9 || _Mo == 11) {
                return _Da <= 30;
            }
            return true;
        }

    private:
        _CHRONO month _Month;
        _CHRONO day _Day;
    };

    _NODISCARD constexpr bool operator==(const month_day& _Left, const month_day& _Right) noexcept {
        return _Left.month() == _Right.month() && _Left.day() == _Right.day();
    }
    _NODISCARD constexpr strong_ordering operator<=>(const month_day& _Left, const month_day& _Right) noexcept {
        const auto _Comp = _Left.month() <=> _Right.month();
        if (_Comp != 0) {
            return _Comp;
        }

        return _Left.day() <=> _Right.day();
    }

    class month_day_last {
    public:
        constexpr explicit month_day_last(const month& _Month_) noexcept : _Month{_Month_} {}

        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Month.ok();
        }

    private:
        _CHRONO month _Month;
    };

    _NODISCARD constexpr bool operator==(const month_day_last& _Left, const month_day_last& _Right) noexcept {
        return _Left.month() == _Right.month();
    }
    _NODISCARD constexpr strong_ordering operator<=>(
        const month_day_last& _Left, const month_day_last& _Right) noexcept {
        return _Left.month() <=> _Right.month();
    }

    class month_weekday {
    public:
        constexpr month_weekday(const month& _Month_, const weekday_indexed& _Wdi) noexcept
            : _Month{_Month_}, _Weekday_index{_Wdi} {}

        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr weekday_indexed weekday_indexed() const noexcept {
            return _Weekday_index;
        }

        _NODISCARD constexpr bool ok() const noexcept {
            return _Month.ok() && _Weekday_index.ok();
        }

    private:
        _CHRONO month _Month;
        _CHRONO weekday_indexed _Weekday_index;
    };

    _NODISCARD constexpr bool operator==(const month_weekday& _Left, const month_weekday& _Right) noexcept {
        return _Left.month() == _Right.month() && _Left.weekday_indexed() == _Right.weekday_indexed();
    }

    class month_weekday_last {
    public:
        constexpr month_weekday_last(const month& _Month_, const weekday_last& _Wdl) noexcept
            : _Month{_Month_}, _Weekday_last{_Wdl} {}

        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr weekday_last weekday_last() const noexcept {
            return _Weekday_last;
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Month.ok() && _Weekday_last.ok();
        }

    private:
        _CHRONO month _Month;
        _CHRONO weekday_last _Weekday_last;
    };

    _NODISCARD constexpr bool operator==(const month_weekday_last& _Left, const month_weekday_last& _Right) noexcept {
        return _Left.month() == _Right.month() && _Left.weekday_last() == _Right.weekday_last();
    }

    class year_month {
    public:
        year_month() = default;
        constexpr year_month(const year& _Year_, const month& _Month_) noexcept : _Year{_Year_}, _Month{_Month_} {}

        _NODISCARD constexpr year year() const noexcept {
            return _Year;
        }
        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }

        template <int = 0>
        constexpr year_month& operator+=(const months& _Months) noexcept;
        template <int = 0>
        constexpr year_month& operator-=(const months& _Months) noexcept;
        constexpr year_month& operator+=(const years& _Years) noexcept;
        constexpr year_month& operator-=(const years& _Years) noexcept;

        _NODISCARD constexpr bool ok() const noexcept {
            return _Year.ok() && _Month.ok();
        }

    private:
        _CHRONO year _Year;
        _CHRONO month _Month;
    };

    _NODISCARD constexpr bool operator==(const year_month& _Left, const year_month& _Right) noexcept {
        return _Left.year() == _Right.year() && _Left.month() == _Right.month();
    }
    _NODISCARD constexpr strong_ordering operator<=>(const year_month& _Left, const year_month& _Right) noexcept {
        const auto _Comp = _Left.year() <=> _Right.year();
        if (_Comp != 0) {
            return _Comp;
        }

        return _Left.month() <=> _Right.month();
    }

    template <int = 0>
    _NODISCARD constexpr year_month operator+(const year_month& _Left, const months& _Right) noexcept {
        const auto _Mo  = static_cast<long long>(static_cast<unsigned int>(_Left.month())) + (_Right.count() - 1);
        const auto _Div = (_Mo >= 0 ? _Mo : _Mo - 11) / 12;
        return year_month{_Left.year() + years{_Div}, month{static_cast<unsigned int>(_Mo - _Div * 12 + 1)}};
    }
    template <int = 0>
    _NODISCARD constexpr year_month operator+(const months& _Left, const year_month& _Right) noexcept {
        return _Right + _Left;
    }

    template <int = 0>
    _NODISCARD constexpr year_month operator-(const year_month& _Left, const months& _Right) noexcept {
        return _Left + -_Right;
    }

    _NODISCARD constexpr months operator-(const year_month& _Left, const year_month& _Right) noexcept {
        return _Left.year() - _Right.year()
             + months{static_cast<int>(static_cast<unsigned int>(_Left.month()))
                      - static_cast<int>(static_cast<unsigned int>(_Right.month()))};
    }

    _NODISCARD constexpr year_month operator+(const year_month& _Left, const years& _Right) noexcept {
        return {year{_Left.year() + _Right}, _Left.month()};
    }

    _NODISCARD constexpr year_month operator+(const years& _Left, const year_month& _Right) noexcept {
        return _Right + _Left;
    }

    _NODISCARD constexpr year_month operator-(const year_month& _Left, const years& _Right) noexcept {
        return _Left + -_Right;
    }

    template <int>
    constexpr year_month& year_month::operator+=(const months& _Months) noexcept {
        *this = *this + _Months;
        return *this;
    }
    template <int>
    constexpr year_month& year_month::operator-=(const months& _Months) noexcept {
        *this = *this - _Months;
        return *this;
    }
    constexpr year_month& year_month::operator+=(const years& _Years) noexcept {
        *this = *this + _Years;
        return *this;
    }
    constexpr year_month& year_month::operator-=(const years& _Years) noexcept {
        *this = *this - _Years;
        return *this;
    }

    // To prevent UB by going out of bounds, four extra days with an invalid day are added.
    inline constexpr day _Last_day_table[] = {day{31}, day{28}, day{31}, day{30}, day{31}, day{30}, day{31}, day{31},
        day{30}, day{31}, day{30}, day{31}, day{255}, day{255}, day{255}, day{255}};

    _NODISCARD constexpr day _Last_day(const year& _Year, const month& _Month) {
        if (_Month == month{2} && _Year.is_leap()) {
            return day{29};
        }

        return _Last_day_table[(static_cast<unsigned int>(_Month) - 1) & 0xF];
    }

    class year_month_day_last;

    class year_month_day {
    public:
        year_month_day() = default;
        constexpr year_month_day(const year& _Year_, const month& _Month_, const day& _Day_) noexcept
            : _Year{_Year_}, _Month{_Month_}, _Day{_Day_} {}
        constexpr year_month_day(const year_month_day_last& _Ymdl) noexcept;
        constexpr year_month_day(const sys_days& _Sys_days) noexcept
            : year_month_day{_Civil_from_days(_Sys_days.time_since_epoch().count())} {}
        constexpr explicit year_month_day(const local_days& _Local_days) noexcept
            : year_month_day{_Civil_from_days(_Local_days.time_since_epoch().count())} {}

        template <int = 0>
        constexpr year_month_day& operator+=(const months& _Months) noexcept;
        template <int = 0>
        constexpr year_month_day& operator-=(const months& _Months) noexcept;
        constexpr year_month_day& operator+=(const years& _Years) noexcept;
        constexpr year_month_day& operator-=(const years& _Years) noexcept;

        _NODISCARD constexpr year year() const noexcept {
            return _Year;
        }
        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr day day() const noexcept {
            return _Day;
        }

        _NODISCARD constexpr operator sys_days() const noexcept {
            return sys_days{_Days_from_civil()};
        }
        _NODISCARD constexpr explicit operator local_days() const noexcept {
            return local_days{static_cast<sys_days>(*this).time_since_epoch()};
        }
        _NODISCARD constexpr bool ok() const noexcept {
            if (!_Year.ok() || !_Month.ok()) {
                return false;
            }

            return _Day >= _CHRONO day{1} && _Day <= _Last_day(_Year, _Month);
        }

        _NODISCARD constexpr int _Calculate_weekday() const noexcept {
            const int _Day_int   = static_cast<int>(static_cast<unsigned int>(_Day));
            const int _Month_int = static_cast<int>(static_cast<unsigned int>(_Month));

            const int _Era_year = static_cast<int>(_Year) - (_Month_int <= 2);
            const int _Era      = (_Era_year >= 0 ? _Era_year : _Era_year - 399) / 400;
            const int _Yoe      = _Era_year - _Era * 400;
            const int _Yday_era = ((979 * (_Month_int + (_Month_int > 2 ? -3 : 9)) + 19) >> 5) + _Day_int - 1;
            const int _Doe      = ((1461 * _Yoe) >> 2) - _Yoe / 100 + _Yday_era;
            return (_Doe + 3) % 7; // the era began on a Wednesday
        }

    private:
        _CHRONO year _Year;
        _CHRONO month _Month;
        _CHRONO day _Day;

        // _Civil_from_days and _Days_from_civil perform conversions between the dates in the (proleptic) Gregorian
        // calendar and the continuous count of days since 1970-01-01.

        // To simplify the handling of leap days (February 29th), the algorithm below uses a modified calendar
        // internally, in which each year begins on March 1st, while January and February belong to the previous year.
        // We denote the modified year and month number as _Yp and _Mp. We also define modified centuries that begin on
        // each modified year whose _Yp is a multiple of 100.

        // _Mp | Month     | Day of Year
        // --- | --------- | -----------
        //  0  | March     | [  0,  30]
        //  1  | April     | [ 31,  60]
        //  2  | May       | [ 61,  91]
        //  3  | June      | [ 92, 121]
        //  4  | July      | [122, 152]
        //  5  | August    | [153, 183]
        //  6  | September | [184, 213]
        //  7  | October   | [214, 244]
        //  8  | November  | [245, 274]
        //  9  | December  | [275, 305]
        // 10  | January   | [306, 336]
        // 11  | February  | [337, 365] on leap years, [337, 364] on regular years

        // _Yp |  First Day  | Last Day (inclusive) | Leap Year?
        // --- | ----------- | -------------------- | ----------
        //  -4 | -0004-03-01 |     -0003-02-28      |     No
        //  -3 | -0003-03-01 |     -0002-02-28      |     No
        //  -2 | -0002-03-01 |     -0001-02-28      |     No
        //  -1 | -0001-03-01 |      0000-02-29      |     Yes
        //   0 |  0000-03-01 |      0001-02-28      |     No
        //   1 |  0001-03-01 |      0002-02-28      |     No
        //   2 |  0002-03-01 |      0003-02-28      |     No
        //   3 |  0003-03-01 |      0004-02-29      |     Yes

        // _Century |  First Day  | Last Day (inclusive) | Long Century?
        // -------- | ----------- | -------------------- | -------------
        //    -4    | -0400-03-01 |     -0300-02-28      |      No
        //    -3    | -0300-03-01 |     -0200-02-28      |      No
        //    -2    | -0200-03-01 |     -0100-02-28      |      No
        //    -1    | -0100-03-01 |      0000-02-29      |      Yes
        //     0    |  0000-03-01 |      0100-02-28      |      No
        //     1    |  0100-03-01 |      0200-02-28      |      No
        //     2    |  0200-03-01 |      0300-02-28      |      No
        //     3    |  0300-03-01 |      0400-02-29      |      Yes

        // The structure of the modified calendar:
        // 1 ) It has a period of 4 centuries.
        // 2 ) Each calendar period (146097 days) contains 3 regular centuries followed by a long century (36525 days).
        // 3 ) Each regular century (36524 days) contains 24 regular 4-year spans followed by a short 4-year span.
        // 3') Each long century (36525 days) contains 25 regular 4-year spans.
        // 4 ) Each regular 4-year span (1461 days) contains 3 regular years followed by a leap year.
        // 4') Each short 4-year span (1460 days) contains 4 regular years.

        // Formula 1: Compute _Day_of_year of the first day of month _Mp
        //
        //   _Day_of_year = (979 * _Mp + 19) >> 5
        //
        // A more well-known formula is 30 * _Mp + floor((3 * _Mp + 2) / 5) or floor((153 * _Mp + 2) / 5), which is used
        // in Howard Hinnant's paper.
        //
        // The formula above returns the same result for all _Mp in [0, 11].
        // Note that 979 / 2^5 = 30.59375 ~= 30.6 = 153 / 5.

        // Formula 1': Compute _Mp from _Day_of_year
        //
        //   _Mp = (535 * _Day_of_year + 333) >> 14
        //
        // Howard Hinnant's paper uses floor((5 * _Day_of_year + 2) / 153), the inverse of floor((153 * _Mp + 2) / 5) or
        // ceil((153 * _Mp - 2) / 5).
        //
        // The formula above returns the same result for all _Day_of_year in [0, 365].
        // Note that 2^14 / 535 = 30.624... ~= 30.6 = 153 / 5.

        // Formula 2: Compute _Zx of the first day of year _Yp, where _Zx is the continuous count of days since
        // 0000-03-01.
        //
        //   _Zx = ((1461 * _Yp) >> 2) - _Century + (_Century >> 2)
        //
        // Start with multiplying by the number of days in regular years (365), add one day for the leap year in each
        // 4-year span, subtract one day for the short 4-year span in each century, and finally add one day for the long
        // century in each calendar period. This gives us 365 * _Yp + floor(_Yp / 4) - _Century + floor(_Century / 4).

        // Formula 2-1: Compute _Day_of_century of the first day of year _Year_of_century
        //
        //   _Day_of_century = (1461 * _Year_of_century) >> 2
        //
        // Start with multiplying by the number of days in regular years (365), add one day for the leap year in each
        // 4-year span. This gives us 365 * _Year_of_century + floor(_Year_of_century / 4)
        // == floor(1461 * _Year_of_century / 4).

        // Formula 2-1': Compute _Year_of_century from _Day_of_century
        //
        //   _Year_of_century = (91867 * (_Day_of_century + 1)) >> 25
        //
        // The inverse of floor(1461 * _Year_of_century / 4) or ceil((1461 * _Year_of_century - 3) / 4) is
        // floor((4 * _Day_of_century + 3) / 1461).
        //
        // The formula above returns the same result for all _Day_of_century in [0, 36524].
        // Note that 2^25 / 91867 = 365.2501... ~= 365.25 = 1461 / 4.

        // Formula 2-2: Compute _Zx of the first day of century _Century, where _Zx is the continuous count of days
        // since 0000-03-01.
        //
        //   _Zx = (146097 * _Century) >> 2
        //
        // Start with multiplying by the number of days in regular centuries (36524), add one day for the long century
        // in each calendar period. This gives us 36524 * _Century + floor(_Century / 4) = floor(146097 * _Century / 4).

        // Formula 2-2': Compute _Century from _Zx, where _Zx is the continuous count of days since 0000-03-01.
        //
        //   _Century = floor((4 * _Zx + 3) / 146097)
        //
        // This is the inverse of floor(146097 * _Year_of_century / 4) or ceil((146097 * _Year_of_century - 3) / 4)

        // courtesy of Howard Hinnant
        // https://howardhinnant.github.io/date_algorithms.html#civil_from_days
        _NODISCARD static constexpr year_month_day _Civil_from_days(int _Tp) noexcept {
            static_assert(numeric_limits<unsigned int>::digits >= 32);
            static_assert(numeric_limits<int>::digits >= 26);
            const int _Zx = _Tp + 719468; // Shift epoch to 0000-03-01
            // Formula 2-2'
            const int _Century = (_Zx >= 0 ? 4 * _Zx + 3 : 4 * _Zx - 146093) / 146097;
            // Formula 2-2
            const unsigned int _Day_of_century =
                static_cast<unsigned int>(_Zx - ((146097 * _Century) >> 2)); // [0, 36524]
            // Formula 2-1'
            const unsigned int _Year_of_century = (91867 * (_Day_of_century + 1)) >> 25; // [0, 99]
            const int _Yp = static_cast<int>(_Year_of_century) + _Century * 100; // Where March is the first month
            // Formula 2-1
            const unsigned int _Day_of_year = _Day_of_century - ((1461 * _Year_of_century) >> 2); // [0, 365]
            // Formula 1'
            const unsigned int _Mp = (535 * _Day_of_year + 333) >> 14; // [0, 11]
            // Formula 1
            const unsigned int _Day   = _Day_of_year - ((979 * _Mp + 19) >> 5) + 1; // [1, 31]
            const unsigned int _Month = _Mp + (_Mp < 10 ? 3 : static_cast<unsigned int>(-9)); // [1, 12]
            return year_month_day{_CHRONO year{_Yp + (_Month <= 2)}, _CHRONO month{_Month}, _CHRONO day{_Day}};
        }
        // courtesy of Howard Hinnant
        // https://howardhinnant.github.io/date_algorithms.html#days_from_civil
        _NODISCARD constexpr days _Days_from_civil() const noexcept {
            static_assert(numeric_limits<unsigned int>::digits >= 18);
            static_assert(numeric_limits<int>::digits >= 26);
            const unsigned int _Mo = static_cast<unsigned int>(_Month); // [1, 12]
            const int _Yp          = static_cast<int>(_Year) - (_Mo <= 2);
            const int _Century     = (_Yp >= 0 ? _Yp : _Yp - 99) / 100;
            const unsigned int _Mp = _Mo + (_Mo > 2 ? static_cast<unsigned int>(-3) : 9); // [0, 11]
            // Formula 1
            const int _Day_of_year = static_cast<int>(((979 * _Mp + 19) >> 5) + static_cast<unsigned int>(_Day)) - 1;
            // Formula 2
            return days{((1461 * _Yp) >> 2) - _Century + (_Century >> 2) + _Day_of_year - 719468};
        }
    };

    _NODISCARD constexpr bool operator==(const year_month_day& _Left, const year_month_day& _Right) noexcept {
        return _Left.year() == _Right.year() && _Left.month() == _Right.month() && _Left.day() == _Right.day();
    }
    _NODISCARD constexpr strong_ordering operator<=>(
        const year_month_day& _Left, const year_month_day& _Right) noexcept {
        auto _Comp = _Left.year() <=> _Right.year();
        if (_Comp != 0) {
            return _Comp;
        }

        _Comp = _Left.month() <=> _Right.month();
        if (_Comp != 0) {
            return _Comp;
        }

        return _Left.day() <=> _Right.day();
    }

    template <int = 0>
    _NODISCARD constexpr year_month_day operator+(const year_month_day& _Left, const months& _Right) noexcept {
        const auto _Ym = year_month{_Left.year(), _Left.month()} + _Right;
        return {_Ym.year(), _Ym.month(), _Left.day()};
    }

    template <int = 0>
    _NODISCARD constexpr year_month_day operator+(const months& _Left, const year_month_day& _Right) noexcept {
        return _Right + _Left;
    }

    template <int = 0>
    _NODISCARD constexpr year_month_day operator-(const year_month_day& _Left, const months& _Right) noexcept {
        return _Left + -_Right;
    }

    _NODISCARD constexpr year_month_day operator+(const year_month_day& _Left, const years& _Right) noexcept {
        return {_Left.year() + _Right, _Left.month(), _Left.day()};
    }

    _NODISCARD constexpr year_month_day operator+(const years& _Left, const year_month_day& _Right) noexcept {
        return _Right + _Left;
    }

    _NODISCARD constexpr year_month_day operator-(const year_month_day& _Left, const years& _Right) noexcept {
        return _Left + -_Right;
    }

    template <int>
    constexpr year_month_day& year_month_day::operator+=(const months& _Months) noexcept {
        *this = *this + _Months;
        return *this;
    }
    template <int>
    constexpr year_month_day& year_month_day::operator-=(const months& _Months) noexcept {
        *this = *this - _Months;
        return *this;
    }
    constexpr year_month_day& year_month_day::operator+=(const years& _Years) noexcept {
        *this = *this + _Years;
        return *this;
    }
    constexpr year_month_day& year_month_day::operator-=(const years& _Years) noexcept {
        *this = *this - _Years;
        return *this;
    }

    class year_month_day_last {
    public:
        constexpr year_month_day_last(const year& _Year_, const month_day_last& _Mdl) noexcept
            : _Year{_Year_}, _Month_day_last{_Mdl} {}

        template <int = 0>
        constexpr year_month_day_last& operator+=(const months& _Months) noexcept;
        template <int = 0>
        constexpr year_month_day_last& operator-=(const months& _Months) noexcept;
        constexpr year_month_day_last& operator+=(const years& _Years) noexcept;
        constexpr year_month_day_last& operator-=(const years& _Years) noexcept;

        _NODISCARD constexpr year year() const noexcept {
            return _Year;
        }
        _NODISCARD constexpr month month() const noexcept {
            return _Month_day_last.month();
        }
        _NODISCARD constexpr month_day_last month_day_last() const noexcept {
            return _Month_day_last;
        }
        _NODISCARD constexpr day day() const noexcept {
            return _Last_day(year(), month());
        }

        _NODISCARD constexpr operator sys_days() const noexcept {
            return sys_days{year_month_day{year(), month(), day()}};
        }
        _NODISCARD constexpr explicit operator local_days() const noexcept {
            return local_days{static_cast<sys_days>(*this).time_since_epoch()};
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Year.ok() && _Month_day_last.ok();
        }

    private:
        _CHRONO year _Year;
        _CHRONO month_day_last _Month_day_last;
    };

    _NODISCARD constexpr bool operator==(const year_month_day_last& _Left, const year_month_day_last& _Right) noexcept {
        return _Left.year() == _Right.year() && _Left.month_day_last() == _Right.month_day_last();
    }
    _NODISCARD constexpr strong_ordering operator<=>(
        const year_month_day_last& _Left, const year_month_day_last& _Right) noexcept {
        const auto _Comp = _Left.year() <=> _Right.year();
        if (_Comp != 0) {
            return _Comp;
        }

        return _Left.month_day_last() <=> _Right.month_day_last();
    }

    template <int = 0>
    _NODISCARD constexpr year_month_day_last operator+(
        const year_month_day_last& _Left, const months& _Right) noexcept {
        const auto _Ym = year_month{_Left.year(), _Left.month()} + _Right;
        return {_Ym.year(), month_day_last{_Ym.month()}};
    }
    template <int = 0>
    _NODISCARD constexpr year_month_day_last operator+(
        const months& _Left, const year_month_day_last& _Right) noexcept {
        return _Right + _Left;
    }

    template <int = 0>
    _NODISCARD constexpr year_month_day_last operator-(
        const year_month_day_last& _Left, const months& _Right) noexcept {
        return _Left + -_Right;
    }

    _NODISCARD constexpr year_month_day_last operator+(const year_month_day_last& _Left, const years& _Right) noexcept {
        return {_Left.year() + _Right, _Left.month_day_last()};
    }
    _NODISCARD constexpr year_month_day_last operator+(const years& _Left, const year_month_day_last& _Right) noexcept {
        return _Right + _Left;
    }
    _NODISCARD constexpr year_month_day_last operator-(const year_month_day_last& _Left, const years& _Right) noexcept {
        return _Left + -_Right;
    }

    template <int>
    constexpr year_month_day_last& year_month_day_last::operator+=(const months& _Months) noexcept {
        *this = *this + _Months;
        return *this;
    }
    template <int>
    constexpr year_month_day_last& year_month_day_last::operator-=(const months& _Months) noexcept {
        *this = *this - _Months;
        return *this;
    }
    constexpr year_month_day_last& year_month_day_last::operator+=(const years& _Years) noexcept {
        *this = *this + _Years;
        return *this;
    }
    constexpr year_month_day_last& year_month_day_last::operator-=(const years& _Years) noexcept {
        *this = *this - _Years;
        return *this;
    }

    constexpr year_month_day::year_month_day(const year_month_day_last& _Ymdl) noexcept
        : _Year{_Ymdl.year()}, _Month{_Ymdl.month()}, _Day{_Ymdl.day()} {}

    class year_month_weekday {
    public:
        year_month_weekday() = default;
        constexpr year_month_weekday(const year& _Year_, const month& _Month_, const weekday_indexed& _Wdi) noexcept
            : _Year{_Year_}, _Month{_Month_}, _Weekday_index{_Wdi} {}
        constexpr year_month_weekday(const sys_days& _Sys_days) noexcept
            : year_month_weekday{_Ymwd_from_days(_Sys_days.time_since_epoch())} {}
        constexpr explicit year_month_weekday(const local_days& _Local_days) noexcept
            : year_month_weekday{_Ymwd_from_days(_Local_days.time_since_epoch())} {}

        template <int = 0>
        constexpr year_month_weekday& operator+=(const months& _Months) noexcept;
        template <int = 0>
        constexpr year_month_weekday& operator-=(const months& _Months) noexcept;
        constexpr year_month_weekday& operator+=(const years& _Years) noexcept;
        constexpr year_month_weekday& operator-=(const years& _Years) noexcept;

        _NODISCARD constexpr year year() const noexcept {
            return _Year;
        }
        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr weekday weekday() const noexcept {
            return _Weekday_index.weekday();
        }
        _NODISCARD constexpr unsigned int index() const noexcept {
            return _Weekday_index.index();
        }
        _NODISCARD constexpr weekday_indexed weekday_indexed() const noexcept {
            return _Weekday_index;
        }

        _NODISCARD constexpr operator sys_days() const noexcept {
            const sys_days _First = year_month_day{_Year, _Month, day{1}};
            const days _Diff      = weekday() - _CHRONO weekday{_First};
            const days _Days      = _Diff + days{(static_cast<int>(index()) - 1) * 7};
            return _First + _Days;
        }
        _NODISCARD constexpr explicit operator local_days() const noexcept {
            return local_days{static_cast<sys_days>(*this).time_since_epoch()};
        }
        _NODISCARD constexpr bool ok() const noexcept {
            if (!_Year.ok() || !_Month.ok() || !_Weekday_index.ok()) {
                return false;
            }

            if (_Weekday_index.index() <= 4) {
                return true;
            }

            // As index() == 5 is not always valid
            // Determine the date of the first weekday and check if + days{28} is <= last day of the month
            const sys_days _First_of_month = year_month_day{_Year, _Month, day{1}};
            const days _First_weekday      = weekday() - _CHRONO weekday{_First_of_month} + days{1};
            const days _Last               = _First_weekday + days{28};
            return static_cast<unsigned int>(_Last.count()) <= static_cast<unsigned int>(_Last_day(_Year, _Month));
        }

    private:
        _CHRONO year _Year;
        _CHRONO month _Month;
        _CHRONO weekday_indexed _Weekday_index;

        _NODISCARD static constexpr year_month_weekday _Ymwd_from_days(days _Dp) noexcept {
            const _CHRONO year_month_day _Ymd = sys_days{_Dp};
            const _CHRONO weekday _Wd         = sys_days{_Dp};
            const auto _Idx                   = ((static_cast<unsigned int>(_Ymd.day()) - 1) / 7) + 1;
            return {_Ymd.year(), _Ymd.month(), _Wd[_Idx]};
        }
    };

    _NODISCARD constexpr bool operator==(const year_month_weekday& _Left, const year_month_weekday& _Right) noexcept {
        return _Left.year() == _Right.year() && _Left.month() == _Right.month()
            && _Left.weekday_indexed() == _Right.weekday_indexed();
    }

    template <int = 0>
    _NODISCARD constexpr year_month_weekday operator+(const year_month_weekday& _Left, const months& _Right) noexcept {
        const auto _Ym = year_month{_Left.year(), _Left.month()} + _Right;
        return {_Ym.year(), _Ym.month(), _Left.weekday_indexed()};
    }
    template <int = 0>
    _NODISCARD constexpr year_month_weekday operator+(const months& _Left, const year_month_weekday& _Right) noexcept {
        return _Right + _Left;
    }

    template <int = 0>
    _NODISCARD constexpr year_month_weekday operator-(const year_month_weekday& _Left, const months& _Right) noexcept {
        return _Left + -_Right;
    }

    _NODISCARD constexpr year_month_weekday operator+(const year_month_weekday& _Left, const years& _Right) noexcept {
        return year_month_weekday{_Left.year() + _Right, _Left.month(), _Left.weekday_indexed()};
    }
    _NODISCARD constexpr year_month_weekday operator+(const years& _Left, const year_month_weekday& _Right) noexcept {
        return _Right + _Left;
    }

    _NODISCARD constexpr year_month_weekday operator-(const year_month_weekday& _Left, const years& _Right) noexcept {
        return _Left + -_Right;
    }

    template <int>
    constexpr year_month_weekday& year_month_weekday::operator+=(const months& _Months) noexcept {
        *this = *this + _Months;
        return *this;
    }
    template <int>
    constexpr year_month_weekday& year_month_weekday::operator-=(const months& _Months) noexcept {
        *this = *this - _Months;
        return *this;
    }
    constexpr year_month_weekday& year_month_weekday::operator+=(const years& _Years) noexcept {
        *this = *this + _Years;
        return *this;
    }
    constexpr year_month_weekday& year_month_weekday::operator-=(const years& _Years) noexcept {
        *this = *this - _Years;
        return *this;
    }

    class year_month_weekday_last {
    public:
        constexpr year_month_weekday_last(const year& _Year_, const month& _Month_, const weekday_last& _Wdl) noexcept
            : _Year{_Year_}, _Month{_Month_}, _Weekday_last{_Wdl} {}

        template <int = 0>
        constexpr year_month_weekday_last& operator+=(const months& _Months) noexcept;
        template <int = 0>
        constexpr year_month_weekday_last& operator-=(const months& _Months) noexcept;
        constexpr year_month_weekday_last& operator+=(const years& _Years) noexcept;
        constexpr year_month_weekday_last& operator-=(const years& _Years) noexcept;

        _NODISCARD constexpr year year() const noexcept {
            return _Year;
        }
        _NODISCARD constexpr month month() const noexcept {
            return _Month;
        }
        _NODISCARD constexpr weekday weekday() const noexcept {
            return _Weekday_last.weekday();
        }

        _NODISCARD constexpr weekday_last weekday_last() const noexcept {
            return _Weekday_last;
        }

        _NODISCARD constexpr operator sys_days() const noexcept {
            const sys_days _Last = year_month_day_last{_Year, month_day_last{_Month}};
            const auto _Diff     = _CHRONO weekday{_Last} - weekday();
            return _Last - _Diff;
        }
        _NODISCARD constexpr explicit operator local_days() const noexcept {
            return local_days{static_cast<sys_days>(*this).time_since_epoch()};
        }
        _NODISCARD constexpr bool ok() const noexcept {
            return _Year.ok() && _Month.ok() && _Weekday_last.ok();
        }

    private:
        _CHRONO year _Year;
        _CHRONO month _Month;
        _CHRONO weekday_last _Weekday_last;
    };

    _NODISCARD constexpr bool operator==(
        const year_month_weekday_last& _Left, const year_month_weekday_last& _Right) noexcept {
        return _Left.year() == _Right.year() && _Left.month() == _Right.month()
            && _Left.weekday_last() == _Right.weekday_last();
    }

    template <int = 0>
    _NODISCARD constexpr year_month_weekday_last operator+(
        const year_month_weekday_last& _Left, const months& _Right) noexcept {
        const auto _Ym = year_month{_Left.year(), _Left.month()} + _Right;
        return {_Ym.year(), _Ym.month(), _Left.weekday_last()};
    }
    template <int = 0>
    _NODISCARD constexpr year_month_weekday_last operator+(
        const months& _Left, const year_month_weekday_last& _Right) noexcept {
        return _Right + _Left;
    }

    template <int = 0>
    _NODISCARD constexpr year_month_weekday_last operator-(
        const year_month_weekday_last& _Left, const months& _Right) noexcept {
        return _Left + -_Right;
    }

    _NODISCARD constexpr year_month_weekday_last operator+(
        const year_month_weekday_last& _Left, const years& _Right) noexcept {
        return {_Left.year() + _Right, _Left.month(), _Left.weekday_last()};
    }
    _NODISCARD constexpr year_month_weekday_last operator+(
        const years& _Left, const year_month_weekday_last& _Right) noexcept {
        return _Right + _Left;
    }

    _NODISCARD constexpr year_month_weekday_last operator-(
        const year_month_weekday_last& _Left, const years& _Right) noexcept {
        return _Left + -_Right;
    }

    template <int>
    constexpr year_month_weekday_last& year_month_weekday_last::operator+=(const months& _Months) noexcept {
        *this = *this + _Months;
        return *this;
    }
    template <int>
    constexpr year_month_weekday_last& year_month_weekday_last::operator-=(const months& _Months) noexcept {
        *this = *this - _Months;
        return *this;
    }
    constexpr year_month_weekday_last& year_month_weekday_last::operator+=(const years& _Years) noexcept {
        *this = *this + _Years;
        return *this;
    }
    constexpr year_month_weekday_last& year_month_weekday_last::operator-=(const years& _Years) noexcept {
        *this = *this - _Years;
        return *this;
    }

    // Civil calendar conventional syntax operators
    _NODISCARD constexpr year_month operator/(const year& _Year, const month& _Month) noexcept {
        return {_Year, _Month};
    }
    _NODISCARD constexpr year_month operator/(const year& _Year, int _Month) noexcept {
        return _Year / month{static_cast<unsigned int>(_Month)};
    }
    _NODISCARD constexpr month_day operator/(const month& _Month, const day& _Day) noexcept {
        return {_Month, _Day};
    }
    _NODISCARD constexpr month_day operator/(const month& _Month, int _Day) noexcept {
        return _Month / day{static_cast<unsigned int>(_Day)};
    }
    _NODISCARD constexpr month_day operator/(int _Month, const day& _Day) noexcept {
        return month{static_cast<unsigned int>(_Month)} / _Day;
    }
    _NODISCARD constexpr month_day operator/(const day& _Day, const month& _Month) noexcept {
        return _Month / _Day;
    }
    _NODISCARD constexpr month_day operator/(const day& _Day, int _Month) noexcept {
        return month{static_cast<unsigned int>(_Month)} / _Day;
    }
    _NODISCARD constexpr month_day_last operator/(const month& _Month, last_spec) noexcept {
        return month_day_last{_Month};
    }
    _NODISCARD constexpr month_day_last operator/(int _Month, last_spec) noexcept {
        return month{static_cast<unsigned int>(_Month)} / last;
    }
    _NODISCARD constexpr month_day_last operator/(last_spec, const month& _Month) noexcept {
        return _Month / last;
    }
    _NODISCARD constexpr month_day_last operator/(last_spec, int _Month) noexcept {
        return month{static_cast<unsigned int>(_Month)} / last;
    }
    _NODISCARD constexpr month_weekday operator/(const month& _Month, const weekday_indexed& _Wdi) noexcept {
        return {_Month, _Wdi};
    }
    _NODISCARD constexpr month_weekday operator/(int _Month, const weekday_indexed& _Wdi) noexcept {
        return month{static_cast<unsigned int>(_Month)} / _Wdi;
    }
    _NODISCARD constexpr month_weekday operator/(const weekday_indexed& _Wdi, const month& _Month) noexcept {
        return _Month / _Wdi;
    }
    _NODISCARD constexpr month_weekday operator/(const weekday_indexed& _Wdi, int _Month) noexcept {
        return month{static_cast<unsigned int>(_Month)} / _Wdi;
    }
    _NODISCARD constexpr month_weekday_last operator/(const month& _Month, const weekday_last& _Wdl) noexcept {
        return {_Month, _Wdl};
    }
    _NODISCARD constexpr month_weekday_last operator/(int _Month, const weekday_last& _Wdl) noexcept {
        return month{static_cast<unsigned int>(_Month)} / _Wdl;
    }
    _NODISCARD constexpr month_weekday_last operator/(const weekday_last& _Wdl, const month& _Month) noexcept {
        return _Month / _Wdl;
    }
    _NODISCARD constexpr month_weekday_last operator/(const weekday_last& _Wdl, int _Month) noexcept {
        return month{static_cast<unsigned int>(_Month)} / _Wdl;
    }
    _NODISCARD constexpr year_month_day operator/(const year_month& _Ym, const day& _Day) noexcept {
        return {_Ym.year(), _Ym.month(), _Day};
    }
    _NODISCARD constexpr year_month_day operator/(const year_month& _Ym, int _Day) noexcept {
        return _Ym / day{static_cast<unsigned int>(_Day)};
    }
    _NODISCARD constexpr year_month_day operator/(const year& _Year, const month_day& _Md) noexcept {
        return _Year / _Md.month() / _Md.day();
    }
    _NODISCARD constexpr year_month_day operator/(int _Year, const month_day& _Md) noexcept {
        return year{_Year} / _Md.month() / _Md.day();
    }
    _NODISCARD constexpr year_month_day operator/(const month_day& _Md, const year& _Year) noexcept {
        return _Year / _Md.month() / _Md.day();
    }
    _NODISCARD constexpr year_month_day operator/(const month_day& _Md, int _Year) noexcept {
        return year{_Year} / _Md.month() / _Md.day();
    }
    _NODISCARD constexpr year_month_day_last operator/(const year_month& _Ym, last_spec) noexcept {
        return {_Ym.year(), month_day_last{_Ym.month()}};
    }
    _NODISCARD constexpr year_month_day_last operator/(const year& _Year, const month_day_last& _Mdl) noexcept {
        return {_Year, _Mdl};
    }
    _NODISCARD constexpr year_month_day_last operator/(int _Year, const month_day_last& _Mdl) noexcept {
        return year{_Year} / _Mdl;
    }
    _NODISCARD constexpr year_month_day_last operator/(const month_day_last& _Mdl, const year& _Year) noexcept {
        return _Year / _Mdl;
    }
    _NODISCARD constexpr year_month_day_last operator/(const month_day_last& _Mdl, int _Year) noexcept {
        return year{_Year} / _Mdl;
    }
    _NODISCARD constexpr year_month_weekday operator/(const year_month& _Ym, const weekday_indexed& _Wdi) noexcept {
        return year_month_weekday{_Ym.year(), _Ym.month(), _Wdi};
    }
    _NODISCARD constexpr year_month_weekday operator/(const year& _Year, const month_weekday& _Mwd) noexcept {
        return year_month_weekday{_Year, _Mwd.month(), _Mwd.weekday_indexed()};
    }
    _NODISCARD constexpr year_month_weekday operator/(int _Year, const month_weekday& _Mwd) noexcept {
        return year{_Year} / _Mwd;
    }
    _NODISCARD constexpr year_month_weekday operator/(const month_weekday& _Mwd, const year& _Year) noexcept {
        return _Year / _Mwd;
    }
    _NODISCARD constexpr year_month_weekday operator/(const month_weekday& _Mwd, int _Year) noexcept {
        return year{_Year} / _Mwd;
    }
    _NODISCARD constexpr year_month_weekday_last operator/(const year_month& _Ym, const weekday_last& _Wdl) noexcept {
        return {_Ym.year(), _Ym.month(), _Wdl};
    }
    _NODISCARD constexpr year_month_weekday_last operator/(
        const year& _Year, const month_weekday_last& _Mwdl) noexcept {
        return {_Year, _Mwdl.month(), _Mwdl.weekday_last()};
    }
    _NODISCARD constexpr year_month_weekday_last operator/(int _Year, const month_weekday_last& _Mwdl) noexcept {
        return year{_Year} / _Mwdl;
    }
    _NODISCARD constexpr year_month_weekday_last operator/(
        const month_weekday_last& _Mwdl, const year& _Year) noexcept {
        return _Year / _Mwdl;
    }
    _NODISCARD constexpr year_month_weekday_last operator/(const month_weekday_last& _Mwdl, int _Year) noexcept {
        return year{_Year} / _Mwdl;
    }

    // Calendrical constants
    inline constexpr weekday Sunday{0};
    inline constexpr weekday Monday{1};
    inline constexpr weekday Tuesday{2};
    inline constexpr weekday Wednesday{3};
    inline constexpr weekday Thursday{4};
    inline constexpr weekday Friday{5};
    inline constexpr weekday Saturday{6};

    inline constexpr month January{1};
    inline constexpr month February{2};
    inline constexpr month March{3};
    inline constexpr month April{4};
    inline constexpr month May{5};
    inline constexpr month June{6};
    inline constexpr month July{7};
    inline constexpr month August{8};
    inline constexpr month September{9};
    inline constexpr month October{10};
    inline constexpr month November{11};
    inline constexpr month December{12};

    _NODISCARD constexpr intmax_t _Pow10(const unsigned int _Exp) {
        intmax_t _Result = 1;
        for (unsigned int _Ix = 0; _Ix < _Exp; ++_Ix) {
            _Result *= 10;
        }
        return _Result;
    }

    template <class _Duration>
    class hh_mm_ss {
    public:
        static_assert(_Is_duration_v<_Duration>,
            "N4885 [time.hms.overview]/2 mandates Duration to be a specialization of chrono::duration.");

        static constexpr unsigned int fractional_width = [] {
            auto _Num           = _Duration::period::num;
            constexpr auto _Den = _Duration::period::den;
            // Returns the number of fractional digits of _Num / _Den in the range [0, 18].
            // If it can't be represented, 6 is returned.
            // Example: _Fractional_width(1, 8) would return 3 for 0.125.
            _STL_ASSERT(_Num > 0 && _Den > 0, "Numerator and denominator can't be less than 1.");
            unsigned int _Result = 0;
            for (; _Num % _Den != 0 && _Result < 19; _Num = _Num % _Den * 10, ++_Result) {
            }
            return _Result == 19 ? 6 : _Result;
        }();
        using precision =
            duration<common_type_t<typename _Duration::rep, seconds::rep>, ratio<1, _Pow10(fractional_width)>>;

        constexpr hh_mm_ss() noexcept : hh_mm_ss{_Duration::zero()} {}
        // clang-format off
        constexpr explicit hh_mm_ss(_Duration _Dur)
            : _Is_neg{_Dur < _Duration::zero()},
              _Hours{_CHRONO duration_cast<_CHRONO hours>(_CHRONO abs(_Dur))},
              _Mins{_CHRONO duration_cast<_CHRONO minutes>(_CHRONO abs(_Dur) - hours())},
              _Secs{_CHRONO duration_cast<_CHRONO seconds>(_CHRONO abs(_Dur) - hours() - minutes())} {
            // clang-format on
            if constexpr (treat_as_floating_point_v<typename precision::rep>) {
                _Sub_secs = _CHRONO abs(_Dur) - hours() - minutes() - seconds();
            } else {
                _Sub_secs = _CHRONO duration_cast<precision>(_CHRONO abs(_Dur) - hours() - minutes() - seconds());
            }
        }

        _NODISCARD constexpr bool is_negative() const noexcept {
            return _Is_neg;
        }
        _NODISCARD constexpr hours hours() const noexcept {
            return _Hours;
        }
        _NODISCARD constexpr minutes minutes() const noexcept {
            return _Mins;
        }
        _NODISCARD constexpr seconds seconds() const noexcept {
            return _Secs;
        }
        _NODISCARD constexpr precision subseconds() const noexcept {
            return _Sub_secs;
        }

        _NODISCARD constexpr explicit operator precision() const noexcept {
            return to_duration();
        }
        _NODISCARD constexpr precision to_duration() const noexcept {
            const auto _Dur = _Hours + _Mins + _Secs + _Sub_secs;
            return _Is_neg ? -_Dur : _Dur;
        }

    private:
        bool _Is_neg;
        _CHRONO hours _Hours;
        _CHRONO minutes _Mins;
        _CHRONO seconds _Secs;
        precision _Sub_secs;
    };

    _NODISCARD constexpr bool is_am(const hours& _Hours) noexcept {
        return _Hours >= hours{0} && _Hours <= hours{11};
    }
    _NODISCARD constexpr bool is_pm(const hours& _Hours) noexcept {
        return _Hours >= hours{12} && _Hours <= hours{23};
    }

    _NODISCARD constexpr hours make12(const hours& _Hours) noexcept {
        const auto _H_count{_Hours.count()};
        auto _Ret{_H_count == 0 ? 12 : _H_count};
        if (_Ret > 12) {
            _Ret -= 12;
        }

        return hours{_Ret};
    }
    _NODISCARD constexpr hours make24(const hours& _Hours, bool _Is_pm) noexcept {
        const auto _H_count{_Hours.count()};
        auto _Ret{_H_count == 12 ? 0 : _H_count};
        if (_Is_pm) {
            _Ret += 12;
        }

        return hours{_Ret};
    }

    // [time.zone.info]

    struct sys_info {
        sys_seconds begin;
        sys_seconds end;
        seconds offset;
        minutes save;
        string abbrev;
    };

    struct local_info {
        static constexpr int unique      = 0;
        static constexpr int nonexistent = 1;
        static constexpr int ambiguous   = 2;

        int result;
        sys_info first;
        sys_info second;
    };

    class nonexistent_local_time : public runtime_error {
    public:
        template <class _Duration>
        nonexistent_local_time(const local_time<_Duration>& _Tp, const local_info& _Info)
            : runtime_error(_Make_string(_Tp, _Info)) {}

    private:
#ifdef __cpp_lib_format
        template <class _Duration>
        _NODISCARD static string _Make_string(const local_time<_Duration>& _Tp, const local_info& _Info);
#else // ^^^ no workaround / workaround vvv
        template <class _Duration>
        _NODISCARD static string _Make_string(const local_time<_Duration>&, const local_info&) {
            return "nonexistent_local_time";
        }
#endif // ^^^ workaround ^^^
    };

    class ambiguous_local_time : public runtime_error {
    public:
        template <class _Duration>
        ambiguous_local_time(const local_time<_Duration>& _Tp, const local_info& _Info)
            : runtime_error(_Make_string(_Tp, _Info)) {}

    private:
#ifdef __cpp_lib_format
        template <class _Duration>
        _NODISCARD static string _Make_string(const local_time<_Duration>& _Tp, const local_info& _Info);
#else // ^^^ no workaround / workaround vvv
        template <class _Duration>
        _NODISCARD static string _Make_string(const local_time<_Duration>&, const local_info&) {
            return "ambiguous_local_time";
        }
#endif // ^^^ workaround ^^^
    };

    // [time.zone.timezone]

    enum class choose { earliest, latest };

    class time_zone {
    public:
        explicit time_zone(string_view _Name_) : _Name(_Name_) {}

        time_zone(time_zone&&) = default;
        time_zone& operator=(time_zone&&) = default;

        _NODISCARD string_view name() const noexcept {
            return _Name;
        }

        template <class _Duration>
        _NODISCARD sys_info get_info(const sys_time<_Duration>& _Sys) const {
            return _Get_info(_Sys.time_since_epoch());
        }

        template <class _Duration>
        _NODISCARD local_info get_info(const local_time<_Duration>& _Local) const {
            local_info _Info{};
            const auto _Time_since_ep = _Local.time_since_epoch();
            _Info.first               = _Get_info(_Time_since_ep);

            const sys_seconds _Local_sys{_CHRONO duration_cast<sys_seconds::duration>(_Time_since_ep)};
            const auto _Curr_sys = _Local_sys - _Info.first.offset;
            if (_Info.first.begin != _Min_seconds && _Curr_sys < _Info.first.begin + days{1}) {
                // get previous transition information
                _Info.second = get_info(_Info.first.begin - seconds{1});

                const auto _Transition = _Info.first.begin;
                const auto _Prev_sys   = _Local_sys - _Info.second.offset;
                if (_Curr_sys >= _Transition) {
                    if (_Prev_sys < _Transition) {
                        _Info.result = local_info::ambiguous;
                        _STD swap(_Info.first, _Info.second);
                    } else {
                        _Info.result = local_info::unique;
                        _Info.second = {};
                    }
                } else {
                    if (_Prev_sys >= _Transition) {
                        _Info.result = local_info::nonexistent;
                        _STD swap(_Info.first, _Info.second);
                    } else {
                        _Info.result = local_info::unique;
                        _Info.first  = _STD move(_Info.second);
                        _Info.second = {};
                    }
                }
            } else if (_Info.first.end != _Max_seconds && _Curr_sys > _Info.first.end - days{1}) {
                // get next transition information
                _Info.second = get_info(_Info.first.end + seconds{1});

                const auto _Transition = _Info.first.end;
                const auto _Next_sys   = _Local_sys - _Info.second.offset;
                if (_Curr_sys < _Transition) {
                    if (_Next_sys >= _Transition) {
                        _Info.result = local_info::ambiguous;
                    } else {
                        _Info.result = local_info::unique;
                        _Info.second = {};
                    }
                } else {
                    if (_Next_sys < _Transition) {
                        _Info.result = local_info::nonexistent;
                    } else {
                        _Info.result = local_info::unique;
                        _Info.first  = _STD move(_Info.second);
                        _Info.second = {};
                    }
                }
            } else {
                // local time is contained inside of first transition boundaries by at least 1 day
                _Info.result = local_info::unique;
                _Info.second = {};
            }

            return _Info;
        }

        template <class _Duration>
        _NODISCARD sys_time<common_type_t<_Duration, seconds>> to_sys(const local_time<_Duration>& _Local) const {
            const auto _Info = get_info(_Local);
            if (_Info.result == local_info::nonexistent) {
                _THROW(nonexistent_local_time(_Local, _Info));
            } else if (_Info.result == local_info::ambiguous) {
                _THROW(ambiguous_local_time(_Local, _Info));
            }

            return sys_time<common_type_t<_Duration, seconds>>{_Local.time_since_epoch() - _Info.first.offset};
        }

        template <class _Duration>
        _NODISCARD sys_time<common_type_t<_Duration, seconds>> to_sys(
            const local_time<_Duration>& _Local, const choose _Choose) const {
            const auto _Info = get_info(_Local);
            if (_Info.result == local_info::nonexistent) {
                return _Info.first.end;
            }

            const auto _Offset = (_Info.result == local_info::unique || _Choose == choose::earliest)
                                   ? _Info.first.offset
                                   : _Info.second.offset;
            return sys_time<common_type_t<_Duration, seconds>>{_Local.time_since_epoch() - _Offset};
        }

        template <class _Duration>
        _NODISCARD local_time<common_type_t<_Duration, seconds>> to_local(const sys_time<_Duration>& _Sys) const {
            const auto _Info = get_info(_Sys);
            return local_time<common_type_t<_Duration, seconds>>{_Sys.time_since_epoch() + _Info.offset};
        }

        static constexpr sys_seconds _Min_seconds{sys_days{(year::min)() / January / 1}};
        static constexpr sys_seconds _Max_seconds{sys_seconds{sys_days{(year::max)() / December / 32}} - seconds{1}};

    private:
        template <class _Duration>
        _NODISCARD sys_info _Get_info(const _Duration& _Dur) const {
            using _Internal_duration = duration<__std_tzdb_epoch_milli, milli>;
            const auto _Internal_dur = _CHRONO duration_cast<_Internal_duration>(_Dur);
            const unique_ptr<__std_tzdb_sys_info, _Tzdb_deleter<__std_tzdb_sys_info>> _Info{
                __std_tzdb_get_sys_info(_Name.c_str(), _Name.length(), _Internal_dur.count())};
            if (_Info == nullptr) {
                _Xbad_alloc();
            } else if (_Info->_Err == __std_tzdb_error::_Win_error) {
                _XGetLastError();
            } else if (_Info->_Err == __std_tzdb_error::_Icu_error) {
                _Xruntime_error("Internal error loading IANA database information");
            }

            constexpr auto _Min_internal =
                _CHRONO duration_cast<_Internal_duration>(_Min_seconds.time_since_epoch()).count();
            constexpr auto _Max_internal =
                _CHRONO duration_cast<_Internal_duration>(_Max_seconds.time_since_epoch()).count();
            const auto _Begin =
                _Info->_Begin <= _Min_internal
                    ? _Min_seconds
                    : sys_seconds{_CHRONO duration_cast<sys_seconds::duration>(_Internal_duration{_Info->_Begin})};
            const auto _End =
                _Info->_End >= _Max_internal
                    ? _Max_seconds
                    : sys_seconds{_CHRONO duration_cast<sys_seconds::duration>(_Internal_duration{_Info->_End})};
            return {.begin = _Begin,
                .end       = _End,
                .offset    = _CHRONO duration_cast<seconds>(_Internal_duration{_Info->_Offset}),
                .save      = _CHRONO duration_cast<minutes>(_Internal_duration{_Info->_Save}),
                .abbrev    = _Info->_Abbrev};
        }

        string _Name;
    };

    _NODISCARD inline bool operator==(const time_zone& _Left, const time_zone& _Right) noexcept {
        return _Left.name() == _Right.name();
    }

#ifdef __cpp_lib_concepts
    _NODISCARD inline strong_ordering operator<=>(const time_zone& _Left, const time_zone& _Right) noexcept {
        return _Left.name() <=> _Right.name();
    }
#endif // __cpp_lib_concepts

    // [time.zone.leap]

    class leap_second {
    public:
        leap_second(const leap_second&) = default;
        leap_second& operator=(const leap_second&) = default;

        constexpr leap_second(
            const sys_seconds& _Date_, const bool _Is_positive_, const seconds& _Prev_elapsed) noexcept
            : _Date{_Date_}, _Is_positive{_Is_positive_} {
            _Elapsed_offset = _Prev_elapsed + value();
        }

        _NODISCARD constexpr sys_seconds date() const noexcept {
            return _Date;
        }

        _NODISCARD constexpr seconds value() const noexcept {
            return _Is_positive ? seconds{1} : seconds{-1};
        }

        _NODISCARD constexpr bool _Positive() const noexcept {
            return _Is_positive;
        }

        _NODISCARD constexpr seconds _Elapsed() const noexcept {
            return _Elapsed_offset;
        }

    private:
        sys_seconds _Date;
        bool _Is_positive;
        seconds _Elapsed_offset;
    };

    _NODISCARD constexpr bool operator==(const leap_second& _Left, const leap_second& _Right) noexcept {
        return _Left.date() == _Right.date();
    }
    template <class _Duration>
    _NODISCARD constexpr bool operator==(const leap_second& _Left, const sys_time<_Duration>& _Right) noexcept {
        return _Left.date() == _Right;
    }

    template <class _Duration>
    _NODISCARD constexpr bool operator<(const leap_second& _Left, const sys_time<_Duration>& _Right) noexcept {
        return _Left.date() < _Right;
    }
    template <class _Duration>
    _NODISCARD constexpr bool operator<(const sys_time<_Duration>& _Left, const leap_second& _Right) noexcept {
        return _Left < _Right.date();
    }

    template <class _Duration>
    _NODISCARD constexpr bool operator>(const leap_second& _Left, const sys_time<_Duration>& _Right) noexcept {
        return _Right < _Left.date();
    }
    template <class _Duration>
    _NODISCARD constexpr bool operator>(const sys_time<_Duration>& _Left, const leap_second& _Right) noexcept {
        return _Right.date() < _Left;
    }

    template <class _Duration>
    _NODISCARD constexpr bool operator<=(const leap_second& _Left, const sys_time<_Duration>& _Right) noexcept {
        return !(_Right < _Left.date());
    }
    template <class _Duration>
    _NODISCARD constexpr bool operator<=(const sys_time<_Duration>& _Left, const leap_second& _Right) noexcept {
        return !(_Right.date() < _Left);
    }

    template <class _Duration>
    _NODISCARD constexpr bool operator>=(const leap_second& _Left, const sys_time<_Duration>& _Right) noexcept {
        return !(_Left.date() < _Right);
    }
    template <class _Duration>
    _NODISCARD constexpr bool operator>=(const sys_time<_Duration>& _Left, const leap_second& _Right) noexcept {
        return !(_Left < _Right.date());
    }

#ifdef __cpp_lib_concepts
    // clang-format off
    template <class _Duration>
        requires three_way_comparable_with<sys_seconds, sys_time<_Duration>>
    _NODISCARD constexpr auto operator<=>(
        const leap_second& _Left, const sys_time<_Duration>& _Right) noexcept {
        // clang-format on
        return _Left.date() <=> _Right;
    }
    _NODISCARD constexpr strong_ordering operator<=>(const leap_second& _Left, const leap_second& _Right) noexcept {
        return _Left.date() <=> _Right.date();
    }
#endif // __cpp_lib_concepts

    // [time.zone.link]

    class time_zone_link {
    public:
        explicit time_zone_link(string_view _Name_, string_view _Target_) : _Name(_Name_), _Target(_Target_) {}

        time_zone_link(time_zone_link&&) = default;
        time_zone_link& operator=(time_zone_link&&) = default;

        _NODISCARD string_view name() const noexcept {
            return _Name;
        }

        _NODISCARD string_view target() const noexcept {
            return _Target;
        }

    private:
        string _Name;
        string _Target;
    };

    _NODISCARD inline bool operator==(const time_zone_link& _Left, const time_zone_link& _Right) noexcept {
        return _Left.name() == _Right.name();
    }

#ifdef __cpp_lib_concepts
    _NODISCARD inline strong_ordering operator<=>(const time_zone_link& _Left, const time_zone_link& _Right) noexcept {
        return _Left.name() <=> _Right.name();
    }
#endif // __cpp_lib_concepts

    // [time.zone.db]

    _NODISCARD inline string _Tzdb_generate_current_zone() {
        unique_ptr<__std_tzdb_current_zone_info, _Tzdb_deleter<__std_tzdb_current_zone_info>> _Info{
            __std_tzdb_get_current_zone()};
        if (_Info == nullptr) {
            _Xbad_alloc();
        } else if (_Info->_Err == __std_tzdb_error::_Win_error) {
            _XGetLastError();
        } else if (_Info->_Err == __std_tzdb_error::_Icu_error) {
            _Xruntime_error("Internal error loading IANA database information");
        }

        return {_Info->_Tz_name};
    }

    template <class _Ty>
    _NODISCARD const _Ty* _Locate_zone_impl(const vector<_Ty>& _Vec, string_view _Name) {
        const auto _Result = _STD find_if(_Vec.begin(), _Vec.end(), [&](auto& _Tz) { return _Tz.name() == _Name; });
        return _Result == _Vec.end() ? nullptr : &*_Result;
    }

    struct tzdb {
        string version;
        vector<time_zone> zones;
        vector<time_zone_link> links;
        vector<leap_second> leap_seconds;
        bool _All_ls_positive;

        _NODISCARD const time_zone* locate_zone(string_view _Tz_name) const {
            auto _Tz = _Locate_zone_impl(zones, _Tz_name);
            if (_Tz != nullptr) {
                return _Tz;
            }

            const auto _Link = _Locate_zone_impl(links, _Tz_name);
            if (_Link != nullptr) {
                _Tz = _Locate_zone_impl(zones, _Link->target());
                if (_Tz != nullptr) {
                    return _Tz;
                }
            }

            _Xruntime_error("unable to locate time_zone with given name");
        }

        _NODISCARD const time_zone* current_zone() const {
            return locate_zone(_Tzdb_generate_current_zone());
        }
    };

    _NODISCARD inline tuple<string, decltype(tzdb::zones), decltype(tzdb::links)> _Tzdb_generate_time_zones() {
        unique_ptr<__std_tzdb_time_zones_info, _Tzdb_deleter<__std_tzdb_time_zones_info>> _Info{
            __std_tzdb_get_time_zones()};
        if (_Info == nullptr) {
            _Xbad_alloc();
        } else if (_Info->_Err == __std_tzdb_error::_Win_error) {
            _XGetLastError();
        } else if (_Info->_Err == __std_tzdb_error::_Icu_error) {
            _Xruntime_error("Internal error loading IANA database information");
        }

        decltype(tzdb::zones) _Time_zones;
        decltype(tzdb::links) _Time_zone_links;
        for (size_t _Idx = 0; _Idx < _Info->_Num_time_zones; ++_Idx) {
            const string_view _Name{_Info->_Names[_Idx]};
            if (_Info->_Links[_Idx] == nullptr) {
                _Time_zones.emplace_back(_Name);
            } else {
                const string_view _Target{_Info->_Links[_Idx]};
                _Time_zone_links.emplace_back(_Name, _Target);
            }
        }

        return {_Info->_Version, _STD move(_Time_zones), _STD move(_Time_zone_links)};
    }

    _NODISCARD inline pair<decltype(tzdb::leap_seconds), bool> _Tzdb_generate_leap_seconds(const size_t _Current_size) {
        // Returns empty vector if no new leap seconds are found.
        static constexpr leap_second _Known_leap_seconds[]{
            {sys_seconds{seconds{78796800}}, true, seconds{0}},
            {sys_seconds{seconds{94694400}}, true, seconds{1}},
            {sys_seconds{seconds{126230400}}, true, seconds{2}},
            {sys_seconds{seconds{157766400}}, true, seconds{3}},
            {sys_seconds{seconds{189302400}}, true, seconds{4}},
            {sys_seconds{seconds{220924800}}, true, seconds{5}},
            {sys_seconds{seconds{252460800}}, true, seconds{6}},
            {sys_seconds{seconds{283996800}}, true, seconds{7}},
            {sys_seconds{seconds{315532800}}, true, seconds{8}},
            {sys_seconds{seconds{362793600}}, true, seconds{9}},
            {sys_seconds{seconds{394329600}}, true, seconds{10}},
            {sys_seconds{seconds{425865600}}, true, seconds{11}},
            {sys_seconds{seconds{489024000}}, true, seconds{12}},
            {sys_seconds{seconds{567993600}}, true, seconds{13}},
            {sys_seconds{seconds{631152000}}, true, seconds{14}},
            {sys_seconds{seconds{662688000}}, true, seconds{15}},
            {sys_seconds{seconds{709948800}}, true, seconds{16}},
            {sys_seconds{seconds{741484800}}, true, seconds{17}},
            {sys_seconds{seconds{773020800}}, true, seconds{18}},
            {sys_seconds{seconds{820454400}}, true, seconds{19}},
            {sys_seconds{seconds{867715200}}, true, seconds{20}},
            {sys_seconds{seconds{915148800}}, true, seconds{21}},
            {sys_seconds{seconds{1136073600}}, true, seconds{22}},
            {sys_seconds{seconds{1230768000}}, true, seconds{23}},
            {sys_seconds{seconds{1341100800}}, true, seconds{24}},
            {sys_seconds{seconds{1435708800}}, true, seconds{25}},
            {sys_seconds{seconds{1483228800}}, true, seconds{26}},
        };

        // __std_tzdb_get_leap_seconds gets leap second (LS) data from the registry, but only if it contains more
        // LSs than we already know about. The registry only contains LSs after 2018, so we need to tell it how many of
        // *those* we already know about. The *total* number of LSs known at this point is a combination of what the
        // caller knows (_Current_size, 0 on first call) and the _Known_leap_seconds entries.
        constexpr size_t _Pre_2018_count = 27;
        const size_t _Known_post_2018_ls_size =
            (_STD max)(_Current_size, _STD size(_Known_leap_seconds)) - _Pre_2018_count;

        size_t _Reg_post_2018_ls_size; // number of post-2018 LSs found in the registry
        unique_ptr<__std_tzdb_leap_info[], _Tzdb_deleter<__std_tzdb_leap_info[]>> _Reg_ls_data{
            __std_tzdb_get_leap_seconds(_Known_post_2018_ls_size, &_Reg_post_2018_ls_size)};

        if (_Reg_post_2018_ls_size > _Known_post_2018_ls_size && !_Reg_ls_data) {
            _Xbad_alloc(); // registry has new data, but failed to allocate storage
        } else if (_Reg_post_2018_ls_size == 0 && _Reg_ls_data) {
            _XGetLastError(); // allocated storage for registry data, but failed to read
        }

        const size_t _New_size = _Pre_2018_count + _Reg_post_2018_ls_size; // total size with registry data
        decltype(tzdb::leap_seconds) _Leap_sec_info;
        bool _All_ls_positive = true;

        if (_New_size > _Current_size) {
            _Leap_sec_info.reserve(_New_size);
            _Leap_sec_info.assign(_STD cbegin(_Known_leap_seconds), _STD cend(_Known_leap_seconds));

            for (size_t _Idx = 0; _Idx < _Reg_post_2018_ls_size; ++_Idx) {
                // Leap seconds occur at _Ls._Hour:59:59. We store the next second after, so we need to add an entire
                // hour.
                const auto& _Ls = _Reg_ls_data[_Idx];
                const auto _Date =
                    static_cast<sys_days>(year_month_day{year{_Ls._Year}, month{_Ls._Month}, day{_Ls._Day}})
                    + hours{_Ls._Hour + 1};
                _Leap_sec_info.emplace_back(_Date, !_Ls._Negative, _Leap_sec_info.back()._Elapsed());
                _All_ls_positive = _All_ls_positive && !_Ls._Negative;
            }
        }

        return {_STD move(_Leap_sec_info), _All_ls_positive};
    }

    _NODISCARD inline string _Tzdb_update_version(const string_view _Version, const size_t _Num_leap_seconds) {
        string _Icu_version{_Version.substr(0, _Version.find_last_of('.'))};
        return _STD move(_Icu_version) + "." + _STD to_string(_Num_leap_seconds);
    }

    class tzdb_list {
    private:
        using _ListType = forward_list<tzdb, _Crt_allocator<tzdb>>;

    public:
        using const_iterator = _ListType::const_iterator;

        tzdb_list(const tzdb_list&) = delete;
        tzdb_list& operator=(const tzdb_list&) = delete;

        tzdb_list() {
            auto [_Icu_version, _Zones, _Links] = _Tzdb_generate_time_zones();
            auto [_Leap_sec, _All_ls_positive]  = _Tzdb_generate_leap_seconds(0);
            auto _Version                       = _Icu_version + "." + _STD to_string(_Leap_sec.size());
            _Tzdb_list.emplace_front(tzdb{
                _STD move(_Version), _STD move(_Zones), _STD move(_Links), _STD move(_Leap_sec), _All_ls_positive});
        }

        _NODISCARD const tzdb& front() const noexcept {
            _Shared_lock _Lk(_Tzdb_mutex);
            return _Tzdb_list.front();
        }

        const_iterator erase_after(const_iterator _Where) noexcept /* strengthened */ {
            _Unique_lock _Lk(_Tzdb_mutex);
            return _Tzdb_list.erase_after(_Where);
        }

        _NODISCARD const_iterator begin() const noexcept {
            _Shared_lock _Lk(_Tzdb_mutex);
            return _Tzdb_list.begin();
        }

        _NODISCARD const_iterator end() const noexcept {
            return _Tzdb_list.end(); // no lock necessary for forward_list::end()
        }

        _NODISCARD const_iterator cbegin() const noexcept {
            _Shared_lock _Lk(_Tzdb_mutex);
            return _Tzdb_list.cbegin();
        }

        _NODISCARD const_iterator cend() const noexcept {
            return _Tzdb_list.cend(); // no lock necessary for forward_list::cend()
        }

        template <class... _ArgsTy>
        void _Emplace_front(_ArgsTy&&... _Args) {
            _Unique_lock _Lk(_Tzdb_mutex);
            _Tzdb_list.emplace_front(_STD forward<_ArgsTy>(_Args)...);
        }

        const tzdb& _Reload() {
            _Unique_lock _Lk(_Tzdb_mutex);
            auto [_Leap_sec, _All_ls_positive] = _Tzdb_generate_leap_seconds(_Tzdb_list.front().leap_seconds.size());
            if (!_Leap_sec.empty()) {
                const auto& _Tzdb = _Tzdb_list.front();
                vector<time_zone> _Zones;
                _STD transform(_Tzdb.zones.begin(), _Tzdb.zones.end(), _STD back_inserter(_Zones),
                    [](const auto& _Tz) { return time_zone{_Tz.name()}; });
                vector<time_zone_link> _Links;
                _STD transform(
                    _Tzdb.links.begin(), _Tzdb.links.end(), _STD back_inserter(_Links), [](const auto& _Link) {
                        return time_zone_link{_Link.name(), _Link.target()};
                    });
                auto _Version = _Tzdb_update_version(_Tzdb.version, _Leap_sec.size());
                _Tzdb_list.emplace_front(tzdb{
                    _STD move(_Version), _STD move(_Zones), _STD move(_Links), _STD move(_Leap_sec), _All_ls_positive});
            }
            return _Tzdb_list.front();
        }

    private:
        _ListType _Tzdb_list;
        mutable _Smtx_t _Tzdb_mutex = {};

        struct _NODISCARD _Shared_lock {
            explicit _Shared_lock(_Smtx_t& _Mtx_) : _Mtx{&_Mtx_} {
                _Smtx_lock_shared(_Mtx);
            }

            _Shared_lock(const _Shared_lock&) = delete;
            _Shared_lock& operator=(const _Shared_lock&) = delete;

            ~_Shared_lock() {
                _Smtx_unlock_shared(_Mtx);
            }

            _Smtx_t* _Mtx;
        };

        struct _NODISCARD _Unique_lock {
            explicit _Unique_lock(_Smtx_t& _Mtx_) : _Mtx{&_Mtx_} {
                _Smtx_lock_exclusive(_Mtx);
            }

            _Unique_lock(const _Unique_lock&) = delete;
            _Unique_lock& operator=(const _Unique_lock&) = delete;

            ~_Unique_lock() {
                _Smtx_unlock_exclusive(_Mtx);
            }

            _Smtx_t* _Mtx;
        };
    };

    inline atomic<tzdb_list*> _Global_tzdb_list;

    _NODISCARD inline tzdb_list& get_tzdb_list() {
        auto* _Tzdb_ptr = _Global_tzdb_list.load();
        if (_Tzdb_ptr != nullptr) {
            return *_Tzdb_ptr;
        }

        auto _My_tzdb = static_cast<tzdb_list*>(__std_calloc_crt(1, sizeof(tzdb_list)));
        if (_My_tzdb == nullptr) {
            _Xruntime_error("bad allocation"); // not bad_alloc, see N4878 [time.zone.db.access]/4
        }

        _TRY_BEGIN
        _STD construct_at(_My_tzdb);
        _CATCH(const runtime_error&)
        __std_free_crt(_My_tzdb);
        _RERAISE;
        _CATCH(const exception& _Except)
#if _HAS_EXCEPTIONS
        __std_free_crt(_My_tzdb);
        _Xruntime_error(_Except.what());
#endif // _HAS_EXCEPTIONS
        _CATCH_END

        if (_Global_tzdb_list.compare_exchange_strong(_Tzdb_ptr, _My_tzdb)) {
            _Tzdb_ptr = _My_tzdb;
        } else {
            _STD destroy_at(_My_tzdb);
            __std_free_crt(_My_tzdb);
        }

        return *_Tzdb_ptr;
    }

    _NODISCARD inline const tzdb& get_tzdb() {
        return _CHRONO get_tzdb_list().front();
    }

    _NODISCARD inline const time_zone* locate_zone(string_view _Tz_name) {
        return _CHRONO get_tzdb().locate_zone(_Tz_name);
    }

    _NODISCARD inline const time_zone* current_zone() {
        return _CHRONO get_tzdb().current_zone();
    }

    inline const tzdb& reload_tzdb() {
        _TRY_BEGIN
        return _CHRONO get_tzdb_list()._Reload();
        _CATCH(const runtime_error&)
        _RERAISE;
        _CATCH(const exception& _Except)
#if _HAS_EXCEPTIONS
        _Xruntime_error(_Except.what());
#endif // _HAS_EXCEPTIONS
        _CATCH_END
    }

    _NODISCARD inline string remote_version() {
        const auto& _Tzdb                = _CHRONO get_tzdb();
        const auto& _Version             = _Tzdb.version;
        const auto [_Leap_sec, _Ignored] = _Tzdb_generate_leap_seconds(_Tzdb.leap_seconds.size());
        return _Leap_sec.empty() ? _Version : _Tzdb_update_version(_Version, _Leap_sec.size());
    }

    // [time.zone.zonedtraits]

    template <class _Ty>
    struct zoned_traits {};

    template <>
    struct zoned_traits<const time_zone*> {
        _NODISCARD static const time_zone* default_zone() {
            return _CHRONO get_tzdb().locate_zone("UTC");
        }

        _NODISCARD static const time_zone* locate_zone(string_view _Name) {
            return _CHRONO get_tzdb().locate_zone(_Name);
        }
    };

    // [time.zone.zonedtime]

    template <class _Duration, class _TimeZonePtr = const time_zone*>
    class zoned_time {
    private:
        static_assert(_Is_duration_v<_Duration>,
            "N4885 [time.zone.zonedtime.overview]/2 mandates Duration to be a specialization of chrono::duration.");

        using _Traits = zoned_traits<_TimeZonePtr>;

    public:
        using duration = common_type_t<_Duration, seconds>;

        template <class _Traits2 = _Traits, class = void_t<decltype(_Traits2::default_zone())>>
        zoned_time() : _Zone{_Traits::default_zone()} {}
        zoned_time(const zoned_time&) = default;
        zoned_time& operator=(const zoned_time&) = default;

        template <class _Traits2 = _Traits, class = void_t<decltype(_Traits2::default_zone())>>
        zoned_time(const sys_time<_Duration>& _Sys) : _Zone{_Traits::default_zone()}, _Tp{_Sys} {}

        explicit zoned_time(_TimeZonePtr _Tz) noexcept /* strengthened */ : _Zone{_STD move(_Tz)} {}

        // clang-format off
        template <class _Traits2 = _Traits, enable_if_t<is_constructible< // TRANSITION, VSO-1303556
            zoned_time, decltype(_Traits2::locate_zone(string_view{}))>::value,
            int> = 0>
        // clang-format on
        explicit zoned_time(string_view _Name) : _Zone{_Traits::locate_zone(_Name)} {}

        template <class _Duration2, enable_if_t<is_convertible_v<sys_time<_Duration2>, sys_time<_Duration>>, int> = 0>
        zoned_time(const zoned_time<_Duration2, _TimeZonePtr>& _Zt) noexcept /* strengthened */
            : _Zone{_Zt.get_time_zone()}, _Tp{_Zt.get_sys_time()} {}

        zoned_time(_TimeZonePtr _Tz, const sys_time<_Duration>& _Sys) : _Zone{_STD move(_Tz)}, _Tp{_Sys} {}

        // clang-format off
        template <class _Traits2 = _Traits, enable_if_t<is_constructible< // TRANSITION, VSO-1303556
            zoned_time, decltype(_Traits2::locate_zone(string_view{})), const sys_time<_Duration>&>::value,
            int> = 0>
        // clang-format on
        zoned_time(string_view _Name, type_identity_t<const sys_time<_Duration>&> _Sys)
            : zoned_time{_Traits::locate_zone(_Name), _Sys} {}

        template <class _Ptr = _TimeZonePtr,
            enable_if_t<
                is_convertible_v<decltype(_STD declval<_Ptr&>()->to_sys(local_time<_Duration>{})), sys_time<duration>>,
                int> = 0>
        zoned_time(_TimeZonePtr _Tz, const local_time<_Duration>& _Local)
            : _Zone{_STD move(_Tz)}, _Tp{_Zone->to_sys(_Local)} {}

        // clang-format off
        template <class _Traits2 = _Traits, enable_if_t<is_constructible< // TRANSITION, VSO-1303556
            zoned_time, decltype(_Traits2::locate_zone(string_view{})), const local_time<_Duration>&>::value,
            int> = 0>
        // clang-format on
        zoned_time(string_view _Name, type_identity_t<const local_time<_Duration>>& _Local)
            : zoned_time{_Traits::locate_zone(_Name), _Local} {}

        template <class _Ptr = _TimeZonePtr,
            enable_if_t<
                is_convertible_v<decltype(_STD declval<_Ptr&>()->to_sys(local_time<_Duration>{}, choose::earliest)),
                    sys_time<duration>>,
                int> = 0>
        zoned_time(_TimeZonePtr _Tz, const local_time<_Duration>& _Local, choose _Choose)
            : _Zone{_STD move(_Tz)}, _Tp{_Zone->to_sys(_Local, _Choose)} {}

        // clang-format off
        template <class _Traits2 = _Traits, enable_if_t<is_constructible< // TRANSITION, VSO-1303556
            zoned_time, decltype(_Traits2::locate_zone(string_view{})), const local_time<_Duration>&, choose>::value,
            int> = 0>
        // clang-format on
        zoned_time(string_view _Name, type_identity_t<const local_time<_Duration>&> _Local, choose _Choose)
            : zoned_time{_Traits::locate_zone(_Name), _Local, _Choose} {}

        template <class _Duration2, class _TimeZonePtr2,
            enable_if_t<is_convertible_v<sys_time<_Duration2>, sys_time<_Duration>>, int> = 0>
        zoned_time(_TimeZonePtr _Tz, const zoned_time<_Duration2, _TimeZonePtr2>& _Zt) noexcept /* strengthened */
            : _Zone{_STD move(_Tz)}, _Tp{_Zt.get_sys_time()} {}

        template <class _Duration2, class _TimeZonePtr2,
            enable_if_t<is_convertible_v<sys_time<_Duration2>, sys_time<_Duration>>, int> = 0>
        zoned_time(
            _TimeZonePtr _Tz, const zoned_time<_Duration2, _TimeZonePtr2>& _Zt, choose) noexcept /* strengthened */
            : zoned_time{_Tz, _Zt} {}

        // clang-format off
        template <class _Duration2, class _TimeZonePtr2,
            class _Traits2 = _Traits, enable_if_t<is_constructible< // TRANSITION, VSO-1303556
            zoned_time, decltype(_Traits2::locate_zone(string_view{})),
            const zoned_time<_Duration2, _TimeZonePtr2>&>::value,
            int> = 0>
        // clang-format on
        zoned_time(string_view _Name, const zoned_time<_Duration2, _TimeZonePtr2>& _Zt)
            : zoned_time{_Traits::locate_zone(_Name), _Zt} {}

        // clang-format off
        template <class _Duration2, class _TimeZonePtr2,
            class _Traits2 = _Traits, enable_if_t<is_constructible< // TRANSITION, VSO-1303556
            zoned_time, decltype(_Traits2::locate_zone(string_view{})),
            const zoned_time<_Duration2, _TimeZonePtr2>&, choose>::value,
            int> = 0>
        // clang-format on
        zoned_time(string_view _Name, const zoned_time<_Duration2, _TimeZonePtr2>& _Zt, choose _Choose)
            : zoned_time{_Traits::locate_zone(_Name), _Zt, _Choose} {}

        zoned_time& operator=(const sys_time<_Duration>& _Sys) noexcept /* strengthened */ {
            _Tp = _Sys;
            return *this;
        }

        zoned_time& operator=(const local_time<_Duration>& _Local) {
            _Tp = _Zone->to_sys(_Local);
            return *this;
        }

        operator sys_time<duration>() const noexcept /* strengthened */ {
            return get_sys_time();
        }

        explicit operator local_time<duration>() const {
            return get_local_time();
        }

        _NODISCARD _TimeZonePtr get_time_zone() const noexcept /* strengthened */ {
            return _Zone;
        }

        _NODISCARD local_time<duration> get_local_time() const {
            return _Zone->to_local(_Tp);
        }

        _NODISCARD sys_time<duration> get_sys_time() const noexcept /* strengthened */ {
            return _Tp;
        }

        _NODISCARD sys_info get_info() const {
            return _Zone->get_info(_Tp);
        }

    private:
        _TimeZonePtr _Zone;
        sys_time<duration> _Tp{};
    };

    zoned_time()->zoned_time<seconds>;

    template <class _Duration>
    zoned_time(sys_time<_Duration>) -> zoned_time<common_type_t<_Duration, seconds>>;

    template <class _TimeZonePtrOrName>
    using _Time_zone_representation = conditional_t<is_convertible_v<_TimeZonePtrOrName, string_view>, const time_zone*,
        remove_cvref_t<_TimeZonePtrOrName>>;

    template <class _TimeZonePtrOrName>
    zoned_time(_TimeZonePtrOrName&&) -> zoned_time<seconds, _Time_zone_representation<_TimeZonePtrOrName>>;

    template <class _TimeZonePtrOrName, class _Duration>
    zoned_time(_TimeZonePtrOrName&&, sys_time<_Duration>)
        -> zoned_time<common_type_t<_Duration, seconds>, _Time_zone_representation<_TimeZonePtrOrName>>;

    template <class _TimeZonePtrOrName, class _Duration>
    zoned_time(_TimeZonePtrOrName&&, local_time<_Duration>, choose = choose::earliest)
        -> zoned_time<common_type_t<_Duration, seconds>, _Time_zone_representation<_TimeZonePtrOrName>>;

    template <class _Duration, class _TimeZonePtrOrName, class _TimeZonePtr2>
    zoned_time(_TimeZonePtrOrName&&, zoned_time<_Duration, _TimeZonePtr2>, choose = choose::earliest)
        -> zoned_time<common_type_t<_Duration, seconds>, _Time_zone_representation<_TimeZonePtrOrName>>;

    using zoned_seconds = zoned_time<seconds>;

    template <class _Duration1, class _Duration2, class _TimeZonePtr>
    _NODISCARD bool operator==(
        const zoned_time<_Duration1, _TimeZonePtr>& _Left, const zoned_time<_Duration2, _TimeZonePtr>& _Right) {
        return _Left.get_time_zone() == _Right.get_time_zone() && _Left.get_sys_time() == _Right.get_sys_time();
    }

    // [time.clock.utc]

    class utc_clock;
    template <class _Duration>
    using utc_time    = time_point<utc_clock, _Duration>;
    using utc_seconds = utc_time<seconds>;

    struct leap_second_info {
        bool is_leap_second;
        seconds elapsed;
    };

    template <class _Duration>
    _NODISCARD leap_second_info get_leap_second_info(const utc_time<_Duration>& _Time) {
        const utc_seconds _Time_floor = _CHRONO floor<seconds>(_Time);
        const auto& _Tzdb             = _CHRONO get_tzdb();
        const auto& _Ls_vector        = _Tzdb.leap_seconds;

        // Find first leap second after _Time.
        vector<leap_second>::const_iterator _It;
        if (_Tzdb._All_ls_positive) {
            // Where "target_ls" is the next leap second at or after _Time, _It either points to:
            //    (1) The 2nd leap second after _Time if _Time_floor is in the range [target_ls - _Elapsed() - 1,
            //    target_ls), or
            //    (2) The leap second just after _Time otherwise.
            // Note that we can always use prev(_It) to determine whether _Time is *during* a leap second insertion,
            // since that falls under case (2) above. However, when we fall under case (1), we need to execute an
            // additional decrement to get the correct elapsed offset. For example, if leap seconds are inserted at
            // seconds {100, 200, 300, 400}, we have:
            //
            //  UTC   sys   *_It
            //   99    99    100
            //  100     X    200
            //  101   100    200
            //  102   101    200
            //  ...
            //  199   198    200
            //  200   199    300^
            //  201     X    300
            //  202   200    300
            //  ...
            //  299   297    300
            //  300   298    400^
            //  301   299    400^
            //  302     X    400
            //  303   300    400
            //
            //  ^_It points to 2nd leap second

            _It = _STD upper_bound(_Ls_vector.begin(), _Ls_vector.end(), sys_seconds{_Time_floor.time_since_epoch()});
        } else {
            seconds _Prev_elapsed{0};
            for (_It = _Ls_vector.begin(); _It != _Ls_vector.end(); ++_It) {
                // UTC time when leap second insertion begins. In all cases, _It->date() + _It->_Elapsed() is the *end*
                // of the insertion. For a negative leap that's also the beginning, but for a positive one, insertion
                // begins 1 second earlier.
                const utc_seconds _This_ls_begin{
                    _It->date().time_since_epoch() + (_It->_Positive() ? _Prev_elapsed : _It->_Elapsed())};
                if (_This_ls_begin > _Time_floor) {
                    break;
                }
                _Prev_elapsed = _It->_Elapsed();
            }
        }

        if (_It == _Ls_vector.begin()) {
            return {false, seconds{0}};
        } else {
            // Convert to the last leap second before or equal to _Time.
            const auto& _Last_leap = *--_It;
            const utc_seconds _Utc_leap_second{_Last_leap.date().time_since_epoch() + _It->_Elapsed() - seconds{1}};
#ifdef __cpp_lib_concepts
            const auto _Leap_cmp = _Utc_leap_second <=> _Time_floor;
#else // ^^^ __cpp_lib_concepts / TRANSITION, GH-395 workaround vvv
            const auto _Leap_cmp = _Utc_leap_second > _Time_floor ? strong_ordering::greater
                                 : _Utc_leap_second == _Time_floor ? strong_ordering::equal
                                                                   : strong_ordering::less;
#endif // ^^^ workaround
            if (_Tzdb._All_ls_positive && _STD is_gt(_Leap_cmp)) { // Case (1)
                --_It;
            }
            return {_Last_leap._Positive() && _STD is_eq(_Leap_cmp), _It->_Elapsed()};
        }
    }

    class utc_clock {
    public:
        using rep                       = system_clock::rep;
        using period                    = system_clock::period;
        using duration                  = _CHRONO duration<rep, period>;
        using time_point                = _CHRONO time_point<utc_clock>;
        static constexpr bool is_steady = system_clock::is_steady;

        _NODISCARD static time_point now() {
            return from_sys(system_clock::now());
        }

        template <class _Duration>
        _NODISCARD static sys_time<common_type_t<_Duration, seconds>> to_sys(const utc_time<_Duration>& _Utc_time) {
            using _CommonType = common_type_t<_Duration, seconds>;
            const auto _Lsi{_CHRONO get_leap_second_info(_Utc_time)};
            _CommonType _Ticks;
            if (_Lsi.is_leap_second) {
                const auto _Leap_sec_minus_one = _CHRONO floor<seconds>(_Utc_time.time_since_epoch()) - _Lsi.elapsed;
                if constexpr (is_integral_v<typename _Duration::rep>) {
                    constexpr auto _Delta{seconds{1} - _CommonType{1}};
                    _Ticks = _Leap_sec_minus_one + _Delta;
                } else {
                    const auto _Leap_sec_begin = _CHRONO ceil<_CommonType>(_Leap_sec_minus_one + seconds{1});
                    _Ticks = _CommonType{_STD nextafter(_Leap_sec_begin.count(), typename _CommonType::rep{0})};
                }
            } else {
                _Ticks = _Utc_time.time_since_epoch() - _Lsi.elapsed;
            }
            return sys_time<_CommonType>{_Ticks};
        }

        template <class _Duration>
        _NODISCARD static utc_time<common_type_t<_Duration, seconds>> from_sys(const sys_time<_Duration>& _Sys_time) {
            const auto& _Tzdb      = _CHRONO get_tzdb();
            const auto& _Ls_vector = _Tzdb.leap_seconds;
            auto _It = _STD upper_bound(_Ls_vector.begin(), _Ls_vector.end(), _CHRONO floor<seconds>(_Sys_time));
            const auto _Offset = _It == _Ls_vector.begin() ? seconds{0} : (--_It)->_Elapsed();
            return utc_time<common_type_t<_Duration, seconds>>{_Sys_time.time_since_epoch() + _Offset};
        }
    };

    // [time.clock.tai]

    class tai_clock;

    template <class _Duration>
    using tai_time    = time_point<tai_clock, _Duration>;
    using tai_seconds = tai_time<seconds>;

    class tai_clock {
    public:
        using rep                       = system_clock::rep;
        using period                    = system_clock::period;
        using duration                  = _CHRONO duration<rep, period>;
        using time_point                = _CHRONO time_point<tai_clock>;
        static constexpr bool is_steady = system_clock::is_steady;

        static constexpr seconds _Tai_epoch_adjust{378691210};

        _NODISCARD static time_point now() {
            return from_utc(utc_clock::now());
        }

        template <class _Duration>
        _NODISCARD static utc_time<common_type_t<_Duration, seconds>> to_utc(
            const tai_time<_Duration>& _Time) noexcept {
            return utc_time<common_type_t<_Duration, seconds>>{_Time.time_since_epoch()} - _Tai_epoch_adjust;
        }

        template <class _Duration>
        _NODISCARD static tai_time<common_type_t<_Duration, seconds>> from_utc(
            const utc_time<_Duration>& _Time) noexcept {
            return tai_time<common_type_t<_Duration, seconds>>{_Time.time_since_epoch()} + _Tai_epoch_adjust;
        }
    };

    // [time.clock.gps]

    class gps_clock;

    template <class _Duration>
    using gps_time    = time_point<gps_clock, _Duration>;
    using gps_seconds = gps_time<seconds>;

    class gps_clock {
    public:
        using rep                       = system_clock::rep;
        using period                    = system_clock::period;
        using duration                  = _CHRONO duration<rep, period>;
        using time_point                = _CHRONO time_point<gps_clock>;
        static constexpr bool is_steady = system_clock::is_steady;

        static constexpr seconds _Gps_epoch_adjust{-315964809};

        _NODISCARD static time_point now() {
            return from_utc(utc_clock::now());
        }

        template <class _Duration>
        _NODISCARD static utc_time<common_type_t<_Duration, seconds>> to_utc(
            const gps_time<_Duration>& _Time) noexcept {
            return utc_time<common_type_t<_Duration, seconds>>{_Time.time_since_epoch()} - _Gps_epoch_adjust;
        }

        template <class _Duration>
        _NODISCARD static gps_time<common_type_t<_Duration, seconds>> from_utc(
            const utc_time<_Duration>& _Time) noexcept {
            return gps_time<common_type_t<_Duration, seconds>>{_Time.time_since_epoch()} + _Gps_epoch_adjust;
        }
    };
} // namespace chrono

namespace filesystem {
    struct _File_time_clock;
} // namespace filesystem

namespace chrono {
    using file_clock = filesystem::_File_time_clock;

    template <class _Duration>
    using file_time = time_point<file_clock, _Duration>;
} // namespace chrono
#endif // ^^^ _HAS_CXX20

#if _HAS_CXX17
namespace filesystem {
    inline constexpr long long __std_fs_file_time_epoch_adjustment = 0x19DB1DED53E8000LL; // TRANSITION, ABI

    struct _File_time_clock { // Implementation of trivial-clock
        using rep                       = long long;
        using period                    = chrono::system_clock::period;
        using duration                  = chrono::duration<rep, period>;
        using time_point                = chrono::time_point<_File_time_clock>;
        static constexpr bool is_steady = false;

        _NODISCARD static time_point now() noexcept { // get current time; undo epoch adjustment
            return time_point(duration(_Xtime_get_ticks() + __std_fs_file_time_epoch_adjustment)); // TRANSITION, ABI
        }

#if _HAS_CXX20
        // Assumes that FILETIME counts leap seconds only after the first 27 (i.e., after 1 January 2017), even though
        // systems can opt out of this behavior.
        static constexpr chrono::seconds _Skipped_filetime_leap_seconds{27};
        static constexpr chrono::sys_days _Cutoff{
            chrono::year_month_day{chrono::year{2017}, chrono::January, chrono::day{1}}};

        template <class _Duration>
        _NODISCARD static chrono::utc_time<common_type_t<_Duration, chrono::seconds>> to_utc(
            const chrono::file_time<_Duration>& _File_time) {
            using namespace chrono;
            using _CommonType = common_type_t<_Duration, seconds>;
            const auto _Ticks = _File_time.time_since_epoch()
                              - _CHRONO duration_cast<seconds>(duration{__std_fs_file_time_epoch_adjustment});

            if (_Ticks < _Cutoff.time_since_epoch()) {
                return utc_clock::from_sys(sys_time<_CommonType>{_Ticks});
            } else {
                return utc_time<_CommonType>{_Ticks + _Skipped_filetime_leap_seconds};
            }
        }

        template <class _Duration>
        _NODISCARD static chrono::file_time<common_type_t<_Duration, chrono::seconds>> from_utc(
            const chrono::utc_time<_Duration>& _Utc_time) {
            using namespace chrono;
            file_time<common_type_t<_Duration, seconds>> _File_time{
                _CHRONO duration_cast<seconds>(duration{__std_fs_file_time_epoch_adjustment})};

            if (_Utc_time < utc_seconds{_Cutoff.time_since_epoch()} + _Skipped_filetime_leap_seconds) {
                _File_time += utc_clock::to_sys(_Utc_time).time_since_epoch();
            } else {
                _File_time += _Utc_time.time_since_epoch() - _Skipped_filetime_leap_seconds;
            }

            return _File_time;
        }
#endif // ^^^ _HAS_CXX20
    };
} // namespace filesystem
#endif // ^^^ _HAS_CXX17

#if _HAS_CXX20
namespace chrono {
    // [time.clock.conv]

    template <class _DestClock, class _SourceClock>
    struct clock_time_conversion {};

    // [time.clock.cast.id]

    template <class _Clock>
    struct clock_time_conversion<_Clock, _Clock> {
        template <class _Duration>
        _NODISCARD time_point<_Clock, _Duration> operator()(const time_point<_Clock, _Duration>& _Time) const
            noexcept(is_arithmetic_v<typename _Duration::rep>) /* strengthened */ {
            return _Time;
        }
    };

    template <>
    struct clock_time_conversion<system_clock, system_clock> {
        template <class _Duration>
        _NODISCARD sys_time<_Duration> operator()(const sys_time<_Duration>& _Time) const
            noexcept(is_arithmetic_v<typename _Duration::rep>) /* strengthened */ {
            return _Time;
        }
    };

    template <>
    struct clock_time_conversion<utc_clock, utc_clock> {
        template <class _Duration>
        _NODISCARD utc_time<_Duration> operator()(const utc_time<_Duration>& _Time) const
            noexcept(is_arithmetic_v<typename _Duration::rep>) /* strengthened */ {
            return _Time;
        }
    };

    // [time.clock.cast.sys.utc]

    template <>
    struct clock_time_conversion<utc_clock, system_clock> {
        template <class _Duration>
        _NODISCARD utc_time<common_type_t<_Duration, seconds>> operator()(const sys_time<_Duration>& _Sys_time) const {
            return utc_clock::from_sys(_Sys_time);
        }
    };

    template <>
    struct clock_time_conversion<system_clock, utc_clock> {
        template <class _Duration>
        _NODISCARD sys_time<common_type_t<_Duration, seconds>> operator()(const utc_time<_Duration>& _Utc_time) const {
            return utc_clock::to_sys(_Utc_time);
        }
    };

    // [time.clock.cast.sys]

    template <class _TimePoint, class _Clock>
    inline constexpr bool _Is_time_point_for_clock = false;

    template <class _Clock, class _Duration>
    inline constexpr bool _Is_time_point_for_clock<time_point<_Clock, _Duration>, _Clock> = true;

    template <class _SourceClock>
    struct clock_time_conversion<system_clock, _SourceClock> {
        template <class _Duration, class _SourceClock2 = _SourceClock,
            class =
                void_t<decltype(_SourceClock2::to_sys(_STD declval<const time_point<_SourceClock2, _Duration>&>()))>>
        _NODISCARD auto operator()(const time_point<_SourceClock, _Duration>& _Time) const
            noexcept(noexcept(_SourceClock::to_sys(_Time))) /* strengthened */ {
            static_assert(_Is_time_point_for_clock<decltype(_SourceClock::to_sys(_Time)), system_clock>,
                "N4885 [time.clock.cast.sys]/2: Mandates: SourceClock::to_sys(t) returns a sys_time<Duration>");
            return _SourceClock::to_sys(_Time);
        }
    };

    template <class _DestClock>
    struct clock_time_conversion<_DestClock, system_clock> {
        template <class _Duration, class _DestClock2 = _DestClock,
            class = void_t<decltype(_DestClock2::from_sys(_STD declval<const sys_time<_Duration>&>()))>>
        _NODISCARD auto operator()(const sys_time<_Duration>& _Time) const
            noexcept(noexcept(_DestClock::from_sys(_Time))) /* strengthened */ {
            static_assert(_Is_time_point_for_clock<decltype(_DestClock::from_sys(_Time)), _DestClock>,
                "N4885 [time.clock.cast.sys]/5: Mandates: DestClock::from_sys(t) returns a "
                "time_point<DestClock, Duration>");
            return _DestClock::from_sys(_Time);
        }
    };

    // [time.clock.cast.utc]

    template <class _SourceClock>
    struct clock_time_conversion<utc_clock, _SourceClock> {
        template <class _Duration, class _SourceClock2 = _SourceClock,
            class =
                void_t<decltype(_SourceClock2::to_utc(_STD declval<const time_point<_SourceClock2, _Duration>&>()))>>
        _NODISCARD auto operator()(const time_point<_SourceClock, _Duration>& _Time) const
            noexcept(noexcept(_SourceClock::to_utc(_Time))) /* strengthened */ {
            static_assert(_Is_time_point_for_clock<decltype(_SourceClock::to_utc(_Time)), utc_clock>,
                "N4885 [time.clock.cast.utc]/2: Mandates: SourceClock::to_utc(t) returns a utc_time<Duration>");
            return _SourceClock::to_utc(_Time);
        }
    };

    template <class _DestClock>
    struct clock_time_conversion<_DestClock, utc_clock> {
        template <class _Duration, class _DestClock2 = _DestClock,
            class = void_t<decltype(_DestClock2::from_utc(_STD declval<const utc_time<_Duration>&>()))>>
        _NODISCARD auto operator()(const utc_time<_Duration>& _Time) const
            noexcept(noexcept(_DestClock::from_utc(_Time))) /* strengthened */ {
            static_assert(_Is_time_point_for_clock<decltype(_DestClock::from_utc(_Time)), _DestClock>,
                "N4885 [time.clock.cast.utc]/5: Mandates: DestClock::from_utc(t) returns a "
                "time_point<DestClock, Duration>");
            return _DestClock::from_utc(_Time);
        }
    };

    // [time.clock.cast.fn]

    enum class _Clock_cast_strategy {
        _Direct,
        _Via_sys,
        _Via_utc,
        _Via_utc_from_sys,
        _Via_sys_from_utc,
        _Two_step_ambiguous,
        _Three_step_ambiguous,
        _None,
    };

    template <class _Conv1, class _Conv2, class _Tp, class = void>
    inline constexpr bool _Has_two_step_conversion = false;

    template <class _Conv1, class _Conv2, class _Tp>
    inline constexpr bool
        _Has_two_step_conversion<_Conv1, _Conv2, _Tp, void_t<decltype(_Conv1{}(_Conv2{}(_STD declval<_Tp>())))>> = true;

    template <class _Conv1, class _Conv2, class _Conv3, class _Tp, class = void>
    inline constexpr bool _Has_three_step_conversion = false;

    template <class _Conv1, class _Conv2, class _Conv3, class _Tp>
    inline constexpr bool _Has_three_step_conversion<_Conv1, _Conv2, _Conv3, _Tp,
        void_t<decltype(_Conv1{}(_Conv2{}(_Conv3{}(_STD declval<_Tp>()))))>> = true;

    template <class _DestClock, class _SourceClock, class _Duration>
    _NODISCARD _CONSTEVAL _Clock_cast_strategy _Choose_clock_cast() noexcept {
        using _Tp = const time_point<_SourceClock, _Duration>&;

        if constexpr (is_invocable_v<clock_time_conversion<_DestClock, _SourceClock>, _Tp>) {
            return _Clock_cast_strategy::_Direct;
        } else {
            constexpr bool _Has_sys = _Has_two_step_conversion< //
                clock_time_conversion<_DestClock, system_clock>, //
                clock_time_conversion<system_clock, _SourceClock>, _Tp>;

            constexpr bool _Has_utc = _Has_two_step_conversion< //
                clock_time_conversion<_DestClock, utc_clock>, //
                clock_time_conversion<utc_clock, _SourceClock>, _Tp>;

            if constexpr (_Has_sys && _Has_utc) {
                return _Clock_cast_strategy::_Two_step_ambiguous;
            } else if constexpr (_Has_sys) {
                return _Clock_cast_strategy::_Via_sys;
            } else if constexpr (_Has_utc) {
                return _Clock_cast_strategy::_Via_utc;
            } else {
                constexpr bool _Has_utc_from_sys = _Has_three_step_conversion< //
                    clock_time_conversion<_DestClock, utc_clock>, //
                    clock_time_conversion<utc_clock, system_clock>, //
                    clock_time_conversion<system_clock, _SourceClock>, _Tp>;

                constexpr bool _Has_sys_from_utc = _Has_three_step_conversion< //
                    clock_time_conversion<_DestClock, system_clock>, //
                    clock_time_conversion<system_clock, utc_clock>, //
                    clock_time_conversion<utc_clock, _SourceClock>, _Tp>;

                if constexpr (_Has_utc_from_sys && _Has_sys_from_utc) {
                    return _Clock_cast_strategy::_Three_step_ambiguous;
                } else if constexpr (_Has_utc_from_sys) {
                    return _Clock_cast_strategy::_Via_utc_from_sys;
                } else if constexpr (_Has_sys_from_utc) {
                    return _Clock_cast_strategy::_Via_sys_from_utc;
                } else {
                    return _Clock_cast_strategy::_None;
                }
            }
        }
    }

    template <class _DestClock, class _SourceClock, class _Duration>
    inline constexpr auto _Clock_cast_choice = _Choose_clock_cast<_DestClock, _SourceClock, _Duration>();

    template <class _DestClock, class _SourceClock, class _Duration,
        enable_if_t<_Clock_cast_choice<_DestClock, _SourceClock, _Duration> != _Clock_cast_strategy::_None, int> = 0>
    _NODISCARD auto clock_cast(const time_point<_SourceClock, _Duration>& _Time) {
        constexpr auto _Strat = _Clock_cast_choice<_DestClock, _SourceClock, _Duration>;

        if constexpr (_Strat == _Clock_cast_strategy::_Direct) {
            return clock_time_conversion<_DestClock, _SourceClock>{}(_Time);
        } else if constexpr (_Strat == _Clock_cast_strategy::_Via_sys) {
            return clock_time_conversion<_DestClock, system_clock>{}(
                clock_time_conversion<system_clock, _SourceClock>{}(_Time));
        } else if constexpr (_Strat == _Clock_cast_strategy::_Via_utc) {
            return clock_time_conversion<_DestClock, utc_clock>{}(
                clock_time_conversion<utc_clock, _SourceClock>{}(_Time));
        } else if constexpr (_Strat == _Clock_cast_strategy::_Via_utc_from_sys) {
            return clock_time_conversion<_DestClock, utc_clock>{}( //
                clock_time_conversion<utc_clock, system_clock>{}(
                    clock_time_conversion<system_clock, _SourceClock>{}(_Time)));
        } else if constexpr (_Strat == _Clock_cast_strategy::_Via_sys_from_utc) {
            return clock_time_conversion<_DestClock, system_clock>{}( //
                clock_time_conversion<system_clock, utc_clock>{}(
                    clock_time_conversion<utc_clock, _SourceClock>{}(_Time)));
        } else if constexpr (_Strat == _Clock_cast_strategy::_Two_step_ambiguous) {
            static_assert(_Always_false<_Duration>,
                "A two-step clock time conversion is required to be unique, "
                "either through utc_clock or system_clock, but not both (N4878 [time.clock.cast.fn]/2).");
        } else if constexpr (_Strat == _Clock_cast_strategy::_Three_step_ambiguous) {
            static_assert(_Always_false<_Duration>,
                "A three-step clock time conversion is required to be unique, "
                "either utc-to-system or system-to-utc, but not both (N4878 [time.clock.cast.fn]/2).");
        } else {
            static_assert(_Always_false<_Duration>, "should be unreachable");
        }
    }

    // [time.parse]
    struct _Time_parse_fields {
        using _SubsecondType = duration<int64_t, atto>;

        // These are the primary fields, used to set the chrono type being parsed.
        optional<int64_t> _Subsecond;
        optional<int> _Second;
        optional<int> _Minute;
        optional<int> _Hour_24;
        optional<int> _Weekday; // 0-based, starts Sunday
        optional<int> _Day; // 1-based
        optional<int> _Month; // 1-based
        optional<int> _Day_of_year; // 1-based
        optional<int> _Two_dig_year;
        optional<int> _Century;
        optional<int> _Utc_offset; // in minutes
        optional<string> _Tz_name;

        // These are the secondary fields, used to store parsed data. They must be converted to primary fields and
        // checked for consistency.
        optional<int> _Hour_12;
        optional<int> _Ampm;
        optional<int> _Iso_year;
        optional<int> _Two_dig_iso_year;
        optional<int> _Iso_week; // 1-based
        optional<int> _Week_u; // week number, W01 begins on first Sunday
        optional<int> _Week_w; // week number, W01 begins on first Monday

        enum _FieldFlags : unsigned int {
            _F_sec   = 0x01,
            _F_min   = 0x02,
            _F_hr    = 0x04,
            _F_day   = 0x08,
            _F_wkday = 0x10,
            _F_mon   = 0x20,
            _F_doy   = 0x40,
            _F_year  = 0x80,
        };

        _NODISCARD unsigned int _Used_fields() const {
            unsigned int _Ret{0};
            if (_Second || _Subsecond) {
                _Ret |= _F_sec;
            }
            if (_Minute) {
                _Ret |= _F_min;
            }
            if (_Hour_24) {
                _Ret |= _F_hr;
            }
            if (_Day) {
                _Ret |= _F_day;
            }
            if (_Weekday) {
                _Ret |= _F_wkday;
            }
            if (_Month) {
                _Ret |= _F_mon;
            }
            if (_Day_of_year) {
                _Ret |= _F_doy;
            }
            if (_Two_dig_year && _Century) {
                _Ret |= _F_year;
            }
            return _Ret;
        }

        _NODISCARD static bool _Test_bits(
            const unsigned int _Bits, const unsigned int _Must_set, const unsigned int _Optional = 0) {
            return (_Bits & ~_Optional) == _Must_set;
        }


        template <class _Ty>
        static constexpr _Ty _Invalid_time_field{numeric_limits<_Ty>::lowest()};

        static void _Initialize_time_point(tm& _Tp) {
            _Tp.tm_sec  = _Invalid_time_field<int>;
            _Tp.tm_min  = _Invalid_time_field<int>;
            _Tp.tm_hour = _Invalid_time_field<int>;
            _Tp.tm_wday = _Invalid_time_field<int>;
            _Tp.tm_mday = _Invalid_time_field<int>;
            _Tp.tm_mon  = _Invalid_time_field<int>;
            _Tp.tm_yday = _Invalid_time_field<int>;
            _Tp.tm_year = _Invalid_time_field<int>;
        }

        template <class _Ty>
        _NODISCARD static bool _Update(optional<_Ty>& _Val, const _Ty& _New) {
            // Update a field. Ignores invalid values. If _Val already has a value, returns true or false according to
            // whether the new value matches the current one or not, so that inconsistencies can be detected.

            if constexpr (!is_same_v<_Ty, string>) {
                if (_New == _Invalid_time_field<_Ty>) {
                    return true;
                }
            }

            if (!_Val.has_value()) {
                _Val = _New;
                return true;
            } else {
                return _STD exchange(_Val, _New) == _New;
            }
        }

        _NODISCARD static pair<int, int> _Decompose_year(const int _Year) {
            int _Two_d_year = _Year % 100;
            if (_Two_d_year < 0) {
                _Two_d_year += 100;
            }
            return {_Year - _Two_d_year, _Two_d_year};
        }

        _NODISCARD bool _Update_if_valid(const tm& _Tp, const bool _Full_year) {
            bool _No_err{true};
            if (_Tp.tm_hour != _Invalid_time_field<int>) {
                _No_err = _No_err && _Update(_Hour_24, _Tp.tm_hour);
                _No_err = _No_err && _Update(_Ampm, _Hour_24 >= 12 ? 1 : 0);
                _No_err = _No_err && _Update(_Hour_12, _CHRONO make12(hours{*_Hour_24}).count());
            }

            _No_err = _No_err && _Update(_Minute, _Tp.tm_min);
            _No_err = _No_err && _Update(_Second, _Tp.tm_sec);
            _No_err = _No_err && _Update(_Day, _Tp.tm_mday);
            _No_err = _No_err && _Update(_Weekday, _Tp.tm_wday);

            if (_Tp.tm_mon != _Invalid_time_field<int>) {
                _No_err = _No_err && _Update(_Month, _Tp.tm_mon + 1);
            }

            if (_Tp.tm_yday != _Invalid_time_field<int>) {
                _No_err = _No_err && _Update(_Day_of_year, _Tp.tm_yday + 1);
            }

            if (_Tp.tm_year != _Invalid_time_field<int>) {
                // Sometimes we expect only the last two digits.
                const auto _Year_parts = _Decompose_year(_Tp.tm_year + 1900);
                _No_err                = _No_err && _Update(_Two_dig_year, _Year_parts.second);
                if (_Full_year) {
                    _No_err = _No_err && _Update(_Century, _Year_parts.first);
                }
            }

            return _No_err;
        }

        _NODISCARD bool _Yday_to_month_day(const int _Yday, const int _Year) {
            // A day-of-year that's February 28 or earlier, by itself, is a valid month_day. Any later day is
            // ambiguous without a year.
            if (_Out_of_range(_Day_of_year, 1, _Two_dig_year || _Iso_year ? _Days_in_year(_Year) : 59)) {
                return false;
            }

            if (_Day_of_year <= 31) {
                return _Update(_Day, _Yday) && _Update(_Month, 1);
            }

            const int _Feb_end{_Is_leap(_Year) ? 60 : 59};
            if (_Day_of_year <= _Feb_end) {
                return _Update(_Day, _Yday - 31) && _Update(_Month, 2);
            }

            // Shift day-of-year so that 1 == March 1. This is the same as year_month_day::_Civil_from_days, except
            // _Day_of_year --> _Shifted_yday-1 and _Mp --> _Month - 3.
            const int _Shifted_yday{*_Day_of_year - _Feb_end};
            return _Update(_Month, (535 * _Shifted_yday + 48950) >> 14)
                && _Update(_Day, _Shifted_yday - ((979 * *_Month - 2918) >> 5));
        }

        static constexpr int _Era_begin_wday{3}; // Wednesday

        _NODISCARD static constexpr int _Jan1_weekday(int _Year) {
            --_Year;
            const int _Era = (_Year >= 0 ? _Year : _Year - 399) / 400;
            const int _Yoe = _Year - _Era * 400;
            // Jan. 1 is always day 306 of the shifted [Mar, ..., Dec, Jan, Feb] year.
            const int _Doe = ((1461 * _Yoe) >> 2) - _Yoe / 100 + 306;
            return (_Doe + _Era_begin_wday) % 7;
        }

        _NODISCARD static constexpr int _Iso8601_weeks(int _Year) {
            const int _P_y = (_Year + _Year / 4 - _Year / 100 + _Year / 400) % 7;
            --_Year;
            const int _P_ym1 = (_Year + _Year / 4 - _Year / 100 + _Year / 400) % 7;
            return 52 + (_P_y == 4 || _P_ym1 == 3);
        }

        _NODISCARD static constexpr int _Iso8601_week(const int _Day_of_year, const int _Weekday, const int _Year) {
            // Jan. 4 is always week 1; rollover to next week always happens on Monday.
            const auto _Week{(_Day_of_year + 9 - _Prev_weekday(_Weekday, 1)) / 7};
            if (_Week < 1) {
                return _Iso8601_weeks(_Year - 1);
            } else if (_Week > _Iso8601_weeks(_Year)) {
                return 1;
            } else {
                return _Week;
            }
        }

        _NODISCARD static constexpr bool _Is_leap(const int _Year) {
            return _Year % 4 == 0 && (_Year % 100 != 0 || _Year % 400 == 0);
        }

        _NODISCARD static constexpr int _Days_in_year(const int _Year) {
            return _Is_leap(_Year) ? 366 : 365;
        }

        _NODISCARD static constexpr int _Next_weekday(const int _Wday, const int _Shift) {
            // 0 <= _Shift <= 6
            int _Result = _Wday + _Shift;
            if (_Result >= 7) {
                _Result -= 7;
            }
            return _Result;
        }

        _NODISCARD static constexpr int _Prev_weekday(const int _Wday, const int _Shift) {
            // 0 <= _Shift <= 6
            return (_Wday >= _Shift ? 0 : 7) + (_Wday - _Shift);
        }

        _NODISCARD bool _Calculate_ymd_from_week_date(const int _Starting_wday, const int _Week, const int _Year) {
            // (a) Calculate day-of-year of first _Starting_wday in January.
            // (b) Shift *_Weekday so that it's relative to _Starting_wday.
            // (c) Offset to desired week.
            const int _Jan1_wday = _Jan1_weekday(_Year);
            const int _Yday      = 1 + _Prev_weekday(_Starting_wday, _Jan1_wday) // (a)
                            + _Prev_weekday(*_Weekday, _Starting_wday) // (b)
                            + 7 * (_Week - 1); // (c)
            return _Update(_Day_of_year, _Yday) && !_Out_of_range(_Day_of_year, 1, _Days_in_year(_Year))
                && _Yday_to_month_day(*_Day_of_year, _Year);
        }

        _NODISCARD bool _Calculate_ymd() {
            bool _No_err = true;
            // Flags to indicate if a field should be checked for consistency with other data. Set to false when the
            // field is used to calculate the date, as it's necessarily self-consistent in that case (barring a bug).
            bool _Check_u    = true;
            bool _Check_w    = true;
            bool _Check_iso  = true;
            bool _Check_wday = true;

            if (_Day_of_year && _Out_of_range(_Day_of_year, 1, 366)) {
                return false;
            }

            bool _Have_year = false;
            int _Year{0};
            if (_Two_dig_year) {
                _Year      = *_Century + *_Two_dig_year;
                _Have_year = true;
                if (_Day_of_year) {
                    _No_err = _No_err && _Yday_to_month_day(*_Day_of_year, _Year);
                } else if (_Week_u || _Week_w) {
                    _Check_wday = false;
                    if (_Week_u) {
                        _Check_u = false;
                        _No_err  = _No_err && _Calculate_ymd_from_week_date(0 /*Sunday*/, *_Week_u, _Year);
                    }

                    if (_Week_w) {
                        _Check_w = false;
                        _No_err  = _No_err && _Calculate_ymd_from_week_date(1 /*Monday*/, *_Week_w, _Year);
                    }
                }
            }

            if (_Iso_year) {
                // ISO weeks begin on Monday. W01 always contains January 4. There is no W00, so the beginning of
                // January can be in W52 or W53 of the previous year. Likewise, the end of December can occur at the
                // beginning of W01 of the following year, depending on where in the week Jan. 4 falls.
                _Check_wday = false;
                _Check_iso  = false;
                _Year       = *_Iso_year;
                _Have_year  = true;

                // Shift weekdays to Monday-based. Jan. 4 is the anchor of week 1, so calculate where the parsed weekday
                // is relative to that point.
                const int _Jan4_wday        = _Next_weekday(_Jan1_weekday(_Year), 3 - 1);
                const int _Offset_from_jan4 = _Prev_weekday(*_Weekday, 1) - _Jan4_wday;
                int _Trial_yday             = 4 + 7 * (*_Iso_week - 1) + _Offset_from_jan4;
                const int _Ref_num_days     = _Trial_yday < 1 ? _Days_in_year(_Year - 1) : _Days_in_year(_Year);
                if (_Trial_yday < 1) {
                    _Trial_yday += _Ref_num_days;
                    --_Year;
                } else if (_Trial_yday > _Ref_num_days) {
                    _Trial_yday -= _Ref_num_days;
                    ++_Year;
                }

                const auto _Year_parts = _Decompose_year(_Year);
                _No_err = _No_err && _Update(_Day_of_year, _Trial_yday) && _Yday_to_month_day(*_Day_of_year, _Year)
                       && _Update(_Century, _Year_parts.first) && _Update(_Two_dig_year, _Year_parts.second);
            }

            // Must have YMD by this point, either parsed directly or calculated above.
            if (!_Have_year || !_Month || !_Day || !_No_err) {
                return false;
            }

            // consistency checks
            if (_Check_wday && _Weekday) {
                const auto _Era_year = _Year - (*_Month <= 2);
                const int _Era       = (_Era_year >= 0 ? _Era_year : _Era_year - 399) / 400;
                const int _Yoe       = _Era_year - _Era * 400;
                const int _Yday_era  = ((979 * (*_Month + (*_Month > 2 ? -3 : 9)) + 19) >> 5) + *_Day - 1;
                const int _Doe       = ((1461 * _Yoe) >> 2) - _Yoe / 100 + _Yday_era;
                _No_err              = _No_err && _Update(_Weekday, (_Doe + _Era_begin_wday) % 7);
            }

            if (_Check_u && _Week_u) {
                _No_err = _No_err && _Update(_Week_u, (*_Day_of_year + 6 - *_Weekday) / 7);
            }

            if (_Check_w && _Week_w) {
                _No_err = _No_err && _Update(_Week_w, (*_Day_of_year + 6 - _Prev_weekday(*_Weekday, 1)) / 7);
            }

            if (_Check_iso && _Iso_week) {
                _No_err = _No_err && _Update(_Iso_week, _Iso8601_week(*_Day_of_year, *_Weekday, _Year));
            }

            return _No_err;
        }

        _NODISCARD bool _Calculate_hour24() {
            if (_Hour_12) {
                return _Update(
                    _Hour_24, _CHRONO make24(hours{*_Hour_12}, static_cast<bool>(_Ampm.value_or(0))).count());
            } else {
                return true;
            }
        }

        _NODISCARD bool _Calculate_year_fields() {
            bool _No_err = true;
            // The order of these updates is significant. Updating the ISO date second allows
            // formats with %g and %y, but not %C, to get the century implicitly from %y.
            if (_Two_dig_year && !_Century) {
                _No_err = _No_err && _Update(_Century, _Two_dig_year >= 69 ? 1900 : 2000);
            }

            // %C is only combined with %g if %G is missing, to avoid an unnecessary parse failure when the ISO and
            // Gregorian years are in different centuries.
            if (_Two_dig_iso_year && _Century && !_Iso_year) {
                _No_err = _No_err && _Update(_Iso_year, *_Century + *_Two_dig_iso_year);
            }

            return _No_err;
        }

        _NODISCARD static bool _Out_of_range(const optional<int>& _Field, const int _Min, const int _Max) {
            return _Field && (_Field < _Min || _Max < _Field);
        }

        _NODISCARD bool _Is_complete() const {
            // Check for data that is incomplete, ambiguous, or obviously out-of-range. The exception is 12-hour time
            // without am/pm. Most strptime implementations will assume am in this case, so we'll do that too. Don't
            // check for consistency yet, because the data might not even be representable by the type being parsed,
            // and calendar computations are relatively expensive.

            // Most time-of-day fields are deferred until we know if we're parsing a time_point.
            if (_Out_of_range(_Hour_12, 1, 12) //
                || _Out_of_range(_Weekday, 0, 6) //
                || _Out_of_range(_Day, 1, 31) //
                || _Out_of_range(_Month, 1, 12)) {
                return false;
            }

            if (_Iso_year || _Two_dig_iso_year || _Iso_week) {
                // Need to have %G or %C+%g. The century can be parsed explicitly, or derived implicitly from %y.
                const bool _Has_complete_year{_Iso_year || ((_Century || _Two_dig_year) && _Two_dig_iso_year)};
                if (!_Has_complete_year || !_Iso_week || !_Weekday || _Out_of_range(_Iso_week, 1, 53)) {
                    return false;
                }
            }

            if (_Week_u || _Week_w) {
                // Need a weekday and year to be complete.
                if (!_Weekday || !_Two_dig_year) {
                    return false;
                }

                if (_Out_of_range(_Week_u, 0, 53) || _Out_of_range(_Week_w, 0, 53)) {
                    return false;
                }
            }

            return true;
        }

        template <class _Duration1, class _Duration2>
        _NODISCARD static constexpr bool _Can_represent() {
            using _Rep1    = typename _Duration1::rep;
            using _Period1 = typename _Duration1::period;
            using _Period2 = typename _Duration2::period;

            // Returns whether _Duration1 can represent _Duration2{1}. Assumes 1 <= _Period2 <=
            // 86,400, i.e., we're interested in time periods between seconds and days.
            if constexpr (is_integral_v<_Rep1>) {
                // Must have _Period1 <= _Period2 and numeric_limits<_Rep1>::max() >= _Period2 / _Period1. For example,
                // std::days can't represent std::seconds, and duration<int, atto>::max() is ~9.2 seconds.
                constexpr auto _Max_tick = static_cast<intmax_t>((numeric_limits<make_signed_t<_Rep1>>::max)());

                // clang-format off
                return ratio_less_equal_v<_Period1, _Period2>
                    && ratio_greater_equal_v<ratio<_Max_tick, _Period2::num>, ratio_divide<ratio<1>, _Period1>>;
                // clang-format on
            } else if (is_floating_point_v<_Rep1>) {
                // With the smallest possible _Period1, ratio<1,INTMAX_MAX>, one day has a tick count of
                // 86,400*INTMAX_MAX ~= 7.97e23. This is representable by float and double, so they can always represent
                // at least one day. On the other hand, one second with the largest possible _Period1 needs a tick count
                // of 1/(INTMAX_MAX) ~= 1.08e-19, which is also representable in both float and double. So, both
                // floating point types can represent durations between one second and one day, regardless of _Period1.
                return true;
            } else {
                // TRANSITION: user-defined arithmetic-like types
                return true;
            }
        }

        enum class _Parse_tp_or_duration { _Time_point, _Duration };

        template <_Parse_tp_or_duration _Parse_type, class _DurationType>
        _NODISCARD bool _Apply_duration_fields(_DurationType& _Result) {
            constexpr bool _Can_rep_sec    = _Can_represent<_DurationType, seconds>();
            constexpr bool _Can_rep_min    = _Can_represent<_DurationType, minutes>();
            constexpr bool _Can_rep_hr     = _Can_represent<_DurationType, hours>();
            constexpr bool _Can_rep_day    = _Can_represent<_DurationType, days>();
            constexpr bool _For_time_point = _Parse_type == _Parse_tp_or_duration::_Time_point;

            const auto _Required{(_For_time_point ? _F_day | _F_mon | _F_year : 0)};
            const auto _Optional{(_For_time_point ? _F_wkday : 0) | (_Can_rep_sec ? _F_sec : 0)
                                 | (_Can_rep_min ? _F_min : 0) | (_Can_rep_hr ? _F_hr : 0)
                                 | (_Can_rep_day ? _F_doy : 0)};

            const auto _Used{_Used_fields()};
            const auto _Time_out_of_range{
                _For_time_point
                && (_Out_of_range(_Second, 0, 60) || _Out_of_range(_Minute, 0, 59) || _Out_of_range(_Hour_24, 0, 23))};

            if (_Time_out_of_range || !_Test_bits(_Used, _Required, _Optional)) {
                return false;
            }

            _Result = _DurationType::zero();
            if constexpr (_Can_rep_sec) {
                if (_Used & _F_sec) {
                    if (_Subsecond) {
                        using _CastedType = duration<int64_t, typename _DurationType::period>;
                        const _SubsecondType _Sub{*_Subsecond};
                        if (treat_as_floating_point_v<_DurationType> //
                            || _CHRONO duration_cast<_CastedType>(_Sub) == _Sub) {
                            _Result += _CHRONO duration_cast<_DurationType>(_Sub);
                        } else {
                            return false;
                        }
                    }

                    if (_Second) {
                        _Result += _CHRONO duration_cast<_DurationType>(seconds{*_Second});
                    }
                }
            }

            if constexpr (_Can_rep_min) {
                if (_Used & _F_min) {
                    _Result += _CHRONO duration_cast<_DurationType>(minutes{*_Minute});
                }

                if (_Utc_offset) {
                    _Result -= _CHRONO duration_cast<_DurationType>(minutes{*_Utc_offset});
                }
            }

            if constexpr (_Can_rep_hr) {
                if (_Used & _F_hr) {
                    _Result += _CHRONO duration_cast<_DurationType>(hours{*_Hour_24});
                }
            }

            if constexpr (_Can_rep_day) {
                if (_For_time_point) {
                    const year_month_day _Ymd{year{*_Century + *_Two_dig_year},
                        month{static_cast<unsigned int>(*_Month)}, day{static_cast<unsigned int>(*_Day)}};
                    _Result += _CHRONO duration_cast<_DurationType>(static_cast<sys_days>(_Ymd).time_since_epoch());
                } else if (_Used & _F_doy) {
                    _Result += _CHRONO duration_cast<_DurationType>(days{*_Day_of_year});
                }
            }

            return true;
        }

        template <class _Rep, class _Period>
        _NODISCARD bool _Make_duration(duration<_Rep, _Period>& _Duration_result) {
            const bool _Consistent = _Calculate_hour24();
            if (_Consistent) {
                return _Apply_duration_fields<_Parse_tp_or_duration::_Duration>(_Duration_result);
            }

            return false;
        }

        enum class _LeapSecondRep : unsigned int {
            _None, // tai_clock, gps_clock; oblivious to leap seconds
            _Negative, // system_clock, observes negative, but not positive, leap seconds
            _All, // utc_clock
            _File_time // _Negative before 1 January 2017, _All afterwards.
        };

        template <class _DurationType>
        _NODISCARD bool _Make_time_point(_DurationType& _Dur, _LeapSecondRep _Leap) {

            const bool _Consistent{_Calculate_hour24() && _Calculate_year_fields() && _Calculate_ymd()};
            if (!_Consistent || !_Apply_duration_fields<_Parse_tp_or_duration::_Time_point>(_Dur)) {
                return false;
            }

            const _LeapSecondRep _Original_leap{_Leap};
            if (_Leap == _LeapSecondRep::_File_time) {
                const int _Year{*_Century + *_Two_dig_year};
                _Leap = _Year <= 2016 ? _LeapSecondRep::_Negative : _LeapSecondRep::_All;
            }

            if (_Second > (_Leap == _LeapSecondRep::_All ? 60 : 59)) {
                return false;
            }

            // A distinction has to be made here between clocks that tick monotonically, even around a positive leap
            // second (everything except system_clock) and ones that have a special representation for leap seconds
            // (utc_clock and, for negative leap seconds, system_clock). gps_clock and tai_clock, in particular, always
            // monotonically count 86,400 seconds/day. The correct tick count, therefore, can be determined without
            // knowing whether any leap seconds have occurred, and there aren't any invalid times due to negative leap
            // second deletions.
            //
            // system_clock also has 86,400 seconds/day, but observes negative leap seconds by skipping them in its tick
            // count. So leap second data is needed to check for a valid time, but not to calculate the tick count.

            if (_Leap != _LeapSecondRep::_None) {
                if (_Hour_24 == 23 && _Minute == 59 && _Second >= 59) {
                    // It's possible that the parsed time doesn't exist because (a) _Seconds == 60 and there *isn't* a
                    // leap second insertion or (b) _Seconds == 59 and there *is* a leap second subtraction.

                    // Check for quick exit.
                    const auto& _Tzdb{_CHRONO get_tzdb()};
                    if (_Leap == _LeapSecondRep::_Negative && _Tzdb._All_ls_positive) {
                        return true;
                    }

                    const bool _Possible_insertion{_Second == 60};
                    const sys_seconds _Target_sys_time{
                        _CHRONO floor<seconds>(_Dur) + (_Possible_insertion ? seconds{0} : seconds{1})};
                    const auto& _Ls_vector{_Tzdb.leap_seconds};
                    const auto _It{_STD lower_bound(_Ls_vector.begin(), _Ls_vector.end(), _Target_sys_time)};
                    const bool _Match_leap{_It != _Ls_vector.end() && *_It == _Target_sys_time};

                    // Condition for a good parse: (_Seconds == 59 && !_Match_leap) || (_Match_leap &&
                    // _It->_Is_positive()). Below is the inverse of this.
                    if (!(_Match_leap ? _It->_Positive() : !_Possible_insertion)) {
                        return false;
                    }
                }

                if (_Leap == _LeapSecondRep::_All) {
                    constexpr bool _Can_rep_sec = _Can_represent<_DurationType, seconds>();
                    if constexpr (_Can_rep_sec) {
                        _Dur = utc_clock::from_sys(sys_time<_DurationType>{_Dur}).time_since_epoch();
                        // If we parsed a positive leap second, then _Dur, interpreted as a system time, refers to the
                        // second *after* insertion. Need to back up one second in this case.
                        if (_Second == 60) {
                            _Dur -= _CHRONO duration_cast<_DurationType>(seconds{1});
                        }

                        if (_Original_leap == _LeapSecondRep::_File_time) {
                            _Dur -= _CHRONO duration_cast<_DurationType>(
                                filesystem::_File_time_clock::_Skipped_filetime_leap_seconds);
                        }
                    } else {
                        // Error if _Dur can't represent the above adjustment.
                        return false;
                    }
                }
            }

            return true;
        }

        _NODISCARD bool _Make_day(day& _Day_result) {
            if (_Used_fields() == _F_day) {
                _Day_result = day{static_cast<unsigned int>(*_Day)};
                return true;
            } else {
                return false;
            }
        }

        _NODISCARD bool _Make_weekday(weekday& _Weekday_result) {
            if (_Used_fields() == _F_wkday) {
                _Weekday_result = weekday{static_cast<unsigned int>(*_Weekday)};
                return true;
            } else {
                return false;
            }
        }

        _NODISCARD bool _Make_month(month& _Month_result) {
            if (_Used_fields() == _F_mon) {
                _Month_result = month{static_cast<unsigned int>(*_Month)};
                return true;
            } else {
                return false;
            }
        }

        _NODISCARD bool _Make_month_day(month_day& _Month_day_result) {
            if (_Day_of_year && !_Yday_to_month_day(*_Day_of_year, 0)) {
                return false;
            }

            constexpr auto _Required = _F_mon | _F_day;
            constexpr auto _Optional = _F_doy;
            if (_Test_bits(_Used_fields(), _Required, _Optional)) {
                _Month_day_result =
                    month_day{month{static_cast<unsigned int>(*_Month)}, day{static_cast<unsigned int>(*_Day)}};
                return true;
            } else {
                return false;
            }
        }

        _NODISCARD bool _Make_year(year& _Year_result) {
            if (_Calculate_year_fields() && _Used_fields() == _F_year) {
                _Year_result = year{*_Century + *_Two_dig_year};
                return true;
            } else {
                return false;
            }
        }

        _NODISCARD bool _Make_year_month(year_month& _Year_month_result) {
            if (_Calculate_year_fields() && _Used_fields() == (_F_mon | _F_year)) {
                _Year_month_result =
                    year_month{year{*_Century + *_Two_dig_year}, month{static_cast<unsigned int>(*_Month)}};
                return true;
            } else {
                return false;
            }
        }

        _NODISCARD bool _Make_year_month_day(
            year_month_day& _Year_month_day_result, const bool _For_time_point = false) {
            const bool _Consistent   = _Calculate_year_fields() && _Calculate_ymd();
            constexpr auto _Required = _F_day | _F_mon | _F_year;
            auto _Optional           = _F_wkday | _F_doy;
            if (_For_time_point) {
                // These fields aren't used here, but they might be used later if converting to a time_point.
                _Optional |= _F_sec | _F_min | _F_hr;
            }

            if (_Consistent && _Test_bits(_Used_fields(), _Required, _Optional)) {
                _Year_month_day_result = year_month_day{year{*_Century + *_Two_dig_year},
                    month{static_cast<unsigned int>(*_Month)}, day{static_cast<unsigned int>(*_Day)}};
                return true;
            } else {
                return false;
            }
        }

        template <class _InIt>
        _NODISCARD _InIt _Parse_time_field(_InIt _First, ios_base& _Iosbase, ios_base::iostate& _State,
            const char _Flag, const char _Modifier, const unsigned int _Width,
            const unsigned int _Subsecond_precision) {
            using _CharT = typename _InIt::value_type;

            const auto& _Ctype_fac = _STD use_facet<ctype<_CharT>>(_Iosbase.getloc());
            const auto& _Time_fac  = _STD use_facet<time_get<_CharT>>(_Iosbase.getloc());
            constexpr _InIt _Last{};

            int _Val{0};
            switch (_Flag) {
            case 'a':
            case 'A':
                {
                    tm _Tp;
                    _Tp.tm_wday = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'a');
                    if (!_Update(_Weekday, _Tp.tm_wday)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'b':
            case 'B':
            case 'h':
                {
                    tm _Tp;
                    _Tp.tm_mon = _Invalid_time_field<int>;
                    _First     = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'b');
                    if (_Tp.tm_mon == _Invalid_time_field<int> || !_Update(_Month, ++_Tp.tm_mon)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'C':
                if (_Modifier != 'E') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                    _Val *= 100;
                } else {
                    tm _Tp;
                    _Tp.tm_year = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'C', 'E');
                    _Val        = _Tp.tm_year;
                    if (_Tp.tm_year != _Invalid_time_field<int>) {
                        _Val += 1900;
                    }
                }

                if (!_Update(_Century, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'd':
            case 'e':
                if (_Modifier != 'O') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                } else {
                    tm _Tp;
                    _Tp.tm_mday = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'd', 'O');
                    _Val        = _Tp.tm_mday;
                }

                if (!_Update(_Day, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'D':
                _First = _Parse_time_field_restricted(_First, _Iosbase, _State, "%m/%d/%y");
                break;

            case 'F':
                {
                    // If modified with a width N, the width is applied to only %Y.
                    _State |= _Get_int(_First, _Width == 0 ? 4u : _Width, _Val, _Ctype_fac);
                    const auto _Year_parts = _Decompose_year(_Val);
                    if (_Update(_Century, _Year_parts.first) && _Update(_Two_dig_year, _Year_parts.second)) {
                        _First = _Parse_time_field_restricted(_First, _Iosbase, _State, "-%m-%d");
                    } else {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'g':
                _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                if (!_Update(_Two_dig_iso_year, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'G':
                {
                    _State |= _Get_int(_First, _Width == 0 ? 4u : _Width, _Val, _Ctype_fac);
                    const auto _Year_parts = _Decompose_year(_Val);
                    if (!_Update(_Iso_year, _Val) || !_Update(_Two_dig_iso_year, _Year_parts.second)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'H':
                if (_Modifier != 'O') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                } else {
                    tm _Tp;
                    _Tp.tm_hour = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'H', 'O');
                    _Val        = _Tp.tm_hour;
                }

                if (!_Update(_Hour_24, _Val)
                    || (_Val < 24
                        && (!_Update(_Ampm, _Val >= 12 ? 1 : 0)
                            || !_Update(_Hour_12, _CHRONO make12(hours{_Val}).count())))) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'I':
                if (_Modifier != 'O') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                } else {
                    tm _Tp;
                    _Tp.tm_hour = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'I', 'O');
                    _Val        = (_Tp.tm_hour == 0) ? 12 : _Tp.tm_hour;
                }

                if (!_Update(_Hour_12, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'j':
                _State |= _Get_int(_First, _Width == 0 ? 3u : _Width, _Val, _Ctype_fac);
                if (!_Update(_Day_of_year, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'M':
                if (_Modifier != 'O') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                } else {
                    tm _Tp;
                    _Tp.tm_min = _Invalid_time_field<int>;
                    _First     = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'M', 'O');
                    _Val       = _Tp.tm_min;
                }

                if (!_Update(_Minute, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'm':
                if (_Modifier != 'O') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                } else {
                    tm _Tp;
                    _Initialize_time_point(_Tp);
                    _First = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'm', 'O');
                    _Val   = _Tp.tm_mon;
                    if (_Tp.tm_mon != _Invalid_time_field<int>) {
                        ++_Val;
                    }
                }

                if (!_Update(_Month, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'p':
                {
                    tm _Tp;
                    _Tp.tm_hour = 0;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'p');
                    if (!_Update(_Ampm, _Tp.tm_hour == 0 ? 0 : 1)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'c':
            case 'r':
            case 'x':
            case 'X':
                {
                    tm _Tp;
                    _Initialize_time_point(_Tp);
                    const bool _Full_year = (_Flag == 'c'); // 'x' reads two-digit year, 'r' and 'X' read times
                    _First                = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, _Flag, _Modifier);
                    if (!_Update_if_valid(_Tp, _Full_year)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'R':
                _First = _Parse_time_field_restricted(_First, _Iosbase, _State, "%H:%M");
                break;

            case 'T':
                _First = _Parse_time_field_restricted(_First, _Iosbase, _State, "%H:%M:%S", _Subsecond_precision);
                break;

            case 'S':
                if (_Subsecond_precision == 0) {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                    if (!_Update(_Second, _Val)) {
                        _State |= ios_base::failbit;
                    }
                } else {
                    const auto& _Numpunct_fac = _STD use_facet<numpunct<_CharT>>(_Iosbase.getloc());
                    _State |=
                        _Get_fixed(_First, _Width == 0 ? 3 + _Subsecond_precision : _Width, _Ctype_fac, _Numpunct_fac);
                }
                break;

            case 'u':
            case 'w':
                if (_Flag == 'w' && _Modifier == 'O') {
                    tm _Tp;
                    _Tp.tm_wday = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'w', 'O');
                    _Val        = _Tp.tm_wday;
                } else {
                    _State |= _Get_int(_First, _Width == 0 ? 1u : _Width, _Val, _Ctype_fac);
                    if (_Flag == 'u') {
                        // ISO weekday: [1,7], 7 == Sunday
                        if (_Val == 7) {
                            _Val = 0;
                        } else if (_Val == 0) {
                            _Val = 7; // out of range
                        }
                    }
                }

                if (!_Update(_Weekday, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'U':
            case 'V':
            case 'W':
                {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                    auto& _Field{(_Flag == 'U') ? _Week_u : (_Flag == 'W' ? _Week_w : _Iso_week)};
                    if (!_Update(_Field, _Val)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'y':
                if (_Modifier == '\0') {
                    _State |= _Get_int(_First, _Width == 0 ? 2u : _Width, _Val, _Ctype_fac);
                } else {
                    tm _Tp;
                    _Tp.tm_year = _Invalid_time_field<int>;
                    _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'y', _Modifier);
                    if (_Modifier == 'E') {
                        _Val = _Tp.tm_year + 1900; // offset from %EC base year
                    } else {
                        const auto _Year_parts = _Decompose_year(_Tp.tm_year);
                        _Val                   = _Year_parts.second;
                    }
                }

                if (!_Update(_Two_dig_year, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'Y':
                {
                    if (_Modifier == 'E') {
                        tm _Tp;
                        _Tp.tm_year = _Invalid_time_field<int>;
                        _First      = _Time_fac.get(_First, _Last, _Iosbase, _State, &_Tp, 'Y', 'E');
                        _Val        = _Tp.tm_year + 1900;
                    } else {
                        _State |= _Get_int(_First, _Width == 0 ? 4u : _Width, _Val, _Ctype_fac);
                    }

                    const auto _Year_parts = _Decompose_year(_Val);
                    if (!_Update(_Century, _Year_parts.first) || !_Update(_Two_dig_year, _Year_parts.second)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            case 'z':
                _State |= _Get_tz_offset(_First, _Ctype_fac, _Modifier == 'E' || _Modifier == 'O', _Val);
                if (!_Update(_Utc_offset, _Val)) {
                    _State |= ios_base::failbit;
                }
                break;

            case 'Z':
                {
                    string _Name;
                    _State |= _Get_tz_name(_First, _Ctype_fac, _Name);
                    if (!_Update(_Tz_name, _Name)) {
                        _State |= ios_base::failbit;
                    }
                    break;
                }

            default:
                // Invalid flag
                _State |= ios_base::failbit;
                break;
            }

            return _First;
        }

        template <class _InIt>
        _NODISCARD _InIt _Parse_time_field_restricted(_InIt _First, ios_base& _Iosbase, ios_base::iostate& _State,
            const char* _Fmt, const unsigned int _Subsecond_precision = 0) {
            using _Ctype = ctype<typename _InIt::value_type>;
            // Parses a restricted format string. It generally doesn't handle anything parsed outside of
            // _Parse_time_field:
            //   (a) any whitespace (' ', %n, %t)
            //   (b) %% literal (other literals are parsed, however)
            //   (c) E or O modifiers
            //   (d) width parameter
            // It also assumes a valid format string, specifically that '%' is always followed by a flag.
            const _Ctype& _Ctype_fac{_STD use_facet<_Ctype>(_Iosbase.getloc())};
            constexpr _InIt _Last{};

            while (*_Fmt != '\0' && (_State & ~ios_base::eofbit) == ios_base::goodbit) {
                if (_First == _Last) {
                    _State |= ios_base::failbit | ios_base::eofbit;
                    break;
                } else if (*_Fmt == '%') {
                    _First = _Parse_time_field(_First, _Iosbase, _State, *++_Fmt, '\0', 0, _Subsecond_precision);
                } else if (_Ctype_fac.narrow(*_First++) != *_Fmt) {
                    _State |= ios_base::failbit;
                }
                ++_Fmt;
            }
            return _First;
        }

        template <class _InIt>
        _NODISCARD ios_base::iostate _Get_fixed(_InIt& _First, unsigned int _Width,
            const ctype<typename _InIt::value_type>& _Ctype_fac,
            const numpunct<typename _InIt::value_type>& _Numpunct_fac) {
            constexpr _InIt _Last{};

            while (_First != _Last && _Ctype_fac.is(ctype_base::space, *_First) && _Width > 0) {
                ++_First;
                --_Width;
            }

            int _Second_        = 0;
            int64_t _Subsecond_ = 0;
            int64_t _Multiplier = _SubsecondType::period::den;
            bool _Found_point   = false;
            bool _Found_digit   = false;

            while (_First != _Last && _Width > 0 && _Multiplier >= 10) {
                const auto _Ch = *_First;
                if (_Ch == _Numpunct_fac.decimal_point() && !_Found_point) {
                    _Found_point = true;
                } else if (_Ctype_fac.is(ctype_base::digit, _Ch)) {
                    _Found_digit      = true;
                    const auto _Digit = _Ctype_fac.narrow(_Ch) - '0';
                    if (_Found_point) {
                        _Multiplier /= 10;
                        _Subsecond_ += _Digit * _Multiplier;
                    } else {
                        if (_Second_ > ((numeric_limits<int>::max)() - _Digit) / 10) {
                            return ios_base::failbit;
                        }

                        _Second_ = _Second_ * 10 + _Digit;
                    }
                } else {
                    break;
                }
                ++_First;
                --_Width;
            }

            ios_base::iostate _State = ios_base::goodbit;
            if (_First == _Last) {
                _State |= ios_base::eofbit;
            }

            if (!(_Found_digit && _Update(_Second, _Second_) && _Update(_Subsecond, _Subsecond_))) {
                _State |= ios_base::failbit;
            }

            return _State;
        }

        template <class _InIt>
        _NODISCARD ios_base::iostate _Get_int(
            _InIt& _First, unsigned int _Width, int& _Val, const ctype<typename _InIt::value_type>& _Ctype_fac) {
            constexpr _InIt _Last{};

            while (_First != _Last && _Ctype_fac.is(ctype_base::space, *_First) && _Width > 0) {
                ++_First;
                --_Width;
            }

            char _Ac[_MAX_INT_DIG];
            char* _Ptr = _Ac;
            if (_First != _Last && _Width > 0) {
                const char _Ch = _Ctype_fac.narrow(*_First);
                if (_Ch == '+' || _Ch == '-') { // copy sign
                    *_Ptr++ = _Ch;
                    ++_First;
                    --_Width;
                }
            }

            bool _Has_leading_zero = false;
            while (_First != _Last && _Width > 0 && _Ctype_fac.narrow(*_First) == '0') { // strip leading zeros
                ++_First;
                --_Width;
                _Has_leading_zero = true;
            }

            if (_Has_leading_zero) {
                *_Ptr++ = '0';
            }

            char _Ch;
            while (_First != _Last && _Width > 0 && '0' <= (_Ch = _Ctype_fac.narrow(*_First))
                   && _Ch <= '9') { // copy digits
                *_Ptr = _Ch;
                if (_Ptr < _STD cend(_Ac)) {
                    ++_Ptr; // drop trailing digits if already too large
                }
                ++_First;
                --_Width;
            }

            *_Ptr      = '\0';
            int _Errno = 0;
            char* _Ep;
            const long _Ans          = _CSTD _Stolx(_Ac, &_Ep, 10, &_Errno);
            ios_base::iostate _State = ios_base::goodbit;

            if (_First == _Last) {
                _State |= ios_base::eofbit;
            }

            if (_Ep == _Ac || _Errno != 0) {
                _State |= ios_base::failbit; // bad conversion
            } else {
                _Val = _Ans; // store valid result
            }

            return _State;
        }

        template <class _InIt>
        _NODISCARD ios_base::iostate _Get_tz_offset(
            _InIt& _First, const ctype<typename _InIt::value_type>& _Ctype_fac, const bool _Is_modified, int& _Offset) {
            constexpr _InIt _Last{};
            if (_First == _Last) {
                return ios_base::eofbit;
            }

            bool _Negative = false;
            switch (_Ctype_fac.narrow(*_First)) {
            case '-':
                _Negative = true;
                [[fallthrough]];
            case '+':
                ++_First;
                break;
            }

            // For a regular offset hh[mm], simply read four digits, with the option of an EOF or non-digit after
            // reading two. The modified form h[h][:mm] is similar, except for the following points:
            //  (a) an EOF or non-digit is allowable after reading *one* digit, not two.
            //  (b) after reading one digit, another optional digit keeps us in the same state, except for decrementing
            //      the number of optional digits allowed. In this state, reading a ':' allows parsing to continue.

            int _Tz_hours        = 0;
            int _Tz_minutes      = 0;
            int _Optional_digits = 1;
            for (int _Count = 0; _Count < 4; ++_Count) {
                const bool _Allow_match_fail{_Count == (_Is_modified ? 1 : 2)};

                if (_First == _Last) {
                    if (_Allow_match_fail) {
                        break;
                    } else {
                        return ios_base::eofbit | ios_base::failbit;
                    }
                }

                const char _Ch       = _Ctype_fac.narrow(*_First++);
                const bool _Is_digit = ('0' <= _Ch && _Ch <= '9');
                if (_Is_modified && _Count == 1) {
                    if (_Ch == ':') {
                        continue;
                    } else if (_Is_digit && _Optional_digits > 0) {
                        _Tz_hours = 10 * _Tz_hours + (_Ch - '0');
                        --_Optional_digits;
                        --_Count;
                    } else {
                        if (_Allow_match_fail) {
                            break;
                        } else {
                            return ios_base::failbit;
                        }
                    }
                } else if (_Is_digit) {
                    auto& _Part = _Count < 2 ? _Tz_hours : _Tz_minutes;
                    _Part       = 10 * _Part + (_Ch - '0');
                } else {
                    if (_Allow_match_fail) {
                        break;
                    } else {
                        return ios_base::failbit;
                    }
                }
            }

            _Offset = 60 * _Tz_hours + _Tz_minutes;
            if (_Negative) {
                _Offset = -_Offset;
            }
            return ios_base::goodbit;
        }

        template <class _InIt>
        _NODISCARD ios_base::iostate _Get_tz_name(
            _InIt& _First, const ctype<typename _InIt::value_type>& _Ctype_fac, string& _Tz_name) {
            constexpr _InIt _Last{};
            _Tz_name.clear();
            while (_First != _Last) {
                const char _Ch{_Ctype_fac.narrow(*_First)};
                if (_STD isalnum(static_cast<unsigned char>(_Ch)) || _Ch == '_' || _Ch == '/' || _Ch == '-'
                    || _Ch == '+') {
                    _Tz_name.push_back(_Ch);
                    ++_First;
                } else {
                    break;
                }
            }
            return _First == _Last ? ios_base::eofbit : ios_base::goodbit;
        }

        template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
        _Time_parse_fields(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _FmtFirst,
            basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr,
            unsigned int _Subsecond_precision = 0) {

            using _Myis = basic_istream<_CharT, _Traits>;

            const auto& _Ctype_fac       = _STD use_facet<ctype<_CharT>>(_Istr.getloc());
            ios_base::iostate _State     = ios_base::goodbit;
            const _CharT* const _FmtLast = _FmtFirst + _Traits::length(_FmtFirst);
            const typename _Myis::sentry _Ok{_Istr, true};

            istreambuf_iterator _First{_Istr};
            constexpr decltype(_First) _Last{};

            if (_Ok) {
                _TRY_IO_BEGIN
                for (; _FmtFirst != _FmtLast && (_State & ~ios_base::eofbit) == ios_base::goodbit; ++_FmtFirst) {
                    if (_First == _Last) {
                        // EOF is not an error if the remaining flags can match zero characters.
                        for (; _FmtFirst != _FmtLast; ++_FmtFirst) {
                            char _Flag{};
                            if (_Ctype_fac.is(ctype_base::space, *_FmtFirst)) {
                                _Flag = ' ';
                            } else {
                                if (_Ctype_fac.narrow(*_FmtFirst) == '%' && ++_FmtFirst != _FmtLast) {
                                    _Flag = _Ctype_fac.narrow(*_FmtFirst);
                                }
                            }

                            if (_Flag != ' ' && _Flag != 't') {
                                _State |= ios_base::failbit | ios_base::eofbit;
                                break;
                            }
                        }
                        break;
                    } else if (_Ctype_fac.narrow(*_FmtFirst) != '%') { // match literal element
                        if (_Ctype_fac.is(ctype_base::space, *_FmtFirst)) {
                            while (_First != _Last && _Ctype_fac.is(ctype_base::space, *_First)) {
                                ++_First;
                            }
                        } else if (*_First == *_FmtFirst) {
                            ++_First;
                        } else {
                            _State |= ios_base::failbit; // bad literal match
                        }
                    } else if (++_FmtFirst == _FmtLast) { // not enough for a valid flag
                        _State |= ios_base::failbit;
                    } else { // get flag after %
                        char _Flag          = _Ctype_fac.narrow(*_FmtFirst);
                        char _Modifier      = '\0';
                        unsigned int _Width = 0;

                        if (_Flag == 'E' || _Flag == 'O') {
                            if (++_FmtFirst == _FmtLast) {
                                _State |= ios_base::failbit;
                                break;
                            }
                            _Modifier = _STD exchange(_Flag, _Ctype_fac.narrow(*_FmtFirst));
                        } else if ('0' <= _Flag && _Flag <= '9') {
                            _Width = static_cast<unsigned int>(_Flag - '0');
                            while (++_FmtFirst != _FmtLast && _Ctype_fac.is(ctype_base::digit, *_FmtFirst)) {
                                const auto _Digit = static_cast<unsigned int>(_Ctype_fac.narrow(*_FmtFirst) - '0');
                                if (_Width > ((numeric_limits<unsigned int>::max)() - _Digit) / 10) {
                                    _State |= ios_base::failbit;
                                    break;
                                }
                                _Width = 10 * _Width + _Digit;
                            }
                            if (_FmtFirst == _FmtLast) {
                                _State |= ios_base::failbit;
                                break;
                            }
                            _Flag = _Ctype_fac.narrow(*_FmtFirst);
                        }

                        switch (_Flag) {
                        case 'n': // exactly one space
                            if (!_Ctype_fac.is(ctype_base::space, *_First++)) {
                                _State |= ios_base::failbit;
                            }
                            break;

                        case 't': // zero or one space
                            if (_Ctype_fac.is(ctype_base::space, *_First)) {
                                ++_First;
                            }
                            break;

                        case '%':
                            if (_Ctype_fac.narrow(*_First++) != '%') {
                                _State |= ios_base::failbit;
                            }
                            break;

                        default:
                            _First = _Parse_time_field(_First, _Istr, _State, _Flag, _Modifier, _Width,
                                _Subsecond_precision); // convert a single field
                            break;
                        }
                    }
                }

                _CATCH_IO_(_Myis, _Istr)
            }

            if (!_Is_complete()) {
                _State |= ios_base::failbit;
            }

            if (!(_State & ios_base::failbit)) {
                if (_Offset != nullptr && _Utc_offset) {
                    *_Offset = minutes{*_Utc_offset};
                }

                if (_Abbrev != nullptr && _Tz_name) {
                    if constexpr (is_same_v<decltype(_Abbrev), string*>) {
                        *_Abbrev = _STD move(*_Tz_name);
                    } else {
                        _Abbrev->clear();
                        for (const char& _Ch : *_Tz_name) {
                            _Abbrev->push_back(_Ctype_fac.widen(_Ch));
                        }
                    }
                }
            }

            _Istr.setstate(_State);
        }
    };

    template <class _CharT, class _Traits, class _Rep, class _Period, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        duration<_Rep, _Period>& _Duration, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, hh_mm_ss<duration<_Rep, _Period>>::fractional_width};
        if (_Istr && !_Time._Make_duration(_Duration)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt, weekday& _Wd,
        basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_weekday(_Wd)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt, day& _Day,
        basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_day(_Day)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        month& _Month, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_month(_Month)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        month_day& _Md, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_month_day(_Md)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt, year& _Year,
        basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_year(_Year)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        year_month& _Ym, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_year_month(_Ym)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        year_month_day& _Ymd, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr, minutes* _Offset = nullptr) {
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset};
        if (_Istr && !_Time._Make_year_month_day(_Ymd)) {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Duration, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        utc_time<_Duration>& _Tp, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        minutes* _Offset = nullptr) {
        constexpr auto _Subsecond_precision{hh_mm_ss<_Duration>::fractional_width};
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, _Subsecond_precision};
        _Duration _Dur;
        if (_Istr && _Time._Make_time_point(_Dur, _Time_parse_fields::_LeapSecondRep::_All)) {
            _Tp = utc_time<_Duration>{_Dur};
        } else {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Duration, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        sys_time<_Duration>& _Tp, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        minutes* _Offset = nullptr) {
        constexpr auto _Subsecond_precision{hh_mm_ss<_Duration>::fractional_width};
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, _Subsecond_precision};
        _Duration _Dur;
        if (_Istr && _Time._Make_time_point(_Dur, _Time_parse_fields::_LeapSecondRep::_Negative)) {
            _Tp = sys_time<_Duration>{_Dur};
        } else {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Duration, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        tai_time<_Duration>& _Tp, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        minutes* _Offset = nullptr) {
        constexpr auto _Subsecond_precision{hh_mm_ss<_Duration>::fractional_width};
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, _Subsecond_precision};
        _Duration _Dur;
        if (_Istr && _Time._Make_time_point(_Dur, _Time_parse_fields::_LeapSecondRep::_None)) {
            _Tp = tai_time<_Duration>{_Dur + _CHRONO duration_cast<_Duration>(days{4383})};
        } else {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Duration, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        gps_time<_Duration>& _Tp, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        chrono::minutes* _Offset = nullptr) {
        constexpr auto _Subsecond_precision{hh_mm_ss<_Duration>::fractional_width};
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, _Subsecond_precision};
        _Duration _Dur;
        if (_Istr && _Time._Make_time_point(_Dur, _Time_parse_fields::_LeapSecondRep::_None)) {
            _Tp = gps_time<_Duration>{_Dur - _CHRONO duration_cast<_Duration>(days{3657})};
        } else {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Duration, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        file_time<_Duration>& _Tp, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        chrono::minutes* _Offset = nullptr) {
        constexpr auto _Subsecond_precision{hh_mm_ss<_Duration>::fractional_width};
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, _Subsecond_precision};
        _Duration _Dur;
        if (_Istr && _Time._Make_time_point(_Dur, _Time_parse_fields::_LeapSecondRep::_File_time)) {
            constexpr auto _File_clock_adj{_CHRONO duration_cast<_Duration>(
                filesystem::_File_time_clock::duration{filesystem::__std_fs_file_time_epoch_adjustment})};
            _Tp = file_time<_Duration>{_Dur} + _File_clock_adj;
        } else {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Duration, class _Alloc = allocator<_CharT>>
    basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& _Istr, const _CharT* _Fmt,
        local_time<_Duration>& _Tp, basic_string<_CharT, _Traits, _Alloc>* _Abbrev = nullptr,
        minutes* _Offset = nullptr) {
        constexpr auto _Subsecond_precision{hh_mm_ss<_Duration>::fractional_width};
        _Time_parse_fields _Time{_Istr, _Fmt, _Abbrev, _Offset, _Subsecond_precision};
        // *_Offset is not subtracted from local_time, see N4885 [time.clock.local]/4.
        _Time._Utc_offset.reset();
        _Duration _Dur;
        if (_Istr && _Time._Make_time_point(_Dur, _Time_parse_fields::_LeapSecondRep::_None)) {
            _Tp = local_time<_Duration>{_Dur};
        } else {
            _Istr.setstate(ios_base::failbit);
        }
        return _Istr;
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable>
    struct _Time_parse_iomanip_c_str {
        _Time_parse_iomanip_c_str(const _CharT* _Fmt_, _Parsable& _Tp_,
            basic_string<_CharT, _Traits, _Alloc>* _Abbrev_ = nullptr, minutes* _Offset_ = nullptr)
            : _Fmt{_Fmt_}, _Tp{_Tp_}, _Abbrev{_Abbrev_}, _Offset{_Offset_} {}

        _Time_parse_iomanip_c_str(_Time_parse_iomanip_c_str&&) = delete;

        const _CharT* _Fmt;
        _Parsable& _Tp;
        basic_string<_CharT, _Traits, _Alloc>* _Abbrev;
        minutes* _Offset;
    };

    template <class _CharT, class _Traits, class _Alloc, class _Parsable>
    struct _Time_parse_iomanip {
        _Time_parse_iomanip(const basic_string<_CharT, _Traits, _Alloc>& _Fmt_, _Parsable& _Tp_,
            basic_string<_CharT, _Traits, _Alloc>* _Abbrev_ = nullptr, minutes* _Offset_ = nullptr)
            : _Fmt{_Fmt_}, _Tp{_Tp_}, _Abbrev{_Abbrev_}, _Offset{_Offset_} {}

        _Time_parse_iomanip(_Time_parse_iomanip&&) = delete;

        const basic_string<_CharT, _Traits, _Alloc>& _Fmt;
        _Parsable& _Tp;
        basic_string<_CharT, _Traits, _Alloc>* _Abbrev;
        minutes* _Offset;
    };

    template <class _CharT, class _Traits, class _Parsable, class... _Rest>
    using _Has_from_stream =
        decltype(static_cast<void>(from_stream(_STD declval<basic_istream<_CharT, _Traits>&>(),
                     _STD declval<const _CharT*>(), _STD declval<_Parsable&>(), _STD declval<_Rest>()...)),
            0); // intentional ADL

    template <class _CharT, class _Parsable, _Has_from_stream<_CharT, char_traits<_CharT>, _Parsable> = 0>
    _NODISCARD auto parse(const _CharT* _Fmt, _Parsable& _Tp) {
        return _Time_parse_iomanip_c_str<_CharT, char_traits<_CharT>, allocator<_CharT>, _Parsable>{_Fmt, _Tp};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable,
        _Has_from_stream<_CharT, _Traits, _Parsable> = 0>
    _NODISCARD auto parse(const basic_string<_CharT, _Traits, _Alloc>& _Fmt, _Parsable& _Tp) {
        return _Time_parse_iomanip{_Fmt, _Tp};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable,
        _Has_from_stream<_CharT, _Traits, _Parsable, basic_string<_CharT, _Traits, _Alloc>*> = 0>
    _NODISCARD auto parse(const _CharT* _Fmt, _Parsable& _Tp, basic_string<_CharT, _Traits, _Alloc>& _Abbrev) {
        return _Time_parse_iomanip_c_str{_Fmt, _Tp, _STD addressof(_Abbrev)};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable,
        _Has_from_stream<_CharT, _Traits, _Parsable, basic_string<_CharT, _Traits, _Alloc>*> = 0>
    _NODISCARD auto parse(const basic_string<_CharT, _Traits, _Alloc>& _Fmt, _Parsable& _Tp,
        basic_string<_CharT, _Traits, _Alloc>& _Abbrev) {
        return _Time_parse_iomanip{_Fmt, _Tp, _STD addressof(_Abbrev)};
    }

    template <class _CharT, class _Parsable,
        _Has_from_stream<_CharT, char_traits<_CharT>, _Parsable, basic_string<_CharT>*, minutes*> = 0>
    _NODISCARD auto parse(const _CharT* _Fmt, _Parsable& _Tp, minutes& _Offset) {
        return _Time_parse_iomanip_c_str{_Fmt, _Tp, static_cast<basic_string<_CharT>*>(nullptr), &_Offset};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable,
        _Has_from_stream<_CharT, _Traits, _Parsable, basic_string<_CharT, _Traits, _Alloc>*, minutes*> = 0>
    _NODISCARD auto parse(const basic_string<_CharT, _Traits, _Alloc>& _Fmt, _Parsable& _Tp, minutes& _Offset) {
        return _Time_parse_iomanip{_Fmt, _Tp, static_cast<basic_string<_CharT, _Traits, _Alloc>*>(nullptr), &_Offset};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable,
        _Has_from_stream<_CharT, _Traits, _Parsable, basic_string<_CharT, _Traits, _Alloc>*, minutes*> = 0>
    _NODISCARD auto parse(
        const _CharT* _Fmt, _Parsable& _Tp, basic_string<_CharT, _Traits, _Alloc>& _Abbrev, minutes& _Offset) {
        return _Time_parse_iomanip_c_str{_Fmt, _Tp, _STD addressof(_Abbrev), &_Offset};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable,
        _Has_from_stream<_CharT, _Traits, _Parsable, basic_string<_CharT, _Traits, _Alloc>*, minutes*> = 0>
    _NODISCARD auto parse(const basic_string<_CharT, _Traits, _Alloc>& _Fmt, _Parsable& _Tp,
        basic_string<_CharT, _Traits, _Alloc>& _Abbrev, minutes& _Offset) {
        return _Time_parse_iomanip{_Fmt, _Tp, _STD addressof(_Abbrev), &_Offset};
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable>
    basic_istream<_CharT, _Traits>& operator>>(
        basic_istream<_CharT, _Traits>& _Is, _Time_parse_iomanip_c_str<_CharT, _Traits, _Alloc, _Parsable>&& _Tpi) {
        from_stream(_Is, _Tpi._Fmt, _Tpi._Tp, _Tpi._Abbrev, _Tpi._Offset); // intentional ADL
        return _Is;
    }

    template <class _CharT, class _Traits, class _Alloc, class _Parsable>
    basic_istream<_CharT, _Traits>& operator>>(
        basic_istream<_CharT, _Traits>& _Is, _Time_parse_iomanip<_CharT, _Traits, _Alloc, _Parsable>&& _Tpi) {
        from_stream(_Is, _Tpi._Fmt.c_str(), _Tpi._Tp, _Tpi._Abbrev, _Tpi._Offset); // intentional ADL
        return _Is;
    }

} // namespace chrono

#ifdef __cpp_lib_format
  // [time.format]

template <class _CharT>
_NODISCARD constexpr const _CharT* _Choose_literal(const char* const _Str, const wchar_t* const _WStr) noexcept {
    if constexpr (is_same_v<_CharT, char>) {
        return _Str;
    } else {
        return _WStr;
    }
}

#define _STATICALLY_WIDEN(_CharT, _Literal) (_Choose_literal<_CharT>(_Literal, L##_Literal))


// clang-format off
template <class _Ty, class _CharT>
concept _Chrono_parse_spec_callbacks = _Parse_align_callbacks<_Ty, _CharT>
                                        && _Parse_width_callbacks<_Ty, _CharT>
                                        && _Parse_precision_callbacks<_Ty, _CharT>
                                        && _Width_adapter_callbacks<_Ty, _CharT>
                                        && _Precision_adapter_callbacks<_Ty, _CharT>
                                        && requires(_Ty _At, basic_string_view<_CharT> _Sv, _Fmt_align _Aln) {
    { _At._On_conversion_spec(char{}, _CharT{}) } -> same_as<void>;
    { _At._On_lit_char(_CharT{}) } -> same_as<void>;
};
// clang-format on

// clang-format off
template <class _Ty>
concept _Has_ok = requires(_Ty _At) {
    { _At.ok() } -> same_as<bool>;
};
// clang-format on

// A chrono spec is either a type (with an optional modifier), OR a literal character, never both.
template <class _CharT>
struct _Chrono_spec {
    _CharT _Lit_char = _CharT{'\0'}; // any character other than '{', '}', or '%'
    char _Modifier   = '\0'; // either 'E' or 'O'
    char _Type       = '\0';
};

template <class _CharT>
struct _Chrono_format_specs {
    int _Width                   = 0;
    int _Precision               = -1;
    int _Dynamic_width_index     = -1;
    int _Dynamic_precision_index = -1;
    _Fmt_align _Alignment        = _Fmt_align::_None;
    uint8_t _Fill_length         = 1;
    bool _Localized              = false;
    // At most one codepoint (so one char32_t or four utf-8 char8_t)
    _CharT _Fill[4 / sizeof(_CharT)] = {_CharT{' '}};
    // recursive definition in grammar, so could have any number of these
    vector<_Chrono_spec<_CharT>> _Chrono_specs_list;
};

// Model of _Chrono_parse_spec_callbacks that fills a _Chrono_format_specs with the parsed data
template <class _CharT, class _ParseContext>
class _Chrono_specs_setter {
public:
    constexpr explicit _Chrono_specs_setter(_Chrono_format_specs<_CharT>& _Specs_, _ParseContext& _Parse_ctx_)
        : _Specs(_Specs_), _Parse_ctx(_Parse_ctx_) {}

    // same as _Specs_setter
    constexpr void _On_align(_Fmt_align _Aln) {
        _Specs._Alignment = _Aln;
    }

    // same as _Specs_setter
    constexpr void _On_fill(basic_string_view<_CharT> _Sv) {
        if (_Sv.size() > _STD size(_Specs._Fill)) {
            _THROW(format_error("Invalid fill (too long)."));
        }

        const auto _Pos = _STD _Copy_unchecked(_Sv._Unchecked_begin(), _Sv._Unchecked_end(), _Specs._Fill);
        _STD fill(_Pos, _STD end(_Specs._Fill), _CharT{});
        _Specs._Fill_length = static_cast<uint8_t>(_Sv.size());
    }

    constexpr void _On_width(int _Width) {
        _Specs._Width = _Width;
    }

    constexpr void _On_precision(int _Prec) {
        _Specs._Precision = _Prec;
    }

    constexpr void _On_dynamic_width(const size_t _Arg_id) {
        _Parse_ctx.check_arg_id(_Arg_id);
        _Specs._Dynamic_width_index = _Verify_dynamic_arg_index_in_range(_Arg_id);
    }

    constexpr void _On_dynamic_width(_Auto_id_tag) {
        _Specs._Dynamic_width_index = _Verify_dynamic_arg_index_in_range(_Parse_ctx.next_arg_id());
    }

    constexpr void _On_dynamic_precision(const size_t _Arg_id) {
        _Parse_ctx.check_arg_id(_Arg_id);
        _Specs._Dynamic_precision_index = _Verify_dynamic_arg_index_in_range(_Arg_id);
    }

    constexpr void _On_dynamic_precision(_Auto_id_tag) {
        _Specs._Dynamic_precision_index = _Verify_dynamic_arg_index_in_range(_Parse_ctx.next_arg_id());
    }

    constexpr void _On_localized() {
        _Specs._Localized = true;
    }

    constexpr void _On_conversion_spec(char _Modifier, _CharT _Type) {
        // NOTE: same performance note from _Basic_format_specs also applies here
        if (_Modifier != '\0' && _Modifier != 'E' && _Modifier != 'O') {
            _THROW(format_error("Invalid modifier specification."));
        }

        if (_Type < 0 || _Type > (numeric_limits<signed char>::max)()) {
            _THROW(format_error("Invalid type specification."));
        }

        _Chrono_spec<_CharT> _Conv_spec{._Modifier = _Modifier, ._Type = static_cast<char>(_Type)};
        _Specs._Chrono_specs_list.push_back(_Conv_spec);
    }

    constexpr void _On_lit_char(_CharT _Lit_ch) {
        _Chrono_spec<_CharT> _Lit_char_spec{._Lit_char = _Lit_ch};
        _Specs._Chrono_specs_list.push_back(_Lit_char_spec);
    }

private:
    _Chrono_format_specs<_CharT>& _Specs;
    _ParseContext& _Parse_ctx;

    _NODISCARD static constexpr int _Verify_dynamic_arg_index_in_range(const size_t _Idx) {
        if (_Idx > static_cast<size_t>((numeric_limits<int>::max)())) {
            _THROW(format_error("Dynamic width or precision index too large."));
        }

        return static_cast<int>(_Idx);
    }
};

// assumes that the required '%' at the beginning of a conversion-spec has already been consumed
template <class _CharT, _Chrono_parse_spec_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_conversion_specs(
    const _CharT* _Begin, const _CharT* _End, _Callbacks_type&& _Callbacks) {
    if (_Begin == _End || *_Begin == '}') {
        _THROW(format_error("Invalid format string."));
    }

    char _Mod  = '\0';
    _CharT _Ch = *_Begin;

    if (_Ch == 'E' || _Ch == 'O') { // includes modifier
        _Mod = static_cast<char>(_Ch);
        ++_Begin;
        if (_Begin == _End || *_Begin == '}') {
            _THROW(format_error("Invalid format string - missing type after modifier."));
        }
    }

    _CharT _Type = *_Begin;
    _Callbacks._On_conversion_spec(_Mod, _Type);

    return ++_Begin;
}

template <class _CharT, _Chrono_parse_spec_callbacks<_CharT> _Callbacks_type>
_NODISCARD constexpr const _CharT* _Parse_chrono_format_specs(
    const _CharT* _Begin, const _CharT* _End, _Callbacks_type&& _Callbacks) {
    if (_Begin == _End || *_Begin == '}') {
        return _Begin;
    }

    _Begin = _Parse_align(_Begin, _End, _Callbacks);
    if (_Begin == _End) {
        return _Begin;
    }

    _Begin = _Parse_width(_Begin, _End, _Callbacks);
    if (_Begin == _End) {
        return _Begin;
    }

    if (*_Begin == '.') {
        _Begin = _Parse_precision(_Begin, _End, _Callbacks);
        if (_Begin == _End) {
            return _Begin;
        }
    }

    if (*_Begin == 'L') {
        _Callbacks._On_localized();
        ++_Begin;
        if (_Begin == _End) {
            return _Begin;
        }
    }

    if (*_Begin != '}' && *_Begin != '%') {
        _THROW(format_error("Invalid format string - chrono-specs must begin with conversion-spec"));
    }

    // chrono-spec
    while (_Begin != _End && *_Begin != '}') {
        if (*_Begin == '%') { // conversion-spec
            if (++_Begin == _End) {
                _THROW(format_error("Invalid format string - missing type after %"));
            }

            switch (*_Begin) {
            case 'n':
                _Callbacks._On_lit_char('\n');
                ++_Begin;
                break;
            case 't':
                _Callbacks._On_lit_char('\t');
                ++_Begin;
                break;
            case '%':
                _Callbacks._On_lit_char('%');
                ++_Begin;
                break;
            default: // some other type
                _Begin = _Parse_conversion_specs(_Begin, _End, _Callbacks);
                break;
            }
        } else { // literal-char
            _Callbacks._On_lit_char(*_Begin);
            ++_Begin;
        }
    }

    return _Begin;
}

namespace chrono {
    template <class _Duration>
    struct _Local_time_format_t {
        local_time<_Duration> _Time;
        const string* _Abbrev      = nullptr;
        const seconds* _Offset_sec = nullptr;
    };

    template <class _Duration>
    _NODISCARD _Local_time_format_t<_Duration> local_time_format(const local_time<_Duration> _Time,
        const string* const _Abbrev = nullptr, const seconds* const _Offset_sec = nullptr) {
        return {_Time, _Abbrev, _Offset_sec};
    }

    // Replacement for %S, as put_time does not honor writing fractional seconds.
    template <class _CharT, class _Traits, class _Ty>
    void _Write_seconds(basic_ostream<_CharT, _Traits>&, const _Ty&) {
        _STL_INTERNAL_CHECK(false);
    }

    template <unsigned int _Fractional_width, class _CharT, class _Traits, class _Precision>
    void _Write_fractional_seconds(
        basic_ostream<_CharT, _Traits>& _Os, const seconds& _Seconds, const _Precision& _Subseconds) {
        _Os << _STD format(_STATICALLY_WIDEN(_CharT, "{:02}"), _Seconds.count());
        if constexpr (_Fractional_width > 0) {
            _Os << _STD use_facet<numpunct<_CharT>>(_Os.getloc()).decimal_point();
            if constexpr (treat_as_floating_point_v<typename _Precision::rep>) {
                _Os << _STD format(
                    _STATICALLY_WIDEN(_CharT, "{:0{}.0f}"), _STD floor(_Subseconds.count()), _Fractional_width);
            } else {
                _Os << _STD format(_STATICALLY_WIDEN(_CharT, "{:0{}}"), _Subseconds.count(), _Fractional_width);
            }
        }
    }

    template <class _CharT, class _Traits, class _Duration>
    void _Write_seconds(basic_ostream<_CharT, _Traits>& _Os, const hh_mm_ss<_Duration>& _Val) {
        _Write_fractional_seconds<hh_mm_ss<_Duration>::fractional_width>(_Os, _Val.seconds(), _Val.subseconds());
    }

    template <class _CharT, class _Traits, class _Clock, class _Duration>
    void _Write_seconds(basic_ostream<_CharT, _Traits>& _Os, const time_point<_Clock, _Duration>& _Val) {
        if constexpr (is_same_v<_Clock, utc_clock>) {
            const auto _Lsi = _CHRONO get_leap_second_info(_Val);
            const auto _Dp =
                _CHRONO floor<days>(_Val - _Lsi.elapsed) + _Lsi.elapsed - seconds{_Lsi.is_leap_second ? 1 : 0};
            const hh_mm_ss _Hms{_Val - _Dp};
            constexpr auto _Fractional_width = decltype(_Hms)::fractional_width;
            if (_Lsi.is_leap_second) {
                _Write_fractional_seconds<_Fractional_width>(_Os, _Hms.seconds() + seconds{60}, _Hms.subseconds());
            } else {
                _Write_fractional_seconds<_Fractional_width>(_Os, _Hms.seconds(), _Hms.subseconds());
            }
        } else {
            const auto _Dp = _CHRONO floor<days>(_Val);
            _Write_seconds(_Os, hh_mm_ss{_Val - _Dp});
        }
    }

    template <class _CharT, class _Traits, class _Duration>
    void _Write_seconds(basic_ostream<_CharT, _Traits>& _Os, const _Local_time_format_t<_Duration>& _Val) {
        _Write_seconds(_Os, _Val._Time);
    }

    template <class _CharT, class _Traits, class _Rep, class _Period>
    void _Write_seconds(basic_ostream<_CharT, _Traits>& _Os, const duration<_Rep, _Period>& _Val) {
        const auto _Dp = _CHRONO duration_cast<days>(_Val);
        _Write_seconds(_Os, hh_mm_ss{_Val - _Dp});
    }

    template <class _Ty>
    _NODISCARD tm _Fill_tm(const _Ty& _Val) {
        unsigned int _Day   = 0;
        unsigned int _Month = 0;
        int _Year           = 0;
        int _Yearday        = 0;
        int _Weekday        = 0;
        int _Hours          = 0;
        int _Minutes        = 0;
        int _Seconds        = 0;

        if constexpr (_Is_specialization_v<_Ty, duration>) {
            const auto _Dp = _CHRONO duration_cast<days>(_Val);
            return _Fill_tm(hh_mm_ss{_Val - _Dp});
        } else if constexpr (_Is_specialization_v<_Ty, _Local_time_format_t>) {
            return _Fill_tm(_Val._Time);
        } else if constexpr (is_same_v<_Ty, day>) {
            _Day = static_cast<unsigned int>(_Val);
        } else if constexpr (is_same_v<_Ty, month>) {
            _Month = static_cast<unsigned int>(_Val);
        } else if constexpr (is_same_v<_Ty, year>) {
            _Year = static_cast<int>(_Val);
        } else if constexpr (is_same_v<_Ty, weekday>) {
            _Weekday = static_cast<int>(_Val.c_encoding());
        } else if constexpr (_Is_any_of_v<_Ty, weekday_indexed, weekday_last>) {
            _Weekday = static_cast<int>(_Val.weekday().c_encoding());
        } else if constexpr (is_same_v<_Ty, month_day>) {
            _Day   = static_cast<unsigned int>(_Val.day());
            _Month = static_cast<unsigned int>(_Val.month());
            if (_Val.month() == January) {
                _Yearday = static_cast<int>(_Day) - 1;
            } else if (_Val.month() == February) {
                _Yearday = 31 + static_cast<int>(_Day) - 1;
            }
        } else if constexpr (is_same_v<_Ty, month_day_last>) {
            _Month = static_cast<unsigned int>(_Val.month());
            _Day   = static_cast<unsigned int>(_Last_day_table[(_Month - 1) & 0xF]);
            if (_Val.month() == January) {
                _Yearday = 30;
            }
        } else if constexpr (is_same_v<_Ty, month_weekday>) {
            _Month   = static_cast<unsigned int>(_Val.month());
            _Weekday = static_cast<int>(_Val.weekday_indexed().weekday().c_encoding());
        } else if constexpr (is_same_v<_Ty, month_weekday_last>) {
            _Month   = static_cast<unsigned int>(_Val.month());
            _Weekday = static_cast<int>(_Val.weekday_last().weekday().c_encoding());
        } else if constexpr (is_same_v<_Ty, year_month>) {
            _Month = static_cast<unsigned int>(_Val.month());
            _Year  = static_cast<int>(_Val.year());
        } else if constexpr (_Is_any_of_v<_Ty, year_month_day, year_month_day_last>) {
            _Day   = static_cast<unsigned int>(_Val.day());
            _Month = static_cast<unsigned int>(_Val.month());
            _Year  = static_cast<int>(_Val.year());
            if (_Val.ok()) {
                const year_month_day& _Ymd = _Val;
                _Weekday                   = _Ymd._Calculate_weekday();
                _Yearday = (static_cast<sys_days>(_Val) - static_cast<sys_days>(_Val.year() / January / 1)).count();
            }
        } else if constexpr (_Is_any_of_v<_Ty, year_month_weekday, year_month_weekday_last>) {
            auto _Tm    = _Fill_tm(year_month_day{_Val});
            _Tm.tm_wday = static_cast<int>(_Val.weekday().c_encoding());
            return _Tm;
        } else if constexpr (_Is_specialization_v<_Ty, hh_mm_ss>) {
            _Hours   = _Val.hours().count();
            _Minutes = _Val.minutes().count();
            _Seconds = static_cast<int>(_Val.seconds().count());
        } else if constexpr (_Is_any_of_v<_Ty, sys_info, local_info>) {
            return {}; // none of the valid conversion specifiers need tm fields
        } else if constexpr (_Is_specialization_v<_Ty, time_point>) {
            const auto _Dp = _CHRONO floor<days>(_Val);
            const year_month_day _Ymd{_Dp};
            const hh_mm_ss _Time{_Val - _Dp};
            const auto _Hms = _Fill_tm(_Time);
            auto _Tm        = _Fill_tm(_Ymd);
            _Tm.tm_sec      = _Hms.tm_sec;
            _Tm.tm_min      = _Hms.tm_min;
            _Tm.tm_hour     = _Hms.tm_hour;
            return _Tm;
        }

        tm _Time;
        _Time.tm_sec  = _Seconds;
        _Time.tm_min  = _Minutes;
        _Time.tm_hour = _Hours;
        _Time.tm_mday = static_cast<int>(_Day);
        _Time.tm_mon  = static_cast<int>(_Month) - 1;
        _Time.tm_year = _Year - 1900;
        _Time.tm_yday = _Yearday;
        _Time.tm_wday = _Weekday;
        return _Time;
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const day& _Val) {
        return _Os << (_Val.ok() ? _STD format(_STATICALLY_WIDEN(_CharT, "{:%d}"), _Val)
                                 : _STD format(_STATICALLY_WIDEN(_CharT, "{:%d} is not a valid day"), _Val));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const month& _Val) {
        return _Os << (_Val.ok() ? _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%b}"), _Val)
                                 : _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{} is not a valid month"),
                                     static_cast<unsigned int>(_Val)));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const year& _Val) {
        return _Os << (_Val.ok() ? _STD format(_STATICALLY_WIDEN(_CharT, "{:%Y}"), _Val)
                                 : _STD format(_STATICALLY_WIDEN(_CharT, "{:%Y} is not a valid year"), _Val));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const weekday& _Val) {
        return _Os << (_Val.ok() ? _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%a}"), _Val)
                                 : _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{} is not a valid weekday"),
                                     _Val.c_encoding()));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const weekday_indexed& _Val) {
        const auto _Idx = _Val.index();
        return _Os << (_Idx >= 1 && _Idx <= 5
                           ? _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}[{}]"), _Val.weekday(), _Idx)
                           : _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}[{} is not a valid index]"),
                               _Val.weekday(), _Idx));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const weekday_last& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}[last]"), _Val.weekday());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const month_day& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}/{}"), _Val.month(), _Val.day());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const month_day_last& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}/last"), _Val.month());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const month_weekday& _Val) {
        return _Os << _STD format(
                   _Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}/{:L}"), _Val.month(), _Val.weekday_indexed());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const month_weekday_last& _Val) {
        return _Os << _STD format(
                   _Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L}/{:L}"), _Val.month(), _Val.weekday_last());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const year_month& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{}/{:L}"), _Val.year(), _Val.month());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const year_month_day& _Val) {
        return _Os << (_Val.ok() ? _STD format(_STATICALLY_WIDEN(_CharT, "{:%F}"), _Val)
                                 : _STD format(_STATICALLY_WIDEN(_CharT, "{:%F} is not a valid date"), _Val));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const year_month_day_last& _Val) {
        return _Os << _STD format(
                   _Os.getloc(), _STATICALLY_WIDEN(_CharT, "{}/{:L}"), _Val.year(), _Val.month_day_last());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const year_month_weekday& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{}/{:L}/{:L}"), _Val.year(), _Val.month(),
                   _Val.weekday_indexed());
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(
        basic_ostream<_CharT, _Traits>& _Os, const year_month_weekday_last& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{}/{:L}/{:L}"), _Val.year(), _Val.month(),
                   _Val.weekday_last());
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const hh_mm_ss<_Duration>& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%T}"), _Val);
    }

#pragma warning(push)
#pragma warning(disable : 4365) // 'argument': conversion from 'char' to 'const wchar_t', signed/unsigned mismatch
    template <class _CharT>
    _NODISCARD decltype(auto) _Widen_string(const string& _Str) {
        if constexpr (is_same_v<_CharT, char>) {
            return _Str;
        } else {
            return wstring{_Str.begin(), _Str.end()}; // TRANSITION, should probably use ctype::widen
        }
    }
#pragma warning(pop)

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const sys_info& _Val) {
        return _Os << _STD format(_Os.getloc(),
                   _STATICALLY_WIDEN(_CharT, "begin: {}, end: {}, offset: {}, save: {}, abbrev: {}"), //
                   _Val.begin, _Val.end, _Val.offset, _Val.save, _Widen_string<_CharT>(_Val.abbrev));
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const local_info& _Val) {
        switch (_Val.result) {
        case local_info::unique:
            return _Os << _STD format(_Os.getloc(), //
                       _STATICALLY_WIDEN(_CharT, "result: unique, first: ({})"), //
                       _Val.first);
        case local_info::nonexistent:
            return _Os << _STD format(_Os.getloc(),
                       _STATICALLY_WIDEN(_CharT, "result: nonexistent, first: ({}), second: ({})"), //
                       _Val.first, _Val.second);
        case local_info::ambiguous:
            return _Os << _STD format(_Os.getloc(),
                       _STATICALLY_WIDEN(_CharT, "result: ambiguous, first: ({}), second: ({})"), //
                       _Val.first, _Val.second);
        default:
            return _Os << _STD format(_Os.getloc(), //
                       _STATICALLY_WIDEN(_CharT, "result: {}, first: ({}), second: ({})"), //
                       _Val.result, _Val.first, _Val.second);
        }
    }

    template <class _CharT, class _Traits, class _Duration>
    // clang-format off
        requires (!treat_as_floating_point_v<typename _Duration::rep> && _Duration{1} < days{1})
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const sys_time<_Duration>& _Val) {
        // clang-format on
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%F %T}"), _Val);
    }

    template <class _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const sys_days& _Val) {
        return _Os << year_month_day{_Val};
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const utc_time<_Duration>& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%F %T}"), _Val);
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const tai_time<_Duration>& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%F %T}"), _Val);
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const gps_time<_Duration>& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%F %T}"), _Val);
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const file_time<_Duration>& _Val) {
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%F %T}"), _Val);
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& _Os, const local_time<_Duration>& _Val) {
        return _Os << sys_time<_Duration>{_Val.time_since_epoch()};
    }

    template <class _CharT, class _Traits, class _Duration>
    basic_ostream<_CharT, _Traits>& operator<<(
        basic_ostream<_CharT, _Traits>& _Os, const _Local_time_format_t<_Duration>& _Val) {
        // Doesn't appear in the Standard, but allowed by N4885 [global.functions]/2.
        // Implements N4885 [time.zone.zonedtime.nonmembers]/2 for zoned_time.
        return _Os << _STD format(_Os.getloc(), _STATICALLY_WIDEN(_CharT, "{:L%F %T %Z}"), _Val);
    }

    template <class _CharT, class _Traits, class _Duration, class _TimeZonePtr>
    basic_ostream<_CharT, _Traits>& operator<<(
        basic_ostream<_CharT, _Traits>& _Os, const zoned_time<_Duration, _TimeZonePtr>& _Val) {
        const auto _Info = _Val.get_info();
        return _Os << _Local_time_format_t<_Duration>{_Val.get_local_time(), &_Info.abbrev};
    }

    template <class _CharT>
    struct _Chrono_formatter {
        _Chrono_formatter() = default;

        constexpr explicit _Chrono_formatter(const basic_string_view<_CharT> _Time_zone_abbreviation_)
            : _Time_zone_abbreviation{_Time_zone_abbreviation_} {}

        template <class _Ty>
        _NODISCARD constexpr auto _Parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
            _Chrono_specs_setter<_CharT, basic_format_parse_context<_CharT>> _Callback{_Specs, _Parse_ctx};
            const auto _It =
                _Parse_chrono_format_specs(_Parse_ctx._Unchecked_begin(), _Parse_ctx._Unchecked_end(), _Callback);
            const auto _Res_iter = _Parse_ctx.begin() + (_It - _Parse_ctx._Unchecked_begin());

            if (_It != _Parse_ctx._Unchecked_end() && *_It != '}') {
                _THROW(format_error("Missing '}' in format string."));
            }

            if constexpr (_Is_specialization_v<_Ty, duration>) {
                if constexpr (!treat_as_floating_point_v<typename _Ty::rep>) {
                    if (_Specs._Precision != -1) {
                        _THROW(format_error("Precision specification invalid for chrono::duration type with "
                                            "integral representation type, see N4885 [time.format]/1."));
                    }
                }
            } else {
                if (_Specs._Precision != -1) {
                    _THROW(format_error("Precision specification invalid for non-chrono::duration type, "
                                        "see N4885 [time.format]/1."));
                }
            }

            for (const auto& _Spec : _Specs._Chrono_specs_list) {
                if (_Spec._Type != '\0' && !_Is_valid_type<_Ty>(_Spec._Type)) {
                    _THROW(format_error("Invalid type."));
                }
                _Check_modifier(_Spec._Type, _Spec._Modifier);
            }

            return _Res_iter;
        }

        enum _Allowed_bit : uint8_t { _E_mod = 1, _O_mod = 2, _EO_mod = _E_mod | _O_mod };

        struct _Table_entry {
            char _Type;
            _Allowed_bit _Allowed;
        };

        static constexpr _Table_entry _Table[] = {
            {'c', _E_mod},
            {'C', _E_mod},
            {'d', _O_mod},
            {'e', _O_mod},
            {'H', _O_mod},
            {'I', _O_mod},
            {'m', _O_mod},
            {'M', _O_mod},
            {'S', _O_mod},
            {'u', _O_mod},
            {'U', _O_mod},
            {'V', _O_mod},
            {'w', _O_mod},
            {'W', _O_mod},
            {'x', _E_mod},
            {'X', _E_mod},
            {'y', _EO_mod},
            {'Y', _E_mod},
            {'z', _EO_mod},
        };

        static constexpr void _Check_modifier(const char _Type, const char _Modifier) {
            if (_Modifier == '\0') {
                return;
            }

            const _Allowed_bit _Mod = _Modifier == 'E' ? _E_mod : _O_mod;

            if (auto _It = _RANGES find(_Table, _Type, &_Table_entry::_Type); _It != _STD end(_Table)) {
                if (_It->_Allowed & _Mod) {
                    return;
                }
            }

            _THROW(format_error("Incompatible modifier for type"));
        }

        template <class _Ty>
        _NODISCARD static constexpr bool _Is_valid_type(const char _Type) noexcept {
            if constexpr (_Is_specialization_v<_Ty, duration>) {
                return _Type == 'j' || _Type == 'q' || _Type == 'Q' || _Is_valid_type<hh_mm_ss<seconds>>(_Type);
            } else if constexpr (is_same_v<_Ty, day>) {
                return _Type == 'd' || _Type == 'e';
            } else if constexpr (is_same_v<_Ty, month>) {
                return _Type == 'b' || _Type == 'B' || _Type == 'h' || _Type == 'm';
            } else if constexpr (is_same_v<_Ty, year>) {
                return _Type == 'Y' || _Type == 'y' || _Type == 'C';
            } else if constexpr (_Is_any_of_v<_Ty, weekday, weekday_indexed, weekday_last>) {
                return _Type == 'a' || _Type == 'A' || _Type == 'u' || _Type == 'w';
            } else if constexpr (_Is_any_of_v<_Ty, month_day, month_day_last>) {
                return _Type == 'j' || _Is_valid_type<month>(_Type) || _Is_valid_type<day>(_Type);
            } else if constexpr (_Is_any_of_v<_Ty, month_weekday, month_weekday_last>) {
                return _Is_valid_type<month>(_Type) || _Is_valid_type<weekday>(_Type);
            } else if constexpr (is_same_v<_Ty, year_month>) {
                return _Type == 'g' || _Type == 'G' || _Is_valid_type<year>(_Type) || _Is_valid_type<month>(_Type);
            } else if constexpr (_Is_any_of_v<_Ty, year_month_day, year_month_day_last, year_month_weekday,
                                     year_month_weekday_last>) {
                return _Type == 'D' || _Type == 'F' || _Type == 'g' || _Type == 'G' || _Type == 'j' || _Type == 'U'
                    || _Type == 'V' || _Type == 'W' || _Is_valid_type<year>(_Type) || _Is_valid_type<month>(_Type)
                    || _Is_valid_type<day>(_Type) || _Is_valid_type<weekday>(_Type);
            } else if constexpr (_Is_specialization_v<_Ty, hh_mm_ss>) {
                return _Type == 'H' || _Type == 'I' || _Type == 'M' || _Type == 'S' || _Type == 'r' || _Type == 'R'
                    || _Type == 'T' || _Type == 'p';
            } else if constexpr (_Is_any_of_v<_Ty, sys_info, local_info>) {
                return _Type == 'z' || _Type == 'Z';
            } else if constexpr (_Is_specialization_v<_Ty, time_point>) {
                if constexpr (!is_same_v<typename _Ty::clock, local_t>) {
                    if (_Type == 'z' || _Type == 'Z') {
                        return true;
                    }
                }
                return _Type == 'c' || _Type == 'x' || _Type == 'X' || _Is_valid_type<year_month_day>(_Type)
                    || _Is_valid_type<hh_mm_ss<seconds>>(_Type);
            } else if constexpr (_Is_specialization_v<_Ty, _Local_time_format_t>) {
                return _Type == 'z' || _Type == 'Z' || _Is_valid_type<decltype(_Ty::_Time)>(_Type);
            } else {
                static_assert(_Always_false<_Ty>, "should be unreachable");
            }
        }

        template <class _FormatContext, class _Ty>
        _NODISCARD auto _Write(_FormatContext& _FormatCtx, const _Ty& _Val, const tm& _Time) const {
            basic_ostringstream<_CharT> _Stream;

            _Stream.imbue(_Specs._Localized ? _FormatCtx.locale() : locale::classic());
            if (_Specs._Chrono_specs_list.empty()) {
                _Stream << _Val; // N4885 [time.format]/6
            } else {
                if constexpr (_Is_specialization_v<_Ty, hh_mm_ss>) {
                    if (_Val.is_negative()) {
                        _Stream << _CharT{'-'};
                    }
                } else if constexpr (_Is_specialization_v<_Ty, duration>) {
                    if (_Val < _Ty::zero()) {
                        _Stream << _CharT{'-'};
                    }
                }

                for (const auto& _Spec : _Specs._Chrono_specs_list) {
                    if (_Spec._Lit_char != _CharT{'\0'}) {
                        _Stream << _Spec._Lit_char;
                        continue;
                    }

                    // We need to manually do certain writes, either because the specification is different from
                    // put_time or custom logic is needed.
                    if (_Custom_write(_Stream, _Spec, _Time, _Val)) {
                        continue;
                    }

                    _Validate_specifiers(_Spec, _Val);

                    _CharT _Fmt_str[4];
                    _Stream << _STD put_time<_CharT>(&_Time, _Fmt_string(_Spec, _Fmt_str));
                }
            }

            int _Estimated_width = -1;
            (void) _Measure_string_prefix(_Stream.view(), _Estimated_width);
            return _Write_aligned(_STD move(_FormatCtx.out()), _Estimated_width, _Specs, _Fmt_align::_Left,
                [&](auto _Out) { return _Fmt_write(_STD move(_Out), _Stream.view()); });
        }

        // This echoes the functionality of put_time, but is able to handle invalid dates (when !ok()) since the
        // Standard mandates that invalid dates still be formatted properly.  For example, put_time isn't able to handle
        // a tm_mday of 40, but format("{:%d}", day{40}) should return "40" and operator<< for day prints
        // "40 is not a valid day".
        template <class _Ty>
        bool _Custom_write(
            basic_ostream<_CharT>& _Os, const _Chrono_spec<_CharT>& _Spec, const tm& _Time, const _Ty& _Val) const {
            if constexpr (is_same_v<_Ty, local_info>) {
                if (_Val.result != local_info::unique) {
                    _THROW(format_error("Cannot print non-unique local_info"));
                }
            }

            const auto _Year         = _Time.tm_year + 1900;
            const auto _Month        = _Time.tm_mon + 1;
            const bool _Has_modifier = _Spec._Modifier != '\0';
            switch (_Spec._Type) {
            case 'd': // Print days as a decimal, even if invalid.
            case 'e':
                // Most months have a proper last day, but February depends on the year.
                if constexpr (is_same_v<_Ty, month_day_last>) {
                    if (_Val.month() == February) {
                        _THROW(format_error("Cannot print the last day of February without a year"));
                    }

                    if (!_Val.ok()) {
                        return false;
                    }
                }

                if (_Has_modifier) {
                    return false;
                }

                if (_Time.tm_mday < 10) {
                    _Os << (_Spec._Type == 'd' ? _CharT{'0'} : _CharT{' '});
                }
                _Os << _Time.tm_mday;
                return true;
            case 'g':
            case 'G':
                if constexpr (is_same_v<_Ty, year_month>) {
                    if (_Val.month() == January || _Val.month() == December) {
                        _THROW(format_error(
                            "The ISO week-based year for a year_month of January or December is ambiguous."));
                    }

                    if (!_Val.ok()) {
                        _THROW(format_error("The ISO week-based year for an out-of-bounds year_month is ambiguous."));
                    }

                    const char _Gregorian_type = _Spec._Type == 'g' ? 'y' : 'Y';
                    _CharT _Fmt_str[4];
                    _Os << _STD put_time(&_Time, _Fmt_string({._Type = _Gregorian_type}, _Fmt_str));
                    return true;
                }

                return false;
            case 'r':
                // put_time uses _Strftime in order to bypass reference-counting that locale uses. This function
                // takes the locale information by pointer, but the pointer (from _Gettnames) returns a copy.
                // _Strftime delegates to other functions but eventually (for the C locale) has the %r specifier
                // rewritten. It checks for the locale by comparing pointers, which do not compare equal as we have
                // a copy of the pointer instead of the original. Therefore, we replace %r for the C locale
                // ourselves.
                if (_Os.getloc() == locale::classic()) {
                    _Os << _STD put_time(&_Time, _STATICALLY_WIDEN(_CharT, "%I:%M:%S %p"));
                    return true;
                }

                return false;
            case 'j':
                if constexpr (_Is_specialization_v<_Ty, duration>) {
                    _Os << _STD abs(_CHRONO duration_cast<days>(_Val).count());
                    return true;
                }
                return false;
            case 'q':
                if constexpr (_Is_specialization_v<_Ty, duration>) {
                    _Write_unit_suffix<typename _Ty::period>(_Os);
                }
                return true;
            case 'Q':
                if constexpr (_Is_specialization_v<_Ty, duration>) {
                    _Os << _STD abs(_Val.count());
                }
                return true;
            case 'm': // Print months as a decimal, even if invalid.
                if (_Has_modifier) {
                    return false;
                }

                if (_Month < 10) {
                    _Os << _CharT{'0'};
                }
                _Os << _Month;
                return true;
            case 'Y': // Print years as a decimal, even if invalid.
                if (_Has_modifier) {
                    return false;
                }

                if (_Year < 0) {
                    _Os << _CharT{'-'};
                }
                _Os << _STD format(_STATICALLY_WIDEN(_CharT, "{:04}"), _STD abs(_Year));
                return true;
            case 'y': // Print the two-digit year as a decimal, even if invalid.
                if (_Has_modifier) {
                    return false;
                }
                _Os << _STD format(
                    _STATICALLY_WIDEN(_CharT, "{:02}"), _Time_parse_fields::_Decompose_year(_Year).second);
                return true;
            case 'C': // Print the century as a decimal, even if invalid.
                if (_Has_modifier) {
                    return false;
                }

                if (_Year < 0) {
                    _Os << _CharT{'-'};
                }
                _Os << _STD format(_STATICALLY_WIDEN(_CharT, "{:02}"),
                    _STD abs(_Time_parse_fields::_Decompose_year(_Year).first) / 100);
                return true;
            case 'F': // Print YMD even if invalid.
                _Custom_write(_Os, {._Type = 'Y'}, _Time, _Val);
                _Os << _CharT{'-'};
                _Custom_write(_Os, {._Type = 'm'}, _Time, _Val);
                _Os << _CharT{'-'};
                _Custom_write(_Os, {._Type = 'd'}, _Time, _Val);
                return true;
            case 'D': // Print YMD even if invalid.
                _Custom_write(_Os, {._Type = 'm'}, _Time, _Val);
                _Os << _CharT{'/'};
                _Custom_write(_Os, {._Type = 'd'}, _Time, _Val);
                _Os << _CharT{'/'};
                _Custom_write(_Os, {._Type = 'y'}, _Time, _Val);
                return true;
            case 'T':
                // Alias for %H:%M:%S but we need to rewrite %S to display fractions of a second.
                _Validate_specifiers({._Type = 'H'}, _Val);
                _Os << _STD put_time(&_Time, _STATICALLY_WIDEN(_CharT, "%H:%M:"));
                [[fallthrough]];
            case 'S':
                if (_Has_modifier) {
                    return false;
                }
                _Write_seconds(_Os, _Val);
                return true;
            case 'Z':
                if constexpr (is_same_v<_Ty, sys_info>) {
                    _Os << _Widen_string<_CharT>(_Val.abbrev);
                } else if constexpr (is_same_v<_Ty, local_info>) {
                    _Os << _Widen_string<_CharT>(_Val.first.abbrev);
                } else if constexpr (_Is_specialization_v<_Ty, _Local_time_format_t>) {
                    if (_Val._Abbrev == nullptr) {
                        _THROW(format_error("Cannot print local-time-format-t with null abbrev."));
                    }
                    _Os << _Widen_string<_CharT>(*_Val._Abbrev);
                } else {
                    _Os << _Time_zone_abbreviation;
                }
                return true;
            case 'z':
                {
                    hh_mm_ss<seconds> _Offset;

                    if constexpr (is_same_v<_Ty, sys_info>) {
                        _Offset = hh_mm_ss<seconds>{_Val.offset};
                    } else if constexpr (is_same_v<_Ty, local_info>) {
                        _Offset = hh_mm_ss<seconds>{_Val.first.offset};
                    } else if constexpr (_Is_specialization_v<_Ty, _Local_time_format_t>) {
                        if (_Val._Offset_sec == nullptr) {
                            _THROW(format_error("Cannot print local-time-format-t with null offset_sec."));
                        }
                        _Offset = hh_mm_ss<seconds>{*_Val._Offset_sec};
                    } else {
                        _Offset = hh_mm_ss<seconds>{};
                    }

                    const auto _Sign = _Offset.is_negative() ? _CharT{'-'} : _CharT{'+'};
                    const auto _Separator =
                        _Has_modifier ? _STATICALLY_WIDEN(_CharT, ":") : _STATICALLY_WIDEN(_CharT, "");

                    _Os << _STD format(_STATICALLY_WIDEN(_CharT, "{}{:02}{}{:02}"), _Sign, _Offset.hours().count(),
                        _Separator, _Offset.minutes().count());
                    return true;
                }
            default:
                return false;
            }
        }

        template <class _Ty>
        static void _Validate_specifiers(const _Chrono_spec<_CharT>& _Spec, const _Ty& _Val) {
            // clang-format off
            if constexpr (_Is_specialization_v<_Ty, duration> || is_same_v<_Ty, sys_info>
                || _Is_specialization_v<_Ty, time_point> || _Is_specialization_v<_Ty, _Local_time_format_t>) {
                return;
            }
            // clang-format on

            if constexpr (_Is_specialization_v<_Ty, hh_mm_ss>) {
                if (_Spec._Type == 'H' && _Val.hours() >= hours{24}) {
                    _THROW(format_error("Cannot localize hh_mm_ss with an absolute value of 24 hours or more."));
                }
                return;
            }

            constexpr bool _Is_ymd =
                _Is_any_of_v<_Ty, year_month_day, year_month_day_last, year_month_weekday, year_month_weekday_last>;

            const auto _Validate = [&] {
                switch (_Spec._Type) {
                case 'a':
                case 'A':
                case 'u':
                case 'w':
                    if constexpr (_Is_any_of_v<_Ty, weekday, weekday_last>) {
                        return _Val.ok();
                    } else if constexpr (_Is_any_of_v<_Ty, weekday_indexed, year_month_weekday,
                                             year_month_weekday_last>) {
                        return _Val.weekday().ok();
                    } else if constexpr (is_same_v<_Ty, month_weekday>) {
                        return _Val.weekday_indexed().weekday().ok();
                    } else if constexpr (is_same_v<_Ty, month_weekday_last>) {
                        return _Val.weekday_last().ok();
                    } else if constexpr (_Is_any_of_v<_Ty, year_month_day, year_month_day_last>) {
                        return _Val.ok();
                    }
                    break;

                case 'b':
                case 'B':
                case 'h':
                case 'm':
                    if constexpr (is_same_v<_Ty, month>) {
                        return _Val.ok();
                    } else if constexpr (_Is_any_of_v<_Ty, month_day, month_day_last, month_weekday, month_weekday_last,
                                             year_month> || _Is_ymd) {
                        return _Val.month().ok();
                    }
                    break;

                case 'C':
                case 'y':
                case 'Y':
                    if constexpr (is_same_v<_Ty, year>) {
                        return _Val.ok();
                    } else if constexpr (_Is_any_of_v<_Ty, year_month> || _Is_ymd) {
                        return _Val.year().ok();
                    }
                    break;

                case 'd':
                case 'e':
                    if constexpr (_Is_any_of_v<_Ty, day, month_day_last>) {
                        return _Val.ok();
                    } else if constexpr (is_same_v<_Ty, month_day>) {
                        return _Val.day().ok();
                    } else if constexpr (_Is_ymd) {
                        const year_month_day& _Ymd{_Val};
                        return _Ymd.day().ok();
                    }
                    break;

                case 'D':
                case 'F':
                    if constexpr (_Has_ok<_Ty>) {
                        return _Val.ok();
                    }
                    break;

                case 'j':
                    if constexpr (is_same_v<_Ty, month_day>) {
                        if (_Val.month() > February) {
                            _THROW(format_error("The day of year for a month_day past February is ambiguous."));
                        }
                        return true;
                    } else if constexpr (is_same_v<_Ty, month_day_last>) {
                        if (_Val.month() >= February) {
                            _THROW(
                                format_error("The day of year for a month_day_last other than January is ambiguous"));
                        }
                        return true;
                    } else if constexpr (_Is_ymd) {
                        return _Val.ok();
                    }
                    break;

                case 'g':
                case 'G':
                case 'U':
                case 'V':
                case 'W':
                    if constexpr (_Is_ymd) {
                        return _Val.ok();
                    }
                    break;

                default:
                    if constexpr (_Has_ok<_Ty>) {
                        return _Val.ok();
                    }
                    return true;
                }
                _STL_INTERNAL_CHECK(false);
                return false;
            };
            if (!_Validate()) {
                _THROW(format_error("Cannot localize out-of-bounds time point."));
            }
        }

        _NODISCARD static const _CharT* _Fmt_string(const _Chrono_spec<_CharT>& _Spec, _CharT (&_Fmt_str)[4]) {
            size_t _Next_idx      = 0;
            _Fmt_str[_Next_idx++] = _CharT{'%'};
            if (_Spec._Modifier != '\0') {
                _Fmt_str[_Next_idx++] = static_cast<_CharT>(_Spec._Modifier);
            }
            _Fmt_str[_Next_idx++] = static_cast<_CharT>(_Spec._Type);
            _Fmt_str[_Next_idx]   = _CharT{'\0'};
            return _Fmt_str;
        }

        _Chrono_format_specs<_CharT> _Specs{};
        basic_string_view<_CharT> _Time_zone_abbreviation{};
    };
} // namespace chrono

template <class _Ty, class _CharT>
struct _Fill_tm_formatter {
    constexpr auto parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
        return _Impl.template _Parse<_Ty>(_Parse_ctx);
    }

    template <class _FormatContext>
    auto format(const _Ty& _Val, _FormatContext& _FormatCtx) const {
        return _Impl._Write(_FormatCtx, _Val, _Fill_tm(_Val));
    }

private:
    _CHRONO _Chrono_formatter<_CharT> _Impl;
};

template <class _Rep, class _Period, class _CharT>
struct formatter<_CHRONO duration<_Rep, _Period>, _CharT>
    : _Fill_tm_formatter<_CHRONO duration<_Rep, _Period>, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO day, _CharT> //
    : _Fill_tm_formatter<_CHRONO day, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO month, _CharT> //
    : _Fill_tm_formatter<_CHRONO month, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO year, _CharT> //
    : _Fill_tm_formatter<_CHRONO year, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO weekday, _CharT> //
    : _Fill_tm_formatter<_CHRONO weekday, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO weekday_indexed, _CharT> //
    : _Fill_tm_formatter<_CHRONO weekday_indexed, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO weekday_last, _CharT> //
    : _Fill_tm_formatter<_CHRONO weekday_last, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO month_day, _CharT> //
    : _Fill_tm_formatter<_CHRONO month_day, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO month_day_last, _CharT> //
    : _Fill_tm_formatter<_CHRONO month_day_last, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO month_weekday, _CharT> //
    : _Fill_tm_formatter<_CHRONO month_weekday, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO month_weekday_last, _CharT> //
    : _Fill_tm_formatter<_CHRONO month_weekday_last, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO year_month, _CharT> //
    : _Fill_tm_formatter<_CHRONO year_month, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO year_month_day, _CharT> //
    : _Fill_tm_formatter<_CHRONO year_month_day, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO year_month_day_last, _CharT> //
    : _Fill_tm_formatter<_CHRONO year_month_day_last, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO year_month_weekday, _CharT> //
    : _Fill_tm_formatter<_CHRONO year_month_weekday, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO year_month_weekday_last, _CharT>
    : _Fill_tm_formatter<_CHRONO year_month_weekday_last, _CharT> {};

template <class _Rep, class _Period, class _CharT>
struct formatter<_CHRONO hh_mm_ss<_CHRONO duration<_Rep, _Period>>, _CharT>
    : _Fill_tm_formatter<_CHRONO hh_mm_ss<_CHRONO duration<_Rep, _Period>>, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO sys_info, _CharT> //
    : _Fill_tm_formatter<_CHRONO sys_info, _CharT> {};

template <class _CharT>
struct formatter<_CHRONO local_info, _CharT> //
    : _Fill_tm_formatter<_CHRONO local_info, _CharT> {};

template <class _Duration, class _CharT>
struct formatter<_CHRONO sys_time<_Duration>, _CharT> {
    constexpr auto parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
        return _Impl.template _Parse<_CHRONO sys_time<_Duration>>(_Parse_ctx);
    }

    template <class _FormatContext>
    auto format(const _CHRONO sys_time<_Duration>& _Val, _FormatContext& _FormatCtx) const {
        return _Impl._Write(_FormatCtx, _Val, _Fill_tm(_Val));
    }

private:
    _CHRONO _Chrono_formatter<_CharT> _Impl{_STATICALLY_WIDEN(_CharT, "UTC")};
};

template <class _Duration, class _CharT>
struct formatter<_CHRONO utc_time<_Duration>, _CharT> {
    constexpr auto parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
        return _Impl.template _Parse<_CHRONO utc_time<_Duration>>(_Parse_ctx);
    }

    template <class _FormatContext>
    auto format(const _CHRONO utc_time<_Duration>& _Val, _FormatContext& _FormatCtx) const {
        const auto _Sys = _CHRONO utc_clock::to_sys(_Val);
        return _Impl._Write(_FormatCtx, _Val, _Fill_tm(_Sys));
    }

private:
    _CHRONO _Chrono_formatter<_CharT> _Impl{_STATICALLY_WIDEN(_CharT, "UTC")};
};

template <class _Duration, class _CharT>
struct formatter<_CHRONO tai_time<_Duration>, _CharT> {
    constexpr auto parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
        return _Impl.template _Parse<_CHRONO tai_time<_Duration>>(_Parse_ctx);
    }

    template <class _FormatContext>
    auto format(const _CHRONO tai_time<_Duration>& _Val, _FormatContext& _FormatCtx) const {
        using namespace chrono;
        using _Common = common_type_t<_Duration, days>; // slightly optimize by performing conversion at compile time
        constexpr _Common _Offset{sys_days{year{1970} / January / 1} - sys_days{year{1958} / January / 1}};
        const auto _Sys = sys_time<_Duration>{_Val.time_since_epoch()} - _Offset;
        return _Impl._Write(_FormatCtx, _Val, _Fill_tm(_Sys));
    }

private:
    _CHRONO _Chrono_formatter<_CharT> _Impl{_STATICALLY_WIDEN(_CharT, "TAI")};
};

template <class _Duration, class _CharT>
struct formatter<_CHRONO gps_time<_Duration>, _CharT> {
    constexpr auto parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
        return _Impl.template _Parse<_CHRONO gps_time<_Duration>>(_Parse_ctx);
    }

    template <class _FormatContext>
    auto format(const _CHRONO gps_time<_Duration>& _Val, _FormatContext& _FormatCtx) const {
        using namespace chrono;
        using _Common = common_type_t<_Duration, days>; // slightly optimize by performing conversion at compile time
        constexpr _Common _Offset{sys_days{year{1980} / January / Sunday[1]} - sys_days{year{1970} / January / 1}};
        const auto _Sys = sys_time<_Duration>{_Val.time_since_epoch()} + _Offset;
        return _Impl._Write(_FormatCtx, _Val, _Fill_tm(_Sys));
    }

private:
    _CHRONO _Chrono_formatter<_CharT> _Impl{_STATICALLY_WIDEN(_CharT, "GPS")};
};

template <class _Duration, class _CharT>
struct formatter<_CHRONO file_time<_Duration>, _CharT> {
    constexpr auto parse(basic_format_parse_context<_CharT>& _Parse_ctx) {
        return _Impl.template _Parse<_CHRONO file_time<_Duration>>(_Parse_ctx);
    }

    template <class _FormatContext>
    auto format(const _CHRONO file_time<_Duration>& _Val, _FormatContext& _FormatCtx) const {
        const auto _Utc = _CHRONO file_clock::to_utc(_Val);
        const auto _Sys = _CHRONO utc_clock::to_sys(_Utc);
        return _Impl._Write(_FormatCtx, _Utc, _Fill_tm(_Sys));
    }

private:
    _CHRONO _Chrono_formatter<_CharT> _Impl{_STATICALLY_WIDEN(_CharT, "UTC")};
};

template <class _Duration, class _CharT>
struct formatter<_CHRONO local_time<_Duration>, _CharT> //
    : _Fill_tm_formatter<_CHRONO local_time<_Duration>, _CharT> {};

template <class _Duration, class _CharT>
struct formatter<_CHRONO _Local_time_format_t<_Duration>, _CharT>
    : _Fill_tm_formatter<_CHRONO _Local_time_format_t<_Duration>, _CharT> {};

template <class _Duration, class _TimeZonePtr, class _CharT>
struct formatter<_CHRONO zoned_time<_Duration, _TimeZonePtr>, _CharT>
    : formatter<_CHRONO _Local_time_format_t<_Duration>, _CharT> {

    template <class _FormatContext>
    auto format(const _CHRONO zoned_time<_Duration, _TimeZonePtr>& _Val, _FormatContext& _FormatCtx) const {
        using _Mybase    = formatter<_CHRONO _Local_time_format_t<_Duration>, _CharT>;
        const auto _Info = _Val.get_info();
        return _Mybase::format({_Val.get_local_time(), &_Info.abbrev, &_Info.offset}, _FormatCtx);
    }
};

namespace chrono {
    template <class _Duration>
    _NODISCARD string nonexistent_local_time::_Make_string(const local_time<_Duration>& _Tp, const local_info& _Info) {
        ostringstream _Os;
        _Os << _Tp << " is in a gap between\n"
            << local_seconds{_Info.first.end.time_since_epoch()} + _Info.first.offset << ' ' << _Info.first.abbrev
            << " and\n"
            << local_seconds{_Info.second.begin.time_since_epoch()} + _Info.second.offset << ' ' << _Info.second.abbrev
            << " which are both equivalent to\n"
            << _Info.first.end << " UTC";
        return _STD move(_Os).str();
    }

    template <class _Duration>
    _NODISCARD string ambiguous_local_time::_Make_string(const local_time<_Duration>& _Tp, const local_info& _Info) {
        ostringstream _Os;
        _Os << _Tp << " is ambiguous. It could be\n"
            << _Tp << ' ' << _Info.first.abbrev << " == " << _Tp - _Info.first.offset << " UTC or\n"
            << _Tp << ' ' << _Info.second.abbrev << " == " << _Tp - _Info.second.offset << " UTC";
        return _STD move(_Os).str();
    }
} // namespace chrono
#endif // __cpp_lib_format

inline namespace literals {
    inline namespace chrono_literals {
        _NODISCARD constexpr _CHRONO day operator"" d(unsigned long long _Day) noexcept {
            return _CHRONO day{static_cast<unsigned int>(_Day)};
        }
        _NODISCARD constexpr _CHRONO year operator"" y(unsigned long long _Year) noexcept {
            return _CHRONO year{static_cast<int>(_Year)};
        }
    } // namespace chrono_literals
} // namespace literals
#endif // _HAS_CXX20

#undef _STATICALLY_WIDEN

_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CHRONO_
