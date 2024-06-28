// xlocale internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XLOCALE_
#define _XLOCALE_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <climits>
#include <cstring>
#include <memory>
#include <typeinfo>
#include <xfacet>
#include <xlocinfo>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <class _Dummy>
class _Locbase { // define templatized category constants, instantiate on demand
public:
    _PGLOBAL static const int collate  = _M_COLLATE;
    _PGLOBAL static const int ctype    = _M_CTYPE;
    _PGLOBAL static const int monetary = _M_MONETARY;
    _PGLOBAL static const int numeric  = _M_NUMERIC;
    _PGLOBAL static const int time     = _M_TIME;
    _PGLOBAL static const int messages = _M_MESSAGES;
    _PGLOBAL static const int all      = _M_ALL;
    _PGLOBAL static const int none     = 0;
};

template <class _Dummy>
const int _Locbase<_Dummy>::collate;
template <class _Dummy>
const int _Locbase<_Dummy>::ctype;
template <class _Dummy>
const int _Locbase<_Dummy>::monetary;
template <class _Dummy>
const int _Locbase<_Dummy>::numeric;
template <class _Dummy>
const int _Locbase<_Dummy>::time;
template <class _Dummy>
const int _Locbase<_Dummy>::messages;
template <class _Dummy>
const int _Locbase<_Dummy>::all;
template <class _Dummy>
const int _Locbase<_Dummy>::none;

template <class _Elem>
class collate;

struct _CRTIMP2_PURE_IMPORT _Crt_new_delete { // base class for marking allocations as CRT blocks
#ifdef _DEBUG
    void* __CLRCALL_OR_CDECL operator new(size_t _Size) { // replace operator new
        void* _Ptr = operator new(_Size, nothrow);
        if (!_Ptr) {
            _Xbad_alloc();
        }

        return _Ptr;
    }

    void* __CLRCALL_OR_CDECL operator new(size_t _Size, const nothrow_t&) noexcept { // replace nothrow operator new
        return _malloc_dbg(_Size > 0 ? _Size : 1, _CRT_BLOCK, __FILE__, __LINE__);
    }

    void __CLRCALL_OR_CDECL operator delete(void* _Ptr) noexcept { // replace operator delete
        _CSTD free(_Ptr);
    }

    void __CLRCALL_OR_CDECL operator delete(void* _Ptr, const nothrow_t&) noexcept { // replace nothrow operator delete
        operator delete(_Ptr);
    }

    void* __CLRCALL_OR_CDECL operator new(size_t, void* _Ptr) noexcept { // imitate True Placement New
        return _Ptr;
    }

    void __CLRCALL_OR_CDECL operator delete(void*, void*) noexcept {} // imitate True Placement Delete
#endif // _DEBUG
};

class locale : public _Locbase<int>, public _Crt_new_delete { // nonmutable collection of facets that describe a locale
public:
    using category = int;

    class _CRTIMP2_PURE_IMPORT id { // identifier stamp, unique for each distinct kind of facet
    public:
        __CLR_OR_THIS_CALL id(size_t _Val = 0) : _Id(_Val) {}

        id(const id&) = delete;
        id& operator=(const id&) = delete;

        __CLR_OR_THIS_CALL operator size_t() { // get stamp, with lazy allocation
            if (_Id == 0) { // still zero, allocate stamp
                _BEGIN_LOCK(_LOCK_LOCALE)
                if (_Id == 0) {
                    _Id = static_cast<size_t>(++_Id_cnt);
                }
                _END_LOCK()
            }
            return _Id;
        }

    private:
        size_t _Id; // the identifier stamp

        __PURE_APPDOMAIN_GLOBAL static int _Id_cnt;
    };

    class _Locimp;

    class _CRTIMP2_PURE_IMPORT facet : public _Facet_base, public _Crt_new_delete {
        // base class for all locale facets, performs reference counting
    private:
        friend struct _Facet_guard;

    public:
        static size_t __CLRCALL_OR_CDECL _Getcat(const facet** = nullptr, const locale* = nullptr) {
            // get category value, or -1 if no corresponding C category
            return static_cast<size_t>(-1);
        }

        virtual void __CLR_OR_THIS_CALL _Incref() noexcept override { // increment use count
            _MT_INCR(_Myrefs);
        }

        virtual _Facet_base* __CLR_OR_THIS_CALL _Decref() noexcept override { // decrement use count
            if (_MT_DECR(_Myrefs) == 0) {
                return this;
            }

            return nullptr;
        }

    private:
        _Atomic_counter_t _Myrefs; // the reference count

    protected:
        explicit __CLR_OR_THIS_CALL facet(size_t _Initrefs = 0)
            : _Myrefs(static_cast<_Atomic_counter_t>(_Initrefs)) // non-atomic initialization
        {}

        virtual __CLR_OR_THIS_CALL ~facet() noexcept override {}

    public:
        __CLR_OR_THIS_CALL facet(const facet&) = delete;
        facet& __CLR_OR_THIS_CALL operator=(const facet&) = delete;
    };

    struct _NODISCARD _Facet_guard {
        facet* _Target;
        ~_Facet_guard() {
            if (_Target) {
                delete _Target->_Decref();
            }
        }
    };

    class _CRTIMP2_PURE_IMPORT _Locimp : public facet { // reference-counted actual implementation of a locale
    protected:
        __CLR_OR_THIS_CALL ~_Locimp() noexcept {
            _Locimp_dtor(this);
        }

    private:
        static _Locimp* __CLRCALL_PURE_OR_CDECL _New_Locimp(bool _Transparent = false);
        static _Locimp* __CLRCALL_PURE_OR_CDECL _New_Locimp(const _Locimp& _Right);

        static void __CLRCALL_PURE_OR_CDECL _Locimp_dtor(_Locimp*);
        static void __CLRCALL_PURE_OR_CDECL _Locimp_Addfac(_Locimp*, facet*, size_t); // add a facet
        static void __CLRCALL_PURE_OR_CDECL _Locimp_ctor(_Locimp*, const _Locimp&);

        friend locale;

        __CLR_OR_THIS_CALL _Locimp(bool _Transparent)
            : locale::facet(1), _Facetvec(nullptr), _Facetcount(0), _Catmask(none), _Xparent(_Transparent), _Name("*") {
        }

        __CLR_OR_THIS_CALL _Locimp(const _Locimp& _Right)
            : locale::facet(1), _Facetvec(nullptr), _Facetcount(_Right._Facetcount), _Catmask(_Right._Catmask),
              _Xparent(_Right._Xparent), _Name(_Right._Name.c_str()) {
            _Locimp_ctor(this, _Right);
        }

        void __CLR_OR_THIS_CALL _Addfac(facet* _Pfacet, size_t _Id) { // add a facet
            _Locimp_Addfac(this, _Pfacet, _Id);
        }

        static _Locimp* __CLRCALL_OR_CDECL _Makeloc(
            const _Locinfo&, category, _Locimp*, const locale*); // make essential facets

        static void __CLRCALL_OR_CDECL _Makewloc(
            const _Locinfo&, category, _Locimp*, const locale*); // make wchar_t facets

#ifdef _NATIVE_WCHAR_T_DEFINED
        static void __CLRCALL_OR_CDECL _Makeushloc(
            const _Locinfo&, category, _Locimp*, const locale*); // make ushort facets
#endif // _NATIVE_WCHAR_T_DEFINED

        static void __CLRCALL_OR_CDECL _Makexloc(
            const _Locinfo&, category, _Locimp*, const locale*); // make remaining facets

        facet** _Facetvec; // pointer to vector of facets
        size_t _Facetcount; // size of vector of facets
        category _Catmask; // mask describing implemented categories
        bool _Xparent; // true if locale is transparent
        _Yarn<char> _Name; // locale name, or "*" if not known

        __PURE_APPDOMAIN_GLOBAL static _Locimp* _Clocptr;

    public:
        _Locimp& __CLR_OR_THIS_CALL operator=(const _Locimp&) = delete;
    };

    template <class _Elem, class _Traits, class _Alloc>
    bool operator()(
        const basic_string<_Elem, _Traits, _Alloc>& _Left, const basic_string<_Elem, _Traits, _Alloc>& _Right) const {
        // compare _Left and _Right strings using collate facet in locale
        const auto& _Coll_fac = _STD use_facet<_STD collate<_Elem>>(*this);

        const _Elem* const _Left_data  = _Left.data();
        const _Elem* const _Right_data = _Right.data();
        return _Coll_fac.compare(_Left_data, _Left_data + _Left.size(), _Right_data, _Right_data + _Right.size()) < 0;
    }

    template <class _Facet>
    locale combine(const locale& _Loc) const { // combine two locales
        _Facet* _Facptr;

        _TRY_BEGIN
        _Facptr = const_cast<_Facet*>(_STD addressof(_STD use_facet<_Facet>(_Loc)));
        _CATCH_ALL
        _Xruntime_error("locale::combine facet missing");
        _CATCH_END

        _Locimp* _Newimp = _Locimp::_New_Locimp(*_Ptr);
        _Newimp->_Addfac(_Facptr, _Facet::id);
        _Newimp->_Catmask = none;
        _Newimp->_Name    = "*";
        return locale(_Newimp);
    }

    template <class _Facet>
    locale(const locale& _Loc, const _Facet* _Facptr) : _Ptr(_Locimp::_New_Locimp(*_Loc._Ptr)) {
        if (_Facptr) { // replace facet
            _Ptr->_Addfac(const_cast<_Facet*>(_Facptr), _Facet::id);
            _Ptr->_Catmask = none;
            _Ptr->_Name    = "*";
        }
    }

    locale(_Uninitialized) {} // defer construction

    locale(const locale& _Right) noexcept : _Ptr(_Right._Ptr) {
        _Ptr->_Incref();
    }

    locale() noexcept : _Ptr(_Init(true)) {}

#if !defined(MRTDLL) || !defined(_CRTBLD)
    locale(const locale& _Loc, const locale& _Other, category _Cat) : _Ptr(_Locimp::_New_Locimp(*_Loc._Ptr)) {
        // construct a locale by copying named facets
        if (_Cat != none) { // worth adding, do it
            _Facet_guard _Guard{_Ptr};
            _BEGIN_LOCINFO(_Lobj)
            _Locimp::_Makeloc(_Lobj, _Cat, _Ptr, &_Other);
            _Lobj._Addcats(_Loc._Ptr->_Catmask, _Loc.name().c_str());
            _Lobj._Addcats(_Other._Ptr->_Catmask, _Other.name().c_str());
            _Ptr->_Catmask = _Loc._Ptr->_Catmask | _Other._Ptr->_Catmask;
            _Ptr->_Name    = _Lobj._Getname();
            _END_LOCINFO()
            _Guard._Target = nullptr;
        }
    }

private:
    void _Construct(const string& _Str, category _Cat) {
        // construct a locale with named facets
        bool _Bad = false;
        _Init();
        if (_Cat != none) { // worth adding, do it
            _Facet_guard _Guard{_Ptr};
            _BEGIN_LOCINFO(_Lobj(_Cat, _Str.c_str()))
            if (_Badname(_Lobj)) {
                _Bad = true;
            } else { // name okay, build the locale
                _Locimp::_Makeloc(_Lobj, _Cat, _Ptr, nullptr);
                _Ptr->_Catmask = _Cat;
                _Ptr->_Name    = _Str.c_str();
            }
            _END_LOCINFO()
            _Guard._Target = nullptr;
        }

        if (_Bad) { // Don't throw within _BEGIN_LOCINFO if we can avoid it
            delete _Ptr->_Decref();
            _Xruntime_error("bad locale name");
        }
    }

public:
    explicit locale(const char* _Locname, category _Cat = all) : _Ptr(_Locimp::_New_Locimp()) {
        // construct a locale with named facets
        // _Locname might have been returned from setlocale().
        // Therefore, _Construct() takes const string&.
        if (_Locname) {
            _Construct(_Locname, _Cat);
            return;
        }

        _Xruntime_error("bad locale name");
    }

    locale(const locale& _Loc, const char* _Locname, category _Cat) : _Ptr(_Locimp::_New_Locimp(*_Loc._Ptr)) {
        // construct a locale by copying, replacing named facets
        // _Locname might have been returned from setlocale().
        // Therefore, _Construct() takes const string&.
        if (_Locname) {
            _Construct(_Locname, _Cat);
            return;
        }

        _Xruntime_error("bad locale name");
    }

    explicit locale(const string& _Str, category _Cat = all) : _Ptr(_Locimp::_New_Locimp()) {
        // construct a locale with named facets
        _Construct(_Str, _Cat);
    }

    locale(const locale& _Loc, const string& _Str, category _Cat) : _Ptr(_Locimp::_New_Locimp(*_Loc._Ptr)) {
        // construct a locale by copying, replacing named facets
        _Construct(_Str, _Cat);
    }
#endif // !MRTDLL || !_CRTBLD

    ~locale() noexcept {
        if (_Ptr) {
            delete _Ptr->_Decref();
        }
    }

