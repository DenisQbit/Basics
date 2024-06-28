// shared_mutex standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _SHARED_MUTEX_
#define _SHARED_MUTEX_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#ifdef _M_CEE_PURE
#error <shared_mutex> is not supported when compiling with /clr:pure.
#endif // _M_CEE_PURE

#include <mutex>
#include <xthreads.h>
#ifndef _M_CEE
#include <condition_variable>
#endif // _M_CEE

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
class shared_mutex { // class for mutual exclusion shared across threads
public:
    using native_handle_type = _Smtx_t*;

    shared_mutex() noexcept // strengthened
        : _Myhandle(nullptr) {}

    ~shared_mutex() noexcept {}

    void lock() noexcept /* strengthened */ { // lock exclusive
        _Smtx_lock_exclusive(&_Myhandle);
    }

    _NODISCARD bool try_lock() noexcept /* strengthened */ { // try to lock exclusive
        return _Smtx_try_lock_exclusive(&_Myhandle) != 0;
    }

    void unlock() noexcept /* strengthened */ { // unlock exclusive
        _Smtx_unlock_exclusive(&_Myhandle);
    }

    void lock_shared() noexcept /* strengthened */ { // lock non-exclusive
        _Smtx_lock_shared(&_Myhandle);
    }

    _NODISCARD bool try_lock_shared() noexcept /* strengthened */ { // try to lock non-exclusive
        return _Smtx_try_lock_shared(&_Myhandle) != 0;
    }

    void unlock_shared() noexcept /* strengthened */ { // unlock non-exclusive
        _Smtx_unlock_shared(&_Myhandle);
    }

    _NODISCARD native_handle_type native_handle() noexcept /* strengthened */ { // get native handle
        return &_Myhandle;
    }

    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

private:
    _Smtx_t _Myhandle;
};

// shared_timed_mutex is not supported under /clr
#ifndef _M_CEE
class shared_timed_mutex { // class for mutual exclusion shared across threads
    using _Read_cnt_t = unsigned int;

    static constexpr _Read_cnt_t _Max_readers = static_cast<_Read_cnt_t>(-1);

public:
    shared_timed_mutex() noexcept // strengthened
        : _Mymtx(), _Read_queue(), _Write_queue(), _Readers(0), _Writing(false) {}

    ~shared_timed_mutex() noexcept {}

    void lock() { // lock exclusive
        unique_lock<mutex> _Lock(_Mymtx);
        while (_Writing) {
            _Write_queue.wait(_Lock);
        }

        _Writing = true;
        while (0 < _Readers) {
            _Read_queue.wait(_Lock); // wait for writing, no readers
        }
    }

    _NODISCARD bool try_lock() { // try to lock exclusive
        lock_guard<mutex> _Lock(_Mymtx);
        if (_Writing || 0 < _Readers) {
            return false;
        } else { // set writing, no readers
            _Writing = true;
            return true;
        }
    }

    template <class _Rep, class _Period>
    _NODISCARD bool try_lock_for(const chrono::duration<_Rep, _Period>& _Rel_time) { // try to lock for duration
        return try_lock_until(_To_absolute_time(_Rel_time));
    }

    template <class _Clock, class _Duration>
    _NODISCARD bool try_lock_until(const chrono::time_point<_Clock, _Duration>& _Abs_time) {
        // try to lock until time point
#if _HAS_CXX20
        static_assert(chrono::is_clock_v<_Clock>, "Clock type required");
#endif // _HAS_CXX20
        auto _Not_writing  = [this] { return !_Writing; };
        auto _Zero_readers = [this] { return _Readers == 0; };
        unique_lock<mutex> _Lock(_Mymtx);

        if (!_Write_queue.wait_until(_Lock, _Abs_time, _Not_writing)) {
            return false;
        }

        _Writing = true;

        if (!_Read_queue.wait_until(_Lock, _Abs_time, _Zero_readers)) { // timeout, leave writing state
            _Writing = false;
            _Lock.unlock(); // unlock before notifying, for efficiency
            _Write_queue.notify_all();
            return false;
        }

        return true;
    }

