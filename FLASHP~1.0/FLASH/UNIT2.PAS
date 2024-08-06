unit Unit2;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Buttons, NetLabel, ExtCtrls, KvLabel;

type
  TForm2 = class(TForm)
    NetLabel1: TNetLabel;
    Label1: TLabel;
    Label2: TLabel;
    KvLabel1: TKvLabel;
    SpeedButton1: TSpeedButton;
    Panel1: TPanel;
    Image1: TImage;
    Bevel1: TBevel;
    procedure BitBtn1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form2: TForm2;

implementation

{$R *.DFM}

procedure TForm2.BitBtn1Click(Sender: TObject);
begin
 form2.Close;
end;

procedure TForm2.FormCreate(Sender: TObject);
begin
 form2.Left:=round((screen.Width-form2.Width)/2);
 form2.top:=round((screen.Height-form2.Height)/2);
end;

procedure TForm2.SpeedButton1Click(Sender: TObject);
begin
 form2.Close;
end;

end.
 