    const locale& operator=(const locale& _Right) noexcept {
        if (_Ptr != _Right._Ptr) { // different implementation, point at new one
            delete _Ptr->_Decref();
            _Ptr = _Right._Ptr;
            _Ptr->_Incref();
        }
        return *this;
    }

    string name() const {
        return _Ptr ? _Ptr->_Name.c_str() : string();
    }

    _Ret_z_ const char* c_str() const {
        return _Ptr ? _Ptr->_Name.c_str() : "";
    }

    const facet* _Getfacet(size_t _Id) const { // look up a facet in locale object
        const facet* _Facptr = _Id < _Ptr->_Facetcount ? _Ptr->_Facetvec[_Id] : nullptr; // null if id off end
        if (_Facptr || !_Ptr->_Xparent) {
            return _Facptr; // found facet or not transparent
        }

        // look in current locale
        locale::_Locimp* _Ptr0 = _Getgloballocale();
        if (_Id < _Ptr0->_Facetcount) {
            return _Ptr0->_Facetvec[_Id]; // get from current locale
        }

        return nullptr; // no entry in current locale
    }

    _NODISCARD bool operator==(const locale& _Loc) const { // compare locales for equality
        return _Ptr == _Loc._Ptr || (name().compare("*") != 0 && name().compare(_Loc.name()) == 0);
    }

#if !_HAS_CXX20
    _NODISCARD bool operator!=(const locale& _Right) const {
        return !(*this == _Right);
    }
#endif // !_HAS_CXX20

    static _MRTIMP2_PURE const locale& __CLRCALL_PURE_OR_CDECL classic(); // classic "C" locale

    static _MRTIMP2_PURE locale __CLRCALL_PURE_OR_CDECL global(const locale&); // current locale

    static _MRTIMP2_PURE locale __CLRCALL_PURE_OR_CDECL empty(); // empty (transparent) locale

private:
    locale(_Locimp* _Ptrimp) : _Ptr(_Ptrimp) {}

    static _MRTIMP2_PURE _Locimp* __CLRCALL_PURE_OR_CDECL _Init(bool _Do_incref = false); // initialize locale
    static _MRTIMP2_PURE _Locimp* __CLRCALL_PURE_OR_CDECL _Getgloballocale();
    static _MRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Setgloballocale(void*);

    bool _Badname(const _Locinfo& _Lobj) { // test if name is "*"
        return _CSTD strcmp(_Lobj._Getname(), "*") == 0;
    }

    _Locimp* _Ptr; // pointer to locale implementation object
};

template <class _Facet>
struct _Facetptr { // store pointer to lazy facet for use_facet
    __PURE_APPDOMAIN_GLOBAL static const locale::facet* _Psave;
};

template <class _Facet>
__PURE_APPDOMAIN_GLOBAL const locale::facet* _Facetptr<_Facet>::_Psave = nullptr;

template <class _Facet>
const _Facet& __CRTDECL use_facet(const locale& _Loc) { // get facet reference from locale
    _BEGIN_LOCK(_LOCK_LOCALE) // the thread lock, make get atomic
    const locale::facet* _Psave = _Facetptr<_Facet>::_Psave; // static pointer to lazy facet

    const size_t _Id         = _Facet::id;
    const locale::facet* _Pf = _Loc._Getfacet(_Id);

    if (!_Pf) {
        if (_Psave) {
            _Pf = _Psave; // lazy facet already allocated
        } else if (_Facet::_Getcat(&_Psave, &_Loc) == static_cast<size_t>(-1)) {
#if _HAS_EXCEPTIONS
            _Throw_bad_cast(); // lazy disallowed
#else // _HAS_EXCEPTIONS
            _CSTD abort(); // lazy disallowed
#endif // _HAS_EXCEPTIONS
        } else { // queue up lazy facet for destruction
            auto _Pfmod = const_cast<locale::facet*>(_Psave);
            unique_ptr<_Facet_base> _Psave_guard(static_cast<_Facet_base*>(_Pfmod));

#if defined(_M_CEE)
            _Facet_Register_m(_Pfmod);
#else // defined(_M_CEE)
            _Facet_Register(_Pfmod);
#endif // defined(_M_CEE)

            _Pfmod->_Incref();
            _Facetptr<_Facet>::_Psave = _Psave;
            _Pf                       = _Psave;

            (void) _Psave_guard.release();
        }
    }

    return static_cast<const _Facet&>(*_Pf); // should be dynamic_cast
    _END_LOCK()
} // end of use_facet body

template <class _Elem>
char __CRTDECL _Maklocbyte(_Elem _Char, const _Locinfo::_Cvtvec&) {
    // convert _Elem to char using _Cvtvec
    return static_cast<char>(static_cast<unsigned char>(_Char));
}

template <>
inline char __CRTDECL _Maklocbyte(wchar_t _Char, const _Locinfo::_Cvtvec& _Cvt) {
    // convert wchar_t to char using _Cvtvec
    char _Byte       = '\0';
    mbstate_t _Mbst1 = {};
    _Wcrtomb(&_Byte, _Char, &_Mbst1, &_Cvt);
    return _Byte;
}

#ifdef _NATIVE_WCHAR_T_DEFINED
template <>
inline char __CRTDECL _Maklocbyte(unsigned short _Char, const _Locinfo::_Cvtvec& _Cvt) {
    // convert unsigned short to char using _Cvtvec
    char _Byte       = '\0';
    mbstate_t _Mbst1 = {};
    _Wcrtomb(&_Byte, static_cast<wchar_t>(_Char), &_Mbst1, &_Cvt);
    return _Byte;
}
#endif // _NATIVE_WCHAR_T_DEFINED

template <class _Elem>
_Elem __CRTDECL _Maklocchr(char _Byte, _Elem*, const _Locinfo::_Cvtvec&) {
    // convert char to _Elem using _Cvtvec
    return static_cast<_Elem>(static_cast<unsigned char>(_Byte));
}

template <>
inline wchar_t __CRTDECL _Maklocchr(char _Byte, wchar_t*, const _Locinfo::_Cvtvec& _Cvt) {
    // convert char to wchar_t using _Cvtvec
    wchar_t _Wc      = L'\0';
    mbstate_t _Mbst1 = {};
    _Mbrtowc(&_Wc, &_Byte, 1, &_Mbst1, &_Cvt);
    return _Wc;
}

#ifdef _NATIVE_WCHAR_T_DEFINED
template <>
inline unsigned short __CRTDECL _Maklocchr(char _Byte, unsigned short*, const _Locinfo::_Cvtvec& _Cvt) {
    // convert char to unsigned short using _Cvtvec
    unsigned short _Wc = 0;
    mbstate_t _Mbst1   = {};
    _Mbrtowc(reinterpret_cast<wchar_t*>(&_Wc), &_Byte, 1, &_Mbst1, &_Cvt);
    return _Wc;
}
#endif // _NATIVE_WCHAR_T_DEFINED

template <class _Elem>
_Elem* __CRTDECL _Maklocstr(const char* _Ptr, _Elem*, const _Locinfo::_Cvtvec&) {
    // convert C string to _Elem sequence using _Cvtvec
    size_t _Count = _CSTD strlen(_Ptr) + 1;

    _Elem* _Ptrdest = static_cast<_Elem*>(_calloc_dbg(_Count, sizeof(_Elem), _CRT_BLOCK, __FILE__, __LINE__));

    if (!_Ptrdest) {
        _Xbad_alloc();
    }

    for (_Elem* _Ptrnext = _Ptrdest; 0 < _Count; --_Count, ++_Ptrnext, ++_Ptr) {
        *_Ptrnext = static_cast<_Elem>(static_cast<unsigned char>(*_Ptr));
    }

    return _Ptrdest;
}

template <>
inline wchar_t* __CRTDECL _Maklocstr(const char* _Ptr, wchar_t*, const _Locinfo::_Cvtvec& _Cvt) {
    // convert C string to wchar_t sequence using _Cvtvec
    size_t _Count;
    size_t _Count1;
    size_t _Wchars;
    const char* _Ptr1;
    int _Bytes;
    wchar_t _Wc;
    mbstate_t _Mbst1 = {};

    _Count1 = _CSTD strlen(_Ptr) + 1;
    for (_Count = _Count1, _Wchars = 0, _Ptr1 = _Ptr; 0 < _Count; _Count -= _Bytes, _Ptr1 += _Bytes, ++_Wchars) {
        if ((_Bytes = _Mbrtowc(&_Wc, _Ptr1, _Count, &_Mbst1, &_Cvt)) <= 0) {
            break;
        }
    }

    ++_Wchars; // count terminating nul

    wchar_t* _Ptrdest = static_cast<wchar_t*>(_calloc_dbg(_Wchars, sizeof(wchar_t), _CRT_BLOCK, __FILE__, __LINE__));

    if (!_Ptrdest) {
        _Xbad_alloc();
    }

    wchar_t* _Ptrnext = _Ptrdest;
    mbstate_t _Mbst2  = {};

    for (; 0 < _Wchars; _Count -= _Bytes, _Ptr += _Bytes, --_Wchars, ++_Ptrnext) {
        if ((_Bytes = _Mbrtowc(_Ptrnext, _Ptr, _Count1, &_Mbst2, &_Cvt)) <= 0) {
            break;
        }
    }

    *_Ptrnext = L'\0';

    return _Ptrdest;
}

#ifdef _NATIVE_WCHAR_T_DEFINED
template <>
inline unsigned short* __CRTDECL _Maklocstr(const char* _Ptr, unsigned short*, const _Locinfo::_Cvtvec& _Cvt) {
    // convert C string to unsigned short sequence using _Cvtvec
    size_t _Count;
    size_t _Count1;
    size_t _Wchars;
    const char* _Ptr1;
    int _Bytes;
    unsigned short _Wc;
    mbstate_t _Mbst1 = {};

    _Count1 = _CSTD strlen(_Ptr) + 1;
    for (_Count = _Count1, _Wchars = 0, _Ptr1 = _Ptr; 0 < _Count; _Count -= _Bytes, _Ptr1 += _Bytes, ++_Wchars) {
        if ((_Bytes = _Mbrtowc(reinterpret_cast<wchar_t*>(&_Wc), _Ptr1, _Count, &_Mbst1, &_Cvt)) <= 0) {
            break;
        }
    }

    ++_Wchars; // count terminating nul

    wchar_t* _Ptrdest = static_cast<wchar_t*>(_calloc_dbg(_Wchars, sizeof(wchar_t), _CRT_BLOCK, __FILE__, __LINE__));

    if (!_Ptrdest) {
        _Xbad_alloc();
    }

    wchar_t* _Ptrnext = _Ptrdest;
    mbstate_t _Mbst2  = {};
    for (; 0 < _Wchars; _Count -= _Bytes, _Ptr += _Bytes, --_Wchars, ++_Ptrnext) {
        if ((_Bytes = _Mbrtowc(_Ptrnext, _Ptr, _Count1, &_Mbst2, &_Cvt)) <= 0) {
            break;
        }
    }

    *_Ptrnext = L'\0';
    return reinterpret_cast<unsigned short*>(_Ptrdest);
}
#endif // _NATIVE_WCHAR_T_DEFINED

class _CRTIMP2_PURE_IMPORT codecvt_base : public locale::facet { // base class for codecvt
public:
    enum { // constants for different parse states
        ok,
        partial,
        error,
        noconv
    };
    using result = int;

    __CLR_OR_THIS_CALL codecvt_base(size_t _Refs = 0) : locale::facet(_Refs) {}

    bool __CLR_OR_THIS_CALL always_noconv() const noexcept {
        // return true if conversions never change input (from codecvt)
        return do_always_noconv();
    }

    int __CLR_OR_THIS_CALL max_length() const noexcept {
        // return maximum length required for a conversion (from codecvt)
        return do_max_length();
    }

    int __CLR_OR_THIS_CALL encoding() const noexcept {
        return do_encoding();
    }

    __CLR_OR_THIS_CALL ~codecvt_base() noexcept {}

protected:
    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept {
        // return true if conversions never change input (from codecvt)
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept {
        // return maximum length required for a conversion (from codecvt)
        return 1;
    }

    virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept {
        return 1; // -1 ==> state dependent, 0 ==> varying length
    }
};

template <class _Elem, class _Byte, class _Statype>
class codecvt : public codecvt_base { // facet for converting between _Elem and _Byte sequences
public:
    // Explicitly specialized below:
    // codecvt<wchar_t, char, mbstate_t>
    // codecvt<unsigned short, char, mbstate_t> (extension)
    // codecvt<char16_t, char, mbstate_t>
    // codecvt<char32_t, char, mbstate_t>
    // codecvt<char16_t, char8_t, mbstate_t>
    // codecvt<char32_t, char8_t, mbstate_t>

    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || is_same_v<codecvt, codecvt<char, char, mbstate_t>>,
        _FACET_SPECIALIZATION_MESSAGE);

    using intern_type = _Elem;
    using extern_type = _Byte;
    using state_type  = _Statype;

