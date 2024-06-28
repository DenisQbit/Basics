// sstream standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _SSTREAM_
#define _SSTREAM_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <istream>
#include <string>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
_STD_BEGIN

template <class _Elem, class _Traits, class _Alloc>
class basic_stringbuf
    : public basic_streambuf<_Elem, _Traits> { // stream buffer maintaining an allocated character array
public:
    using allocator_type = _Alloc;
    using _Mysb          = basic_streambuf<_Elem, _Traits>;
    using _Mystr         = basic_string<_Elem, _Traits, _Alloc>;
    using _Mysize_type   = typename _Mystr::size_type;

#if _HAS_CXX20
    using _Mystr_view = basic_string_view<_Elem, _Traits>;
#endif // _HAS_CXX20

    basic_stringbuf() : _Seekhigh(nullptr), _Mystate(_Getstate(ios_base::in | ios_base::out)), _Al() {}

    explicit basic_stringbuf(ios_base::openmode _Mode) : _Seekhigh(nullptr), _Mystate(_Getstate(_Mode)), _Al() {}

    explicit basic_stringbuf(const _Mystr& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
        : _Al(_Str.get_allocator()) {
        _Init(_Str.c_str(), _Str.size(), _Getstate(_Mode));
    }

#if _HAS_CXX20
    explicit basic_stringbuf(const _Alloc& _Al_) : basic_stringbuf(ios_base::in | ios_base::out, _Al_) {}

    basic_stringbuf(ios_base::openmode _Mode, const _Alloc& _Al_)
        : _Seekhigh(nullptr), _Mystate(_Getstate(_Mode)), _Al(_Al_) {}

    explicit basic_stringbuf(_Mystr&& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
        : _Al(_STD move(_Str._Getal())) {
        _Init_string_inplace(_STD move(_Str), _Getstate(_Mode));
    }

    template <class _Alloc2>
    basic_stringbuf(const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode, const _Alloc& _Al_)
        : _Al(_Al_) {
        _Init(_Str.c_str(), _Str.size(), _Getstate(_Mode));
    }

    template <class _Alloc2>
    basic_stringbuf(const basic_string<_Elem, _Traits, _Alloc2>& _Str, const _Alloc& _Al_)
        : basic_stringbuf(_Str, ios_base::in | ios_base::out, _Al_) {}

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    explicit basic_stringbuf(
        const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
        : basic_stringbuf(_Str, _Mode, _Alloc{}) {}

    basic_stringbuf(basic_stringbuf&& _Right, const _Alloc& _Al_) : _Al(_Al_) {
        _Assign_rv(_STD move(_Right));
    }
#endif // _HAS_CXX20

    basic_stringbuf(basic_stringbuf&& _Right) {
        _Assign_rv(_STD move(_Right));
    }

    basic_stringbuf& operator=(basic_stringbuf&& _Right) {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    void _Assign_rv(basic_stringbuf&& _Right) {
        if (this != _STD addressof(_Right)) {
            _Tidy();
            this->swap(_Right);
        }
    }

    void swap(basic_stringbuf& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            if constexpr (!allocator_traits<_Alloc>::propagate_on_container_swap::value) {
                _STL_ASSERT(
                    _Al == _Right._Al, "The allocators of basic_stringbuf should propagate or be equal on swap.");
            }

            _Mysb::swap(_Right);
            _STD swap(_Seekhigh, _Right._Seekhigh);
            _STD swap(_Mystate, _Right._Mystate);
            // The same as basic_string::swap
            _Pocs(_Al, _Right._Al);
        }
    }

    basic_stringbuf(const basic_stringbuf&) = delete;
    basic_stringbuf& operator=(const basic_stringbuf&) = delete;

    virtual ~basic_stringbuf() noexcept {
        _Tidy();
    }

    enum { // constants for bits in stream state
        _Allocated   = 1, // set if character array storage has been allocated, eback() points to allocated storage
        _Constant    = 2, // set if character array nonmutable
        _Noread      = 4, // set if character array cannot be read
        _Append      = 8, // set if all writes are appends
        _Atend       = 16, // set if initial writes are appends
        _From_rvalue = 32 // set if character array is released from an rvalue of basic_string
    };

    using int_type = typename _Traits::int_type;
    using pos_type = typename _Traits::pos_type;
    using off_type = typename _Traits::off_type;

    struct _Buffer_view {
        _Elem* _Ptr;
        _Mysize_type _Size;
        _Mysize_type _Res;
    };

    _NODISCARD _Buffer_view _Get_buffer_view() const noexcept {
        _Buffer_view _Result{};
        if ((!(_Mystate & _Constant) || (_Mystate & _From_rvalue)) && _Mysb::pptr()) {
            // writable, make string view from write buffer
            const auto _Base = _Mysb::pbase();
            _Result._Ptr     = _Base;
            _Result._Size    = static_cast<_Mysize_type>((_STD max)(_Mysb::pptr(), _Seekhigh) - _Base);
            _Result._Res     = static_cast<_Mysize_type>(_Mysb::epptr() - _Base);
        } else if (!(_Mystate & _Noread) && _Mysb::gptr()) {
            // readable, make string view from read buffer
            const auto _Base = _Mysb::eback();
            _Result._Ptr     = _Base;
            _Result._Size    = static_cast<_Mysize_type>(_Mysb::egptr() - _Base);
            _Result._Res     = _Result._Size;
        }
        return _Result;
    }

#if _HAS_CXX20
    _NODISCARD _Mystr str() const&
#else
    _NODISCARD _Mystr str() const
#endif // _HAS_CXX20
    {
        _Mystr _Result(_Al);
        const auto _View = _Get_buffer_view();
        if (_View._Ptr) {
            _Result.assign(_View._Ptr, _View._Size);
        }
        return _Result;
    }

    void str(const _Mystr& _Newstr) { // replace character array from string
        _Tidy();
        _Init(_Newstr.c_str(), _Newstr.size(), _Mystate);
    }

#if _HAS_CXX20
    template <class _Alloc2, enable_if_t<_Is_allocator<_Alloc2>::value, int> = 0>
    _NODISCARD basic_string<_Elem, _Traits, _Alloc2> str(const _Alloc2& _Al) const {
        return basic_string<_Elem, _Traits, _Alloc2>{view(), _Al};
    }

    // The buffer cannot be moved to a string directly, because
    // the buffer may already be full, and the terminating char is not '\0'.
    // In that case, copy the string as usual.
    _NODISCARD _Mystr str() && {
        _Mystr _Result{_String_constructor_rvalue_allocator_tag{}, _STD move(_Al)};
        const auto _View = _Get_buffer_view();
        // _Size cannot be larger than _Res, but it could be equal,
        // because basic_stringbuf doesn't allocate for the terminating '\0'.
        if (_View._Size == _View._Res) {
            // the buffer is full
            _Result.assign(_View._Ptr, _View._Size);
        } else {
            _Traits::assign(_View._Ptr[_View._Size], _Elem());
            if (_Result._Move_assign_from_buffer(_View._Ptr, _View._Size, _View._Res)) {
                _Mystate &= ~_Allocated;
            }
        }
        _Tidy();
        return _Result;
    }

    _NODISCARD _Mystr_view view() const noexcept {
        const auto _View = _Get_buffer_view();
        return _Mystr_view{_View._Ptr, _View._Size};
    }

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    void str(const basic_string<_Elem, _Traits, _Alloc2>& _Newstr) {
        _Tidy();
        _Init(_Newstr.c_str(), _Newstr.size(), _Mystate);
    }

    void _Str(_Mystr&& _Newstr, _Equal_allocators) {
        _Tidy();
        _Pocma(_Al, _Newstr._Getal());
        _Init_string_inplace(_STD move(_Newstr), _Mystate);
    }

    void _Str(_Mystr&& _Newstr, _Propagate_allocators) {
        _Str(_STD move(_Newstr), _Equal_allocators{});
    }

    void _Str(_Mystr&& _Newstr, _No_propagate_allocators) {
        if (_Al == _Newstr._Getal()) {
            _Str(_STD move(_Newstr), _Equal_allocators{});
        } else {
            _Tidy();
            _Init(_Newstr.c_str(), _Newstr.size(), _Mystate);
        }
    }

    void str(_Mystr&& _Newstr) {
        _Str(_STD move(_Newstr), _Choose_pocma<_Alloc>{});
    }

    _NODISCARD allocator_type get_allocator() const noexcept {
        return _Al;
    }
#endif // _HAS_CXX20

protected:
    virtual int_type overflow(int_type _Meta = _Traits::eof()) { // put an element to stream
        if (_Mystate & _Constant) {
            return _Traits::eof(); // array nonmutable, fail
        }

        if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
            return _Traits::not_eof(_Meta); // EOF, return success code
        }

        const auto _Pptr  = _Mysb::pptr();
        const auto _Epptr = _Mysb::epptr();
        if (_Pptr && _Pptr < _Epptr) { // room in buffer, store it
            *_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
            _Seekhigh        = _Pptr + 1;
            return _Meta;
        }

        // grow buffer and store element
        size_t _Oldsize    = 0;
        const auto _Oldptr = _Mysb::eback();
        if (_Pptr) {
            _Oldsize = static_cast<size_t>(_Epptr - _Oldptr);
        }

        size_t _Newsize;
        if (_Oldsize < _MINSIZE) {
            _Newsize = _MINSIZE;
        } else if (_Oldsize < INT_MAX / 2) { // grow by 50 percent
            _Newsize = _Oldsize << 1;
        } else if (_Oldsize < INT_MAX) {
            _Newsize = INT_MAX;
        } else { // buffer can't grow, fail
            return _Traits::eof();
        }

        const auto _Newptr = _Unfancy(_Al.allocate(_Newsize));
        _Traits::copy(_Newptr, _Oldptr, _Oldsize);

        const auto _New_pnext = _Newptr + _Oldsize;
        _Seekhigh             = _New_pnext + 1; // to include _Meta

        _Mysb::setp(_Newptr, _New_pnext, _Newptr + _Newsize);
        if (_Mystate & _Noread) { // maintain eback() == allocated pointer invariant
            _Mysb::setg(_Newptr, nullptr, _Newptr);
        } else { // if readable, set the get area to initialized region
            _Mysb::setg(_Newptr, _Newptr + (_Mysb::gptr() - _Oldptr), _Seekhigh);
        }

        if (_Mystate & _Allocated) {
            _Al.deallocate(_Ptr_traits::pointer_to(*_Oldptr), _Oldsize);
        }

        _Mystate |= _Allocated;
        *_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
        return _Meta;
    }

    virtual int_type pbackfail(int_type _Meta = _Traits::eof()) { // put an element back to stream
        const auto _Gptr = _Mysb::gptr();
        if (!_Gptr || _Gptr <= _Mysb::eback()
            || (!_Traits::eq_int_type(_Traits::eof(), _Meta) && !_Traits::eq(_Traits::to_char_type(_Meta), _Gptr[-1])
                && (_Mystate & _Constant))) { // can't put back, fail
            return _Traits::eof();
        }

        // back up one position and store put-back character
        _Mysb::gbump(-1);
        if (!_Traits::eq_int_type(_Traits::eof(), _Meta)) {
            *_Mysb::gptr() = _Traits::to_char_type(_Meta);
        }

        return _Traits::not_eof(_Meta);
    }

    virtual int_type underflow() { // get an element from stream, but don't point past it
        const auto _Gptr = _Mysb::gptr();
        if (!_Gptr) { // no character buffer, fail
            return _Traits::eof();
        }

        if (_Gptr < _Mysb::egptr()) {
            return _Traits::to_int_type(*_Gptr);
        }

        // try to add initialized characters from the put area into the get area
        const auto _Pptr = _Mysb::pptr();
        if (!_Pptr || (_Mystate & _Noread)) { // no put area or read disallowed
            return _Traits::eof();
        }

        const auto _Local_highwater = (_STD max)(_Seekhigh, _Pptr);
        if (_Local_highwater <= _Gptr) { // nothing in the put area to take
            return _Traits::eof();
        }

        _Seekhigh = _Local_highwater;
        _Mysb::setg(_Mysb::eback(), _Mysb::gptr(), _Local_highwater);
        return _Traits::to_int_type(*_Mysb::gptr());
    }

    virtual pos_type seekoff(
        off_type _Off, ios_base::seekdir _Way, ios_base::openmode _Mode = ios_base::in | ios_base::out) {
        // change position by _Off, according to _Way, _Mode
        const auto _Gptr_old = _Mysb::gptr();
        const auto _Pptr_old = (_Mystate & _Constant) ? nullptr : _Mysb::pptr();
        if (_Pptr_old && _Seekhigh < _Pptr_old) { // update high-water pointer
            _Seekhigh = _Pptr_old;
        }

        const auto _Seeklow  = _Mysb::eback();
        const auto _Seekdist = _Seekhigh - _Seeklow;
        off_type _Newoff;
        switch (_Way) {
        case ios_base::beg:
            _Newoff = 0;
            break;
        case ios_base::end:
            _Newoff = _Seekdist;
            break;
        case ios_base::cur:
            {
                constexpr auto _Both = ios_base::in | ios_base::out;
                if ((_Mode & _Both)
                    != _Both) { // prohibited by N4727 [stringbuf.virtuals] Table 107 "seekoff positioning"
                    if (_Mode & ios_base::in) {
                        if (_Gptr_old || !_Seeklow) {
                            _Newoff = _Gptr_old - _Seeklow;
                            break;
                        }
                    } else if ((_Mode & ios_base::out) && (_Pptr_old || !_Seeklow)) {
                        _Newoff = _Pptr_old - _Seeklow;
                        break;
                    }
                }
            }

            // fallthrough
        default:
            return pos_type(off_type(-1));
        }

        if (static_cast<unsigned long long>(_Off) + _Newoff > static_cast<unsigned long long>(_Seekdist)) {
            return pos_type(off_type(-1));
        }

        _Off += _Newoff;
        if (_Off != 0 && (((_Mode & ios_base::in) && !_Gptr_old) || ((_Mode & ios_base::out) && !_Pptr_old))) {
            return pos_type(off_type(-1));
        }

        const auto _Newptr = _Seeklow + _Off; // may perform nullptr + 0
        if ((_Mode & ios_base::in) && _Gptr_old) {
            _Mysb::setg(_Seeklow, _Newptr, _Seekhigh);
        }

        if ((_Mode & ios_base::out) && _Pptr_old) {
            _Mysb::setp(_Seeklow, _Newptr, _Mysb::epptr());
        }

        return pos_type(_Off);
    }

    virtual pos_type seekpos(pos_type _Pos, ios_base::openmode _Mode = ios_base::in | ios_base::out) {
        // change position to _Pos, according to _Mode
        const auto _Off      = static_cast<streamoff>(_Pos);
        const auto _Gptr_old = _Mysb::gptr();
        const auto _Pptr_old = (_Mystate & _Constant) ? nullptr : _Mysb::pptr();
        if (_Pptr_old && _Seekhigh < _Pptr_old) { // update high-water pointer
            _Seekhigh = _Pptr_old;
        }

        const auto _Seeklow  = _Mysb::eback();
        const auto _Seekdist = _Seekhigh - _Seeklow;
        if (static_cast<unsigned long long>(_Off) > static_cast<unsigned long long>(_Seekdist)) {
            return pos_type(off_type(-1));
        }

        if (_Off != 0 && (((_Mode & ios_base::in) && !_Gptr_old) || ((_Mode & ios_base::out) && !_Pptr_old))) {
            return pos_type(off_type(-1));
        }

        const auto _Newptr = _Seeklow + _Off; // may perform nullptr + 0
        if ((_Mode & ios_base::in) && _Gptr_old) {
            _Mysb::setg(_Seeklow, _Newptr, _Seekhigh);
        }

        if ((_Mode & ios_base::out) && _Pptr_old) {
            _Mysb::setp(_Seeklow, _Newptr, _Mysb::epptr());
        }

        return pos_type(_Off);
    }

protected:
    void _Init(const _Elem* _Ptr, _Mysize_type _Count, int _State) {
        // initialize buffer to [_Ptr, _Ptr + _Count), set state
        _State &= ~_From_rvalue;

        if (_Count > INT_MAX) { // TRANSITION, VSO-485517
            _Xbad_alloc();
        }

        if (_Count != 0
            && (_State & (_Noread | _Constant))
                   != (_Noread | _Constant)) { // finite buffer that can be read or written, set it up
            const auto _Pnew = _Unfancy(_Al.allocate(_Count));
            _Traits::copy(_Pnew, _Ptr, _Count);
            _Seekhigh = _Pnew + _Count;

            if (!(_State & _Noread)) {
                _Mysb::setg(_Pnew, _Pnew, _Seekhigh); // setup read buffer
            }

            if (!(_State & _Constant)) { // setup write buffer, and maybe read buffer
                _Mysb::setp(_Pnew, (_State & (_Atend | _Append)) ? _Seekhigh : _Pnew, _Seekhigh);

                if (_State & _Noread) { // maintain "_Allocated == eback() points to buffer base" invariant
                    _Mysb::setg(_Pnew, nullptr, _Pnew);
                }
            }

            _State |= _Allocated;
        } else {
            _Seekhigh = nullptr;
        }

        _Mystate = _State;
    }

#if _HAS_CXX20
    void _Init_string_inplace(_Mystr&& _Str, int _State) {
        _State |= _From_rvalue;

        auto [_Ptr, _Size, _Res] = _Str._Release_to_buffer(_Al);
        _Elem* const _Pnew       = _Unfancy(_Ptr);
        if (_Res != 0 && (_State & (_Noread | _Constant)) != (_Noread | _Constant)) {
            // finite buffer that can be read or written, set it up
            _Seekhigh        = _Pnew + _Size;
            auto _End_buffer = _Pnew + _Res;

            _Mysb::setp(_Pnew, (_State & (_Atend | _Append)) ? _Seekhigh : _Pnew, _End_buffer);

            if (_State & _Noread) { // maintain "_Allocated == eback() points to buffer base" invariant
                _Mysb::setg(_Pnew, nullptr, _Pnew);
            } else {
                _Mysb::setg(_Pnew, _Pnew, _Seekhigh);
            }

            _State |= _Allocated;
        } else {
            _Seekhigh = nullptr;
        }

        _Mystate = _State;
    }
#endif // _HAS_CXX20

    void _Tidy() noexcept { // discard any allocated buffer and clear pointers
        if (_Mystate & _Allocated) {
            _Al.deallocate(_Ptr_traits::pointer_to(*_Mysb::eback()),
                static_cast<typename allocator_traits<allocator_type>::size_type>(
                    (_Mysb::pptr() ? _Mysb::epptr() : _Mysb::egptr()) - _Mysb::eback()));
        }

        _Mysb::setg(nullptr, nullptr, nullptr);
        _Mysb::setp(nullptr, nullptr);
        _Seekhigh = nullptr;
        _Mystate &= ~_Allocated;
    }

private:
    using _Ptr_traits = pointer_traits<typename allocator_traits<allocator_type>::pointer>;

    enum { // constant for minimum buffer size
        _MINSIZE = 32
    };

    static int _Getstate(ios_base::openmode _Mode) { // convert open mode to stream state bits
        int _State = 0;
        if (!(_Mode & ios_base::in)) {
            _State |= _Noread;
        }

        if (!(_Mode & ios_base::out)) {
            _State |= _Constant;
        }

        if (_Mode & ios_base::app) {
            _State |= _Append;
        }

        if (_Mode & ios_base::ate) {
            _State |= _Atend;
        }

        return _State;
    }

    // TRANSITION, ABI, see GH-938
    _Elem* _Seekhigh; // the high-water pointer in character array
    int _Mystate; // the stream state
    allocator_type _Al; // the allocator object
};

template <class _Elem, class _Traits, class _Alloc>
void swap(basic_stringbuf<_Elem, _Traits, _Alloc>& _Left, basic_stringbuf<_Elem, _Traits, _Alloc>& _Right) noexcept
/* strengthened */ {
    _Left.swap(_Right);
}

template <class _Elem, class _Traits, class _Alloc>
class basic_istringstream : public basic_istream<_Elem, _Traits> { // input stream associated with a character array
public:
    using _Mybase        = basic_istream<_Elem, _Traits>;
    using allocator_type = _Alloc;
    using _Mysb          = basic_stringbuf<_Elem, _Traits, _Alloc>;
    using _Mystr         = basic_string<_Elem, _Traits, _Alloc>;

#if _HAS_CXX20
    using _Mystr_view = basic_string_view<_Elem, _Traits>;
#endif // _HAS_CXX20

    basic_istringstream() : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(ios_base::in) {}

    explicit basic_istringstream(ios_base::openmode _Mode)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Mode | ios_base::in) {}

    explicit basic_istringstream(const _Mystr& _Str, ios_base::openmode _Mode = ios_base::in)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode | ios_base::in) {}

#if _HAS_CXX20
    basic_istringstream(ios_base::openmode _Mode, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Mode | ios_base::in, _Al) {}

    explicit basic_istringstream(_Mystr&& _Str, ios_base::openmode _Mode = ios_base::in)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_STD move(_Str), _Mode | ios_base::in) {}

    template <class _Alloc2>
    basic_istringstream(const basic_string<_Elem, _Traits, _Alloc2>& _Str, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, ios_base::in, _Al) {}

    template <class _Alloc2>
    basic_istringstream(const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode | ios_base::in, _Al) {}

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    explicit basic_istringstream(
        const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode = ios_base::in)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode | ios_base::in) {}
