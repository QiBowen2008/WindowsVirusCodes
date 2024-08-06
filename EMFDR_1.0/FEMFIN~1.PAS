unit FEmfinder;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Buttons, ExtCtrls, spider, ComCtrls, Spin;

type
  TfmURL = class(TForm)
    spdEmail: TSpider;
    pnSetup: TPanel;
    lbKey: TLabel;
    editKey: TEdit;
    StatusBar1: TStatusBar;
    MemoURL: TMemo;
    btnTran: TBitBtn;
    pnCommand: TPanel;
    btnRun: TBitBtn;
    btnStop: TBitBtn;
    GroupBox2: TGroupBox;
    Label6: TLabel;
    Label7: TLabel;
    edProxyServer: TEdit;
    edProxyPort: TEdit;
    GroupBox1: TGroupBox;
    lbLayer: TLabel;
    speLayer: TSpinEdit;
    lbThread: TLabel;
    speThread: TSpinEdit;
    RadioGroup1: TRadioGroup;
    rdb56K: TRadioButton;
    rdb2M: TRadioButton;
    rdb10M: TRadioButton;
    procedure btnRunClick(Sender: TObject);
    procedure btnStopClick(Sender: TObject);
    procedure spdEmailAfterConnect(ThreadNum, Layer: Integer; URL: TURL);
    procedure spdEmailBeforeConnect(ThreadNum, Layer: Integer; URL: TURL);
    procedure spdEmailEmailFound(ThreadNum, Layer: Integer;
      const EmailAddress: String);
    procedure spdEmailError(ThreadNum: Integer;
      const ErrorMessage: String);
    procedure spdEmailFailConnect(ThreadNum, Layer: Integer; URL: TURL);
    procedure spdEmailGetNextUrlSeed(ThreadNum: Integer;
      var UrlToSearch: String);
    procedure spdEmailLinkFound(ThreadNum, Layer: Integer; URL: TURL;
      var searchThisLink: Boolean);
    procedure spdEmailPageLoaded(ThreadNum, Layer: Integer; URL: TURL;
      const Header, HTMLBody, TextBody: String);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure editKeyKeyPress(Sender: TObject; var Key: Char);
    procedure btnTranClick(Sender: TObject);
  private
    { Private declarations }
    LinkCount, EmailCount: Integer;
    ScanedLinks, FoundEmails: TStringList;
  public
    { Public declarations }
  end;

var
  fmURL: TfmURL;
const
  FOUND_EMAILS_FILE = 'FoundEmails.log';

implementation

{$R *.DFM}

function SearchYahooEn(const Keywords: string): string;
const
  base : string = 'http://search.yahoo.com/bin/search';
var
  p, d, g, s, w, n : string;
  foo : string;
  x : integer;
begin
  { copy keys into buffer }
  foo := Keywords;
  { Check for the existance of a space }
  x := AnsiPos(' ', foo);
  { if space exists, convert to plus }
  while x > 0 do begin
    Delete(foo, x, 1);
    Insert('+', foo, x);
    // ShowMessage(foo);
    x := AnsiPos(' ', foo);
  end;

  { add key terms }
  p := '?p=' + foo;

  { add 'd' paramater }
  d := '&d=y';

  { add 'g' paramater }
  g := '&g=0';

  { add 's' paramater }
  s := '&s=a';

  w := '&w=w';

  n := '&n=100';

  { build string }
{  action := base + p + d + g + s + w + n;  }
  SearchYahooEn:=  base + p + d + g + s + w + n;
end;

function SearchYahooCn(const Keywords: string): string;
const
  base : string = 'http://cn.search.yahoo.com/search/gb';
var
  p, d, g, s, w, n : string;
  foo : string;
  x : integer;
begin
  { copy keys into buffer }
  foo := Keywords;
  { Check for the existance of a space }
  x := AnsiPos(' ', foo);
  { if space exists, convert to plus }
  while x > 0 do begin
    Delete(foo, x, 1);
    Insert('+', foo, x);
    // ShowMessage(foo);
    x := AnsiPos(' ', foo);
  end;

  { add key terms }
  p := '?p=' + foo;

  { add 'd' paramater }
  d := '&d=y';

  { add 'g' paramater }
  g := '&g=0';

  { add 's' paramater }
  s := '&s=a';

  w := '&w=w';

  n := '&n=100';

  { build string }
{  action := base + p + d + g + s + w + n;  }
  SearchYahooCn:=  base + p + d + g + s + w + n;
end;

procedure TfmURL.btnRunClick(Sender: TObject);
begin
  LinkCount := 0;
  EmailCount := 0;
  ScanedLinks.Clear;
  spdEmail.MaxLayers := speLayer.Value;
  spdEmail.ThreadsToUse := speThread.Value;
  spdEmail.ProxyServer := edProxyServer.Text;
  spdEmail.ProxyPort := StrToIntDef(edProxyPort.Text, 80);
  if rdb56K.Checked then spdEmail.RecvBufferSize := 1024;
  if rdb2M.Checked then spdEmail.RecvBufferSize := 8192;
  if rdb10M.Checked then spdEmail.RecvBufferSize := 32768;
  spdEmail.Run;
  btnRun.Enabled := False;
  btnStop.Enabled := True;
  StatusBar1.Panels[0].Text := 'Link Scaned=0';
  StatusBar1.Panels[1].Text := 'Email Found=0';
  StatusBar1.Panels[2].Text := 'Run Clicked';
