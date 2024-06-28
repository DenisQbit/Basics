//---------------------------------------------------------------------------
// C++Builder Sample Components
// Copyright(c) 1998-2011 Embarcadero Technologies, Inc.
//---------------------------------------------------------------------------
#ifndef CGaugesH
#define CGaugesH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Consts.hpp>
#include <math.h>
#include <memory>       //for auto_ptr STL class

//-- type declarations -------------------------------------------------------
enum TCGaugeKind { gkText, gkHorizontalBar, gkVerticalBar, gkPie, gkNeedle };

class PACKAGE TCGauge : public TGraphicControl
{
private:
	Longint FMinValue;
	Longint FMaxValue;
	Longint FCurValue;
	TCGaugeKind FKind;
	Boolean FShowText;
	TFormBorderStyle FBorderStyle;
	TColor FForeColor;
	TColor FBackColor;
	void __fastcall PaintBackground(Graphics::TBitmap *AnImage);
	void __fastcall PaintAsText(Graphics::TBitmap *AnImage, const TRect &PaintRect);
	void __fastcall PaintAsNothing(Graphics::TBitmap *AnImage, const TRect &PaintRect);
	void __fastcall PaintAsBar(Graphics::TBitmap *AnImage, const TRect &PaintRect);
	void __fastcall PaintAsPie(Graphics::TBitmap *AnImage, const TRect &PaintRect);
	void __fastcall PaintAsNeedle(Graphics::TBitmap *AnImage, const TRect &PaintRect);
	void __fastcall SeTCGaugeKind(TCGaugeKind Value);
	void __fastcall SetShowText(Boolean Value);
	void __fastcall SetBorderStyle(TBorderStyle Value);
	void __fastcall SetForeColor(TColor Value);
	void __fastcall SetBackColor(TColor Value);
	void __fastcall SetMinValue(Longint Value);
	void __fastcall SetMaxValue(Longint Value);
	void __fastcall SetProgress(Longint Value);
	Longint __fastcall GetPercentDone(void);
	
protected:
	virtual void __fastcall Paint(void);
	
public:
	__fastcall virtual TCGauge(TComponent *AOwner);
	void __fastcall AddProgress(Longint Value);
	__property Longint PercentDone = {read=GetPercentDone, nodefault};
	
__published:
	__property Align ;
	__property Anchors ;
	__property Color ;
	__property Constraints ;
	__property Enabled ;
	__property TCGaugeKind Kind = {read=FKind, write=SeTCGaugeKind, default=1};
	__property Boolean ShowText = {read=FShowText, write=SetShowText, default=1};
	__property Font ;
	__property TBorderStyle BorderStyle = {read=FBorderStyle, write=SetBorderStyle, default=1};
	__property TColor ForeColor = {read=FForeColor, write=SetForeColor, default=0};
	__property TColor BackColor = {read=FBackColor, write=SetBackColor, default=16777215};
	__property Longint MinValue = {read=FMinValue, write=SetMinValue, default=0};
	__property Longint MaxValue = {read=FMaxValue, write=SetMaxValue, default=100};
	__property ParentColor ;
	__property ParentFont ;
	__property ParentShowHint ;
	__property PopupMenu ;
	__property Longint Progress = {read=FCurValue, write=SetProgress, nodefault};
	__property ShowHint ;
	__property Visible ;
};

class TBltBitmap : public Graphics::TBitmap{
public:
    __fastcall virtual TBltBitmap();
    void __fastcall MakeLike(Graphics::TBitmap* ATemplate);
};

//-- var, const, procedure ---------------------------------------------------
#define Pi 3.1415926535897932385
//-- end unit ----------------------------------------------------------------
#endif // CGaugesH