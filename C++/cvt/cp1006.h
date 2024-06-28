// cp1006 -- one- or two-byte/wide-character tables

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#ifndef _CVT_CP1006_
#define _CVT_CP1006_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
//
//  Name:             IBM CP1006  to Unicode
//  Unicode version:  3.0
//  Table version:    1.0
//  Table format:     Format A
//  Date:             1999 July 27
//  Authors:          Ken Whistler
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
//        IBM CP1006 characters map into Unicode.
//
//  Format:  Three tab-separated columns
//    Column #1 is the CP1006 code (in hex as 0xXX)
//    Column #2 is the Unicode (in hex as 0xXXXX)
//    Column #3 the Unicode name (follows a comment sign, '#')
//
//  The entries are in IBM CP1006 order.
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
        struct _tab_cp1006 {
            enum { _Nlow = 0xa1, _Nbytes = 1 };
            static const unsigned short _Btw[0x5f];
            static const unsigned short _Dbvalid[0x01];
            static const unsigned short _Dbtw[0x01];
            static const unsigned short _Wvalid[0x5e];
            static const unsigned char _Wtb[0x5e];
        };

        template <class _Dummy>
        const unsigned short _tab_cp1006<_Dummy>::_Btw[0x5f] = { // map single bytes to words
            0x06f0, 0x06f1, 0x06f2, 0x06f3, 0x06f4, 0x06f5, 0x06f6, 0x06f7, 0x06f8, 0x06f9, 0x060c, 0x061b, 0x00ad,
            0x061f, 0xfe81, 0xfe8d, 0xfe8e, 0xfe8e, 0xfe8f, 0xfe91, 0xfb56, 0xfb58, 0xfe93, 0xfe95, 0xfe97, 0xfb66,
            0xfb68, 0xfe99, 0xfe9b, 0xfe9d, 0xfe9f, 0xfb7a, 0xfb7c, 0xfea1, 0xfea3, 0xfea5, 0xfea7, 0xfea9, 0xfb84,
            0xfeab, 0xfead, 0xfb8c, 0xfeaf, 0xfb8a, 0xfeb1, 0xfeb3, 0xfeb5, 0xfeb7, 0xfeb9, 0xfebb, 0xfebd, 0xfebf,
            0xfec1, 0xfec5, 0xfec9, 0xfeca, 0xfecb, 0xfecc, 0xfecd, 0xfece, 0xfecf, 0xfed0, 0xfed1, 0xfed3, 0xfed5,
            0xfed7, 0xfed9, 0xfedb, 0xfb92, 0xfb94, 0xfedd, 0xfedf, 0xfee0, 0xfee1, 0xfee3, 0xfb9e, 0xfee5, 0xfee7,
            0xfe85, 0xfeed, 0xfba6, 0xfba8, 0xfba9, 0xfbaa, 0xfe80, 0xfe89, 0xfe8a, 0xfe8b, 0xfef1, 0xfef2, 0xfef3,
            0xfbb0, 0xfbae, 0xfe7c, 0xfe7d};

        template <class _Dummy>
        const unsigned short _tab_cp1006<_Dummy>::_Dbvalid[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_cp1006<_Dummy>::_Dbtw[1] = {0};
        template <class _Dummy>
        const unsigned short _tab_cp1006<_Dummy>::_Wvalid[0x5e] = { // words that map to single or double bytes
            0x060c, 0x061b, 0x061f, 0x06f0, 0x06f1, 0x06f2, 0x06f3, 0x06f4, 0x06f5, 0x06f6, 0x06f7, 0x06f8, 0x06f9,
            0xfb56, 0xfb58, 0xfb66, 0xfb68, 0xfb7a, 0xfb7c, 0xfb84, 0xfb8a, 0xfb8c, 0xfb92, 0xfb94, 0xfb9e, 0xfba6,
            0xfba8, 0xfba9, 0xfbaa, 0xfbae, 0xfbb0, 0xfe7c, 0xfe7d, 0xfe80, 0xfe81, 0xfe85, 0xfe89, 0xfe8a, 0xfe8b,
            0xfe8d, 0xfe8e, 0xfe8e, 0xfe8f, 0xfe91, 0xfe93, 0xfe95, 0xfe97, 0xfe99, 0xfe9b, 0xfe9d, 0xfe9f, 0xfea1,
            0xfea3, 0xfea5, 0xfea7, 0xfea9, 0xfeab, 0xfead, 0xfeaf, 0xfeb1, 0xfeb3, 0xfeb5, 0xfeb7, 0xfeb9, 0xfebb,
            0xfebd, 0xfebf, 0xfec1, 0xfec5, 0xfec9, 0xfeca, 0xfecb, 0xfecc, 0xfecd, 0xfece, 0xfecf, 0xfed0, 0xfed1,
            0xfed3, 0xfed5, 0xfed7, 0xfed9, 0xfedb, 0xfedd, 0xfedf, 0xfee0, 0xfee1, 0xfee3, 0xfee5, 0xfee7, 0xfeed,
            0xfef1, 0xfef2, 0xfef3};

        template <class _Dummy>
        const unsigned char _tab_cp1006<_Dummy>::_Wtb[0x5e] = { // map words to bytes
            0x00ab, 0x00ac, 0x00ae, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x00aa,
            0x00b5, 0x00b6, 0x00ba, 0x00bb, 0x00c0, 0x00c1, 0x00c7, 0x00cc, 0x00ca, 0x00e5, 0x00e6, 0x00ec, 0x00f1,
            0x00f2, 0x00f3, 0x00f4, 0x00fd, 0x00fc, 0x00fe, 0x00ff, 0x00f5, 0x00af, 0x00ef, 0x00f6, 0x00f7, 0x00f8,
            0x00b0, 0x00b2, 0x00b1, 0x00b3, 0x00b4, 0x00b7, 0x00b8, 0x00b9, 0x00bc, 0x00bd, 0x00be, 0x00bf, 0x00c2,
            0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c8, 0x00c9, 0x00cb, 0x00cd, 0x00ce, 0x00cf, 0x00d0, 0x00d1, 0x00d2,
            0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
            0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ed, 0x00ee, 0x00f0,
            0x00f9, 0x00fa, 0x00fb};

        template <class _Elem, unsigned long _Maxcode = 0xffff>
        class codecvt_cp1006 : public _Cvt_one_byte<_Elem, _tab_cp1006<int>, _Maxcode> {};
    } // namespace cvt
} // namespace stdext

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CVT_CP1006_
