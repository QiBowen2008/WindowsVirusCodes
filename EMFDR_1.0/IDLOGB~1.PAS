unit IdLogBase;

interface

uses
  Classes,
  IdIntercept,
  IdSocketHandle;

const
  ID_LOGBASE_Active = False;
  ID_LOGBASE_LogTime = True;

type
  TIdLogBase = class(TIdConnectionIntercept)
  protected
    FActive: Boolean;
    FLogTime: Boolean;

    procedure Log(AText: string); virtual; abstract;
    procedure SetActive(const AValue: Boolean); virtual;
  public
    procedure Connect(ABinding: TIdSocketHandle); override;
    constructor Create(AOwner: TComponent); override;
    procedure DataReceived(var ABuffer; const AByteCount: integer); override;
    procedure DataSent(var ABuffer; const AByteCount: integer); override;
    procedure Disconnect; override;
    procedure DoLog(AText: string); virtual;
  published
    property Active: Boolean read FActive write SetActive default
      ID_LOGBASE_Active;
    property LogTime: Boolean read FLogTime write FLogTime default
      ID_LOGBASE_LogTime;
  end;

implementation

uses
  IdGlobal,
  SysUtils;

procedure TIdLogBase.Connect(ABinding: TIdSocketHandle);
begin
  inherited;
  DoLog('Connected.');
end;

constructor TIdLogBase.Create(AOwner: TComponent);
begin
  inherited;
  FActive := ID_LOGBASE_Active;
  FLogTime := ID_LOGBASE_LogTime;
end;

procedure TIdLogBase.DataReceived(var ABuffer; const AByteCount: integer);
var
  s: string;
begin
  inherited;
  SetString(s, PChar(@ABuffer), AByteCount);
  DoLog('Recv: ' + s);
end;

procedure TIdLogBase.DataSent(var ABuffer; const AByteCount: integer);
var
  s: string;
begin
  inherited;
  SetString(s, PChar(@ABuffer), AByteCount);
  DoLog('Sent: ' + s);
end;

procedure TIdLogBase.Disconnect;
begin
  DoLog('Disconnected.');
  inherited;
end;

procedure TIdLogBase.DoLog(AText: string);
begin
  if Active then
  begin
    if LogTime then
    begin
      AText := DateTimeToStr(Now) + ': ' + AText;
    end;
    Log(StringReplace(AText, EOL, '<EOL>', [rfReplaceAll]));
  end;
end;

procedure TIdLogBase.SetActive(const AValue: Boolean);
begin
  FActive := AValue;
end;

end.
