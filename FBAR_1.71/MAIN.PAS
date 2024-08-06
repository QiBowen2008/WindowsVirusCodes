unit main;

interface

uses
  WinTypes, WinProcs, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  Barcode, ExtCtrls, StdCtrls;

type
  TForm1 = class(TForm)
	 Barcode1: TBarcode;
	 Image1: TImage;
    Panel1: TPanel;
    Label1: TLabel;
    Label2: TLabel;
    Edit1: TEdit;
    Label3: TLabel;
    ComboBox1: TComboBox;
    Edit2: TEdit;
    Label4: TLabel;
	 procedure FormCreate(Sender: TObject);
    procedure Label2Click(Sender: TObject);
    procedure Edit1Change(Sender: TObject);
    procedure ComboBox1Click(Sender: TObject);
    procedure Edit2Change(Sender: TObject);
    procedure Image1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
  private
	 { Private-Deklarationen }
  public
	 { Public-Deklarationen }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.FormCreate(Sender: TObject);
begin
	{ TCanvas }
{
	Barcode1.Top := 50;
	Barcode1.Left := 30;
	Barcode1.DrawBarcode(Image1.Canvas);

	Barcode1.Top := 200;
	Barcode1.Left := 230;
	Barcode1.Angle := 70;
	Barcode1.Typ := bcCode_2_5_interleaved;

	Barcode1.DrawBarcode(Image1.Canvas);
	Barcode1.DrawText(Image1.Canvas);
}


end;

procedure TForm1.Label2Click(Sender: TObject);
begin
	WinExec('start "mailto:shmia@bizerba.de?subject=Barcode component"',
		SW_SHOW);
end;

procedure TForm1.Edit1Change(Sender: TObject);
begin
  Image1.Picture := nil;
  Barcode1.Text := Edit1.Text;
	Barcode1.DrawBarcode(Image1.Canvas);
end;

procedure TForm1.ComboBox1Click(Sender: TObject);
begin
  Barcode1.Typ := TBarcodeType(ComboBox1.ItemIndex);
  Edit1Change(Sender);
end;

procedure TForm1.Edit2Change(Sender: TObject);
begin
	try
		Barcode1.Angle := StrToFloat(Edit2.Text);
	except
		Barcode1.Angle := 0.0;
	end;
	Edit1Change(Sender);
end;

procedure TForm1.Image1MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
	Barcode1.Top  := Y;
	Barcode1.Left := X;
	Edit1Change(Sender);
end;

end.
