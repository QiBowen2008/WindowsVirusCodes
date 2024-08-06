unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.Button1Click(Sender: TObject);
var
  hDesktop : THandle;
begin
  hDesktop := FindWindow('Progman', nil);
  ShowWindow(hDesktop, SW_Restore);
end;

procedure TForm1.Button2Click(Sender: TObject);
var
  hDesktop : THandle;
begin
  hDesktop := FindWindow('Progman', nil);
  ShowWindow(hDesktop, SW_hide);
end;
end.
