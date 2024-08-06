//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"
#include "Unit2.h"
#define hpHomePage "欢迎访问:LoveBcb.yeah.net"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
csk1=new GSocket();
slServerAll=new TStringList;
slServerAdd=new TStringList;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
AnsiString TempString;
int ServerAddStart;
try
{
slServerAll->LoadFromFile("服务器.dat");
for(int tempi=0;tempi<slServerAll->Count;tempi++)
{
TempString=slServerAll->Strings[tempi];
if(TempString.SubString(1,1)==AnsiString("#"))
	{
		ServerAddStart=TempString.Pos("$");
		cbServer->Items->Add(TempString.SubString(2,ServerAddStart-2));
		slServerAdd->Add(TempString.SubString(ServerAddStart+1,TempString.Length()-ServerAddStart));
	}
}
cbServer->ItemIndex =0;
cbServerClick(NULL);
}
catch(...)
{
ShowMessage("找不到服务器列表:服务器.dat");
}
try
{
	cbName->Items->LoadFromFile("姓氏.dat");
	cbName->ItemIndex=0;
}
catch(...)
{
//
}
try
{
	cbCallNum->Items->LoadFromFile("号码.dat");
	cbCallNum->ItemIndex=0;
}
catch(...)
{
}
try
{
cbSex->Items->LoadFromFile("称谓.dat");
}
catch(...)
{
 cbSex->Items->Add("先生");
 cbSex->Items->Add("小姐");
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
cbCallNum->Items->Add("1210803");
cbSex->ItemIndex=0;
cbType->ItemIndex =0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::bbtCallClick(TObject *Sender)
{
//
if(!SetParams())return;
int HeadSize=strlen(szHead);
int	MsgSize=strlen(szMsg);
mmStatus->Clear();
mmStatus->Lines->Add(AnsiString("呼叫:")+cbServer->Text+":"+cbServerCode->Text+":"+cbCallNum->Text);
mmStatus->Lines->Add("正在连接...");
csk1->Close();
if(csk1->Create()==0){mmStatus->Lines->Add("无法初始化连接");return;}
if(csk1->Connect(slServerAdd->Strings[cbServer->ItemIndex].c_str(),80)==GSOCKET_ERROR){mmStatus->Lines->Add("无法连接");return;}
mmStatus->Lines->Add("已连接");
mmStatus->Lines->Add("正在发送信息...");
if(csk1->Send(szHead,HeadSize,5)!=HeadSize)mmStatus->Lines->Add("未发送完整的消息");
if(csk1->Send(szMsg,MsgSize,5)!=MsgSize)mmStatus->Lines->Add("未发送完整的消息");
mmStatus->Lines->Add("等待服务器响应...");
ZeroMemory(szReply,sizeof(szReply));
if(csk1->Receive(szReply,szReplyLength-1,10)<=0){mmStatus->Lines->Add("无法得到服务器的回应");return;}
mmStatus->Lines->Add(AnsiString("回应:")+szReply);
mmStatus->Lines->Add(hpHomePage);
csk1->Close();
}
//---------------------------------------------------------------------------

bool __fastcall TForm1::SetParams()
{
	int MsgSize,itServer,itServerCode,itCodeEnd;
	AnsiString asServerCode;
	char Temp[10];
	if((cbCallNum->Text.Length()==0)||(cbName->Text.Length()==0)||(cbSex->Text.Length()==0)||(mmWord->Text.Length()==0))
		{
			ShowMessage("请填满所有内容");
			return false;
		}
	ZeroMemory(szMsg,sizeof(szMsg));
	ZeroMemory(szHead,sizeof(szHead));	ZeroMemory(Temp,sizeof(Temp));	strcpy(szMsg,"page_no=");	strcat(szMsg,cbCallNum->Text.c_str());	strcat(szMsg,"&stn_id=");//191&pager_type=");	//	itServer=slServerAll->IndexOf("#"+cbServer->Items->Strings[cbServer->ItemIndex]+"$"+slServerAdd->Strings[cbServer->ItemIndex]);	itServerCode=itServer+cbServerCode->ItemIndex +1;	itCodeEnd=slServerAll->Strings[itServerCode].Pos(" ");	asServerCode=slServerAll->Strings[itServerCode].SubString(1,itCodeEnd-1);;	strcat(szMsg,asServerCode.c_str());	strcat(szMsg,"&pager_type=");	if(cbType->ItemIndex)		strcat(szMsg,"D");//CALLTYPE应该为C或D	else		strcat(szMsg,"C");	strcat(szMsg,"&firstname=");	strcat(szMsg,cbName->Text.c_str());	strcat(szMsg,"&title=");	strcat(szMsg,cbSex->Text.c_str());	strcat(szMsg,"&msg=");	strcat(szMsg,mmWord->Text.c_str());	strcat(szMsg,"\r\n&answer=null&code=XYZ");	strcpy(szHead,"POST /cgi-bin/tips/webpaging HTTP/1.0\r\n");	strcat(szHead,"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*\r\n");	strcat(szHead,"Content-Type: application/x-www-form-rulencoded\r\n");	strcat(szHead,"User-Agent: Mozilla/3.0 (compatible)\r\n");	strcat(szHead,"Host: ");	strcat(szHead,slServerAdd->Strings[cbServer->ItemIndex].c_str());	strcat(szHead,"\r\nContent-Length: ");	MsgSize=strlen(szMsg);	itoa(MsgSize,Temp,10);	strcat(szHead,Temp);	strcat(szHead,"\r\n\r\n");	//	return true;}
void __fastcall TForm1::edCallNumKeyPress(TObject *Sender, char &Key)
{
if(Key<'0'||Key>'9')
Key=0;	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    csk1->Close();
	if(csk1)delete csk1;
	if(slServerAdd)delete slServerAdd;
	if(slServerAll)delete slServerAll; 
}
//---------------------------------------------------------------------------




void __fastcall TForm1::cbServerClick(TObject *Sender)
{
int iServerCode,
	iServerStart;
AnsiString asTempString;
cbServerCode->Clear();
iServerCode=slServerAll->IndexOf("#"+cbServer->Items->Strings[cbServer->ItemIndex]+"$"+slServerAdd->Strings[cbServer->ItemIndex]);
asTempString=slServerAll->Strings[++iServerCode];
while(asTempString!="$$$$")
{
	iServerStart=asTempString.Pos(" ");
	cbServerCode->Items->Add(asTempString.SubString(iServerStart+1,asTempString.Length()-iServerStart));
	asTempString=slServerAll->Strings[++iServerCode];
}
cbServerCode->ItemIndex=0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormPaint(TObject *Sender)
{
TRect rect(0,0,Width,Height);
if(BkPic)this->Canvas->StretchDraw(rect,BkPic);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BitBtn1Click(TObject *Sender)
{
TForm2*Form2=new TForm2(NULL);
Form2->ShowModal();
delete Form2;
}
//---------------------------------------------------------------------------