end;

procedure TfmURL.btnStopClick(Sender: TObject);
begin
  spdEmail.Stop;
  FoundEmails.SaveToFile(FOUND_EMAILS_FILE);
  btnRun.Enabled := True;
  btnStop.Enabled := False;
  StatusBar1.Panels[2].Text := 'Stop Clicked';
end;

procedure TfmURL.spdEmailAfterConnect(ThreadNum, Layer: Integer;
  URL: TURL);
begin
  StatusBar1.Panels[2].Text := 'Connected: ' + URL.Server + URL.Domain;
end;

procedure TfmURL.spdEmailBeforeConnect(ThreadNum, Layer: Integer;
  URL: TURL);
begin
  StatusBar1.Panels[2].Text := 'Connect To: ' + URL.Server + URL.Domain;
end;

procedure TfmURL.spdEmailEmailFound(ThreadNum, Layer: Integer;
  const EmailAddress: String);
var
  Index: Integer;
  EmailExisted: Boolean;
begin
  EmailExisted := FoundEmails.Find(EmailAddress, Index);
  if not EmailExisted then
  begin
    FoundEmails.Add(EmailAddress);
    EmailCount := EmailCount + 1;
    if EmailCount mod 5 = 0 then FoundEmails.SaveToFile(FOUND_EMAILS_FILE);
    StatusBar1.Panels[1].Text := 'Email Found=' + IntToStr(EmailCount);
  end;
  StatusBar1.Panels[2].Text := 'Email Address Found: ' + EmailAddress;
end;

procedure TfmURL.spdEmailError(ThreadNum: Integer;
  const ErrorMessage: String);
begin
  StatusBar1.Panels[2].Text := 'Error Occured: ' + ErrorMessage;
end;

procedure TfmURL.spdEmailFailConnect(ThreadNum, Layer: Integer; URL: TURL);
begin
  StatusBar1.Panels[2].Text := 'Connection Failed: ' + URL.Server + URL.Domain;
end;

procedure TfmURL.spdEmailGetNextUrlSeed(ThreadNum: Integer;
  var UrlToSearch: String);
begin
  if memoURL.Text <> '' then
  begin
    UrlToSearch := memoURL.Lines[0];
    memoURL.Lines.Delete(0);
    StatusBar1.Panels[2].Text := 'Url Seeded: ' + UrlToSearch;
  end
  else
  begin
    UrlToSearch := '';
    if PublicThreadCount = 1 then
    begin
      FoundEmails.SaveToFile(FOUND_EMAILS_FILE);
      btnRun.Enabled := True;
      btnStop.Enabled := False;
      StatusBar1.Panels[2].Text := 'All Done';
    end
    else
    begin
      StatusBar1.Panels[2].Text := 'Thread Done';
    end;
  end;
end;

procedure TfmURL.spdEmailLinkFound(ThreadNum, Layer: Integer; URL: TURL;
  var searchThisLink: Boolean);
var
  Index: Integer;
  LinkExisted: Boolean;
  TmpStr: string;
begin
  TmpStr := URL.Proto + '://' + URL.Server + URL.Domain + URL.Path + URL.Filename;
  LinkExisted := ScanedLinks.Find(TmpStr, Index);
  if not LinkExisted then
  begin
    ScanedLinks.Add(TmpStr);
    LinkCount := LinkCount + 1;
    StatusBar1.Panels[0].Text := 'Link Scaned=' + IntToStr(LinkCount);
  end;
  if (LinkExisted) or (Layer = spdEmail.MaxLayers) then searchThisLink := False else searchThisLink := True;
  StatusBar1.Panels[2].Text := 'Link Found: ' + TmpStr;
end;

procedure TfmURL.spdEmailPageLoaded(ThreadNum, Layer: Integer; URL: TURL;
  const Header, HTMLBody, TextBody: String);
begin
  StatusBar1.Panels[2].Text := 'Page Loaded: ' + URL.Proto + '://' + URL.Server + URL.Domain + URL.Path + URL.Filename;
end;

procedure TfmURL.FormCreate(Sender: TObject);
begin
  ScanedLinks := TStringList.Create;
  ScanedLinks.Sorted := True;
  FoundEmails := TStringList.Create;
  FoundEmails.Sorted := True;
  if FileExists(FOUND_EMAILS_FILE) then FoundEmails.LoadFromFile(FOUND_EMAILS_FILE);
  StatusBar1.Panels[0].Text := 'Link Scaned=0';
  StatusBar1.Panels[1].Text := 'Email Found=0';
  StatusBar1.Panels[2].Text := 'Ready';
end;

procedure TfmURL.FormDestroy(Sender: TObject);
begin
  ScanedLinks.Free;
  FoundEmails.Free;
end;

procedure TfmURL.editKeyKeyPress(Sender: TObject; var Key: Char);
begin
  if Key = #13 then btnTran.Click;
end;

procedure TfmURL.btnTranClick(Sender: TObject);
var
  TmpStr: string;
begin
  TmpStr := Trim(editKey.Text);
  if TmpStr = '' then exit;
  memoURL.Lines.Add(SearchYahooEn(TmpStr));
  memoURL.Lines.Add(SearchYahooCn(TmpStr));
end;

end.
