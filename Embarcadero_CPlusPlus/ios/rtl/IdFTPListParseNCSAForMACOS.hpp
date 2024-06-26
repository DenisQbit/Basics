﻿// CodeGear C++Builder
// Copyright (c) 1995, 2021 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'IdFTPListParseNCSAForMACOS.pas' rev: 34.00 (iOS)

#ifndef IdftplistparsencsaformacosHPP
#define IdftplistparsencsaformacosHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <System.Classes.hpp>
#include <IdFTPList.hpp>
#include <IdFTPListParseBase.hpp>
#include <IdFTPListTypes.hpp>

//-- user supplied -----------------------------------------------------------
namespace Idftplistparsencsaformacos
{
  _INIT_UNIT(Idftplistparsencsaformacos);
}	/* namespace Idftplistparsencsaformacos */

namespace Idftplistparsencsaformacos
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TIdNCSAforMACOSFTPListItem;
class DELPHICLASS TIdFTPLPNCSAforMACOS;
//-- type declarations -------------------------------------------------------
class PASCALIMPLEMENTATION TIdNCSAforMACOSFTPListItem : public Idftplisttypes::TIdMinimalFTPListItem
{
	typedef Idftplisttypes::TIdMinimalFTPListItem inherited;
	
public:
	/* TIdMinimalFTPListItem.Create */ inline __fastcall virtual TIdNCSAforMACOSFTPListItem(System::Classes::TCollection* AOwner) : Idftplisttypes::TIdMinimalFTPListItem(AOwner) { }
	
public:
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TIdNCSAforMACOSFTPListItem() { }
	
};


class PASCALIMPLEMENTATION TIdFTPLPNCSAforMACOS : public Idftplistparsebase::TIdFTPLPNList
{
	typedef Idftplistparsebase::TIdFTPLPNList inherited;
	
protected:
	__classmethod virtual Idftplist::TIdFTPListItem* __fastcall MakeNewItem(Idftplist::TIdFTPListItems* AOwner);
	__classmethod virtual bool __fastcall ParseLine(Idftplist::TIdFTPListItem* const AItem, const System::UnicodeString APath = System::UnicodeString());
	
public:
	__classmethod virtual System::UnicodeString __fastcall GetIdent();
	__classmethod virtual bool __fastcall CheckListing(System::Classes::TStrings* AListing, const System::UnicodeString ASysDescript = System::UnicodeString(), const bool ADetails = true);
public:
	/* TObject.Create */ inline __fastcall TIdFTPLPNCSAforMACOS() : Idftplistparsebase::TIdFTPLPNList() { }
	/* TObject.Destroy */ inline __fastcall virtual ~TIdFTPLPNCSAforMACOS() { }
	
};


//-- var, const, procedure ---------------------------------------------------
}	/* namespace Idftplistparsencsaformacos */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_IDFTPLISTPARSENCSAFORMACOS)
using namespace Idftplistparsencsaformacos;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// IdftplistparsencsaformacosHPP
