program Project1;

uses
  Windows,
  SysUtils,
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  Unit2 in 'Unit2.pas' {Form2};

{$R *.RES}
var
 s:string;

begin
  s:=extractfilepath(application.ExeName)+'FlashPlay.ini';
  if FileExists(s)=false then
  begin
   messagebox(0,'找不到程序运行所需的ini文件!','运行',0);
   halt;
  end; 
  Application.Initialize;
  Application.Title := 'FlashPlayer';
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.Run;
end.
