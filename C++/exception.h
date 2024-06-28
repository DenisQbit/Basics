// exception standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _EXCEPTION_
#define _EXCEPTION_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <type_traits>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

#if _HAS_DEPRECATED_UNCAUGHT_EXCEPTION
_CXX17_DEPRECATE_UNCAUGHT_EXCEPTION _CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL uncaught_exception() noexcept;
#endif // _HAS_DEPRECATED_UNCAUGHT_EXCEPTION
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL uncaught_exceptions() noexcept;

_STD_END

#if _HAS_EXCEPTIONS

#include <malloc.h>
#include <vcruntime_exception.h>

_STD_BEGIN

using ::terminate;

#ifndef _M_CEE_PURE
using ::set_terminate;
using ::terminate_handler;

_NODISCARD inline terminate_handler __CRTDECL get_terminate() noexcept { // get current terminate handler
    return _get_terminate();
}
#endif // _M_CEE_PURE

#if _HAS_UNEXPECTED
using ::unexpected;

#ifndef _M_CEE_PURE
using ::set_unexpected;
using ::unexpected_handler;

_NODISCARD inline unexpected_handler __CRTDECL get_unexpected() noexcept { // get current unexpected handler
    return _get_unexpected();
}
#endif // _M_CEE_PURE
#endif // _HAS_UNEXPECTED

_STD_END

#else // _HAS_EXCEPTIONS

_STDEXT_BEGIN
class exception;
_STDEXT_END

_STD_BEGIN

using _STDEXT exception;

using _Prhand = void(__cdecl*)(const exception&);

extern _CRTIMP2_PURE_IMPORT _Prhand _Raise_handler; // pointer to raise handler

_STD_END

_STDEXT_BEGIN
class exception { // base of all library exceptions
public:
    static _STD _Prhand _Set_raise_handler(_STD _Prhand _Pnew) { // register a handler for _Raise calls
        const _STD _Prhand _Pold = _STD _Raise_handler;
        _STD _Raise_handler      = _Pnew;
        return _Pold;
    }

    // this constructor is necessary to compile
    // successfully header new for _HAS_EXCEPTIONS==0 scenario
    explicit __CLR_OR_THIS_CALL exception(const char* _Message = "unknown", int = 1) noexcept : _Ptr(_Message) {}

    __CLR_OR_THIS_CALL exception(const exception& _Right) noexcept : _Ptr(_Right._Ptr) {}

    exception& __CLR_OR_THIS_CALL operator=(const exception& _Right) noexcept {
        _Ptr = _Right._Ptr;
        return *this;
    }

    virtual __CLR_OR_THIS_CALL ~exception() noexcept {}

    _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept { // return pointer to message string
        return _Ptr ? _Ptr : "unknown exception";
    }

    [[noreturn]] void __CLR_OR_THIS_CALL _Raise() const { // raise the exception
        if (_STD _Raise_handler) {
            (*_STD _Raise_handler)(*this); // call raise handler if present
        }

        _Doraise(); // call the protected virtual
        _RAISE(*this); // raise this exception
    }

protected:
    virtual void __CLR_OR_THIS_CALL _Doraise() const {} // perform class-specific exception handling

protected:
    const char* _Ptr; // the message pointer
};

class bad_exception : public exception { // base of all bad exceptions
public:
    __CLR_OR_THIS_CALL bad_exception(const char* _Message = "bad exception") noexcept : exception(_Message) {}

    virtual __CLR_OR_THIS_CALL ~bad_exception() noexcept {}

protected:
    virtual void __CLR_OR_THIS_CALL _Doraise() const override { // raise this exception
        _RAISE(*this);
    }
};

class bad_alloc : public exception { // base of all bad allocation exceptions
public:
    __CLR_OR_THIS_CALL bad_alloc() noexcept
        : exception("bad allocation", 1) {} // construct from message string with no memory allocation

    virtual __CLR_OR_THIS_CALL ~bad_alloc() noexcept {}

private:
    friend class bad_array_new_length;

