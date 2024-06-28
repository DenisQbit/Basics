// coroutine standard header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _COROUTINE_
#define _COROUTINE_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
#pragma message("The contents of <coroutine> are not available with /await.")
#pragma message("Remove /await or use /await:strict for standard coroutines.")
#pragma message("Use <experimental/coroutine> for legacy /await support.")
#else // ^^^ /await ^^^ / vvv no /await vvv
#ifndef __cpp_lib_coroutine
#pragma message("The contents of <coroutine> are available only with C++20 or later or /await:strict.")
#else // ^^^ <coroutine> is not available / <coroutine> is available vvv
#ifndef _ALLOW_COROUTINE_ABI_MISMATCH
#pragma detect_mismatch("_COROUTINE_ABI", "2")
#endif // _ALLOW_COROUTINE_ABI_MISMATCH

#if _HAS_CXX20
#include <compare>
#endif // _HAS_CXX20
#include <type_traits>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

template <class _Ret, class = void>
struct _Coroutine_traits {};

template <class _Ret>
struct _Coroutine_traits<_Ret, void_t<typename _Ret::promise_type>> {
    using promise_type = typename _Ret::promise_type;
};

template <class _Ret, class...>
struct coroutine_traits : _Coroutine_traits<_Ret> {};

template <class = void>
struct coroutine_handle;

template <>
struct coroutine_handle<void> {
    constexpr coroutine_handle() noexcept = default;
    constexpr coroutine_handle(nullptr_t) noexcept {}

    coroutine_handle& operator=(nullptr_t) noexcept {
        _Ptr = nullptr;
        return *this;
    }

    _NODISCARD constexpr void* address() const noexcept {
        return _Ptr;
    }

    _NODISCARD static constexpr coroutine_handle from_address(void* const _Addr) noexcept { // strengthened
        coroutine_handle _Result;
        _Result._Ptr = _Addr;
        return _Result;
    }

    constexpr explicit operator bool() const noexcept {
        return _Ptr != nullptr;
    }

    _NODISCARD bool done() const noexcept { // strengthened
        return __builtin_coro_done(_Ptr);
    }

    void operator()() const {
        __builtin_coro_resume(_Ptr);
    }

    void resume() const {
        __builtin_coro_resume(_Ptr);
    }

    void destroy() const noexcept { // strengthened
        __builtin_coro_destroy(_Ptr);
    }

private:
    void* _Ptr = nullptr;
};

template <class _Promise>
struct coroutine_handle {
    constexpr coroutine_handle() noexcept = default;
    constexpr coroutine_handle(nullptr_t) noexcept {}

    _NODISCARD static coroutine_handle from_promise(_Promise& _Prom) noexcept { // strengthened
        const auto _Prom_ptr  = const_cast<void*>(static_cast<const volatile void*>(_STD addressof(_Prom)));
        const auto _Frame_ptr = __builtin_coro_promise(_Prom_ptr, 0, true);
        coroutine_handle _Result;
        _Result._Ptr = _Frame_ptr;
        return _Result;
    }

    coroutine_handle& operator=(nullptr_t) noexcept {
        _Ptr = nullptr;
        return *this;
    }

    _NODISCARD constexpr void* address() const noexcept {
        return _Ptr;
    }

    _NODISCARD static constexpr coroutine_handle from_address(void* const _Addr) noexcept { // strengthened
        coroutine_handle _Result;
        _Result._Ptr = _Addr;
        return _Result;
    }

    constexpr operator coroutine_handle<>() const noexcept {
        return coroutine_handle<>::from_address(_Ptr);
    }

    constexpr explicit operator bool() const noexcept {
        return _Ptr != nullptr;
    }

    _NODISCARD bool done() const noexcept { // strengthened
        return __builtin_coro_done(_Ptr);
    }

    void operator()() const {
        __builtin_coro_resume(_Ptr);
    }

    void resume() const {
        __builtin_coro_resume(_Ptr);
    }

    void destroy() const noexcept { // strengthened
        __builtin_coro_destroy(_Ptr);
    }

    _NODISCARD _Promise& promise() const noexcept { // strengthened
        return *reinterpret_cast<_Promise*>(__builtin_coro_promise(_Ptr, 0, false));
    }

private:
    void* _Ptr = nullptr;
};

_NODISCARD constexpr bool operator==(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
    return _Left.address() == _Right.address();
}

#if _HAS_CXX20
_NODISCARD constexpr strong_ordering operator<=>(
    const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
#ifdef __cpp_lib_concepts
    return compare_three_way{}(_Left.address(), _Right.address());
#else // ^^^ no workaround / workaround vvv
    return _Left.address() <=> _Right.address();
#endif // __cpp_lib_concepts
}
#else // ^^^ <=> exists / <=> does not exist vvv
_NODISCARD constexpr bool operator!=(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD constexpr bool operator<(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
    return less<void*>{}(_Left.address(), _Right.address());
}

_NODISCARD constexpr bool operator>(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
    return _Right < _Left;
}

_NODISCARD constexpr bool operator<=(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
    return !(_Left > _Right);
}

_NODISCARD constexpr bool operator>=(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
    return !(_Left < _Right);
}
#endif // _HAS_CXX20

template <class _Promise>
struct hash<coroutine_handle<_Promise>> {
    _NODISCARD size_t operator()(const coroutine_handle<_Promise>& _Coro) const noexcept {
        return _Hash_representation(_Coro.address());
    }
};

struct noop_coroutine_promise {};

template <>
struct coroutine_handle<noop_coroutine_promise> {
    friend coroutine_handle noop_coroutine() noexcept;

    constexpr operator coroutine_handle<>() const noexcept {
        return coroutine_handle<>::from_address(_Ptr);
    }

    constexpr explicit operator bool() const noexcept {
        return true;
    }
    _NODISCARD constexpr bool done() const noexcept {
        return false;
    }

    constexpr void operator()() const noexcept {}
    constexpr void resume() const noexcept {}
    constexpr void destroy() const noexcept {}

    _NODISCARD noop_coroutine_promise& promise() const noexcept {
        // Returns a reference to the associated promise
        return *reinterpret_cast<noop_coroutine_promise*>(__builtin_coro_promise(_Ptr, 0, false));
    }

    _NODISCARD constexpr void* address() const noexcept {
        return _Ptr;
    }

private:
    coroutine_handle() noexcept = default;

    void* _Ptr = __builtin_coro_noop();
};

using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;

_NODISCARD inline noop_coroutine_handle noop_coroutine() noexcept {
    // Returns a handle to a coroutine that has no observable effects when resumed or destroyed.
    return noop_coroutine_handle{};
}

struct suspend_never {
    _NODISCARD constexpr bool await_ready() const noexcept {
        return true;
    }

    constexpr void await_suspend(coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}
};

struct suspend_always {
    _NODISCARD constexpr bool await_ready() const noexcept {
        return false;
    }

    constexpr void await_suspend(coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}
};

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // <coroutine> is available
#endif // _RESUMABLE_FUNCTIONS_SUPPORTED
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _COROUTINE_
