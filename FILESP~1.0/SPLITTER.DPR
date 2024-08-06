program Splitter;

uses
  Forms,
  SplitFrm in 'SplitFrm.pas' {SplitterForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TSplitterForm, SplitterForm);

  if ParamCount>0 then
   begin
     if ParamStr(1)='/S' then
        begin
           SplitterForm.SourceFileEdit.Text:= ParamStr(2);
           SplitterForm.ResultFileEdit.Text:= ParamStr(2);
           SplitterForm.SplitterBtnClick(SplitterForm.SplitterBtn);
           Application.Terminate;
        end
      else
        begin
           SplitterForm.SourceFileEdit.Text:= ParamStr(1);
           Application.Run;
        end;
   end
   else
     Application.Run;

end.
