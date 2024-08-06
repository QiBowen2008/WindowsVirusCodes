//----------------------------------------------------------------------------
#ifndef Unit2H
#define Unit2H
//----------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <Messages.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
//----------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:
	TButton *Button1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label4;
	TMemo *Memo1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
private:
Graphics::TBitmap *BkPic;
public:
	virtual __fastcall TForm2(TComponent *Owner);
};
//----------------------------------------------------------------------------
#endif
