program emFinder;

uses
  Forms,
  FEmfinder in 'FEmfinder.pas' {fmURL};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'Email Finder';
  Application.CreateForm(TfmURL, fmURL);
  Application.Run;
end.
