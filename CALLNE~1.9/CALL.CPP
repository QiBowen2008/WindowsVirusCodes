//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("Call.res");
USEFORM("Unit1.cpp", Form1);
USEUNIT("GSocket.cpp");
USEFORM("Unit2.cpp", Form2);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
