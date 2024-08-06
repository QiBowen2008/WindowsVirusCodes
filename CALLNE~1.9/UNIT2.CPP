//----------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Unit2.h"
//----------------------------------------------------------------------------
#pragma resource "*.dfm"
//----------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent *Owner)
	: TForm(Owner)
{
}
//----------------------------------------------------------------------------



void __fastcall TForm2::FormCreate(TObject *Sender)
{
try
{
Memo1->Lines->LoadFromFile("หตร๗.txt");
Memo1->SelStart =0;
}
catch(...)
{
}
try
{
BkPic=new Graphics::TBitmap;
BkPic->LoadFromFile("Bk.bmp");
}
catch(...)
{
delete BkPic;
BkPic=NULL;
}

}
//---------------------------------------------------------------------------


void __fastcall TForm2::FormPaint(TObject *Sender)
{
TRect rect(0,0,Width,Height);
if(BkPic)Canvas->StretchDraw(rect,BkPic);
}
//---------------------------------------------------------------------------

