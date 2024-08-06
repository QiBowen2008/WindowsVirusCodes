unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  Registry, StdCtrls;
const
  StrReg='.Default\RemoteAccess\Addresses';
  OpenKeyErr='Error Open The Key!';
  Error1='The system is out of memory or resources.';
  Error2='The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).';
  Error3='The specified file was not found.';
  Error4='The specified path was not found.';
  RunDial='rundll32.exe rnaui.dll,RnaDial ';
  NoConnects='No Connects!';
type
  TForm1 = class(TForm)
    ComboBox1: TComboBox;
    Button1: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}
function FindConnects:TStringList;
var
  MyReg:TRegistry;
  list:TStringList;
begin
  List:=TStringlist.Create;
  MyReg:=TRegistry.Create;
  MyReg.RootKey:=HKEY_USERS;
  if  MyReg.OpenKey(StrReg,False) then begin
    MyReg.GetValueNames(list);
    if list.count=0 then begin
     Result:=nil
    end else Result:=List;
  end else Result:=nil;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  list:TStringList;
  i:integer;
begin
  //List:=TStringList.Create;
  List:=FindConnects;
  if list=nil then begin
    showmessage(NoConnects);
  end else begin;
    for i:= 0 to List.Count-1 do  ComboBox1.Items.Add(List.Strings[i]);
    ComboBox1.ItemIndex:=0;
  end;
end;

procedure TForm1.Button1Click(Sender: TObject);
//var k:integer;
begin
  if ComboBox1.Items.count=0 then begin
    showmessage(NoConnects);
    exit;
  end;
  //k:=
  Winexec(PChar(RunDial+ComboBox1.Items.Strings[ComboBox1.ItemIndex]),9);
 { showmessage(inttostr(k));
  case k of
    0: showmessage(Error1);
    ERROR_BAD_FORMAT:showmessage(Error2);
    ERROR_FILE_NOT_FOUND:showmessage(Error2);
    ERROR_PATH_NOT_FOUND:showmessage(Error2);
  end; }
end;

end.