    __CLR_OR_THIS_CALL bad_alloc(const char* _Message) noexcept
        : exception(_Message, 1) {} // construct from message string with no memory allocation

protected:
    virtual void __CLR_OR_THIS_CALL _Doraise() const override { // perform class-specific exception handling
        _RAISE(*this);
    }
};

class bad_array_new_length : public bad_alloc {
public:
    bad_array_new_length() noexcept : bad_alloc("bad array new length") {}
};

_STDEXT_END

_STD_BEGIN
using terminate_handler = void(__cdecl*)();

inline terminate_handler __CRTDECL set_terminate(terminate_handler) noexcept { // register a terminate handler
    return nullptr;
}

[[noreturn]] inline void __CRTDECL terminate() noexcept { // handle exception termination
    _CSTD abort();
}

_NODISCARD inline terminate_handler __CRTDECL get_terminate() noexcept { // get current terminate handler
    return nullptr;
}

#if _HAS_UNEXPECTED
using unexpected_handler = void(__cdecl*)();

inline unexpected_handler __CRTDECL set_unexpected(unexpected_handler) noexcept { // register an unexpected handler
    return nullptr;
}

inline void __CRTDECL unexpected() {} // handle unexpected exception

_NODISCARD inline unexpected_handler __CRTDECL get_unexpected() noexcept { // get current unexpected handler
    return nullptr;
}
#endif // _HAS_UNEXPECTED

using _STDEXT bad_alloc;
using _STDEXT bad_array_new_length;
using _STDEXT bad_exception;

_STD_END

#endif // _HAS_EXCEPTIONS

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCreate(_Out_ void*) noexcept;
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrDestroy(_Inout_ void*) noexcept;
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCopy(_Out_ void*, _In_ const void*) noexcept;
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrAssign(_Inout_ void*, _In_ const void*) noexcept;
_CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL __ExceptionPtrCompare(_In_ const void*, _In_ const void*) noexcept;
_CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL __ExceptionPtrToBool(_In_ const void*) noexcept;
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrSwap(_Inout_ void*, _Inout_ void*) noexcept;
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCurrentException(void*) noexcept;
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrRethrow(_In_ const void*);
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCopyException(
    _Inout_ void*, _In_ const void*, _In_ const void*) noexcept;

_STD_BEGIN

class exception_ptr {
public:
    exception_ptr() noexcept {
        __ExceptionPtrCreate(this);
    }

    exception_ptr(nullptr_t) noexcept {
        __ExceptionPtrCreate(this);
    }

    ~exception_ptr() noexcept {
        __ExceptionPtrDestroy(this);
    }

    exception_ptr(const exception_ptr& _Rhs) noexcept {
        __ExceptionPtrCopy(this, &_Rhs);
    }

    exception_ptr& operator=(const exception_ptr& _Rhs) noexcept {
        __ExceptionPtrAssign(this, &_Rhs);
        return *this;
    }

    exception_ptr& operator=(nullptr_t) noexcept {
        exception_ptr _Ptr;
        __ExceptionPtrAssign(this, &_Ptr);
        return *this;
    }

    explicit operator bool() const noexcept {
        return __ExceptionPtrToBool(this);
    }

    static exception_ptr _Current_exception() noexcept {
        exception_ptr _Retval;
        __ExceptionPtrCurrentException(&_Retval);
        return _Retval;
    }

    static exception_ptr _Copy_exception(_In_ void* _Except, _In_ const void* _Ptr) {
        exception_ptr _Retval;
        if (!_Ptr) {
            // unsupported exceptions
            return _Retval;
        }
        __ExceptionPtrCopyException(&_Retval, _Except, _Ptr);
        return _Retval;
    }

    friend void swap(exception_ptr& _Lhs, exception_ptr& _Rhs) noexcept {
        __ExceptionPtrSwap(&_Lhs, &_Rhs);
    }

    _NODISCARD_FRIEND bool operator==(const exception_ptr& _Lhs, const exception_ptr& _Rhs) noexcept {
        return __ExceptionPtrCompare(&_Lhs, &_Rhs);
    }

    _NODISCARD_FRIEND bool operator==(nullptr_t, const exception_ptr& _Rhs) noexcept {
        return !_Rhs;
    }

