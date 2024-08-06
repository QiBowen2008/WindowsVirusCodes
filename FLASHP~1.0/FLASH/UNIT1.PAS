unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  OleCtrls, ShockwaveFlashObjects_TLB, StdCtrls, Buttons, Menus,inifiles;

type
  TForm1 = class(TForm)
    ShockwaveFlash1: TShockwaveFlash;
    OpenDialog1: TOpenDialog;
    MainMenu1: TMainMenu;
    F1: TMenuItem;
    O1: TMenuItem;
    N1: TMenuItem;
    X1: TMenuItem;
    O2: TMenuItem;
    P1: TMenuItem;
    U1: TMenuItem;
    S1: TMenuItem;
    N2: TMenuItem;
    H1: TMenuItem;
    H2: TMenuItem;
    A1: TMenuItem;
    C1: TMenuItem;
    procedure O1Click(Sender: TObject);
    procedure X1Click(Sender: TObject);
    procedure P1Click(Sender: TObject);
    procedure U1Click(Sender: TObject);
    procedure S1Click(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure H1Click(Sender: TObject);
    procedure A1Click(Sender: TObject);
  private
    procedure MyClick(sender: tobject);
    function getfilePath: string;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  exit:boolean;
  myini:tinifile;
  sum:string;
  newitem:tmenuitem;

implementation

{$R *.DFM}
uses unit2;

function tform1.getfilePath:string;
var
 PathName:string;
begin
 PathName:=extractfilepath(application.ExeName);
 getfilePath:=pathname;
end;

procedure tform1.MyClick(sender: tobject);
var
 name:string;
 len:integer;
begin
 name:=tmenuitem(sender).caption;
 len:=length(name);
 delete(name,1,1);
 p1.Enabled:=false;
 u1.Enabled:=true;
 s1.Enabled:=true;
 h1.Enabled:=true;
 ShockwaveFlash1.Movie:=name;
end;

procedure TForm1.O1Click(Sender: TObject);
var
 j,menusum,filename:string;
 i:integer;
 fal:boolean;
begin
 fal:=false;
 OpenDialog1.Execute();

 if OpenDialog1.FileName<>'' then
 begin
  if sum<>'' then
  begin
  for i:=0 to strtoint(sum)-1 do
  begin
   filename:=myini.ReadString('AddMenu',inttostr(i),'');
   if opendialog1.FileName=filename then fal:=true;
  end;
  end;

  j:=myini.ReadString('Count','Sum1','');

  if sum='' then sum:='0';

  if sum<>'5' then
   begin
   if fal=false then
   begin
    menusum:=sum;
    myini.WriteString('AddMenu',sum,opendialog1.filename);
    sum:=inttostr(strtoint(sum)+1);
    myini.WriteString('Count','Sum',sum);
    newitem:=tmenuitem.Create(c1);
    try
     newitem.Caption:=opendialog1.FileName;
     c1.Insert(strtoint(menusum),newitem);
     newitem.OnClick:=myclick;
    except
     newitem.Free;
     raise;
    end;
   end;
   end

   else
    begin

     for i:=0 to strtoint(sum)-1 do
     begin
      filename:=myini.ReadString('AddMenu',inttostr(i),'');
      if opendialog1.FileName=filename then fal:=true;
     end;

     if fal=false then
     begin
     if j='4' then j:='0';
     menusum:=j;
     myini.WriteString('AddMenu',j,opendialog1.filename);
     j:=inttostr(strtoint(j)+1);
     myini.WriteString('Count','Sum1',j);
     newitem:=tmenuitem.Create(c1);
     try
      newitem.Caption:=opendialog1.FileName;
      c1.Delete(strtoint(menusum));
      c1.Insert(strtoint(menusum),newitem);
      newitem.OnClick:=myclick;
     except
      newitem.Free;
      raise;
     end;
     end;

    end;
    ShockwaveFlash1.Movie:=OpenDialog1.FileName;
    u1.Enabled:=true;
    s1.Enabled:=true;
    h1.Enabled:=true;
 end;
end;

procedure TForm1.X1Click(Sender: TObject);
begin
 close;
end;

procedure TForm1.P1Click(Sender: TObject);
begin
 ShockwaveFlash1.Play;
 p1.Enabled:=false;
 u1.Enabled:=true;
 s1.Enabled:=true;
 h1.Enabled:=true;
end;

procedure TForm1.U1Click(Sender: TObject);
begin
  ShockwaveFlash1.StopPlay;
  p1.Enabled:=true;
  u1.Enabled:=false;
  s1.Enabled:=false;
  h1.Enabled:=false;
end;

procedure TForm1.S1Click(Sender: TObject);
begin
 ShockwaveFlash1.Stop;
 p1.Enabled:=true;
 u1.Enabled:=false;
 s1.Enabled:=false;
 h1.Enabled:=false;
end;

procedure TForm1.FormResize(Sender: TObject);
begin
 shockwaveflash1.Width:=form1.Width-8;
 shockwaveflash1.Height:=form1.Height-46;
 shockwaveflash1.Hide;
 shockwaveflash1.Show;
 //shockwaveflash1.SetFocus;
 if exit=false then
    shockwaveflash1.setfocus;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
s:string;
menuname:string;
i:integer;
begin
 form1.left:=round((screen.Width-form1.Width)/2);
 form1.Top:=round((screen.height-form1.height)/2);
 exit:=false;
 s:=getfilepath+'FlashPlay.ini';
 //if FileExists(s) then
  //begin
   myini:=tinifile.Create(s);
   sum:=myini.ReadString('Count','Sum','');
  //end;
 if sum<>'' then
  begin
   for i:=0 to strtoint(sum)-1 do
    begin
     menuname:=myini.ReadString('AddMenu',inttostr(i),'');
     newitem:=tmenuitem.Create(c1);
    try
     newitem.Caption:=menuname;
     c1.Insert(i,newitem);
     newitem.OnClick:=myclick;
    except
     newitem.Free;
     raise;
    end;
    end;
 end;
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
 exit:=true;
end;

procedure TForm1.H1Click(Sender: TObject);
begin
 if h1.Checked then
  begin
  shockwaveflash1.loop:=false;
  h1.Checked:=false;
  end
 else
  begin
  shockwaveflash1.Loop:=true;
  h1.Checked:=true;
  end;
end;

procedure TForm1.A1Click(Sender: TObject);
begin
 form2.showmodal;
end;

end.