#endif // _HAS_CXX20

    basic_istringstream(basic_istringstream&& _Right) : _Mybase(_STD addressof(_Stringbuffer)) {
        _Assign_rv(_STD move(_Right));
    }

    basic_istringstream& operator=(basic_istringstream&& _Right) {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    void _Assign_rv(basic_istringstream&& _Right) {
        if (this != _STD addressof(_Right)) {
            _Stringbuffer.str(_Mystr());
            this->swap(_Right);
        }
    }

    void swap(basic_istringstream& _Right) {
        if (this != _STD addressof(_Right)) {
            _Mybase::swap(_Right);
            _Stringbuffer.swap(_Right._Stringbuffer);
        }
    }

    basic_istringstream(const basic_istringstream&) = delete;
    basic_istringstream& operator=(const basic_istringstream&) = delete;

    virtual ~basic_istringstream() noexcept {}

    _NODISCARD _Mysb* rdbuf() const {
        return const_cast<_Mysb*>(_STD addressof(_Stringbuffer));
    }

#if _HAS_CXX20
    _NODISCARD _Mystr str() const&
#else
    _NODISCARD _Mystr str() const
#endif // _HAS_CXX20
    {
        return _Stringbuffer.str();
    }

    void str(const _Mystr& _Newstr) { // replace character array from string
        _Stringbuffer.str(_Newstr);
    }

#if _HAS_CXX20
    template <class _Alloc2, enable_if_t<_Is_allocator<_Alloc2>::value, int> = 0>
    _NODISCARD basic_string<_Elem, _Traits, _Alloc2> str(const _Alloc2& _Al) const {
        return _Stringbuffer.str(_Al);
    }

    _NODISCARD _Mystr str() && {
        return _STD move(_Stringbuffer).str();
    }

    _NODISCARD _Mystr_view view() const noexcept {
        return _Stringbuffer.view();
    }

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    void str(const basic_string<_Elem, _Traits, _Alloc2>& _Newstr) {
        _Stringbuffer.str(_Newstr);
    }

    void str(_Mystr&& _Newstr) {
        _Stringbuffer.str(_STD move(_Newstr));
    }
#endif // _HAS_CXX20

private:
    _Mysb _Stringbuffer;
};

