// cp737 -- one- or two-byte/wide-character tables

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _CVT_CP737_
#define _CVT_CP737_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
//
//     Name:     cp737_DOSGreek to Unicode table
//     Unicode version: 2.0
//     Table version: 2.00
//     Table format:  Format A
//     Date:          04/24/96
//     Authors:       Lori Brownell
//                    K.D. Chang
//     General notes: none
//
//     Format: Three tab-separated columns
//         Column #1 is the cp737_DOSGreek code (in hex)
//         Column #2 is the Unicode (in hex as 0xXXXX)
//         Column #3 is the Unicode name (follows a comment sign, '#')
//
//     The entries are in cp737_DOSGreek order
//
#include <cvt/xone_byte>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

namespace stdext {
    namespace cvt {
        template <class _Dummy>
        struct _tab_cp737 {
            enum { _Nlow = 0x80, _Nbytes = 1 };
            static const unsigned short _Btw[0x80];
            static const unsigned short _Dbvalid[0x01];
            static const unsigned short _Dbtw[0x01];
            static const unsigned short _Wvalid[0x80];
            static const unsigned char _Wtb[0x80];
        };

        template <class _Dummy>
        const unsigned short _tab_cp737<_Dummy>::_Btw[0x80] = { // map single bytes to words
            0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d,
            0x039e, 0x039f, 0x03a0, 0x03a1, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03b1, 0x03b2,
            0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
            0x03c0, 0x03c1, 0x03c3, 0x03c2, 0x03c4, 0x03c5, 0x03c6, 0x03c7, 0x03c8, 0x2591, 0x2592, 0x2593, 0x2502,
            0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510, 0x2514,
            0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550,
            0x256c, 0x2567, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c,
            0x2588, 0x2584, 0x258c, 0x2590, 0x2580, 0x03c9, 0x03ac, 0x03ad, 0x03ae, 0x03ca, 0x03af, 0x03cc, 0x03cd,
            0x03cb, 0x03ce, 0x0386, 0x0388, 0x0389, 0x038a, 0x038c, 0x038e, 0x038f, 0x00b1, 0x2265, 0x2264, 0x03aa,
            0x03ab, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0};

        template <class _Dummy>
        const unsigned short _tab_cp737<_Dummy>::_Dbvalid[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_cp737<_Dummy>::_Dbtw[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_cp737<_Dummy>::_Wvalid[0x80] = { // words that map to single or double bytes
            0x00a0, 0x00b0, 0x00b1, 0x00b2, 0x00b7, 0x00f7, 0x0386, 0x0388, 0x0389, 0x038a, 0x038c, 0x038e, 0x038f,
            0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d,
            0x039e, 0x039f, 0x03a0, 0x03a1, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03aa, 0x03ab,
            0x03ac, 0x03ad, 0x03ae, 0x03af, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9,
            0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf, 0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6,
            0x03c7, 0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0x207f, 0x2219, 0x221a, 0x2248, 0x2264,
            0x2265, 0x2500, 0x2502, 0x250c, 0x2510, 0x2514, 0x2518, 0x251c, 0x2524, 0x252c, 0x2534, 0x253c, 0x2550,
            0x2551, 0x2552, 0x2553, 0x2554, 0x2555, 0x2556, 0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x255c, 0x255d,
            0x255e, 0x255f, 0x2560, 0x2561, 0x2562, 0x2563, 0x2564, 0x2565, 0x2566, 0x2567, 0x2568, 0x2569, 0x256a,
            0x256b, 0x256c, 0x2580, 0x2584, 0x2588, 0x258c, 0x2590, 0x2591, 0x2592, 0x2593, 0x25a0};

        template <class _Dummy>
        const unsigned char _tab_cp737<_Dummy>::_Wtb[0x80] = { // map words to bytes
            0x00ff, 0x00f8, 0x00f1, 0x00fd, 0x00fa, 0x00f6, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 0x00f0,
            0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008a, 0x008b, 0x008c,
            0x008d, 0x008e, 0x008f, 0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x00f4, 0x00f5,
            0x00e1, 0x00e2, 0x00e3, 0x00e5, 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 0x00a0,
            0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00aa, 0x00a9, 0x00ab, 0x00ac, 0x00ad,
            0x00ae, 0x00af, 0x00e0, 0x00e4, 0x00e8, 0x00e6, 0x00e7, 0x00e9, 0x00fc, 0x00f9, 0x00fb, 0x00f7, 0x00f3,
            0x00f2, 0x00c4, 0x00b3, 0x00da, 0x00bf, 0x00c0, 0x00d9, 0x00c3, 0x00b4, 0x00c2, 0x00c1, 0x00c5, 0x00cd,
            0x00ba, 0x00d5, 0x00d6, 0x00c9, 0x00b8, 0x00b7, 0x00bb, 0x00d4, 0x00d3, 0x00c8, 0x00be, 0x00bd, 0x00bc,
            0x00c6, 0x00c7, 0x00cc, 0x00b5, 0x00b6, 0x00b9, 0x00d1, 0x00d2, 0x00cb, 0x00cf, 0x00d0, 0x00ca, 0x00d8,
            0x00d7, 0x00ce, 0x00df, 0x00dc, 0x00db, 0x00dd, 0x00de, 0x00b0, 0x00b1, 0x00b2, 0x00fe};

        template <class _Elem, unsigned long _Maxcode = 0xffff>
        class codecvt_cp737 : public _Cvt_one_byte<_Elem, _tab_cp737<int>, _Maxcode> {};
    } // namespace cvt
} // namespace stdext

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CVT_CP737_
