//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ScktComp.hpp>
#include <ComCtrls.hpp>
//------------------------------------------------------------------------
#include "GSocket.h"
#define szReplyLength 32
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *bbtCall;
	TComboBox *cbServer;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TComboBox *cbSex;
	TLabel *Label4;
	TMemo *mmWord;
	TLabel *Label5;
	TLabel *Label6;
	TComboBox *cbType;
	TMemo *mmStatus;
	TComboBox *cbCallNum;
	TComboBox *cbName;
	TLabel *Label7;
	TComboBox *cbServerCode;
	TBitBtn *BitBtn1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall bbtCallClick(TObject *Sender);
	void __fastcall edCallNumKeyPress(TObject *Sender, char &Key);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall cbServerClick(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
	GSocket *csk1;
	TStringList *slServerAll;
	TStringList *slServerAdd;
	char szServerCode[32];
	char szHead[1024];
	char szMsg[1024];
	char szReply[szReplyLength];
	Graphics::TBitmap *BkPic;
	bool __fastcall SetParams();
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
