program flister;

uses
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  masksearch in 'masksearch.pas';

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