    result __CLR_OR_THIS_CALL in(_Statype& _State, const _Byte* _First1, const _Byte* _Last1, const _Byte*& _Mid1,
        _Elem* _First2, _Elem* _Last2, _Elem*& _Mid2) const { // convert bytes [_First1, _Last1) to [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL out(_Statype& _State, const _Elem* _First1, const _Elem* _Last1, const _Elem*& _Mid1,
        _Byte* _First2, _Byte* _Last2, _Byte*& _Mid2) const { // convert [_First1, _Last1) to bytes [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL unshift(_Statype& _State, _Byte* _First2, _Byte* _Last2, _Byte*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int __CLR_OR_THIS_CALL length(_Statype& _State, const _Byte* _First1, const _Byte* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count _Elems
        return do_length(_State, _First1, _Last1, _Count);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id; // unique facet id

    explicit __CLR_OR_THIS_CALL codecvt(size_t _Refs = 0) : codecvt_base(_Refs) {
        _Init(_Locinfo());
    }

    explicit __CLR_OR_THIS_CALL codecvt(const _Locinfo& _Lobj, size_t _Refs = 0) : codecvt_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new codecvt(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept {}

    void __CLR_OR_THIS_CALL _Init(const _Locinfo&) {} // initialize from _Locinfo object

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input (from codecvt)
        return is_same_v<_Byte, _Elem>;
    }

    virtual result __CLR_OR_THIS_CALL do_in(_Statype&, const _Byte* _First1, const _Byte* _Last1, const _Byte*& _Mid1,
        _Elem* _First2, _Elem* _Last2, _Elem*& _Mid2) const { // convert bytes [_First1, _Last1) to [_First2, _Last2)
        _Mid1 = _First1;
        _Mid2 = _First2;
        if constexpr (is_same_v<_Byte, _Elem>) {
            return noconv; // convert nothing
        } else {
            // types differ, copy one for one
            for (; _Mid1 != _Last1; ++_Mid1, ++_Mid2) {
                if (_Mid2 == _Last2) {
                    return partial;
                }
                *_Mid2 = static_cast<_Elem>(*_Mid1);
            }

            return ok;
        }
    }

    virtual result __CLR_OR_THIS_CALL do_out(_Statype&, const _Elem* _First1, const _Elem* _Last1, const _Elem*& _Mid1,
        _Byte* _First2, _Byte* _Last2, _Byte*& _Mid2) const { // convert [_First1, _Last1) to bytes [_First2, _Last2)
        _Mid1 = _First1;
        _Mid2 = _First2;
        if constexpr (is_same_v<_Byte, _Elem>) {
            return noconv; // convert nothing
        } else {
            // types differ, copy one for one
            for (; _Mid1 != _Last1; ++_Mid1, ++_Mid2) {
                if (_Mid2 == _Last2) {
                    return partial;
                }
                *_Mid2 = static_cast<_Byte>(*_Mid1);
            }

            return ok;
        }
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(_Statype&, _Byte* _First2, _Byte*, _Byte*& _Mid2) const {
        // generate bytes to return to default shift state
        _Mid2 = _First2;
        return noconv; // no termination required
    }

    virtual int __CLR_OR_THIS_CALL do_length(
        _Statype&, const _Byte* _First1, const _Byte* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count _Elems
        // assumes 1:1 conversion
        const auto _Dist = static_cast<size_t>((_STD min)(_Last1 - _First1, ptrdiff_t{INT_MAX}));
        return static_cast<int>((_STD min)(_Count, _Dist));
    }
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdllimport-static-field-def"
#endif // __clang__

template <class _Elem, class _Byte, class _Statype>
__PURE_APPDOMAIN_GLOBAL locale::id codecvt<_Elem, _Byte, _Statype>::id;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

template <class _CvtTy, class _Byte, class _Statype>
_NODISCARD int _Codecvt_do_length(
    const _CvtTy& _Cvt, _Statype& _State, const _Byte* _First1, const _Byte* _Last1, size_t _Count) {
    // return p - _First1, for the largest value p in [_First1, _Last1] such that _Cvt will successfully convert
    // [_First1, p) to at most _Count wide characters

    _STL_DISABLE_DEPRECATED_WARNING
    using _Elem = typename _CvtTy::intern_type;
    _STL_RESTORE_DEPRECATED_WARNING

    _Adl_verify_range(_First1, _Last1);
    const auto _Old_first1 = _First1;

    while (_Count > 0 && _First1 != _Last1) { // convert another wide character
        const _Byte* _Mid1;
        _Elem* _Mid2;
        _Elem _Ch;

        // test result of single wide-char conversion

        _STL_DISABLE_DEPRECATED_WARNING
        const auto _Result = _Cvt._CvtTy::do_in(_State, _First1, _Last1, _Mid1, &_Ch, &_Ch + 1, _Mid2);
        _STL_RESTORE_DEPRECATED_WARNING

        if (_Result != codecvt_base::ok) {
            if (_Result == codecvt_base::noconv) {
                _First1 += (_STD min)(static_cast<size_t>(_Last1 - _First1), _Count);
            }

            break; // error, noconv, or partial
        }

        if (_Mid2 == &_Ch + 1) {
            --_Count; // do_in converted an output character
        }

        _First1 = _Mid1;
    }

    return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{INT_MAX}));
}

enum _Codecvt_mode { _Consume_header = 4, _Generate_header = 2 };

template <>
class _CRTIMP2_PURE_IMPORT_UNLESS_CODECVT_ID_SATELLITE _CXX20_DEPRECATE_CODECVT_FACETS
    codecvt<char16_t, char, mbstate_t> : public codecvt_base {
    // facet for converting between char16_t and UTF-8 byte sequences
public:
    using intern_type = char16_t;
    using extern_type = char;
    using state_type  = mbstate_t;

    result __CLR_OR_THIS_CALL in(mbstate_t& _State, const char* _First1, const char* _Last1, const char*& _Mid1,
        char16_t* _First2, char16_t* _Last2, char16_t*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL out(mbstate_t& _State, const char16_t* _First1, const char16_t* _Last1,
        const char16_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL unshift(mbstate_t& _State, char* _First2, char* _Last2, char*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int __CLR_OR_THIS_CALL length(mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count UTF-16 code units
        return do_length(_State, _First1, _Last1, _Count);
    }

    _CRT_SATELLITE_CODECVT_IDS_NOIMPORT static locale::id id;

    explicit __CLR_OR_THIS_CALL codecvt(size_t _Refs = 0)
        : codecvt_base(_Refs), _Maxcode(0x10ffff), _Mode(_Consume_header) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    explicit __CLR_OR_THIS_CALL codecvt(const _Locinfo& _Lobj, size_t _Refs = 0)
        : codecvt_base(_Refs), _Maxcode(0x10ffff), _Mode(_Consume_header) {
        _Init(_Lobj);
    }

    __CLR_OR_THIS_CALL codecvt(
        const _Locinfo& _Lobj, unsigned long _Maxcode_arg, _Codecvt_mode _Mode_arg, size_t _Refs = 0)
        : codecvt_base(_Refs), _Maxcode(_Maxcode_arg), _Mode(_Mode_arg) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            _STL_DISABLE_DEPRECATED_WARNING
            *_Ppf = new codecvt(_Locinfo(_Ploc->c_str()));
            _STL_RESTORE_DEPRECATED_WARNING
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept {}

    void __CLR_OR_THIS_CALL _Init(const _Locinfo&) {} // initialize

    virtual result __CLR_OR_THIS_CALL do_in(mbstate_t& _State, const char* _First1, const char* _Last1,
        const char*& _Mid1, char16_t* _First2, char16_t* _Last2, char16_t*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        unsigned short* _Pstate = reinterpret_cast<unsigned short*>(&_State);
        _Mid1                   = _First1;
        _Mid2                   = _First2;

        while (_Mid1 != _Last1 && _Mid2 != _Last2) { // convert a multibyte sequence
            unsigned char _By = static_cast<unsigned char>(*_Mid1);
            unsigned long _Ch;
            int _Nextra;
            int _Nskip;

            if (*_Pstate > 1) {
                if (_By < 0x80 || 0xc0 <= _By) {
                    return codecvt::error; // not continuation byte
                }

                // deliver second half of two-word value
                ++_Mid1;
                *_Mid2++ = static_cast<char16_t>(*_Pstate | (_By & 0x3f));
                *_Pstate = 1;
                continue;
            }

            if (_By < 0x80u) {
                _Ch     = _By;
                _Nextra = 0;
            } else if (_By < 0xc0u) { // 0x80-0xbf not first byte
                ++_Mid1;
                return codecvt::error;
            } else if (_By < 0xe0u) {
                _Ch     = static_cast<unsigned long>(_By & 0x1f);
                _Nextra = 1;
            } else if (_By < 0xf0u) {
                _Ch     = static_cast<unsigned long>(_By & 0x0f);
                _Nextra = 2;
            } else if (_By < 0xf8u) {
                _Ch     = static_cast<unsigned long>(_By & 0x07);
                _Nextra = 3;
            } else {
                _Ch     = static_cast<unsigned long>(_By & 0x03);
                _Nextra = _By < 0xfc ? 4 : 5;
            }

            _Nskip  = _Nextra < 3 ? 0 : 1; // leave a byte for 2nd word
            _First1 = _Mid1; // roll back point

            if (_Nextra == 0) {
                ++_Mid1;
            } else if (_Last1 - _Mid1 < _Nextra + 1 - _Nskip) {
                break; // not enough input
            } else {
                for (++_Mid1; _Nskip < _Nextra; --_Nextra, ++_Mid1) {
                    if ((_By = static_cast<unsigned char>(*_Mid1)) < 0x80u || 0xc0u <= _By) {
                        return codecvt::error; // not continuation byte
                    } else {
                        _Ch = _Ch << 6 | (_By & 0x3f);
                    }
                }
            }

            if (0 < _Nskip) {
                _Ch <<= 6; // get last byte on next call
            }

            if ((_Maxcode < 0x10ffffu ? _Maxcode : 0x10ffffu) < _Ch) {
                return codecvt::error; // value too large
            }

            if (0xffffu < _Ch) { // deliver first half of two-word value, save second word
                unsigned short _Ch0 = static_cast<unsigned short>(0xd800 | (_Ch >> 10) - 0x0040);

                *_Mid2++ = static_cast<char16_t>(_Ch0);
                *_Pstate = static_cast<unsigned short>(0xdc00 | (_Ch & 0x03ff));
                continue;
            }

            if (_Nskip != 0) {
                if (_Mid1 == _Last1) { // not enough bytes, noncanonical value
                    _Mid1 = _First1;
                    break;
                }

                if ((_By = static_cast<unsigned char>(*_Mid1++)) < 0x80u || 0xc0u <= _By) {
                    return codecvt::error; // not continuation byte
                }

                _Ch |= _By & 0x3f; // complete noncanonical value
            }

            if (*_Pstate == 0u) { // first time, maybe look for and consume header
                *_Pstate = 1;

                if ((_Mode & _Consume_header) != 0 && _Ch == 0xfeffu) { // drop header and retry
                    const result _Ans = do_in(_State, _Mid1, _Last1, _Mid1, _First2, _Last2, _Mid2);

                    if (_Ans == codecvt::partial) { // roll back header determination
                        *_Pstate = 0;
                        _Mid1    = _First1;
                    }

                    return _Ans;
                }
            }

            *_Mid2++ = static_cast<char16_t>(_Ch);
        }

        return _First1 == _Mid1 ? codecvt::partial : codecvt::ok;
    }

    virtual result __CLR_OR_THIS_CALL do_out(mbstate_t& _State, const char16_t* _First1, const char16_t* _Last1,
        const char16_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        unsigned short* _Pstate = reinterpret_cast<unsigned short*>(&_State);
        _Mid1                   = _First1;
        _Mid2                   = _First2;

        while (_Mid1 != _Last1 && _Mid2 != _Last2) { // convert and put a wide char
            unsigned long _Ch;
            const unsigned short _Ch1 = static_cast<unsigned short>(*_Mid1);
            bool _Save                = false;

            if (1 < *_Pstate) { // get saved MS 11 bits from *_Pstate
                if (_Ch1 < 0xdc00u || 0xe000u <= _Ch1) {
                    return codecvt::error; // bad second word
                }

                _Ch = static_cast<unsigned long>((*_Pstate << 10) | (_Ch1 - 0xdc00));
            } else if (0xd800u <= _Ch1 && _Ch1 < 0xdc00u) { // get new first word
                _Ch   = static_cast<unsigned long>((_Ch1 - 0xd800 + 0x0040) << 10);
                _Save = true; // put only first byte, rest with second word
            } else {
                _Ch = _Ch1; // not first word, just put it
            }

            char _By;
            int _Nextra;

            if (_Ch < 0x0080u) {
                _By     = static_cast<char>(_Ch);
                _Nextra = 0;
            } else if (_Ch < 0x0800u) {
                _By     = static_cast<char>(0xc0 | _Ch >> 6);
                _Nextra = 1;
            } else if (_Ch < 0x10000u) {
                _By     = static_cast<char>(0xe0 | _Ch >> 12);
                _Nextra = 2;
            } else {
                _By     = static_cast<char>(0xf0 | _Ch >> 18);
                _Nextra = 3;
            }

            int _Nput = _Nextra < 3 ? _Nextra + 1 : _Save ? 1 : 3;

            if (_Last2 - _Mid2 < _Nput) {
                break; // not enough room, even without header
            }

            if (*_Pstate == 0u && (_Mode & _Generate_header) != 0) {
                if (_Last2 - _Mid2 < 3 + _Nput) {
                    break; // not enough room for header + output
                }

                // prepend header
                *_Mid2++ = '\xef';
                *_Mid2++ = '\xbb';
                *_Mid2++ = '\xbf';
            }

            ++_Mid1;
            if (_Save || _Nextra < 3) { // put first byte of sequence, if not already put
                *_Mid2++ = _By;
                --_Nput;
            }

            for (; 0 < _Nput; --_Nput) {
                *_Mid2++ = static_cast<char>((_Ch >> 6 * --_Nextra & 0x3f) | 0x80);
            }

            *_Pstate = static_cast<unsigned short>(_Save ? _Ch >> 10 : 1);
        }

        return _First1 == _Mid1 ? codecvt::partial : codecvt::ok;
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t& _State, char* _First2, char*, char*& _Mid2) const {
        // generate bytes to return to default shift state
        unsigned short* _Pstate = reinterpret_cast<unsigned short*>(&_State);
        _Mid2                   = _First2;

        return 1u < *_Pstate ? codecvt::error : codecvt::ok; // fail if trailing first word
    }

    friend int _Codecvt_do_length<>(const codecvt&, mbstate_t&, const char*, const char*, size_t);

    virtual int __CLR_OR_THIS_CALL do_length(
        mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        return _Codecvt_do_length(*this, _State, _First1, _Last1, _Count);
    }

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {
        // return maximum length required for a conversion

        if ((_Mode & _Consume_header) != 0) {
            return 9; // header + max input
        }

        if ((_Mode & _Generate_header) != 0) {
            return 7; // header + max output
        }

        return 6; // 6-byte max input sequence, no 3-byte header
    }

    virtual int do_encoding() const noexcept override {
        return 0; // 0 => varying length
    }

private:
    unsigned long _Maxcode; // default: 0x10ffff
    _Codecvt_mode _Mode; // default: _Consume_header
};

template <>
class _CRTIMP2_PURE_IMPORT_UNLESS_CODECVT_ID_SATELLITE _CXX20_DEPRECATE_CODECVT_FACETS
    codecvt<char32_t, char, mbstate_t> : public codecvt_base {
    // facet for converting between char32_t and UTF-8 byte sequences
public:
    using intern_type = char32_t;
    using extern_type = char;
    using state_type  = mbstate_t;

    result __CLR_OR_THIS_CALL in(mbstate_t& _State, const char* _First1, const char* _Last1, const char*& _Mid1,
        char32_t* _First2, char32_t* _Last2, char32_t*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL out(mbstate_t& _State, const char32_t* _First1, const char32_t* _Last1,
        const char32_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL unshift(mbstate_t& _State, char* _First2, char* _Last2, char*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int __CLR_OR_THIS_CALL length(mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count UTF-32 code units
        return do_length(_State, _First1, _Last1, _Count);
    }

    _CRT_SATELLITE_CODECVT_IDS_NOIMPORT static locale::id id;

    explicit __CLR_OR_THIS_CALL codecvt(size_t _Refs = 0)
        : codecvt_base(_Refs), _Maxcode(0xffffffff), _Mode(_Consume_header) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    explicit __CLR_OR_THIS_CALL codecvt(const _Locinfo& _Lobj, size_t _Refs = 0)
        : codecvt_base(_Refs), _Maxcode(0xffffffff), _Mode(_Consume_header) {
        _Init(_Lobj);
    }

    __CLR_OR_THIS_CALL codecvt(
        const _Locinfo& _Lobj, unsigned long _Maxcode_arg, _Codecvt_mode _Mode_arg, size_t _Refs = 0)
        : codecvt_base(_Refs), _Maxcode(_Maxcode_arg), _Mode(_Mode_arg) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            _STL_DISABLE_DEPRECATED_WARNING
            *_Ppf = new codecvt(_Locinfo(_Ploc->c_str()));
            _STL_RESTORE_DEPRECATED_WARNING
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept {}

    void __CLR_OR_THIS_CALL _Init(const _Locinfo&) {} // initialize

    virtual result __CLR_OR_THIS_CALL do_in(mbstate_t& _State, const char* _First1, const char* _Last1,
        const char*& _Mid1, char32_t* _First2, char32_t* _Last2, char32_t*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        char* _Pstate = reinterpret_cast<char*>(&_State);
        _Mid1         = _First1;
        _Mid2         = _First2;

        while (_Mid1 != _Last1 && _Mid2 != _Last2) { // convert a multibyte sequence
            unsigned char _By = static_cast<unsigned char>(*_Mid1);
            unsigned long _Ch;
            int _Nextra;

            if (_By < 0x80u) {
                _Ch     = _By;
                _Nextra = 0;
            } else if (_By < 0xc0u) { // 0x80-0xbf not first byte
                ++_Mid1;
                return codecvt::error;
            } else if (_By < 0xe0u) {
                _Ch     = static_cast<unsigned long>(_By & 0x1f);
                _Nextra = 1;
            } else if (_By < 0xf0u) {
                _Ch     = static_cast<unsigned long>(_By & 0x0f);
                _Nextra = 2;
            } else if (_By < 0xf8u) {
                _Ch     = static_cast<unsigned long>(_By & 0x07);
                _Nextra = 3;
            } else {
                _Ch     = static_cast<unsigned long>(_By & 0x03);
                _Nextra = _By < 0xfc ? 4 : 5;
            }

            if (_Nextra == 0) {
                ++_Mid1;
            } else if (_Last1 - _Mid1 < _Nextra + 1) {
                break; // not enough input
            } else {
                for (++_Mid1; 0 < _Nextra; --_Nextra, ++_Mid1) {
                    if ((_By = static_cast<unsigned char>(*_Mid1)) < 0x80u || 0xc0u <= _By) {
                        return codecvt::error; // not continuation byte
                    } else {
                        _Ch = _Ch << 6 | (_By & 0x3f);
                    }
                }
            }

            if (*_Pstate == 0) { // first time, maybe look for and consume header
                *_Pstate = 1;

                if ((_Mode & _Consume_header) != 0 && _Ch == 0xfeffu) { // drop header and retry
                    const result _Ans = do_in(_State, _Mid1, _Last1, _Mid1, _First2, _Last2, _Mid2);

                    if (_Ans == codecvt::partial) { // roll back header determination
                        *_Pstate = 0;
                        _Mid1    = _First1;
                    }
                    return _Ans;
                }
            }

            if (_Maxcode < _Ch) {
                return codecvt::error; // code too large
            }

            *_Mid2++ = static_cast<char32_t>(_Ch);
        }

        return _First1 == _Mid1 ? codecvt::partial : codecvt::ok;
    }

    virtual result __CLR_OR_THIS_CALL do_out(mbstate_t& _State, const char32_t* _First1, const char32_t* _Last1,
        const char32_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        char* _Pstate = reinterpret_cast<char*>(&_State);
        _Mid1         = _First1;
        _Mid2         = _First2;

        while (_Mid1 != _Last1 && _Mid2 != _Last2) { // convert and put a wide char
            char _By;
            int _Nextra;
            unsigned long _Ch = static_cast<unsigned long>(*_Mid1);

            if (_Maxcode < _Ch) {
                return codecvt::error;
            }

            if (_Ch < 0x0080u) {
                _By     = static_cast<char>(_Ch);
                _Nextra = 0;
            } else if (_Ch < 0x0800u) {
                _By     = static_cast<char>(0xc0 | _Ch >> 6);
                _Nextra = 1;
            } else if (_Ch < 0x00010000u) {
                _By     = static_cast<char>(0xe0 | _Ch >> 12);
                _Nextra = 2;
            } else if (_Ch < 0x00200000u) {
                _By     = static_cast<char>(0xf0 | _Ch >> 18);
                _Nextra = 3;
            } else if (_Ch < 0x04000000u) {
                _By     = static_cast<char>(0xf8 | _Ch >> 24);
                _Nextra = 4;
            } else {
                _By     = static_cast<char>(0xfc | (_Ch >> 30 & 0x03));
                _Nextra = 5;
            }

            if (*_Pstate == 0) { // first time, maybe generate header
                *_Pstate = 1;
                if ((_Mode & _Generate_header) != 0) {
                    if (_Last2 - _Mid2 < 3 + 1 + _Nextra) {
                        return codecvt::partial; // not enough room for both
                    }

                    // prepend header
                    *_Mid2++ = '\xef';
                    *_Mid2++ = '\xbb';
                    *_Mid2++ = '\xbf';
                }
            }

            if (_Last2 - _Mid2 < 1 + _Nextra) {
                break; // not enough room for output
            }

            ++_Mid1;
            for (*_Mid2++ = _By; 0 < _Nextra;) {
                *_Mid2++ = static_cast<char>((_Ch >> 6 * --_Nextra & 0x3f) | 0x80);
            }
        }
        return _First1 == _Mid1 ? codecvt::partial : codecvt::ok;
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t&, char* _First2, char*, char*& _Mid2) const {
        // generate bytes to return to default shift state
        _Mid2 = _First2;
        return codecvt::noconv;
    }

    friend int _Codecvt_do_length<>(const codecvt&, mbstate_t&, const char*, const char*, size_t);

    virtual int __CLR_OR_THIS_CALL do_length(
        mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        return _Codecvt_do_length(*this, _State, _First1, _Last1, _Count);
    }

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {
        // return maximum length required for a conversion
        return (_Mode & (_Consume_header | _Generate_header)) != 0 ? 9 : 6;
    }

    virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept override {
        // return length of code sequence (from codecvt)
        return (_Mode & (_Consume_header | _Generate_header)) != 0 ? -1
                                                                   : 0; // -1 => state dependent, 0 => varying length
    }

private:
    unsigned long _Maxcode; // default: 0xffffffff
    _Codecvt_mode _Mode; // default: _Consume_header
};

#if defined(__cpp_char8_t) && !defined(_M_CEE_PURE)
template <class _From, class _To>
struct _NODISCARD _Codecvt_guard {
    const _From* const& _First1;
    const _From*& _Mid1;
    _To* const& _First2;
    _To*& _Mid2;

    _Codecvt_guard(const _From* const& _First1_, const _From*& _Mid1_, _To* const& _First2_, _To*& _Mid2_)
        : _First1{_First1_}, _Mid1{_Mid1_}, _First2{_First2_}, _Mid2{_Mid2_} {}

    _Codecvt_guard(const _Codecvt_guard&) = delete;
    _Codecvt_guard& operator=(const _Codecvt_guard&) = delete;

    ~_Codecvt_guard() {
        _Mid1 = _First1;
        _Mid2 = _First2;
    }
};

template <>
class codecvt<char16_t, char8_t, mbstate_t> : public codecvt_base {
    // facet for converting between UTF-16 and UTF-8 sequences
public:
    using intern_type = char16_t;
    using extern_type = char8_t;
    using state_type  = mbstate_t;

    result in(mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, const char8_t*& _Mid1,
        char16_t* _First2, char16_t* _Last2, char16_t*& _Mid2) const {
        // convert UTF-8 [_First1, _Last1) to UTF-16 [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result out(mbstate_t& _State, const char16_t* _First1, const char16_t* _Last1, const char16_t*& _Mid1,
        char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {
        // convert UTF-16 [_First1, _Last1) to UTF-8 [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result unshift(mbstate_t& _State, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int length(mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count UTF-16 code units
        return do_length(_State, _First1, _Last1, _Count);
    }

    _CRT_SATELLITE_CODECVT_IDS static locale::id id;

    explicit codecvt(size_t _Refs = 0) : codecvt_base(_Refs) {} // construct "from current locale"
    explicit codecvt(const _Locinfo&, size_t _Refs = 0) : codecvt_base(_Refs) {} // construct "from specified locale"

    static size_t _Getcat(const locale::facet** _Ppf = nullptr, const locale* = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new codecvt;
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept override = default;

    virtual result __CLR_OR_THIS_CALL do_in(mbstate_t&, const char8_t* _First1, const char8_t* _Last1,
        const char8_t*& _Mid1, char16_t* _First2, char16_t* _Last2, char16_t*& _Mid2) const {
        // convert UTF-8 [_First1, _Last1) to UTF-16 [_First2, _Last2)
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);

        _Codecvt_guard<char8_t, char16_t> _Guard{_First1, _Mid1, _First2, _Mid2};

        for (; _First1 != _Last1; ++_First1, ++_First2) {
            if (_First2 == _Last2) {
                return partial;
            }

            char8_t _Lead_byte = *_First1;
            if (_Lead_byte < 0b1000'0000u) { // single-byte sequence
                *_First2 = static_cast<char16_t>(_Lead_byte);
                continue;
            }

            int _Trailing_count = 1;
            if (_Lead_byte < 0b1110'0000u) {
                if (_Lead_byte < 0b1100'0000u) { // out-of-sequence trailing byte
                    return error;
                }

                // lead byte of 2-byte sequence
                _Lead_byte &= 0b0001'1111u;
            } else if (_Lead_byte < 0b1111'0000u) { // lead byte of 3-byte sequence
                _Lead_byte &= 0b0000'1111u;
                _Trailing_count = 2;
            } else if (_Lead_byte < 0b1111'1000u) { // lead byte of 4-byte sequence
                if (_Last2 - _First2 < 2) { // not enough output for a surrogate pair
                    return partial;
                }

                _Lead_byte &= 0b0000'0111u;
                _Trailing_count = 3;
            } else { // Invalid UTF-8 code unit
                return error;
            }

            if (_Last1 - _First1 < _Trailing_count + 1) { // not enough input
                return partial;
            }

            const char8_t* _Peek = _First1;
            char32_t _Code_point = _Lead_byte;
            do {
                const char8_t _By = *++_Peek;
                if ((_By & 0b1100'0000u) != 0b1000'0000u) { // out-of-sequence lead byte
                    return error;
                }

                _Code_point = (_Code_point << 6) | (_By & 0b11'1111u);
            } while (--_Trailing_count != 0);

            if (_Code_point < 0x10000u) {
                if (_Code_point >= 0xd800u && _Code_point < 0xe000u) { // invalid code point (surrogate)
                    return error;
                }
                // Output single code unit
                *_First2 = static_cast<char16_t>(_Code_point);
            } else if (_Code_point >= 0x110000u) { // Invalid code point (out of range)
                return error;
            } else {
                // Output surrogate pair
                _Code_point -= 0x10000u;
                // High surrogate: 0xd800 | <upper ten bits>
                *_First2 = static_cast<char16_t>(0xd800u | (_Code_point >> 10));
                // Low surrogate: 0xdc00 | <lower ten bits>
                *++_First2 = static_cast<char16_t>(0xdc00u | (_Code_point & 0b11'1111'1111u));
            }

            _First1 = _Peek;
        }

        return ok;
    }

    virtual result __CLR_OR_THIS_CALL do_out(mbstate_t&, const char16_t* _First1, const char16_t* _Last1,
        const char16_t*& _Mid1, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {
        // convert UTF-16 [_First1, _Last1) to UTF-8 [_First2, _Last2)
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);

        _Codecvt_guard<char16_t, char8_t> _Guard{_First1, _Mid1, _First2, _Mid2};

        for (; _First1 != _Last1; ++_First1, ++_First2) {
            if (_First2 == _Last2) { // no more output
                return partial;
            }

            char32_t _Code_point = *_First1;
            if (_Code_point < 0x80u) { // encode 1-byte sequence
                *_First2 = static_cast<char8_t>(_Code_point);
                continue;
            }

            int _Trailing_count = 1;
            if (_Code_point < 0x800u) { // encode 2-byte sequence
                *_First2 = static_cast<char8_t>(0b1100'0000u | (_Code_point >> 6));
            } else if (_Code_point < 0xd800u || _Code_point >= 0xe000u) { // encode 3-byte sequence
                *_First2        = static_cast<char8_t>(0b1110'0000u | (_Code_point >> 12));
                _Trailing_count = 2;
            } else if (_Code_point < 0xdc00u) { // high surrogate; parse the low surrogate
                if (_Last1 - _First1 < 2) { // not enough input
                    return partial;
                }

                const char16_t _Low_surrogate = *++_First1;
                if (_Low_surrogate < 0xdc00u || _Low_surrogate >= 0xe000u) { // invalid low surrogate
                    --_First1;
                    return error;
                }
                _Code_point = 0x10000u + (((_Code_point & 0b11'1111'1111u) << 10) | (_Low_surrogate & 0b11'1111'1111u));

                // encode 4-byte sequence
                *_First2        = static_cast<char8_t>(0b1111'0000u | (_Code_point >> 18));
                _Trailing_count = 3;
            } else { // out-of-sequence low surrogate
                return error;
            }

            if (_Last2 - _First2 < _Trailing_count + 1) { // not enough output
                if (_Trailing_count > 2) { // input was a surrogate pair; revert
                    --_First1;
                }
                return partial;
            }

            do {
                --_Trailing_count;
                *++_First2 = static_cast<char8_t>(0b1000'0000u | ((_Code_point >> (6 * _Trailing_count)) & 0b11'1111u));
            } while (_Trailing_count != 0);
        }

        return ok;
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t&, char8_t* _First2, char8_t*, char8_t*& _Mid2) const {
        // terminate encoding sequence
        _Mid2 = _First2;
        return noconv; // no termination necessary
    }

    virtual int __CLR_OR_THIS_CALL do_length(
        mbstate_t&, const char8_t* _First1, const char8_t* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count UTF-16 code units
        _Adl_verify_range(_First1, _Last1);

        const auto _Old_first1 = _First1;

        for (; _First1 != _Last1 && _Count > 0u; ++_First1, --_Count) {
            char32_t _Code_point = *_First1;
            if (_Code_point < 0b1000'0000u) { // single-byte sequence
                continue;
            }

            int _Trailing_count = 1;
            if (_Code_point < 0b1110'0000u) {
                if (_Code_point < 0b1100'0000u) { // out-of-sequence trailing byte
                    break;
                }

                // lead byte of 2-byte sequence
                _Code_point &= 0b0001'1111u;
            } else if (_Code_point < 0b1111'0000u) { // lead byte of 3-byte sequence
                _Code_point &= 0b0000'1111u;
                _Trailing_count = 2;
            } else if (_Code_point < 0b1111'1000u) { // lead byte of 4-byte sequence
                if (_Count < 2u) { // not enough output
                    break;
                }

                _Code_point &= 0b0000'0111u;
                _Trailing_count = 3;
            } else { // Invalid UTF-8 code unit
                break;
            }

            if (_Last1 - _First1 < _Trailing_count + 1) { // not enough input
                break;
            }

            const char8_t* _Peek = _First1;
            bool _Done           = false;
            do {
                const char8_t _By = *++_Peek;
                if ((_By & 0b1100'0000u) != 0b1000'0000u) { // out-of-sequence lead byte
                    _Done = true;
                    break;
                }

                _Code_point = (_Code_point << 6) | (_By & 0b11'1111u);
            } while (--_Trailing_count != 0);

            if (_Done) {
                break;
            }

            if (_Code_point < 0x10000u) { // Output single code unit
                if (_Code_point >= 0xd800u && _Code_point < 0xe0000u) { // invalid code point (surrogate)
                    break;
                }
            } else if (_Code_point < 0x110000u) { // Output surrogate pair
                --_Count;
            } else { // Invalid code point (out of range)
                break;
            }

            _First1 = _Peek;
        }

        return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{INT_MAX}));
    }

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {
        // return maximum length required for a conversion
        return 4; // 4-byte max input sequence
    }

    virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept override {
        // return length of code sequence (from codecvt)
        return 0; // 0 => varying length
    }
};

template <>
class codecvt<char32_t, char8_t, mbstate_t> : public codecvt_base {
    // facet for converting between UTF-32 and UTF-8 sequences
public:
    using intern_type = char32_t;
    using extern_type = char8_t;
    using state_type  = mbstate_t;

    result in(mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, const char8_t*& _Mid1,
        char32_t* _First2, char32_t* _Last2, char32_t*& _Mid2) const {
        // convert UTF-8 [_First1, _Last1) to UTF-32 [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result out(mbstate_t& _State, const char32_t* _First1, const char32_t* _Last1, const char32_t*& _Mid1,
        char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {
        // convert UTF-32 [_First1, _Last1) to UTF-8 [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result unshift(mbstate_t& _State, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int length(mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count UTF-32 code units
        return do_length(_State, _First1, _Last1, _Count);
    }

    _CRT_SATELLITE_CODECVT_IDS static locale::id id;

    explicit codecvt(size_t _Refs = 0) : codecvt_base(_Refs) {} // construct "from current locale"
    explicit codecvt(const _Locinfo&, size_t _Refs = 0) : codecvt_base(_Refs) {} // construct "from specified locale"

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new codecvt;
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept override = default;

    virtual result __CLR_OR_THIS_CALL do_in(mbstate_t&, const char8_t* _First1, const char8_t* _Last1,
        const char8_t*& _Mid1, char32_t* _First2, char32_t* _Last2, char32_t*& _Mid2) const {
        // convert UTF-8 [_First1, _Last1) to UTF-32 [_First2, _Last2)
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);

        _Codecvt_guard<char8_t, char32_t> _Guard{_First1, _Mid1, _First2, _Mid2};

        for (; _First1 != _Last1; ++_First1, ++_First2) {
            if (_First2 == _Last2) {
                return partial;
            }

            char8_t _Lead_byte = *_First1;
            if (_Lead_byte < 0b1000'0000u) { // single-byte sequence
                *_First2 = _Lead_byte;
                continue;
            }

            int _Trailing_count = 1;
            if (_Lead_byte < 0b1110'0000u) {
                if (_Lead_byte < 0b1100'0000u) { // out-of-sequence trailing byte
                    return partial;
                }

                // lead byte of 2-byte sequence
                _Lead_byte &= 0b0001'1111u;
            } else if (_Lead_byte < 0b1111'0000u) { // lead byte of 3-byte sequence
                _Lead_byte &= 0b0000'1111u;
                _Trailing_count = 2;
            } else if (_Lead_byte < 0b1111'1000u) { // lead byte of 4-byte sequence
                _Lead_byte &= 0b0000'0111u;
                _Trailing_count = 3;
            } else { // invalid UTF-8 code unit
                return error;
            }

            if (_Last1 - _First1 < _Trailing_count + 1) { // not enough input
                return partial;
            }

            const char8_t* _Peek = _First1;
            char32_t _Code_point = _Lead_byte;
            do {
                const char8_t _By = *++_Peek;
                if ((_By & 0b1100'0000u) != 0b1000'0000u) { // out-of-sequence lead byte
                    return error;
                }

                _Code_point = (_Code_point << 6) | (_By & 0b11'1111u);
            } while (--_Trailing_count != 0);

            if (_Code_point >= 0xd800u && (_Code_point < 0xe000u || _Code_point >= 0x110000u)) {
                // invalid code point (surrogate or out of range)
                return error;
            }

            _First1  = _Peek;
            *_First2 = _Code_point;
        }

        return ok;
    }

    virtual result __CLR_OR_THIS_CALL do_out(mbstate_t&, const char32_t* _First1, const char32_t* _Last1,
        const char32_t*& _Mid1, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {
        // convert UTF-32 [_First1, _Last1) to UTF-8 [_First2, _Last2)
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);

        _Codecvt_guard<char32_t, char8_t> _Guard{_First1, _Mid1, _First2, _Mid2};

        for (; _First1 != _Last1; ++_First1, ++_First2) {
            if (_First2 == _Last2) { // no more output
                return partial;
            }

            const char32_t _Code_point = *_First1;
            if (_Code_point < 0x80u) { // encode 1-byte sequence
                *_First2 = static_cast<char8_t>(_Code_point);
                continue;
            }

            int _Trailing_count = 1;
            if (_Code_point < 0x800u) { // encode 2-byte sequence
                *_First2 = static_cast<char8_t>(0b1100'0000u | (_Code_point >> 6));
            } else if (_Code_point < 0x10000u) { // encode 3-byte sequence
                if (_Code_point >= 0xd800u && _Code_point < 0xe000u) { // invalid code point (surrogate)
                    return error;
                }

                *_First2        = static_cast<char8_t>(0b1110'0000u | (_Code_point >> 12));
                _Trailing_count = 2;
            } else if (_Code_point < 0x110000u) { // encode 4-byte sequence
                *_First2        = static_cast<char8_t>(0b1111'0000u | (_Code_point >> 18));
                _Trailing_count = 3;
            } else { // Invalid code point (out of range)
                return error;
            }

            if (_Last2 - _First2 < _Trailing_count + 1) { // not enough output
                return partial;
            }

            do {
                --_Trailing_count;
                *++_First2 = static_cast<char8_t>(0b1000'0000u | ((_Code_point >> (6 * _Trailing_count)) & 0b11'1111u));
            } while (_Trailing_count != 0);
        }

        return ok;
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t&, char8_t* _First2, char8_t*, char8_t*& _Mid2) const {
        // terminate encoding sequence
        _Mid2 = _First2;
        return noconv; // no termination necessary
    }

    virtual int __CLR_OR_THIS_CALL do_length(
        mbstate_t&, const char8_t* _First1, const char8_t* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count UTF-32 code units
        _Adl_verify_range(_First1, _Last1);

        const auto _Old_first1 = _First1;

        for (; _First1 != _Last1 && _Count > 0u; ++_First1, --_Count) {
            char32_t _Code_point = *_First1;
            if (_Code_point < 0b1000'0000u) { // single-byte sequence
                continue;
            }

            int _Trailing_count = 1;
            if (_Code_point < 0b1110'0000u) {
                if (_Code_point < 0b1100'0000u) { // out-of-sequence trailing byte
                    break;
                }

                // lead byte of 2-byte sequence
                _Code_point &= 0b0001'1111u;
            } else if (_Code_point < 0b1111'0000u) { // lead byte of 3-byte sequence
                _Code_point &= 0b0000'1111u;
                _Trailing_count = 2;
            } else if (_Code_point < 0b1111'1000u) { // lead byte of 4-byte sequence
                _Code_point &= 0b0000'0111u;
                _Trailing_count = 3;
            } else { // invalid UTF-8 code unit
                break;
            }

            if (_Last1 - _First1 < _Trailing_count + 1) { // not enough input
                break;
            }

            const char8_t* _Peek = _First1;
            bool _Done           = false;
            do {
                const char8_t _By = *++_Peek;
                if ((_By & 0b1100'0000u) != 0b1000'0000u) { // out-of-sequence lead byte
                    _Done = true;
                    break;
                }

                _Code_point = (_Code_point << 6) | (_By & 0b11'1111u);
            } while (--_Trailing_count != 0);

            if (_Done || (_Code_point >= 0xd800u && (_Code_point < 0xe000u || _Code_point >= 0x110000u))) {
                // invalid code point (surrogate or out of range)
                break;
            }

            _First1 = _Peek;
        }

        return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{INT_MAX}));
    }

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {
        // return maximum length required for a conversion
        return 4;
    }

    virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept override {
        // return length of code sequence (from codecvt)
        return 0; // varying length
    }
};
#endif // defined(__cpp_char8_t) && !defined(_M_CEE_PURE)

template <>
class _CRTIMP2_PURE_IMPORT codecvt<wchar_t, char, mbstate_t> : public codecvt_base {
    // facet for converting between wchar_t and char (_Byte) sequences
public:
    using intern_type = wchar_t;
    using extern_type = char;
    using state_type  = mbstate_t;

    result __CLR_OR_THIS_CALL in(mbstate_t& _State, const char* _First1, const char* _Last1, const char*& _Mid1,
        wchar_t* _First2, wchar_t* _Last2, wchar_t*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL out(mbstate_t& _State, const wchar_t* _First1, const wchar_t* _Last1,
        const wchar_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL unshift(mbstate_t& _State, char* _First2, char* _Last2, char*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int __CLR_OR_THIS_CALL length(mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count wide characters
        return do_length(_State, _First1, _Last1, _Count);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id;

    explicit __CLR_OR_THIS_CALL codecvt(size_t _Refs = 0) : codecvt_base(_Refs) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    explicit __CLR_OR_THIS_CALL codecvt(const _Locinfo& _Lobj, size_t _Refs = 0) : codecvt_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new codecvt(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept {}

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Cvt = _Lobj._Getcvt();
    }

    virtual result __CLR_OR_THIS_CALL do_in(mbstate_t&, const char* _First1, const char* _Last1, const char*& _Mid1,
        wchar_t* _First2, wchar_t* _Last2, wchar_t*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        mbstate_t _Mystate{};
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);
        _Mid1 = _First1;
        _Mid2 = _First2;
        for (;;) {
            if (_Mid1 == _Last1) {
                return ok;
            }

            if (_Mid2 == _Last2) {
                return partial;
            }

            int _Bytes = _Mbrtowc(_Mid2, _Mid1, static_cast<size_t>(_Last1 - _Mid1), &_Mystate, &_Cvt);
            switch (_Bytes) {
            case -2: // partial conversion
                return partial;

            case -1: // failed conversion
                return error;

            case 0: // converted NULL character, TRANSITION, VSO-654347
                _Bytes = 1;
                // [[fallthrough]];

            default: // converted some other character
                _Mid1 += _Bytes;
                ++_Mid2;
                break;
            }
        }
    }

    virtual result __CLR_OR_THIS_CALL do_out(mbstate_t& _State, const wchar_t* _First1, const wchar_t* _Last1,
        const wchar_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);
        _Mid1 = _First1;
        _Mid2 = _First2;
        int _Bytes;

        while (_Mid1 != _Last1 && _Mid2 != _Last2) {
            if (MB_LEN_MAX <= _Last2 - _Mid2) {
                if ((_Bytes = _Wcrtomb(_Mid2, *_Mid1, &_State, &_Cvt)) < 0) {
                    return error; // locale-specific wcrtomb failed
                } else {
                    ++_Mid1;
                    _Mid2 += _Bytes;
                }
            } else { // destination too small, convert into buffer
                char _Buf[MB_LEN_MAX];
                mbstate_t _Stsave = _State;

                if ((_Bytes = _Wcrtomb(_Buf, *_Mid1, &_State, &_Cvt)) < 0) {
                    return error; // locale-specific wcrtomb failed
                } else if (_Last2 - _Mid2 < _Bytes) { // converted too many, roll back and return previous
                    _State = _Stsave;
                    break;
                } else { // copy converted bytes from buffer
                    _CSTD memcpy(_Mid2, _Buf, static_cast<size_t>(_Bytes));
                    ++_Mid1;
                    _Mid2 += _Bytes;
                }
            }
        }

        return _Mid1 == _Last1 ? ok : partial;
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t& _State, char* _First2, char* _Last2, char*& _Mid2) const {
        // generate bytes to return to default shift state
        _Adl_verify_range(_First2, _Last2);
        _Mid2       = _First2;
        result _Ans = ok;
        int _Bytes;
        char _Buf[MB_LEN_MAX];
        mbstate_t _Stsave = _State;

        if ((_Bytes = _Wcrtomb(_Buf, L'\0', &_State, &_Cvt)) <= 0) {
            _Ans = error; // locale-specific wcrtomb failed
        } else if (_Last2 - _Mid2 < --_Bytes) { // converted too many, roll back and return
            _State = _Stsave;
            _Ans   = partial;
        } else if (0 < _Bytes) { // copy converted bytes from buffer
            _CSTD memcpy(_Mid2, _Buf, static_cast<size_t>(_Bytes));
            _Mid2 += _Bytes;
        }
        return _Ans;
    }

    virtual int __CLR_OR_THIS_CALL do_length(
        mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count wide characters
        _Adl_verify_range(_First1, _Last1);
        const auto _Old_first1 = _First1;

        for (; _Count > 0u && _First1 != _Last1; --_Count) {
            wchar_t _Ch;
            int _Bytes = _Mbrtowc(&_Ch, _First1, static_cast<size_t>(_Last1 - _First1), &_State, &_Cvt);
            if (_Bytes < 0) { // partial or failed conversion
                break;
            }

            if (_Bytes == 0) { // converted NULL character, TRANSITION, VSO-654347
                _Bytes = 1;
            }

            // converted _Bytes bytes to a wide character
            _First1 += _Bytes;
        }

        return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{INT_MAX}));
    }

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {
        // return maximum length required for a conversion (from codecvt)
        return static_cast<int>(_Cvt._Mbcurmax);
    }

    virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept override {
        // return length of code sequence (from codecvt)
        return _Cvt._Mbcurmax == 1; // 0 => varying length, 1 => fixed length
    }

private:
    _Locinfo::_Cvtvec _Cvt; // locale info passed to _Mbrtowc, _Wcrtomb
};

#ifdef _NATIVE_WCHAR_T_DEFINED
template <>
class _CRTIMP2_PURE_IMPORT codecvt<unsigned short, char, mbstate_t> : public codecvt_base {
    // facet for converting between unsigned short and char sequences
public:
    using intern_type = unsigned short;
    using extern_type = char;
    using state_type  = mbstate_t;

    result __CLR_OR_THIS_CALL in(mbstate_t& _State, const char* _First1, const char* _Last1, const char*& _Mid1,
        unsigned short* _First2, unsigned short* _Last2, unsigned short*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL out(mbstate_t& _State, const unsigned short* _First1, const unsigned short* _Last1,
        const unsigned short*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
    }

    result __CLR_OR_THIS_CALL unshift(mbstate_t& _State, char* _First2, char* _Last2, char*& _Mid2) const {
        // generate bytes to return to default shift state
        return do_unshift(_State, _First2, _Last2, _Mid2);
    }

    int __CLR_OR_THIS_CALL length(mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count _Elems
        return do_length(_State, _First1, _Last1, _Count);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id;

    explicit __CLR_OR_THIS_CALL codecvt(size_t _Refs = 0) : codecvt_base(_Refs) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    explicit __CLR_OR_THIS_CALL codecvt(const _Locinfo& _Lobj, size_t _Refs = 0) : codecvt_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        // return locale category mask and construct standard facet
        if (_Ppf && !*_Ppf) {
            *_Ppf = new codecvt(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt() noexcept {}

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Cvt = _Lobj._Getcvt();
    }

    virtual result __CLR_OR_THIS_CALL do_in(mbstate_t&, const char* _First1, const char* _Last1, const char*& _Mid1,
        unsigned short* _First2, unsigned short* _Last2, unsigned short*& _Mid2) const {
        // convert bytes [_First1, _Last1) to [_First2, _Last2)
        mbstate_t _Mystate{};
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);
        _Mid1 = _First1;
        _Mid2 = _First2;
        for (;;) {
            if (_Mid1 == _Last1) {
                return ok;
            }

            if (_Mid2 == _Last2) {
                return partial;
            }

            int _Bytes = _Mbrtowc(
                reinterpret_cast<wchar_t*>(_Mid2), _Mid1, static_cast<size_t>(_Last1 - _Mid1), &_Mystate, &_Cvt);
            switch (_Bytes) {
            case -2: // partial conversion
                return partial;

            case -1: // failed conversion
                return error;

            case 0: // converted NULL character, TRANSITION, VSO-654347
                _Bytes = 1;
                // [[fallthrough]];

            default: // converted some other character
                _Mid1 += _Bytes;
                ++_Mid2;
                break;
            }
        }
    }

    virtual result __CLR_OR_THIS_CALL do_out(mbstate_t& _State, const unsigned short* _First1,
        const unsigned short* _Last1, const unsigned short*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const {
        // convert [_First1, _Last1) to bytes [_First2, _Last2)
        _Adl_verify_range(_First1, _Last1);
        _Adl_verify_range(_First2, _Last2);
        _Mid1 = _First1;
        _Mid2 = _First2;
        int _Bytes;

        while (_Mid1 != _Last1 && _Mid2 != _Last2) {
            if (MB_LEN_MAX <= _Last2 - _Mid2) {
                if ((_Bytes = _Wcrtomb(_Mid2, *_Mid1, &_State, &_Cvt)) < 0) {
                    return error; // locale-specific wcrtomb failed
                } else {
                    ++_Mid1;
                    _Mid2 += _Bytes;
                }
            } else { // destination too small, convert into buffer
                char _Buf[MB_LEN_MAX];
                mbstate_t _Stsave = _State;

                if ((_Bytes = _Wcrtomb(_Buf, *_Mid1, &_State, &_Cvt)) < 0) {
                    return error; // locale-specific wcrtomb failed
                } else if (_Last2 - _Mid2 < _Bytes) { // converted too many, roll back and return previous
                    _State = _Stsave;
                    break;
                } else { // copy converted bytes from buffer
                    _CSTD memcpy(_Mid2, _Buf, static_cast<size_t>(_Bytes));
                    ++_Mid1;
                    _Mid2 += _Bytes;
                }
            }
        }

        return _Mid1 == _Last1 ? ok : partial;
    }

    virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t& _State, char* _First2, char* _Last2, char*& _Mid2) const {
        // generate bytes to return to default shift state
        _Adl_verify_range(_First2, _Last2);
        _Mid2       = _First2;
        result _Ans = ok;
        int _Bytes;
        char _Buf[MB_LEN_MAX];
        mbstate_t _Stsave = _State;

        if ((_Bytes = _Wcrtomb(_Buf, L'\0', &_State, &_Cvt)) <= 0) {
            _Ans = error; // locale-specific wcrtomb failed
        } else if (_Last2 - _Mid2 < --_Bytes) { // converted too many, roll back and return
            _State = _Stsave;
            _Ans   = partial;
        } else if (0 < _Bytes) { // copy converted bytes from buffer
            _CSTD memcpy(_Mid2, _Buf, static_cast<size_t>(_Bytes));
            _Mid2 += _Bytes;
        }

        return _Ans;
    }

    virtual int __CLR_OR_THIS_CALL do_length(
        mbstate_t& _State, const char* _First1, const char* _Last1, size_t _Count) const {
        // return p - _First1, for the largest value p in [_First1, _Last1] such that [_First1, p) successfully
        // converts to at most _Count wide characters
        _Adl_verify_range(_First1, _Last1);
        const auto _Old_first1 = _First1;

        for (; _Count > 0u && _First1 != _Last1; --_Count) {
            wchar_t _Ch;
            int _Bytes = _Mbrtowc(&_Ch, _First1, static_cast<size_t>(_Last1 - _First1), &_State, &_Cvt);
            if (_Bytes < 0) { // partial or failed conversion
                break;
            }

            if (_Bytes == 0) { // converted NULL character, TRANSITION, VSO-654347
                _Bytes = 1;
            }

            // converted _Bytes bytes to a wide character
            _First1 += _Bytes;
        }

        return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{INT_MAX}));
    }

    virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {
        // return true if conversions never change input
        return false;
    }

    virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {
        // return maximum length required for a conversion (from codecvt)
        return static_cast<int>(_Cvt._Mbcurmax);
    }

    virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept override {
        // return length of code sequence (from codecvt)
        return _Cvt._Mbcurmax == 1u; // 0 => varying length, 1 => fixed length
    }

private:
    _Locinfo::_Cvtvec _Cvt; // locale info passed to _Mbrtowc, _Wcrtomb
};
#endif // _NATIVE_WCHAR_T_DEFINED

template <class _Elem, class _Byte, class _Statype>
class codecvt_byname : public codecvt<_Elem, _Byte, _Statype> { // codecvt for named locale
public:
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS
                      || _Is_any_of_v<codecvt_byname,
#ifdef __cpp_char8_t
                          codecvt_byname<char16_t, char8_t, mbstate_t>, codecvt_byname<char32_t, char8_t, mbstate_t>,
#endif // __cpp_char8_t
                          codecvt_byname<char, char, mbstate_t>, codecvt_byname<wchar_t, char, mbstate_t>>,
        _FACET_SPECIALIZATION_MESSAGE);

    explicit __CLR_OR_THIS_CALL codecvt_byname(const char* _Locname, size_t _Refs = 0)
        : codecvt<_Elem, _Byte, _Statype>(_Locinfo(_Locname), _Refs) {} // construct for named locale

    explicit __CLR_OR_THIS_CALL codecvt_byname(const string& _Str, size_t _Refs = 0)
        : codecvt<_Elem, _Byte, _Statype>(_Locinfo(_Str.c_str()), _Refs) {} // construct for named locale

protected:
    virtual __CLR_OR_THIS_CALL ~codecvt_byname() noexcept {}
};

#define _XA 0x100 // extra alphabetic
#define _BB _CONTROL // BEL, BS, etc.
#define _CN _SPACE // CR, FF, HT, NL, VT
#define _DI _DIGIT // '0'-'9'
#define _LO _LOWER // 'a'-'z'
#define _PU _PUNCT // punctuation
#define _SP _BLANK // space
#define _UP _UPPER // 'A'-'Z'
#define _XD _HEX // '0'-'9', 'A'-'F', 'a'-'f'

struct _CRTIMP2_PURE_IMPORT ctype_base : locale::facet { // base for ctype
    enum { // constants for character classifications
        alnum  = _DI | _LO | _UP | _XA,
        alpha  = _LO | _UP | _XA,
        cntrl  = _BB,
        digit  = _DI,
        graph  = _DI | _LO | _PU | _UP | _XA,
        lower  = _LO,
        print  = _DI | _LO | _PU | _SP | _UP | _XA | _XD,
        punct  = _PU,
        space  = _CN | _SP,
        upper  = _UP,
        xdigit = _XD,
        blank  = _CN | _SP
    };
    using mask = short; // to match <ctype.h>

    __CLR_OR_THIS_CALL ctype_base(size_t _Refs = 0) : locale::facet(_Refs) {}

    __CLR_OR_THIS_CALL ~ctype_base() noexcept {}
};

#undef _XA
#undef _BB
#undef _CN
#undef _DI
#undef _LO
#undef _PU
#undef _SP
#undef _UP
#undef _XD

template <class _Elem>
class ctype : public ctype_base { // facet for classifying elements, converting cases
public:
    // ctype<char>, ctype<wchar_t>, and ctype<unsigned short> are explicitly specialized below.
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || _Always_false<_Elem>, _FACET_SPECIALIZATION_MESSAGE);

    using char_type = _Elem;

    bool __CLR_OR_THIS_CALL is(mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return do_is(_Maskval, _Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        return do_is(_First, _Last, _Dest);
    }

    const _Elem* __CLR_OR_THIS_CALL scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        return do_scan_is(_Maskval, _First, _Last);
    }

    const _Elem* __CLR_OR_THIS_CALL scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        return do_scan_not(_Maskval, _First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL tolower(_Elem _Ch) const { // convert element to lower case
        return do_tolower(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL tolower(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        return do_tolower(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL toupper(_Elem _Ch) const { // convert element to upper case
        return do_toupper(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL toupper(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        return do_toupper(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL widen(char _Byte) const { // widen char
        return do_widen(_Byte);
    }

    const char* __CLR_OR_THIS_CALL widen(const char* _First, const char* _Last,
        _Elem* _Dest) const { // widen chars in [_First, _Last)
        return do_widen(_First, _Last, _Dest);
    }

    char __CLR_OR_THIS_CALL narrow(_Elem _Ch, char _Dflt = '\0') const { // narrow element to char
        return do_narrow(_Ch, _Dflt);
    }

    const _Elem* __CLR_OR_THIS_CALL narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        return do_narrow(_First, _Last, _Dflt, _Dest);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id;

    explicit __CLR_OR_THIS_CALL ctype(size_t _Refs = 0) : ctype_base(_Refs) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL ctype(const _Locinfo& _Lobj, size_t _Refs = 0) : ctype_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        if (_Ppf && !*_Ppf) {
            *_Ppf = new ctype<_Elem>(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~ctype() noexcept {
        if (_Ctype._Delfl) {
            _CSTD free(const_cast<short*>(_Ctype._Table));
        }

        _CSTD free(_Ctype._LocaleName);
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Ctype = _Lobj._Getctype();
        _Cvt   = _Lobj._Getcvt();
    }

    virtual bool __CLR_OR_THIS_CALL do_is(
        mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return (_Ctype._Table[static_cast<unsigned char>(narrow(_Ch))] & _Maskval) != 0;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Ctype._Table[static_cast<unsigned char>(narrow(*_First))];
        }

        return _First;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && !is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_tolower(_Elem _Ch) const { // convert element to lower case
        unsigned char _Byte = static_cast<unsigned char>(narrow(_Ch, '\0'));
        if (_Byte == '\0') {
            return _Ch;
        }

        return widen(static_cast<char>(_Tolower(_Byte, &_Ctype)));
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_tolower(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) { // convert *_First to lower case
            unsigned char _Byte = static_cast<unsigned char>(narrow(*_First, '\0'));
            if (_Byte != '\0') {
                *_First = (widen(static_cast<char>(_Tolower(_Byte, &_Ctype))));
            }
        }
        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_toupper(_Elem _Ch) const { // convert element to upper case
        unsigned char _Byte = static_cast<unsigned char>(narrow(_Ch, '\0'));
        if (_Byte == '\0') {
            return _Ch;
        }

        return widen(static_cast<char>(_Toupper(_Byte, &_Ctype)));
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_toupper(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) { // convert *_First to upper case
            unsigned char _Byte = static_cast<unsigned char>(narrow(*_First, '\0'));
            if (_Byte != '\0') {
                *_First = (widen(static_cast<char>(_Toupper(_Byte, &_Ctype))));
            }
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_widen(char _Byte) const { // widen char
        return _Maklocchr(_Byte, static_cast<_Elem*>(nullptr), _Cvt);
    }

    virtual const char* __CLR_OR_THIS_CALL do_widen(
        const char* _First, const char* _Last, _Elem* _Dest) const { // widen chars in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Maklocchr(*_First, static_cast<_Elem*>(nullptr), _Cvt);
        }

        return _First;
    }

    char __CLR_OR_THIS_CALL _Donarrow(_Elem _Ch, char _Dflt) const { // narrow element to char
        char _Byte;
        if (_Ch == _Elem{}) {
            return '\0';
        }

        if ((_Byte = _Maklocbyte(_Ch, _Cvt)) == '\0') {
            return _Dflt;
        }

        return _Byte;
    }

    virtual char __CLR_OR_THIS_CALL do_narrow(_Elem _Ch, char _Dflt) const { // narrow element to char
        return _Donarrow(_Ch, _Dflt);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Donarrow(*_First, _Dflt);
        }

        return _First;
    }

private:
    _Locinfo::_Ctypevec _Ctype; // locale info passed to _Tolower, etc.
    _Locinfo::_Cvtvec _Cvt; // conversion information
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdllimport-static-field-def"
#endif // __clang__

template <class _Elem>
locale::id ctype<_Elem>::id;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

template <>
class _CRTIMP2_PURE_IMPORT ctype<char> : public ctype_base { // facet for classifying char elements, converting cases
public:
    using _Elem     = char;
    using char_type = _Elem;

    bool __CLR_OR_THIS_CALL is(mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return (_Ctype._Table[static_cast<unsigned char>(_Ch)] & _Maskval) != 0;
    }

    const _Elem* __CLR_OR_THIS_CALL is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Ctype._Table[static_cast<unsigned char>(*_First)];
        }

        return _First;
    }

    const _Elem* __CLR_OR_THIS_CALL scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && !is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    const _Elem* __CLR_OR_THIS_CALL scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    _Elem __CLR_OR_THIS_CALL tolower(_Elem _Ch) const { // convert element to lower case
        return do_tolower(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL tolower(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        return do_tolower(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL toupper(_Elem _Ch) const { // convert element to upper case
        return do_toupper(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL toupper(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        return do_toupper(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL widen(char _Byte) const { // widen char
        return do_widen(_Byte);
    }

    const _Elem* __CLR_OR_THIS_CALL widen(const char* _First, const char* _Last,
        _Elem* _Dest) const { // widen chars in [_First, _Last)
        return do_widen(_First, _Last, _Dest);
    }

    _Elem __CLR_OR_THIS_CALL narrow(_Elem _Ch, char _Dflt = '\0') const { // narrow element to char
        return do_narrow(_Ch, _Dflt);
    }

    const _Elem* __CLR_OR_THIS_CALL narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        return do_narrow(_First, _Last, _Dflt, _Dest);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id;

    explicit __CLR_OR_THIS_CALL ctype(const mask* _Table = nullptr, bool _Deletetable = false,
        size_t _Refs = 0)
        : ctype_base(_Refs) { // construct with specified table and delete flag for table
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()

        _Tidy(); // free existing table, as needed
        if (_Table) { // replace existing char to mask table
            _Ctype._Table = _Table;
            _Ctype._Delfl = _Deletetable ? -1 : 0;
        } else { // use classic table
            _Ctype._Table = classic_table();
            _Ctype._Delfl = 0;
        }
    }

    __CLR_OR_THIS_CALL ctype(const _Locinfo& _Lobj, size_t _Refs = 0) : ctype_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        if (_Ppf && !*_Ppf) {
            *_Ppf = new ctype<_Elem>(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

    const mask* __CLR_OR_THIS_CALL table() const noexcept {
        return _Ctype._Table;
    }

    static const mask* __CLRCALL_OR_CDECL classic_table() noexcept {
        return _STD use_facet<ctype>(locale::classic()).table();
    }

    _PGLOBAL static const size_t table_size = 1 << CHAR_BIT; // size of ctype mapping table, typically 256

protected:
    virtual __CLR_OR_THIS_CALL ~ctype() noexcept {
        _Tidy();
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Ctype = _Lobj._Getctype();
    }

    void __CLR_OR_THIS_CALL _Tidy() noexcept { // free any allocated storage
        if (0 < _Ctype._Delfl) {
            _CSTD free(const_cast<short*>(_Ctype._Table));
        } else if (_Ctype._Delfl < 0) {
            delete[] _Ctype._Table;
        }

        _CSTD free(_Ctype._LocaleName);
    }

    virtual _Elem __CLR_OR_THIS_CALL do_tolower(_Elem _Ch) const { // convert element to lower case
        return static_cast<_Elem>(_Tolower(static_cast<unsigned char>(_Ch), &_Ctype));
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_tolower(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) {
            *_First = static_cast<_Elem>(_Tolower(static_cast<unsigned char>(*_First), &_Ctype));
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_toupper(_Elem _Ch) const { // convert element to upper case
        return static_cast<_Elem>(_Toupper(static_cast<unsigned char>(_Ch), &_Ctype));
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_toupper(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) {
            *_First = static_cast<_Elem>(_Toupper(static_cast<unsigned char>(*_First), &_Ctype));
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_widen(char _Byte) const { // widen char
        return _Byte;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_widen(
        const char* _First, const char* _Last, _Elem* _Dest) const { // widen chars in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        _CSTD memcpy(_Dest, _First, static_cast<size_t>(_Last - _First));
        return _Last;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_narrow(_Elem _Ch, char) const { // narrow char
        return _Ch;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_narrow(const _Elem* _First, const _Elem* _Last, char,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        _Adl_verify_range(_First, _Last);
        _CSTD memcpy(_Dest, _First, static_cast<size_t>(_Last - _First));
        return _Last;
    }

private:
    _Locinfo::_Ctypevec _Ctype; // information
};

template <>
class _CRTIMP2_PURE_IMPORT ctype<wchar_t>
    : public ctype_base { // facet for classifying wchar_t elements, converting cases
public:
    using _Elem     = wchar_t;
    using char_type = _Elem;

    bool __CLR_OR_THIS_CALL is(mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return do_is(_Maskval, _Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        return do_is(_First, _Last, _Dest);
    }

    const _Elem* __CLR_OR_THIS_CALL scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        return do_scan_is(_Maskval, _First, _Last);
    }

    const _Elem* __CLR_OR_THIS_CALL scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        return do_scan_not(_Maskval, _First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL tolower(_Elem _Ch) const { // convert element to lower case
        return do_tolower(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL tolower(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        return do_tolower(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL toupper(_Elem _Ch) const { // convert element to upper case
        return do_toupper(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL toupper(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        return do_toupper(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL widen(char _Byte) const { // widen char
        return do_widen(_Byte);
    }

    const char* __CLR_OR_THIS_CALL widen(const char* _First, const char* _Last,
        _Elem* _Dest) const { // widen chars in [_First, _Last)
        return do_widen(_First, _Last, _Dest);
    }

    char __CLR_OR_THIS_CALL narrow(_Elem _Ch, char _Dflt = '\0') const { // narrow element to char
        return do_narrow(_Ch, _Dflt);
    }

    const _Elem* __CLR_OR_THIS_CALL narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        return do_narrow(_First, _Last, _Dflt, _Dest);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id;

    explicit __CLR_OR_THIS_CALL ctype(size_t _Refs = 0) : ctype_base(_Refs) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL ctype(const _Locinfo& _Lobj, size_t _Refs = 0) : ctype_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        if (_Ppf && !*_Ppf) {
            *_Ppf = new ctype<_Elem>(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~ctype() noexcept {
        if (_Ctype._Delfl) {
            _CSTD free(const_cast<short*>(_Ctype._Table));
        }

        _CSTD free(_Ctype._LocaleName);
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Ctype = _Lobj._Getctype();
        _Cvt   = _Lobj._Getcvt();
    }

    virtual bool __CLR_OR_THIS_CALL do_is(
        mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return (_CSTD _Getwctype(_Ch, &_Ctype) & _Maskval) != 0;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        return _CSTD _Getwctypes(_First, _Last, _Dest, &_Ctype);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && !is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_tolower(_Elem _Ch) const { // convert element to lower case
        return _Towlower(_Ch, &_Ctype);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_tolower(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) {
            *_First = _Towlower(*_First, &_Ctype);
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_toupper(_Elem _Ch) const { // convert element to upper case
        return _Towupper(_Ch, &_Ctype);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_toupper(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) {
            *_First = _Towupper(*_First, &_Ctype);
        }

        return _First;
    }

    _Elem __CLR_OR_THIS_CALL _Dowiden(char _Byte) const { // widen char
        mbstate_t _Mbst = {};
        wchar_t _Wc;
        return _Mbrtowc(&_Wc, &_Byte, 1, &_Mbst, &_Cvt) < 0 ? static_cast<wchar_t>(WEOF) : _Wc;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_widen(char _Byte) const { // widen char
        return _Dowiden(_Byte);
    }

    virtual const char* __CLR_OR_THIS_CALL do_widen(
        const char* _First, const char* _Last, _Elem* _Dest) const { // widen chars in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Dowiden(*_First);
        }

        return _First;
    }

    char __CLR_OR_THIS_CALL _Donarrow(_Elem _Ch, char _Dflt) const { // narrow element to char
        char _Buf[MB_LEN_MAX];
        mbstate_t _Mbst = {};
        return _Wcrtomb(_Buf, _Ch, &_Mbst, &_Cvt) != 1 ? _Dflt : _Buf[0];
    }

    virtual char __CLR_OR_THIS_CALL do_narrow(_Elem _Ch, char _Dflt) const { // narrow element to char
        return _Donarrow(_Ch, _Dflt);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Donarrow(*_First, _Dflt);
        }

        return _First;
    }

private:
    _Locinfo::_Ctypevec _Ctype; // locale info passed to _Tolower, etc.
    _Locinfo::_Cvtvec _Cvt; // conversion information
};

#ifdef _NATIVE_WCHAR_T_DEFINED
template <>
class _CRTIMP2_PURE_IMPORT ctype<unsigned short>
    : public ctype_base { // facet for classifying unsigned short elements, converting cases
public:
    using _Elem     = unsigned short;
    using char_type = _Elem;

    bool __CLR_OR_THIS_CALL is(mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return do_is(_Maskval, _Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        return do_is(_First, _Last, _Dest);
    }

    const _Elem* __CLR_OR_THIS_CALL scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        return do_scan_is(_Maskval, _First, _Last);
    }

    const _Elem* __CLR_OR_THIS_CALL scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        return do_scan_not(_Maskval, _First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL tolower(_Elem _Ch) const { // convert element to lower case
        return do_tolower(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL tolower(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        return do_tolower(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL toupper(_Elem _Ch) const { // convert element to upper case
        return do_toupper(_Ch);
    }

    const _Elem* __CLR_OR_THIS_CALL toupper(
        _Elem* _First, const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        return do_toupper(_First, _Last);
    }

    _Elem __CLR_OR_THIS_CALL widen(char _Byte) const { // widen char
        return do_widen(_Byte);
    }

    const char* __CLR_OR_THIS_CALL widen(const char* _First, const char* _Last,
        _Elem* _Dest) const { // widen chars in [_First, _Last)
        return do_widen(_First, _Last, _Dest);
    }

    char __CLR_OR_THIS_CALL narrow(_Elem _Ch, char _Dflt = '\0') const { // narrow element to char
        return do_narrow(_Ch, _Dflt);
    }

    const _Elem* __CLR_OR_THIS_CALL narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        return do_narrow(_First, _Last, _Dflt, _Dest);
    }

    __PURE_APPDOMAIN_GLOBAL static locale::id id;

    explicit __CLR_OR_THIS_CALL ctype(size_t _Refs = 0) : ctype_base(_Refs) {
        _BEGIN_LOCINFO(_Lobj)
        _Init(_Lobj);
        _END_LOCINFO()
    }

    __CLR_OR_THIS_CALL ctype(const _Locinfo& _Lobj, size_t _Refs = 0) : ctype_base(_Refs) {
        _Init(_Lobj);
    }

    static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {
        if (_Ppf && !*_Ppf) {
            *_Ppf = new ctype<_Elem>(_Locinfo(_Ploc->c_str()));
        }

        return _X_CTYPE;
    }

protected:
    virtual __CLR_OR_THIS_CALL ~ctype() noexcept {
        if (_Ctype._Delfl) {
            _CSTD free(const_cast<short*>(_Ctype._Table));
        }

        _CSTD free(_Ctype._LocaleName);
    }

    void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
        _Ctype = _Lobj._Getctype();
        _Cvt   = _Lobj._Getcvt();
    }

    virtual bool __CLR_OR_THIS_CALL do_is(
        mask _Maskval, _Elem _Ch) const { // test if element fits any mask classifications
        return (_CSTD _Getwctype(_Ch, &_Ctype) & _Maskval) != 0;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_is(const _Elem* _First, const _Elem* _Last,
        mask* _Dest) const { // get mask sequence for elements in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        return reinterpret_cast<const _Elem*>(_CSTD _Getwctypes(
            reinterpret_cast<const wchar_t*>(_First), reinterpret_cast<const wchar_t*>(_Last), _Dest, &_Ctype));
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_scan_is(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) that fits mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && !is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_scan_not(mask _Maskval, const _Elem* _First,
        const _Elem* _Last) const { // find first in [_First, _Last) not fitting mask classification
        _Adl_verify_range(_First, _Last);
        while (_First != _Last && is(_Maskval, *_First)) {
            ++_First;
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_tolower(_Elem _Ch) const { // convert element to lower case
        return _Towlower(_Ch, &_Ctype);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_tolower(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to lower case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) {
            *_First = _Towlower(*_First, &_Ctype);
        }

        return _First;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_toupper(_Elem _Ch) const { // convert element to upper case
        return _Towupper(_Ch, &_Ctype);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_toupper(_Elem* _First,
        const _Elem* _Last) const { // convert [_First, _Last) in place to upper case
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First) {
            *_First = _Towupper(*_First, &_Ctype);
        }

        return _First;
    }

    _Elem __CLR_OR_THIS_CALL _Dowiden(char _Byte) const { // widen char
        mbstate_t _Mbst = {};
        unsigned short _Wc;

        if (_Mbrtowc(reinterpret_cast<wchar_t*>(&_Wc), &_Byte, 1, &_Mbst, &_Cvt) < 0) {
            return static_cast<unsigned short>(WEOF);
        }

        return _Wc;
    }

    virtual _Elem __CLR_OR_THIS_CALL do_widen(char _Byte) const { // widen char
        return _Dowiden(_Byte);
    }

    virtual const char* __CLR_OR_THIS_CALL do_widen(
        const char* _First, const char* _Last, _Elem* _Dest) const { // widen chars in [_First, _Last)
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Dowiden(*_First);
        }

        return _First;
    }

    char __CLR_OR_THIS_CALL _Donarrow(_Elem _Ch, char _Dflt) const { // narrow element to char
        char _Buf[MB_LEN_MAX];
        mbstate_t _Mbst = {};
        return _Wcrtomb(_Buf, _Ch, &_Mbst, &_Cvt) != 1 ? _Dflt : _Buf[0];
    }

    virtual char __CLR_OR_THIS_CALL do_narrow(_Elem _Ch, char _Dflt) const { // narrow element to char
        return _Donarrow(_Ch, _Dflt);
    }

    virtual const _Elem* __CLR_OR_THIS_CALL do_narrow(const _Elem* _First, const _Elem* _Last, char _Dflt,
        char* _Dest) const { // narrow elements in [_First, _Last) to chars
        _Adl_verify_range(_First, _Last);
        for (; _First != _Last; ++_First, ++_Dest) {
            *_Dest = _Donarrow(*_First, _Dflt);
        }

        return _First;
    }

private:
    _Locinfo::_Ctypevec _Ctype; // locale info passed to _Tolower, etc.
    _Locinfo::_Cvtvec _Cvt; // conversion information
};
#endif // _NATIVE_WCHAR_T_DEFINED

template <class _Elem>
class ctype_byname : public ctype<_Elem> { // ctype for named locale
public:
    static_assert(!_ENFORCE_FACET_SPECIALIZATIONS || _Is_any_of_v<_Elem, char, wchar_t>, _FACET_SPECIALIZATION_MESSAGE);

    explicit __CLR_OR_THIS_CALL ctype_byname(const char* _Locname, size_t _Refs = 0)
        : ctype<_Elem>(_Locinfo(_Locname), _Refs) {} // construct for named locale

    explicit __CLR_OR_THIS_CALL ctype_byname(const string& _Str, size_t _Refs = 0)
        : ctype<_Elem>(_Locinfo(_Str.c_str()), _Refs) {} // construct for named locale

protected:
    virtual __CLR_OR_THIS_CALL ~ctype_byname() noexcept {}
};

enum class _Case_sensitive : bool { _Nope, _Yes };

template <class _InIt, class _Elem>
int __CRTDECL _Getloctxt(
    _InIt& _First, _InIt& _Last, size_t _Numfields, const _Elem* _Ptr, const _Case_sensitive _Matching) {
    // find field at _Ptr that matches longest in [_First, _Last)
    for (size_t _Off = 0; _Ptr[_Off] != _Elem{}; ++_Off) {
        if (_Ptr[_Off] == _Ptr[0]) {
            ++_Numfields; // add fields with leading mark to initial count
        }
    }

    string _Str(_Numfields, '\0'); // one column counter for each field
    const ctype<_Elem>& _CType = _STD use_facet<ctype<_Elem>>(locale{});

    int _Ans = -2; // no candidates so far
    for (size_t _Column = 1;; ++_Column, (void) ++_First, _Ans = -1) { // test each element against all viable fields
        bool _Prefix  = false; // seen at least one valid prefix
        size_t _Off   = 0; // offset into fields
        size_t _Field = 0; // current field number

        for (; _Field < _Numfields; ++_Field) { // test element at _Column in field _Field
            while (_Ptr[_Off] != _Elem{} && _Ptr[_Off] != _Ptr[0]) { // find beginning of field
                ++_Off;
            }

            if (_Str[_Field] != '\0') {
                _Off += _Str[_Field]; // skip tested columns in field
            } else if (_Ptr[_Off += _Column] == _Ptr[0]
                       || _Ptr[_Off] == _Elem{}) { // matched all of field, save as possible answer
                _Str[_Field] = static_cast<char>(_Column < 127 ? _Column : 127); // save skip count if small enough
                _Ans         = static_cast<int>(_Field); // save answer
            } else if (_First == _Last
                       || (_Matching == _Case_sensitive::_Yes
                               ? _Ptr[_Off] != *_First
                               : _CType.tolower(_Ptr[_Off]) != _CType.tolower(static_cast<_Elem>(*_First)))) {
                _Str[_Field] = static_cast<char>(_Column < 127 ? _Column : 127); // no match, just save skip count
            } else {
                _Prefix = true; // still a valid prefix
            }
        }

        if (!_Prefix || _First == _Last) {
            break; // no pending prefixes or no input, give up
        }
    }
    return _Ans; // return field number or negative value on failure
}

#if defined(_DLL_CPPLIB)
#if !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
template class _CRTIMP2_PURE_IMPORT codecvt<char, char, mbstate_t>;
#endif // !defined(_CRTBLD) || defined(__FORCE_INSTANCE)
#endif // defined(_DLL_CPPLIB)
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XLOCALE_