    void unlock() { // unlock exclusive
        { // unlock before notifying, for efficiency
            lock_guard<mutex> _Lock(_Mymtx);

            _Writing = false;
        }

        _Write_queue.notify_all();
    }

    void lock_shared() { // lock non-exclusive
        unique_lock<mutex> _Lock(_Mymtx);
        while (_Writing || _Readers == _Max_readers) {
            _Write_queue.wait(_Lock);
        }

        ++_Readers;
    }

    _NODISCARD bool try_lock_shared() { // try to lock non-exclusive
        lock_guard<mutex> _Lock(_Mymtx);
        if (_Writing || _Readers == _Max_readers) {
            return false;
        } else { // count another reader
            ++_Readers;
            return true;
        }
    }

    template <class _Rep, class _Period>
    _NODISCARD bool try_lock_shared_for(const chrono::duration<_Rep, _Period>& _Rel_time) {
        // try to lock non-exclusive for relative time
        return try_lock_shared_until(_To_absolute_time(_Rel_time));
    }

    template <class _Time>
    bool _Try_lock_shared_until(_Time _Abs_time) { // try to lock non-exclusive until absolute time
        const auto _Can_acquire = [this] { return !_Writing && _Readers < _Max_readers; };

        unique_lock<mutex> _Lock(_Mymtx);

        if (!_Write_queue.wait_until(_Lock, _Abs_time, _Can_acquire)) {
            return false;
        }

        ++_Readers;
        return true;
    }

    template <class _Clock, class _Duration>
    _NODISCARD bool try_lock_shared_until(const chrono::time_point<_Clock, _Duration>& _Abs_time) {
        // try to lock non-exclusive until absolute time
#if _HAS_CXX20
        static_assert(chrono::is_clock_v<_Clock>, "Clock type required");
#endif // _HAS_CXX20
        return _Try_lock_shared_until(_Abs_time);
    }

    _NODISCARD bool try_lock_shared_until(const xtime* _Abs_time) { // try to lock non-exclusive until absolute time
        return _Try_lock_shared_until(_Abs_time);
    }

    void unlock_shared() { // unlock non-exclusive
        _Read_cnt_t _Local_readers;
        bool _Local_writing;

        { // unlock before notifying, for efficiency
            lock_guard<mutex> _Lock(_Mymtx);
            --_Readers;
            _Local_readers = _Readers;
            _Local_writing = _Writing;
        }

        if (_Local_writing && _Local_readers == 0) {
            _Read_queue.notify_one();
        } else if (!_Local_writing && _Local_readers == _Max_readers - 1) {
            _Write_queue.notify_all();
        }
    }

    shared_timed_mutex(const shared_timed_mutex&) = delete;
    shared_timed_mutex& operator=(const shared_timed_mutex&) = delete;

private:
    mutex _Mymtx;
    condition_variable _Read_queue;
    condition_variable _Write_queue;
    _Read_cnt_t _Readers;
    bool _Writing;
};
#endif // _M_CEE

template <class _Mutex>
class shared_lock { // shareable lock
public:
    using mutex_type = _Mutex;

    shared_lock() noexcept : _Pmtx(nullptr), _Owns(false) {}

    _NODISCARD_CTOR explicit shared_lock(mutex_type& _Mtx)
        : _Pmtx(_STD addressof(_Mtx)), _Owns(true) { // construct with mutex and lock shared
        _Mtx.lock_shared();
    }

    shared_lock(mutex_type& _Mtx, defer_lock_t) noexcept
        : _Pmtx(_STD addressof(_Mtx)), _Owns(false) {} // construct with unlocked mutex

    _NODISCARD_CTOR shared_lock(mutex_type& _Mtx, try_to_lock_t)
        : _Pmtx(_STD addressof(_Mtx)), _Owns(_Mtx.try_lock_shared()) {} // construct with mutex and try to lock shared

    _NODISCARD_CTOR shared_lock(mutex_type& _Mtx, adopt_lock_t)
        : _Pmtx(_STD addressof(_Mtx)), _Owns(true) {} // construct with mutex and adopt ownership