template <class _Elem, class _Traits, class _Alloc>
void swap(basic_istringstream<_Elem, _Traits, _Alloc>& _Left, basic_istringstream<_Elem, _Traits, _Alloc>& _Right) {
    _Left.swap(_Right);
}

template <class _Elem, class _Traits, class _Alloc>
class basic_ostringstream : public basic_ostream<_Elem, _Traits> { // output stream associated with a character array
public:
    using _Mybase        = basic_ostream<_Elem, _Traits>;
    using allocator_type = _Alloc;
    using _Mysb          = basic_stringbuf<_Elem, _Traits, _Alloc>;
    using _Mystr         = basic_string<_Elem, _Traits, _Alloc>;

#if _HAS_CXX20
    using _Mystr_view = basic_string_view<_Elem, _Traits>;
#endif // _HAS_CXX20

    basic_ostringstream() : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(ios_base::out) {}

    explicit basic_ostringstream(ios_base::openmode _Mode)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Mode | ios_base::out) {}

    explicit basic_ostringstream(const _Mystr& _Str, ios_base::openmode _Mode = ios_base::out)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode | ios_base::out) {}

#if _HAS_CXX20
    basic_ostringstream(ios_base::openmode _Mode, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Mode | ios_base::out, _Al) {}

    explicit basic_ostringstream(_Mystr&& _Str, ios_base::openmode _Mode = ios_base::out)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_STD move(_Str), _Mode | ios_base::out) {}

    template <class _Alloc2>
    basic_ostringstream(const basic_string<_Elem, _Traits, _Alloc2>& _Str, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, ios_base::out, _Al) {}

    template <class _Alloc2>
    basic_ostringstream(const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode | ios_base::out, _Al) {}

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    explicit basic_ostringstream(
        const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode = ios_base::out)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode | ios_base::out) {}