    _NODISCARD_FRIEND bool operator==(const exception_ptr& _Lhs, nullptr_t) noexcept {
        return !_Lhs;
    }

    _NODISCARD_FRIEND bool operator!=(const exception_ptr& _Lhs, const exception_ptr& _Rhs) noexcept {
        return !(_Lhs == _Rhs);
    }

    _NODISCARD_FRIEND bool operator!=(nullptr_t _Lhs, const exception_ptr& _Rhs) noexcept {
        return !(_Lhs == _Rhs);
    }

    _NODISCARD_FRIEND bool operator!=(const exception_ptr& _Lhs, nullptr_t _Rhs) noexcept {
        return !(_Lhs == _Rhs);
    }

private:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif // __clang__
    void* _Data1;
    void* _Data2;
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
};

_NODISCARD inline exception_ptr current_exception() noexcept {
    return exception_ptr::_Current_exception();
}

[[noreturn]] inline void rethrow_exception(_In_ exception_ptr _Ptr) {
    __ExceptionPtrRethrow(&_Ptr);
}

template <class _Ex>
void* __GetExceptionInfo(_Ex);

template <class _Ex>
_NODISCARD exception_ptr make_exception_ptr(_Ex _Except) noexcept {
    return exception_ptr::_Copy_exception(_STD addressof(_Except), __GetExceptionInfo(_Except));
}

[[noreturn]] inline void _Throw_bad_array_new_length() {
    _THROW(bad_array_new_length{});
}

class nested_exception { // wrap an exception_ptr
public:
    nested_exception() noexcept : _Exc(_STD current_exception()) {}

    nested_exception(const nested_exception&) noexcept = default;
    nested_exception& operator=(const nested_exception&) noexcept = default;
    virtual ~nested_exception() noexcept {}

    [[noreturn]] void rethrow_nested() const { // throw wrapped exception_ptr
        if (_Exc) {
            _STD rethrow_exception(_Exc);
        } else {
            _STD terminate();
        }
    }

    _NODISCARD exception_ptr nested_ptr() const noexcept { // return wrapped exception_ptr
        return _Exc;
    }

private:
    exception_ptr _Exc;
};

template <class _Ty, class _Uty>
struct _With_nested : _Uty, nested_exception { // glue user exception to nested_exception
    explicit _With_nested(_Ty&& _Arg)
        : _Uty(_STD forward<_Ty>(_Arg)), nested_exception() {} // store user exception and current_exception()
};

template <class _Ty>
[[noreturn]] void throw_with_nested(_Ty&& _Arg) { // throw user exception, glued to nested_exception if possible
    using _Uty = decay_t<_Ty>;

    if constexpr (is_class_v<_Uty> && !is_base_of_v<nested_exception, _Uty> && !is_final_v<_Uty>) {
        // throw user exception glued to nested_exception
        using _Glued = _With_nested<_Ty, _Uty>;
        _THROW(_Glued(_STD forward<_Ty>(_Arg)));
    } else {
        // throw user exception by itself
        _THROW(_STD forward<_Ty>(_Arg));
    }
}

#ifdef _CPPRTTI
template <class _Ty>
void _Rethrow_if_nested(const _Ty* _Ptr, true_type) { // use dynamic_cast
    const auto _Nested = dynamic_cast<const nested_exception*>(_Ptr);

    if (_Nested) {
        _Nested->rethrow_nested();
    }
}

template <class _Ty>
void _Rethrow_if_nested(const _Ty*, false_type) {} // can't use dynamic_cast

template <class _Ty>
void rethrow_if_nested(const _Ty& _Arg) { // detect nested_exception inheritance
    bool_constant<
        is_polymorphic_v<_Ty> && (!is_base_of_v<nested_exception, _Ty> || is_convertible_v<_Ty*, nested_exception*>)>
        _Tag;

    _Rethrow_if_nested(_STD addressof(_Arg), _Tag);
}
#else // _CPPRTTI
template <class _Ty>
void rethrow_if_nested(const _Ty&) = delete; // requires /GR option
#endif // _CPPRTTI

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _EXCEPTION_
