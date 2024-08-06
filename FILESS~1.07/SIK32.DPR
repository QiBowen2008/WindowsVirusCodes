PROGRAM SIK32;

USES
  Forms,
  U_SIK32 in 'U_SIK32.PAS' {frmSynchro};

{$R *.RES}

BEGIN
  Application.Initialize;
  Application.Title := 'Sichern95';
  Application.CreateForm(TfrmSynchro, frmSynchro);
  Application.Run;
END.
