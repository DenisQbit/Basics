// iceland -- one- or two-byte/wide-character tables

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _CVT_ICELAND_
#define _CVT_ICELAND_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
//
//     Name:     cp10079_MacIcelandic to Unicode table
//     Unicode version: 2.0
//     Table version: 2.00
//     Table format:  Format A
//     Date:          04/24/96
//     Authors:       Lori Brownell
//                    K.D. Chang
//     General notes: none
//
//     Format: Three tab-separated columns
//         Column #1 is the cp10079_MacIcelandic code (in hex)
//         Column #2 is the Unicode (in hex as 0xXXXX)
//         Column #3 is the Unicode name (follows a comment sign, '#')
//
//     The entries are in cp10079_MacIcelandic order
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
        struct _tab_iceland {
            enum { _Nlow = 0x80, _Nbytes = 1 };
            static const unsigned short _Btw[0x80];
            static const unsigned short _Dbvalid[0x01];
            static const unsigned short _Dbtw[0x01];
            static const unsigned short _Wvalid[0x79];
            static const unsigned char _Wtb[0x79];
        };

        template <class _Dummy>
        const unsigned short _tab_iceland<_Dummy>::_Btw[0x80] = { // map single bytes to words
            0x00c4, 0x00c5, 0x00c7, 0x00c9, 0x00d1, 0x00d6, 0x00dc, 0x00e1, 0x00e0, 0x00e2, 0x00e4, 0x00e3, 0x00e5,
            0x00e7, 0x00e9, 0x00e8, 0x00ea, 0x00eb, 0x00ed, 0x00ec, 0x00ee, 0x00ef, 0x00f1, 0x00f3, 0x00f2, 0x00f4,
            0x00f6, 0x00f5, 0x00fa, 0x00f9, 0x00fb, 0x00fc, 0x00dd, 0x00b0, 0x00a2, 0x00a3, 0x00a7, 0x2022, 0x00b6,
            0x00df, 0x00ae, 0x00a9, 0x2122, 0x00b4, 0x00a8, 0x2260, 0x00c6, 0x00d8, 0x221e, 0x00b1, 0x2264, 0x2265,
            0x00a5, 0x00b5, 0x2202, 0x2211, 0x220f, 0x03c0, 0x222b, 0x00aa, 0x00ba, 0x2126, 0x00e6, 0x00f8, 0x00bf,
            0x00a1, 0x00ac, 0x221a, 0x0192, 0x2248, 0x2206, 0x00ab, 0x00bb, 0x2026, 0x00a0, 0x00c0, 0x00c3, 0x00d5,
            0x0152, 0x0153, 0x2013, 0x2014, 0x201c, 0x201d, 0x2018, 0x2019, 0x00f7, 0x25ca, 0x00ff, 0x0178, 0x2044,
            0x00a4, 0x00d0, 0x00f0, 0x00de, 0x00fe, 0x00fd, 0x00b7, 0x201a, 0x201e, 0x2030, 0x00c2, 0x00ca, 0x00c1,
            0x00cb, 0x00c8, 0x00cd, 0x00ce, 0x00cf, 0x00cc, 0x00d3, 0x00d4, 0x0000, 0x00d2, 0x00da, 0x00db, 0x00d9,
            0x0131, 0x02c6, 0x02dc, 0x00af, 0x02d8, 0x02d9, 0x02da, 0x00b8, 0x02dd, 0x02db, 0x02c7};

        template <class _Dummy>
        const unsigned short _tab_iceland<_Dummy>::_Dbvalid[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_iceland<_Dummy>::_Dbtw[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_iceland<_Dummy>::_Wvalid[0x79] = { // words that map to single or double bytes
            0x00a0, 0x00a1, 0x00a4, 0x00a5, 0x00a7, 0x00a8, 0x00aa, 0x00ab, 0x00ac, 0x00ae, 0x00af, 0x00b0, 0x00b4,
            0x00b6, 0x00b7, 0x00b8, 0x00ba, 0x00bb, 0x00bf, 0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6,
            0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, 0x00d0, 0x00d1, 0x00d2, 0x00d3,
            0x00d4, 0x00d5, 0x00d6, 0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00df, 0x00e0, 0x00e1, 0x00e2,
            0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
            0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc,
            0x00fd, 0x00fe, 0x00ff, 0x0131, 0x0152, 0x0153, 0x0178, 0x0192, 0x02c6, 0x02c7, 0x02d8, 0x02d9, 0x02da,
            0x02db, 0x02dc, 0x02dd, 0x03c0, 0x2013, 0x2014, 0x2018, 0x2019, 0x201a, 0x201c, 0x201d, 0x201e, 0x2022,
            0x2026, 0x2030, 0x2044, 0x2122, 0x2126, 0x2202, 0x2206, 0x220f, 0x2211, 0x221a, 0x221e, 0x222b, 0x2248,
            0x2260, 0x2264, 0x2265, 0x25ca};

        template <class _Dummy>
        const unsigned char _tab_iceland<_Dummy>::_Wtb[0x79] = { // map words to bytes
            0x00ca, 0x00c1, 0x00db, 0x00b4, 0x00a4, 0x00ac, 0x00bb, 0x00c7, 0x00c2, 0x00a8, 0x00f8, 0x00a1, 0x00ab,
            0x00a6, 0x00e1, 0x00fc, 0x00bc, 0x00c8, 0x00c0, 0x00cb, 0x00e7, 0x00e5, 0x00cc, 0x0080, 0x0081, 0x00ae,
            0x0082, 0x00e9, 0x0083, 0x00e6, 0x00e8, 0x00ed, 0x00ea, 0x00eb, 0x00ec, 0x00dc, 0x0084, 0x00f1, 0x00ee,
            0x00ef, 0x00cd, 0x0085, 0x00af, 0x00f4, 0x00f2, 0x00f3, 0x0086, 0x00a0, 0x00a7, 0x0088, 0x0087, 0x0089,
            0x008b, 0x008a, 0x008c, 0x00be, 0x008d, 0x008f, 0x008e, 0x0090, 0x0091, 0x0093, 0x0092, 0x0094, 0x0095,
            0x00dd, 0x0096, 0x0098, 0x0097, 0x0099, 0x009b, 0x009a, 0x00d6, 0x00bf, 0x009d, 0x009c, 0x009e, 0x009f,
            0x00e0, 0x00df, 0x00d8, 0x00f5, 0x00ce, 0x00cf, 0x00d9, 0x00c4, 0x00f6, 0x00ff, 0x00f9, 0x00fa, 0x00fb,
            0x00fe, 0x00f7, 0x00fd, 0x00b9, 0x00d0, 0x00d1, 0x00d4, 0x00d5, 0x00e2, 0x00d2, 0x00d3, 0x00e3, 0x00a5,
            0x00c9, 0x00e4, 0x00da, 0x00aa, 0x00bd, 0x00b6, 0x00c6, 0x00b8, 0x00b7, 0x00c3, 0x00b0, 0x00ba, 0x00c5,
            0x00ad, 0x00b2, 0x00b3, 0x00d7};

        template <class _Elem, unsigned long _Maxcode = 0xffff>
        class codecvt_iceland : public _Cvt_one_byte<_Elem, _tab_iceland<int>, _Maxcode> {};
    } // namespace cvt
} // namespace stdext

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CVT_ICELAND_
