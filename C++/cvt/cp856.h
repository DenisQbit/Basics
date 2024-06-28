// cp856 -- one- or two-byte/wide-character tables

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _CVT_CP856_
#define _CVT_CP856_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
//
//     Name:     cp856_Hebrew_PC to Unicode table
//     Unicode version: 3.0
//     Table version: 1.0
//     Table format:  Format A
//     Date:          1999 July 27
//     Authors:       Ken Whistler
//
//  Copyright (c) 1998 - 1999 Unicode, Inc.  All Rights reserved.
//
//  This file is provided as-is by Unicode, Inc. (The Unicode Consortium).
//  No claims are made as to fitness for any particular purpose.  No
//  warranties of any kind are expressed or implied.  The recipient
//  agrees to determine applicability of information provided.  If this
//  file has been provided on optical media by Unicode, Inc., the sole
//  remedy for any claim will be exchange of defective media within 90
//  days of receipt.
//
//  Unicode, Inc. hereby grants the right to freely use the information
//  supplied in this file in the creation of products supporting the
//  Unicode Standard, and to make copies of this file in any form for
//  internal or external distribution as long as this notice remains
//  attached.
//
//  General notes:
//
//  This table contains the data the Unicode Consortium has on how
//        CP424 characters map into Unicode.
//
//     Format: Three tab-separated columns
//         Column #1 is the cp856_Hebrew_PC code (in hex)
//         Column #2 is the Unicode (in hex as 0xXXXX)
//         Column #3 is the Unicode name (follows a comment sign, '#')
//
//     The entries are in cp856_Hebrew_PC order
//
//  Version history
//  1.0 version new.
//
//  Updated versions of this file may be found in:
//   <ftp://ftp.unicode.org/Public/MAPPINGS/>
//
//  Use the Unicode reporting form <https://www.unicode.org/reporting.html>
//    for any questions or comments or to report errors in the data.
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
        struct _tab_cp856 {
            enum { _Nlow = 0x80, _Nbytes = 1 };
            static const unsigned short _Btw[0x80];
            static const unsigned short _Dbvalid[0x01];
            static const unsigned short _Dbtw[0x01];
            static const unsigned short _Wvalid[0x57];
            static const unsigned char _Wtb[0x57];
        };

        template <class _Dummy>
        const unsigned short _tab_cp856<_Dummy>::_Btw[0x80] = { // map single bytes to words
            0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7, 0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc,
            0x05dd, 0x05de, 0x05df, 0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7, 0x05e8, 0x05e9,
            0x05ea, 0x0000, 0x00a3, 0x0000, 0x00d7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000, 0x00ae, 0x00ac, 0x00bd, 0x00bc, 0x0000, 0x00ab, 0x00bb, 0x2591, 0x2592, 0x2593, 0x2502,
            0x2524, 0x0000, 0x0000, 0x0000, 0x00a9, 0x2563, 0x2551, 0x2557, 0x255d, 0x00a2, 0x00a5, 0x2510, 0x2514,
            0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x0000, 0x0000, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550,
            0x256c, 0x00a4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2518, 0x250c,
            0x2588, 0x2584, 0x00a6, 0x0000, 0x2580, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00b5, 0x0000,
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00af, 0x00b4, 0x00ad, 0x00b1, 0x2017, 0x00be, 0x00b6,
            0x00a7, 0x00f7, 0x00b8, 0x00b0, 0x00a8, 0x00b7, 0x00b9, 0x00b3, 0x00b2, 0x25a0, 0x00a0};

        template <class _Dummy>
        const unsigned short _tab_cp856<_Dummy>::_Dbvalid[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_cp856<_Dummy>::_Dbtw[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_cp856<_Dummy>::_Wvalid[0x57] = { // words that map to single or double bytes
            0x00a0, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x00ab, 0x00ac, 0x00ad, 0x00ae,
            0x00af, 0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8, 0x00b9, 0x00bb, 0x00bc,
            0x00bd, 0x00be, 0x00d7, 0x00f7, 0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7, 0x05d8,
            0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df, 0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5,
            0x05e6, 0x05e7, 0x05e8, 0x05e9, 0x05ea, 0x2017, 0x2500, 0x2502, 0x250c, 0x2510, 0x2514, 0x2518, 0x251c,
            0x2524, 0x252c, 0x2534, 0x253c, 0x2550, 0x2551, 0x2554, 0x2557, 0x255a, 0x255d, 0x2560, 0x2563, 0x2566,
            0x2569, 0x256c, 0x2580, 0x2584, 0x2588, 0x2591, 0x2592, 0x2593, 0x25a0};

        template <class _Dummy>
        const unsigned char _tab_cp856<_Dummy>::_Wtb[0x57] = { // map words to bytes
            0x00ff, 0x00bd, 0x009c, 0x00cf, 0x00be, 0x00dd, 0x00f5, 0x00f9, 0x00b8, 0x00ae, 0x00aa, 0x00f0, 0x00a9,
            0x00ee, 0x00f8, 0x00f1, 0x00fd, 0x00fc, 0x00ef, 0x00e6, 0x00f4, 0x00fa, 0x00f7, 0x00fb, 0x00af, 0x00ac,
            0x00ab, 0x00f3, 0x009e, 0x00f6, 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088,
            0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095,
            0x0096, 0x0097, 0x0098, 0x0099, 0x009a, 0x00f2, 0x00c4, 0x00b3, 0x00da, 0x00bf, 0x00c0, 0x00d9, 0x00c3,
            0x00b4, 0x00c2, 0x00c1, 0x00c5, 0x00cd, 0x00ba, 0x00c9, 0x00bb, 0x00c8, 0x00bc, 0x00cc, 0x00b9, 0x00cb,
            0x00ca, 0x00ce, 0x00df, 0x00dc, 0x00db, 0x00b0, 0x00b1, 0x00b2, 0x00fe};

        template <class _Elem, unsigned long _Maxcode = 0xffff>
        class codecvt_cp856 : public _Cvt_one_byte<_Elem, _tab_cp856<int>, _Maxcode> {};
    } // namespace cvt
} // namespace stdext

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CVT_CP856_