#endif // _HAS_CXX20

    basic_ostringstream(basic_ostringstream&& _Right) : _Mybase(_STD addressof(_Stringbuffer)) {
        _Assign_rv(_STD move(_Right));
    }

    basic_ostringstream& operator=(basic_ostringstream&& _Right) {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    void _Assign_rv(basic_ostringstream&& _Right) {
        if (this != _STD addressof(_Right)) {
            _Stringbuffer.str(_Mystr());
            this->swap(_Right);
        }
    }

    void swap(basic_ostringstream& _Right) {
        if (this != _STD addressof(_Right)) {
            _Mybase::swap(_Right);
            _Stringbuffer.swap(_Right._Stringbuffer);
        }
    }

    basic_ostringstream(const basic_ostringstream&) = delete;
    basic_ostringstream& operator=(const basic_ostringstream&) = delete;

    virtual ~basic_ostringstream() noexcept {}

    _NODISCARD _Mysb* rdbuf() const {
        return const_cast<_Mysb*>(_STD addressof(_Stringbuffer));
    }

#if _HAS_CXX20
    _NODISCARD _Mystr str() const&
#else
    _NODISCARD _Mystr str() const
#endif // _HAS_CXX20
    {
        return _Stringbuffer.str();
    }

    void str(const _Mystr& _Newstr) { // replace character array from string
        _Stringbuffer.str(_Newstr);
    }

#if _HAS_CXX20
    template <class _Alloc2, enable_if_t<_Is_allocator<_Alloc2>::value, int> = 0>
    _NODISCARD basic_string<_Elem, _Traits, _Alloc2> str(const _Alloc2& _Al) const {
        return _Stringbuffer.str(_Al);
    }

    _NODISCARD _Mystr str() && {
        return _STD move(_Stringbuffer).str();
    }

    _NODISCARD _Mystr_view view() const noexcept {
        return _Stringbuffer.view();
    }

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    void str(const basic_string<_Elem, _Traits, _Alloc2>& _Newstr) {
        _Stringbuffer.str(_Newstr);
    }

    void str(_Mystr&& _Newstr) {
        _Stringbuffer.str(_STD move(_Newstr));
    }
#endif // _HAS_CXX20

private:
    _Mysb _Stringbuffer;
};

