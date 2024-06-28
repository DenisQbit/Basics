// generator experimental header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _EXPERIMENTAL_GENERATOR_
#define _EXPERIMENTAL_GENERATOR_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#ifdef _CPPUNWIND
#include <exception>
#endif
#include <memory>

#if defined(__cpp_impl_coroutine)
#include <coroutine>
#elif defined(__cpp_coroutines)
#include <experimental/resumable>
#else // ^^^ legacy coroutines / no coroutine support vvv
#error <experimental/generator> requires /std:c++latest or /await compiler options
#endif // ^^^ no coroutine support ^^^

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

namespace experimental {
    // NOTE WELL: _CPPUNWIND currently affects the ABI of generator.
    template <class _Ty, class _Alloc = allocator<char>>
    struct generator {
        struct promise_type {
            const _Ty* _Value;
#ifdef _CPPUNWIND // TRANSITION, VSO-1172852
            exception_ptr _Exception;
#endif // TRANSITION, VSO-1172852

            generator get_return_object() noexcept {
                return generator{*this};
            }

            suspend_always initial_suspend() noexcept {
                return {};
            }

            suspend_always final_suspend() noexcept {
                return {};
            }

#ifndef _KERNEL_MODE
#ifdef _CPPUNWIND
#if 1 // TRANSITION, VSO-1172852
            void unhandled_exception() noexcept {
                _Exception = _STD current_exception();
            }
#else // ^^^ workaround / no workaround vvv
            void unhandled_exception() {
                throw;
            }
#endif // TRANSITION, VSO-1172852
#else // ^^^ defined(_CPPUNWIND) / !defined(_CPPUNWIND) vvv
            void unhandled_exception() noexcept {}
#endif // _CPPUNWIND
#endif // _KERNEL_MODE

#ifdef _CPPUNWIND // TRANSITION, VSO-1172852
            void _Rethrow_if_exception() {
                if (_Exception) {
                    _STD rethrow_exception(_Exception);
                }
            }
#endif // TRANSITION, VSO-1172852

            suspend_always yield_value(const _Ty& _Val) noexcept {
                _Value = _STD addressof(_Val);
                return {};
            }

            void return_void() noexcept {}

            template <class _Uty>
            _Uty&& await_transform(_Uty&& _Whatever) {
                static_assert(_Always_false<_Uty>,
                    "co_await is not supported in coroutines of type std::experimental::generator");
                return _STD forward<_Uty>(_Whatever);
            }

            using _Alloc_char = _Rebind_alloc_t<_Alloc, char>;
            static_assert(is_same_v<char*, typename allocator_traits<_Alloc_char>::pointer>,
                "generator does not support allocators with fancy pointer types");
            static_assert(
                allocator_traits<_Alloc_char>::is_always_equal::value && is_default_constructible_v<_Alloc_char>,
                "generator supports only stateless allocators");

            static void* operator new(size_t _Size) {
                _Alloc_char _Al{};
                return allocator_traits<_Alloc_char>::allocate(_Al, _Size);
            }

            static void operator delete(void* _Ptr, size_t _Size) noexcept {
                _Alloc_char _Al{};
                return allocator_traits<_Alloc_char>::deallocate(_Al, static_cast<char*>(_Ptr), _Size);
            }
        };

        struct iterator {
            using iterator_category = input_iterator_tag;
            using difference_type   = ptrdiff_t;
            using value_type        = _Ty;
            using reference         = const _Ty&;
            using pointer           = const _Ty*;

            coroutine_handle<promise_type> _Coro = nullptr;

            iterator() = default;
            explicit iterator(coroutine_handle<promise_type> _Coro_) noexcept : _Coro(_Coro_) {}

            iterator& operator++() {
                _Coro.resume();
                if (_Coro.done()) {
#ifdef _CPPUNWIND // TRANSITION, VSO-1172852
                    _STD exchange(_Coro, nullptr).promise()._Rethrow_if_exception();
#else // ^^^ workaround / no workaround vvv
                    _Coro = nullptr;
#endif // TRANSITION, VSO-1172852
                }

                return *this;
            }

            void operator++(int) {
                // This operator meets the requirements of the C++20 input_iterator concept,
                // but not the Cpp17InputIterator requirements.
                ++*this;
            }

            _NODISCARD bool operator==(const iterator& _Right) const noexcept {
                return _Coro == _Right._Coro;
            }

            _NODISCARD bool operator!=(const iterator& _Right) const noexcept {
                return !(*this == _Right);
            }

            _NODISCARD reference operator*() const noexcept {
                return *_Coro.promise()._Value;
            }

            _NODISCARD pointer operator->() const noexcept {
                return _Coro.promise()._Value;
            }
        };

        _NODISCARD iterator begin() {
            if (_Coro) {
                _Coro.resume();
                if (_Coro.done()) {
#ifdef _CPPUNWIND // TRANSITION, VSO-1172852
                    _Coro.promise()._Rethrow_if_exception();
#endif // TRANSITION, VSO-1172852
                    return {};
                }
            }

            return iterator{_Coro};
        }

        _NODISCARD iterator end() noexcept {
            return {};
        }

        explicit generator(promise_type& _Prom) noexcept : _Coro(coroutine_handle<promise_type>::from_promise(_Prom)) {}

        generator() = default;

        generator(generator&& _Right) noexcept : _Coro(_STD exchange(_Right._Coro, nullptr)) {}

        generator& operator=(generator&& _Right) noexcept {
            _Coro = _STD exchange(_Right._Coro, nullptr);
            return *this;
        }

        ~generator() {
            if (_Coro) {
                _Coro.destroy();
            }
        }

    private:
        coroutine_handle<promise_type> _Coro = nullptr;
    };
} // namespace experimental

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _EXPERIMENTAL_GENERATOR_
