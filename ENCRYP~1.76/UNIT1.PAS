unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,EncryptIt,
  StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    EdtEncrKey: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    EdtEncrInfo: TEdit;
    Label3: TLabel;
    EdtEncrResu: TEdit;
    Label4: TLabel;
    EdtDencrKey: TEdit;
    Label5: TLabel;
    edtDencrInfo: TEdit;
    Label6: TLabel;
    EdtDencrResu: TEdit;
    Button2: TButton;
    Label7: TLabel;
    Memo1: TMemo;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure EdtEncrKeyChange(Sender: TObject);
    procedure EdtEncrResuChange(Sender: TObject);
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
begin
edtEncrResu.text:=encryptit.Encrypt(edtencrinfo.Text,strtoint(edtEncrkey.text));
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
edtDencrResu.text:=encryptIt.Decrypt(edtDencrinfo.Text,strtoint(edtDencrKey.text));
end;

procedure TForm1.EdtEncrKeyChange(Sender: TObject);
begin
edtDencrKey.text:=edtEncrkey.Text;
end;

procedure TForm1.EdtEncrResuChange(Sender: TObject);
begin
edtDencrInfo.text:=edtEncrResu.Text;
end;

end.