template <class _Elem, class _Traits, class _Alloc>
void swap(basic_ostringstream<_Elem, _Traits, _Alloc>& _Left, basic_ostringstream<_Elem, _Traits, _Alloc>& _Right) {
    _Left.swap(_Right);
}

template <class _Elem, class _Traits, class _Alloc>
class basic_stringstream
    : public basic_iostream<_Elem, _Traits> { // input/output stream associated with a character array
public:
    using _Mybase        = basic_iostream<_Elem, _Traits>;
    using char_type      = _Elem;
    using traits_type    = _Traits;
    using allocator_type = _Alloc;
    using int_type       = typename _Traits::int_type;
    using pos_type       = typename _Traits::pos_type;
    using off_type       = typename _Traits::off_type;
    using _Mysb          = basic_stringbuf<_Elem, _Traits, _Alloc>;
    using _Mystr         = basic_string<_Elem, _Traits, _Alloc>;

#if _HAS_CXX20
    using _Mystr_view = basic_string_view<_Elem, _Traits>;
#endif // _HAS_CXX20

    basic_stringstream() : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(ios_base::in | ios_base::out) {}

    explicit basic_stringstream(ios_base::openmode _Mode)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Mode) {}

    explicit basic_stringstream(const _Mystr& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode) {}

#if _HAS_CXX20
    basic_stringstream(ios_base::openmode _Mode, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Mode, _Al) {}

    explicit basic_stringstream(_Mystr&& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_STD move(_Str), _Mode) {}

    template <class _Alloc2>
    basic_stringstream(const basic_string<_Elem, _Traits, _Alloc2>& _Str, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, ios_base::in | ios_base::out, _Al) {}

    template <class _Alloc2>
    basic_stringstream(const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode, const _Alloc& _Al)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode, _Al) {}

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    explicit basic_stringstream(
        const basic_string<_Elem, _Traits, _Alloc2>& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
        : _Mybase(_STD addressof(_Stringbuffer)), _Stringbuffer(_Str, _Mode) {}
#endif // _HAS_CXX20

    basic_stringstream(basic_stringstream&& _Right) : _Mybase(_STD addressof(_Stringbuffer)) {
        _Assign_rv(_STD move(_Right));
    }

    basic_stringstream& operator=(basic_stringstream&& _Right) {
        _Assign_rv(_STD move(_Right));
        return *this;
    }

    void _Assign_rv(basic_stringstream&& _Right) {
        if (this != _STD addressof(_Right)) {
            _Stringbuffer.str(_Mystr());
            this->swap(_Right);
        }
    }

    void swap(basic_stringstream& _Right) {
        if (this != _STD addressof(_Right)) {
            _Mybase::swap(_Right);
            _Stringbuffer.swap(_Right._Stringbuffer);
        }
    }

    basic_stringstream(const basic_stringstream&) = delete;
    basic_stringstream& operator=(const basic_stringstream&) = delete;

    virtual ~basic_stringstream() noexcept {}

    _NODISCARD _Mysb* rdbuf() const {
        return const_cast<_Mysb*>(_STD addressof(_Stringbuffer));
    }

#if _HAS_CXX20
    _NODISCARD _Mystr str() const&
#else
    _NODISCARD _Mystr str() const
#endif // _HAS_CXX20
    {
        return _Stringbuffer.str();
    }

    void str(const _Mystr& _Newstr) { // replace character array from string
        _Stringbuffer.str(_Newstr);
    }

#if _HAS_CXX20
    template <class _Alloc2, enable_if_t<_Is_allocator<_Alloc2>::value, int> = 0>
    _NODISCARD basic_string<_Elem, _Traits, _Alloc2> str(const _Alloc2& _Al) const {
        return _Stringbuffer.str(_Al);
    }

    _NODISCARD _Mystr str() && {
        return _STD move(_Stringbuffer).str();
    }

    _NODISCARD _Mystr_view view() const noexcept {
        return _Stringbuffer.view();
    }

    template <class _Alloc2, enable_if_t<!is_same_v<_Alloc2, _Alloc>, int> = 0>
    void str(const basic_string<_Elem, _Traits, _Alloc2>& _Newstr) {
        _Stringbuffer.str(_Newstr);
    }

    void str(_Mystr&& _Newstr) {
        _Stringbuffer.str(_STD move(_Newstr));
    }
#endif // _HAS_CXX20

private:
    _Mysb _Stringbuffer;
};

template <class _Elem, class _Traits, class _Alloc>
void swap(basic_stringstream<_Elem, _Traits, _Alloc>& _Left, basic_stringstream<_Elem, _Traits, _Alloc>& _Right) {
    _Left.swap(_Right);
}
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _SSTREAM_
