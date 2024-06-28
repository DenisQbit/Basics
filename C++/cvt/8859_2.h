// 8859_2 -- one- or two-byte/wide-character tables

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _CVT_8859_2_
#define _CVT_8859_2_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
//
//  Name:             ISO 8859-2:1999 to Unicode
//  Unicode version:  3.0
//  Table version:    1.0
//  Table format:     Format A
//  Date:             1999 July 27
//  Authors:          Ken Whistler
//
//  Copyright (c) 1991-1999 Unicode, Inc.  All Rights reserved.
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
//        ISO/IEC 8859-2:1999 characters map into Unicode.
//
//  Format:  Three tab-separated columns
//    Column #1 is the ISO/IEC 8859-2 code (in hex as 0xXX)
//    Column #2 is the Unicode (in hex as 0xXXXX)
//    Column #3 the Unicode name (follows a comment sign, '#')
//
//  The entries are in ISO/IEC 8859-2 order.
//
//  Version history
//  1.0 version updates 0.1 version by adding mappings for all
//  control characters.
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
        struct _tab_8859_2 {
            enum { _Nlow = 0xa1, _Nbytes = 1 };
            static const unsigned short _Btw[0x5f];
            static const unsigned short _Dbvalid[0x01];
            static const unsigned short _Dbtw[0x01];
            static const unsigned short _Wvalid[0x39];
            static const unsigned char _Wtb[0x39];
        };

        template <class _Dummy>
        const unsigned short _tab_8859_2<_Dummy>::_Btw[0x5f] = { // map single bytes to words
            0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7, 0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad,
            0x017d, 0x017b, 0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7, 0x00b8, 0x0161, 0x015f,
            0x0165, 0x017a, 0x02dd, 0x017e, 0x017c, 0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
            0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e, 0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4,
            0x0150, 0x00d6, 0x00d7, 0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df, 0x0155, 0x00e1,
            0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, 0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee,
            0x010f, 0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, 0x0159, 0x016f, 0x00fa, 0x0171,
            0x00fc, 0x00fd, 0x0163, 0x02d9};

        template <class _Dummy>
        const unsigned short _tab_8859_2<_Dummy>::_Dbvalid[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_8859_2<_Dummy>::_Dbtw[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_8859_2<_Dummy>::_Wvalid[0x39] = { // words that map to single or double bytes
            0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x010c, 0x010d, 0x010e, 0x010f, 0x0110, 0x0111, 0x0118,
            0x0119, 0x011a, 0x011b, 0x0139, 0x013a, 0x013d, 0x013e, 0x0141, 0x0142, 0x0143, 0x0144, 0x0147, 0x0148,
            0x0150, 0x0151, 0x0154, 0x0155, 0x0158, 0x0159, 0x015a, 0x015b, 0x015e, 0x015f, 0x0160, 0x0161, 0x0162,
            0x0163, 0x0164, 0x0165, 0x016e, 0x016f, 0x0170, 0x0171, 0x0179, 0x017a, 0x017b, 0x017c, 0x017d, 0x017e,
            0x02c7, 0x02d8, 0x02d9, 0x02db, 0x02dd};

        template <class _Dummy>
        const unsigned char _tab_8859_2<_Dummy>::_Wtb[0x39] = { // map words to bytes
            0x00c3, 0x00e3, 0x00a1, 0x00b1, 0x00c6, 0x00e6, 0x00c8, 0x00e8, 0x00cf, 0x00ef, 0x00d0, 0x00f0, 0x00ca,
            0x00ea, 0x00cc, 0x00ec, 0x00c5, 0x00e5, 0x00a5, 0x00b5, 0x00a3, 0x00b3, 0x00d1, 0x00f1, 0x00d2, 0x00f2,
            0x00d5, 0x00f5, 0x00c0, 0x00e0, 0x00d8, 0x00f8, 0x00a6, 0x00b6, 0x00aa, 0x00ba, 0x00a9, 0x00b9, 0x00de,
            0x00fe, 0x00ab, 0x00bb, 0x00d9, 0x00f9, 0x00db, 0x00fb, 0x00ac, 0x00bc, 0x00af, 0x00bf, 0x00ae, 0x00be,
            0x00b7, 0x00a2, 0x00ff, 0x00b2, 0x00bd};

        template <class _Elem, unsigned long _Maxcode = 0xffff>
        class codecvt_8859_2 : public _Cvt_one_byte<_Elem, _tab_8859_2<int>, _Maxcode> {};
    } // namespace cvt
} // namespace stdext

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CVT_8859_2_
