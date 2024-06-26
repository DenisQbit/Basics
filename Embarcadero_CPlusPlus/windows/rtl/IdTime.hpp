﻿// CodeGear C++Builder
// Copyright (c) 1995, 2021 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'IdTime.pas' rev: 34.00 (Windows)

#ifndef IdtimeHPP
#define IdtimeHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <System.Classes.hpp>
#include <IdGlobal.hpp>
#include <IdAssignedNumbers.hpp>
#include <IdGlobalProtocols.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdComponent.hpp>
#include <IdBaseComponent.hpp>

//-- user supplied -----------------------------------------------------------

namespace Idtime
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TIdCustomTime;
class DELPHICLASS TIdTime;
//-- type declarations -------------------------------------------------------
class PASCALIMPLEMENTATION TIdCustomTime : public Idtcpclient::TIdTCPClientCustom
{
	typedef Idtcpclient::TIdTCPClientCustom inherited;
	
protected:
	System::TDateTime FBaseDate;
	unsigned FRoundTripDelay;
	int FTimeout;
	unsigned __fastcall GetDateTimeCard();
	System::TDateTime __fastcall GetDateTime();
	virtual void __fastcall InitComponent();
	
public:
	__fastcall TIdCustomTime(System::Classes::TComponent* AOwner)/* overload */;
	bool __fastcall SyncTime();
	__property unsigned DateTimeCard = {read=GetDateTimeCard, nodefault};
	__property System::TDateTime DateTime = {read=GetDateTime};
	__property unsigned RoundTripDelay = {read=FRoundTripDelay, nodefault};
	
__published:
	__property int Timeout = {read=FTimeout, write=FTimeout, default=2500};
	__property Host = {default=0};
public:
	/* TIdTCPConnection.Destroy */ inline __fastcall virtual ~TIdCustomTime() { }
	
public:
	/* TIdInitializerComponent.Create */ inline __fastcall TIdCustomTime()/* overload */ : Idtcpclient::TIdTCPClientCustom() { }
	
};


class PASCALIMPLEMENTATION TIdTime : public TIdCustomTime
{
	typedef TIdCustomTime inherited;
	
__published:
	__property System::TDateTime BaseDate = {read=FBaseDate, write=FBaseDate};
	__property int Timeout = {read=FTimeout, write=FTimeout, default=2500};
	__property Port = {default=37};
public:
	/* TIdCustomTime.Create */ inline __fastcall TIdTime(System::Classes::TComponent* AOwner)/* overload */ : TIdCustomTime(AOwner) { }
	
public:
	/* TIdTCPConnection.Destroy */ inline __fastcall virtual ~TIdTime() { }
	
public:
	/* TIdInitializerComponent.Create */ inline __fastcall TIdTime()/* overload */ : TIdCustomTime() { }
	
};


//-- var, const, procedure ---------------------------------------------------
static const System::Word TIME_TIMEOUT = System::Word(0x9c4);
}	/* namespace Idtime */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_IDTIME)
using namespace Idtime;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// IdtimeHPP