    template <class _Rep, class _Period>
    _NODISCARD_CTOR shared_lock(mutex_type& _Mtx, const chrono::duration<_Rep, _Period>& _Rel_time)
        : _Pmtx(_STD addressof(_Mtx)), _Owns(_Mtx.try_lock_shared_for(_Rel_time)) {
        // construct with mutex and try to lock for relative time
    }

    template <class _Clock, class _Duration>
    _NODISCARD_CTOR shared_lock(mutex_type& _Mtx, const chrono::time_point<_Clock, _Duration>& _Abs_time)
        : _Pmtx(_STD addressof(_Mtx)), _Owns(_Mtx.try_lock_shared_until(_Abs_time)) {
        // construct with mutex and try to lock until absolute time
#if _HAS_CXX20
        static_assert(chrono::is_clock_v<_Clock>, "Clock type required");
#endif // _HAS_CXX20
    }

    ~shared_lock() noexcept {
        if (_Owns) {
            _Pmtx->unlock_shared();
        }
    }

    _NODISCARD_CTOR shared_lock(shared_lock&& _Other) noexcept : _Pmtx(_Other._Pmtx), _Owns(_Other._Owns) {
        _Other._Pmtx = nullptr;
        _Other._Owns = false;
    }

    shared_lock& operator=(shared_lock&& _Right) noexcept {
        if (_Owns) {
            _Pmtx->unlock_shared();
        }

        _Pmtx        = _Right._Pmtx;
        _Owns        = _Right._Owns;
        _Right._Pmtx = nullptr;
        _Right._Owns = false;
        return *this;
    }

    shared_lock(const shared_lock&) = delete;
    shared_lock& operator=(const shared_lock&) = delete;

    void lock() { // lock the mutex
        _Validate();
        _Pmtx->lock_shared();
        _Owns = true;
    }

    _NODISCARD bool try_lock() { // try to lock the mutex
        _Validate();
        _Owns = _Pmtx->try_lock_shared();
        return _Owns;
    }

    template <class _Rep, class _Period>
    _NODISCARD bool try_lock_for(const chrono::duration<_Rep, _Period>& _Rel_time) {
        // try to lock the mutex for _Rel_time
        _Validate();
        _Owns = _Pmtx->try_lock_shared_for(_Rel_time);
        return _Owns;
    }

    template <class _Clock, class _Duration>
    _NODISCARD bool try_lock_until(const chrono::time_point<_Clock, _Duration>& _Abs_time) {
        // try to lock the mutex until _Abs_time
#if _HAS_CXX20
        static_assert(chrono::is_clock_v<_Clock>, "Clock type required");
#endif // _HAS_CXX20
        _Validate();
        _Owns = _Pmtx->try_lock_shared_until(_Abs_time);
        return _Owns;
    }

    void unlock() { // try to unlock the mutex
        if (!_Pmtx || !_Owns) {
            _Throw_system_error(errc::operation_not_permitted);
        }

        _Pmtx->unlock_shared();
        _Owns = false;
    }

    void swap(shared_lock& _Right) noexcept {
        _STD swap(_Pmtx, _Right._Pmtx);
        _STD swap(_Owns, _Right._Owns);
    }

    mutex_type* release() noexcept {
        _Mutex* _Res = _Pmtx;
        _Pmtx        = nullptr;
        _Owns        = false;
        return _Res;
    }

    _NODISCARD bool owns_lock() const noexcept {
        return _Owns;
    }

    explicit operator bool() const noexcept {
        return _Owns;
    }

    _NODISCARD mutex_type* mutex() const noexcept {
        return _Pmtx;
    }

private:
    _Mutex* _Pmtx;
    bool _Owns;

    void _Validate() const { // check if the mutex can be locked
        if (!_Pmtx) {
            _Throw_system_error(errc::operation_not_permitted);
        }

        if (_Owns) {
            _Throw_system_error(errc::resource_deadlock_would_occur);
        }
    }
};

template <class _Mutex>
void swap(shared_lock<_Mutex>& _Left, shared_lock<_Mutex>& _Right) noexcept {
    _Left.swap(_Right);
}
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _SHARED_MUTEX_
