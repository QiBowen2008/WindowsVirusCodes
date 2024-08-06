unit spider;

interface

uses
  Windows, SysUtils, Classes, dialogs,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP;

const
  Max_LAYERS = 100;

type
  TURL = record
    Proto: string;
    Server: string;
    Domain: string;
    Path: string;
    Filename: string;
  end;
  TOnBeforeConnect = procedure(ThreadNum, Layer: Integer; URL: TUrl) of object;
  TOnFailConnect = procedure(ThreadNum, Layer: Integer; URL: TUrl) of object;
  TOnAfterConnect = procedure(ThreadNum, Layer: Integer; URL: TUrl) of object;
  TOnLinkFound = procedure(ThreadNum, Layer: Integer; URL: TUrl; var searchThisLink: Boolean) of object;
  TOnPageLoaded = procedure(ThreadNum, Layer: Integer; URL: TUrl; const Header: string; const HTMLBody: string; const TextBody: string) of object;
  TOnfoundEmail = procedure(ThreadNum, Layer: integer; const EmailAddress: string) of object;
  TOnGetNextUrlSeed = procedure(ThreadNum: Integer; var UrlToSearch: string) of object;
  TOnError = procedure(ThreadNum: Integer; const ErrorMessage: string) of object;

type
  TSpider = class;
  TSpiderThread = class(TThread)
  private
    TempUrlList: tstringlist;
    LayersURLSToSearch: array[1..max_layers] of tstringlist;
    ThreadNumber: Integer;
    MainSpiderObj: TSpider;
    IdHttp1: TIdHttp;
  protected

  public
    constructor Create(Sender: Tspider; ThreadNum: integer);
    destructor Destroy; override;
    procedure Execute; override;
  published

  end;

  Tspider = class(Tcomponent)
  private
    { Private declarations }
    FProxyServer: string;
    FProxyPort: Integer;
    FRecvBufferSize: Integer;
    FMaxLayers: Integer;
    FThreadsToUse: Integer;
    ThreadList: array[1..100] of TspiderThread;
    FOnBeforeConnect: TOnBeforeConnect;
    FOnAfterConnect: TOnAfterConnect;
    FOnFailConnect: TOnFailConnect;
    FOnLinkFound: TOnLinkfound;
    FOnPageLoaded: TOnPageLoaded;
    FOnEmailfound: TOnfoundEmail;
    FOnGetNextUrlSeed: TOnGetNextUrlSeed;
    FOnError: TOnError;
    function Running: Boolean;
  protected
   { Protected declarations }
  public
    { Public declarations }
    work_CS: TRTLCriticalSection;
    procedure Run;
    constructor Create(Aowner: Tcomponent); override;
    destructor Destroy; override;
    procedure Stop;
  published
    property ProxyServer: string read FProxyServer write FProxyServer;
    property ProxyPort: Integer read FProxyPort write FProxyPort;
    property RecvBufferSize: Integer read FRecvBufferSize write FRecvBufferSize;
    property MaxLayers: integer read fmaxlayers write fmaxlayers;
    property ThreadsToUse: integer read FThreadsToUse write FThreadsToUse;
    property OnAfterConnect: TOnAfterConnect read FOnAfterConnect write FOnAfterConnect;
    property OnBeforeConnect: TOnBeforeConnect read FOnBeforeConnect write FOnBeforeConnect;
    property OnFailConnect: TOnFailConnect read FOnFailConnect write FOnFailConnect;
    property OnLinkFound: TOnLinkfound read FOnLinkFound write FOnLinkFound;
    property OnPageLoaded: TOnPageLoaded read FOnPageLoaded write FOnPageLoaded;
    property OnEmailFound: TOnfoundEmail read FOnEmailFound write FOnEmailFound;
    property OnGetNextUrlSeed: TOnGetNextUrlSeed read FOnGetNextUrlSeed write FOnGetNextUrlSeed;
    property OnError: TOnError read FOnError write FOnError;
    { Published declarations }
  end;

var
  PublicThreadCount: Integer;
  PublicShouldStop: Boolean;

procedure Register;

implementation

function CountF(const S: string; const SubStr: string; Index: Integer): Integer;
var
  TmpStr: string;
  TmpIndex, I: Integer;
begin
  TmpStr := Copy(S, Index, Length(S) - Index + 1);
  I := 0;
  while True do
  begin
    TmpIndex := AnsiPos(SubStr, TmpStr);
    if TmpIndex = 0 then Break;
    Delete(TmpStr, 1, TmpIndex + Length(SubStr) - 1);
    I := I + 1;
  end;
  CountF := I;
end;

function ScanCC(const S: string; const SubStr: string; Count: Integer): Integer;
var
  TmpStr: string;
  TmpIndex, I: Integer;
begin
  TmpStr := S;
  for I := 1 to Count do
  begin
    TmpIndex := AnsiPos(SubStr, TmpStr);
    if TmpIndex = 0 then Break;
    Delete(TmpStr, 1, TmpIndex + Length(SubStr) - 1);
  end;
  ScanCC := Length(S) - Length(TmpStr) - 1;
end;

function ScanB(const S: string; const SubStr: string; Count: Integer): Integer;
begin
  ScanB := ScanCC(S, SubStr, CountF(S, SubStr, 1) - Count) + 1;
end;

function IsNum(const S: string): Boolean;
begin
  if StrToIntDef(S, -999) = -999 then IsNum := False else IsNum := True;
end;

function ParseTag(const S: string; const TagStart: string; const TagEnd: string; var Index: Integer): string;
var
  TmpStr: string;
  IndexStart, IndexEnd: Integer;
begin
  TmpStr := Copy(S, Index, Length(S) - Index + 1);
  IndexStart := AnsiPos(TagStart, TmpStr);
  if IndexStart = 0 then
  begin
    Index := 0;
    ParseTag := '';
    exit;
  end;
  Delete(TmpStr, 1, IndexStart + Length(TagStart) - 1);
  IndexEnd := AnsiPos(TagEnd, TmpStr);
  if IndexEnd = 0 then
  begin
    Index := 0;
    ParseTag := '';
    exit;
  end;
  IndexEnd := IndexStart + Length(TagStart) - 1 + IndexEnd;
  TmpStr := Copy(S, Index - 1 + IndexStart + Length(TagStart), IndexEnd - IndexStart - Length(TagStart));
  Index := Index - 1 + IndexEnd + Length(TagEnd);
  ParseTag := TmpStr;
end;
{
function RemoveControlChar(const S: string): string;
var
  TmpStr: string;
begin
  //remove SPACE, TAB, CR, LF letters
  TmpStr := S;
  TmpStr := StringReplace(TmpStr, #32, '', [rfReplaceAll]);
  TmpStr := StringReplace(TmpStr, #9, '', [rfReplaceAll]);
  TmpStr := StringReplace(TmpStr, #13, '', [rfReplaceAll]);
  TmpStr := StringReplace(TmpStr, #10, '', [rfReplaceAll]);
  RemoveControlChar := TmpStr;
end;
}
function IsIpAddress(const IP: string): Boolean;
begin
  Result := False;
  if (CountF(IP, '.', 1) = 3) and IsNum(IP[1]) and IsNum(Copy(ip, length(ip), 1)) then
    Result := True;
end;

function Parse_Url(const InString: string): TURL;
var
  Tempstr: string;
  TempInt: Integer;
  TempUrl: TUrl;
begin

  // default ret vals.
  TempUrl.proto := '';
  TempUrl.server := '';
  TempUrl.domain := '';
  TempUrl.path := '/';
  TempUrl.filename := '';

  TempStr := Instring;


  if AnsiPos('://', tempstr) > 0 then
  begin
    TempUrl.proto := copy(tempstr, 1, AnsiPos('://', tempstr) - 1);
    tempstr := copy(tempstr, AnsiPos('://', tempstr) + 3, 5000);
  end;

  /// grab host name
  if AnsiPos('/', tempstr) > 0 then
  begin
    TempUrl.Domain := copy(tempstr, 1, AnsiPos('/', tempstr) - 1);
    // grab the balance of the Url line.
    Tempstr := copy(tempstr, AnsiPos('/', tempstr), 5000); // use the balance of the string to gat path and file
  end
  else
    if CountF(tempstr, '.', 1) > 0 then // theres at least one period
    begin
      TempUrl.Domain := tempstr; // it must be a domain
      Tempstr := ''; // no more to do after domain.

    end;

  Tempint := CountF(TempUrl.Domain, '.', 1); // find out how many periods there are..

  if TempInt > 1 then // is there a second period in the domain, then there is a server..
  begin
    if not IsIpAddress(Tempurl.Domain) then
    begin
    // now get the period that separtates server from Domain ..
      if (AnsiPos('.com.', TempUrl.Domain) > 0) or
        (AnsiPos('.edu.', TempUrl.Domain) > 0) or
        (AnsiPos('.gov.', TempUrl.Domain) > 0) or
        (AnsiPos('.net.', TempUrl.Domain) > 0) then
        TempInt := ScanCC(TempUrl.Domain, '.', tempint - 2)
      else
        TempInt := ScanCC(TempUrl.Domain, '.', tempint - 1);

      TempUrl.Server := Copy(TempUrl.Domain, 1, tempint);
      TempUrl.Domain := Copy(TempUrl.Domain, tempint + 1, 5000);
    end;
  end;

  if Length(Tempstr) = 0 then
  begin
    parse_url.proto := TempUrl.proto;
    parse_url.server := TempUrl.server;
    parse_url.domain := TempUrl.Domain;
    parse_url.path := TempUrl.Path;
    parse_url.filename := TempUrl.Filename;
    exit; // no more to do.
  end;

  // scan for '?'
  Tempint := AnsiPos(tempstr, '?');

  if TempInt > 0 then // there are some url variables..
  begin
    TempUrl.filename := copy(tempstr, tempint, 1000); // grab the url vars..
    TempUrl.path := copy(tempstr, 1, tempint - 1);
    Tempstr := TempUrl.path;
  end;


  Tempint := ScanB(Tempstr, '/', 0); // scan backwards for a slash
  if Tempint > 0 then
  begin
    TempUrl.filename := copy(Tempstr, Tempint + 1, 1000) + TempUrl.filename; // add the vars back in..
    TempUrl.path := copy(Tempstr, 1, Tempint);
  end;

  // this might not be nessarry..
  if length(TempUrl.path) = 0 then
    TempUrl.path := '/';

  parse_url.proto := TempUrl.proto;
  parse_url.server := TempUrl.server;
  parse_url.domain := TempUrl.Domain;
  parse_url.path := TempUrl.Path;
  parse_url.filename := TempUrl.Filename;

end;
{
function stripHtml(const HTMLString: string): string;
var
  workstring, token: string;
var
  I: integer;
begin
  SetLength(workstring, 524288);
  workstring := htmlstring;
  I := 1;
  while I > 0 do
  begin
    Token := ParseTag(workstring, '<a', '/a>', I);
    if I > 0 then
    begin
      Delete(workstring, I - (length(token) + 5), length(token) + 5);
      I := 1;
    end;
  end;

  I := 1;
  while I > 0 do
  begin
    Token := ParseTag(workstring, '<', '>', I);
    if I > 0 then
    begin
      Delete(workstring, I - (length(token) + 2), length(token) + 2);
      I := 1;
    end;
  end;

// take off the &nbsp
  while AnsiPos('&nbsp', workstring) > 0 do
  begin
    Delete(workstring, AnsiPos('&nbsp', workstring), 5);
  end;

  result := workstring;
end;
}


constructor Tspider.create(Aowner: Tcomponent);
begin
  inherited create(Aowner);
  FProxyPort := 80;
  FRecvBufferSize := 1024;
  FMaxLayers := 1;
  FThreadsToUse := 1;
  PublicThreadCount := 0;
  InitializeCriticalSection(Work_CS); // initialize my Critical section.
end;

destructor Tspider.Destroy;
begin
  inherited destroy;
end;

procedure Tspider.Run;
var
  x: Integer;
begin
  if running then Stop;
  while(running) do Sleep(500);

  PublicShouldStop := False;
  for x := 1 to FThreadsToUse do
  begin
    ThreadList[x] := TspiderThread.create(self, x);
  end;
end;

procedure Tspider.Stop;
begin
  PublicShouldStop := True;
end;

function Tspider.Running: Boolean;
begin
  if PublicThreadCount > 0 then Running := True else Running := False;
end;


constructor TSpiderThread.Create(Sender: TSpider; ThreadNum: Integer);
begin
  inherited Create(True); // wait to setup
  PublicThreadCount := PublicThreadCount + 1;
  FreeOnTerminate := True;  // automatic free when finished
  Threadnumber := Threadnum;
  Self.MainSpiderObj := Sender;
  IdHttp1 := nil;
  IdHttp1 := TIdHttp.Create(nil);
  IdHttp1.HandleRedirects := True;
  IdHttp1.RecvBufferSize := MainSpiderObj.RecvBufferSize;
  IdHttp1.ProtocolVersion := pv1_0;
  Resume;  // begin to work
end;

destructor TSpiderThread.Destroy;
begin
  if IdHttp1 <> nil then IdHttp1.Free;
  IdHttp1 := nil;
  PublicThreadCount := PublicThreadCount - 1;
  inherited;
end;

procedure TSpiderThread.Execute;
var
  tempstr, workstr: string;
  x, MailPos, which_layer, which_layer_item, I: integer;
  Temp_url: Turl;
  PageHeader, Body: string;
  AddToSearchLater: boolean;
  Textonly: string;
  Url_Info: TURL;
  I2: Integer;
  tempstr2: string;
begin
  SetLength(workstr, 524288);
  for x := 1 to MAX_LAYERS do
    LayersURLSToSearch[x] := tstringlist.create;

  TempUrlList := tstringlist.create;
  with MainSpiderObj do
  try
    try

      while not PublicShouldStop do
      begin

        for x := 1 to MAX_LAYERS do
          LayersURLSToSearch[x].clear;

        TempStr := '';
        // add Seed address ..
        EnterCriticalSection(work_CS);
        try
          if assigned(FOnGetNextUrlSeed) then
            FOnGetNextUrlSeed(Threadnumber, TempStr);
        finally
          LeaveCriticalSection(Work_CS);
        end;

        if Length(Tempstr) <= 0 then // done, no more to do..
        begin
          Exit;
        end;


        LayersURLSToSearch[1].add(AnsiLowerCase(TempStr));

        for Which_layer := 1 to FMaxLayers do
        begin

          if PublicShouldStop then break;

          for which_layer_item := 0 to LayersURLSToSearch[Which_layer].count - 1 do
          begin
            if PublicShouldStop then break;
            Url_Info := Parse_URL(LayersURLSToSearch[Which_layer].strings[which_layer_item]);
            if Length(Url_Info.domain) <= 0 then
              continue;

            try
              // Set the properties for HTTP
              idHttp1.Request.Accept := 'text/html';
              IdHttp1.Request.Username := '';
              IdHttp1.Request.Password := '';
              IdHttp1.Request.ProxyServer := FProxyServer;
              IdHttp1.Request.ProxyPort := FProxyPort;
              IdHttp1.Request.ContentType := '';
              IdHttp1.Intercept := nil;

              EnterCriticalSection(work_CS);
              try
                if assigned(FOnBeforeConnect) then
                  FOnBeforeConnect(Threadnumber, Which_layer, Url_Info);
              finally
                LeaveCriticalsection(work_cs);
              end;
{
              try
                IdHttp1.Head(LayersURLSToSearch[Which_layer].strings[which_layer_item]);
              except
                EnterCriticalSection(work_CS);
                try
                  if assigned(FOnFailConnect) then
                    FOnFailConnect(Threadnumber, Which_layer, Url_Info);
                finally
                  LeaveCriticalsection(work_cs);
                end;
                Continue;
              end;
}
              Body := IdHttp1.Get(LayersURLSToSearch[Which_layer].strings[which_layer_item]);

              EnterCriticalSection(work_CS);
              try
                if assigned(FOnAfterConnect) then
                  FOnAfterConnect(Threadnumber, Which_layer, Url_Info);
              finally
                LeaveCriticalsection(work_cs);
              end;
            except
              on e: exception do
              begin
                EnterCriticalSection(work_CS);
                try
                  if assigned(FOnError) then
                    FOnError(Threadnumber, E.message + LayersURLSToSearch[Which_layer].strings[which_layer_item]);
                finally
                  LeaveCriticalsection(work_cs);
                end;
              end;
            end;

{            I := 1;
            PageHeader := ParseTag(Body, '<head>', '</head>', I);}
            PageHeader := ''; // Replace by me
            Textonly := '';  // Replace by me
            //Textonly := stripHtml(Body);  // I do not need it, but you may use this line
            EnterCriticalSection(work_CS);
            try
              if assigned(FOnPageLoaded) then
                FOnPageLoaded(Threadnumber, Which_Layer, Url_Info, '',
                  '', '');
{                FOnPageLoaded(Threadnumber, Which_Layer, Url_Info, Pageheader,
                  Body, textonly);}
            finally
              LeaveCriticalSection(work_cs);
            end;
          // now look for emails and links...


            workstr := AnsiLowerCase(Body);
            if (Which_layer <= FMaxLayers) then
            begin
              TempUrlList.clear;

              //frames
              I := 1;
              while I > 0 do
              begin
                if PublicShouldStop then break;
                tempstr := ParseTag(workstr, '<frameset', '</frameset>', I);
                if length(tempstr) > 0 then
                begin
                  I2 := 1;
                  while I2 > 0 do
                  begin
                    if PublicShouldStop then break;
                    tempstr2 := ParseTag(tempstr, 'src', '"', I2);
                    if I2 = 0 then Break;
                    if Trim(tempstr2) <> '=' then Continue;
                    tempstr2 := 'src' + tempstr2 + '"';
                    I2 := I2 - Length(tempstr2);
                    tempstr2 := ParseTag(tempstr, tempstr2, '"', I2);
                    // put it into our list only if it's not a gif or jpg
                    if (length(tempstr2) > 0) and not ((AnsiPos('.gif', Tempstr2) > 0) or (AnsiPos('.jpg', Tempstr2) > 0)) then
                    begin
                      TempUrlList.add(tempstr2);
                    end;
                  end;
                end;
              end;

              //Hyperlinks
              I := 1;
              while I > 0 do
              begin
                if PublicShouldStop then break;
                tempstr := ParseTag(workstr, '<a', '</a>', I);
                if length(tempstr) > 0 then
                begin
                  I2 := 1;
                  while I2 > 0 do
                  begin
                    if PublicShouldStop then break;
                    tempstr2 := ParseTag(tempstr, 'href', '"', I2);
                    if I2 = 0 then Break;
                    if Trim(tempstr2) <> '=' then Continue;
                    tempstr2 := 'href' + tempstr2 + '"';
                    I2 := I2 - Length(tempstr2);
                    tempstr2 := ParseTag(tempstr, tempstr2, '"', I2);
                    // put it into our list only if it's not a gif or jpg
                    if (length(tempstr2) > 0) and not ((AnsiPos('.gif', Tempstr2) > 0) or (AnsiPos('.jpg', Tempstr2) > 0)) then
                    begin
                      TempUrlList.add(tempstr2);
                    end;
                  end;
                end;
              end;

              //Scripts
              I := 1;
              while I > 0 do
              begin
                if PublicShouldStop then break;
                tempstr := ParseTag(workstr, '<script', '</script>', I);
                if length(tempstr) > 0 then
                begin
                  //vbscript
                  I2 := 1;
                  while I2 > 0 do
                  begin
                    if PublicShouldStop then break;
                    tempstr2 := ParseTag(tempstr, 'window.open', '"', I2);
                    if I2 = 0 then Break;
                    if Trim(tempstr2) <> '' then Continue;
                    tempstr2 := 'window.open' + tempstr2 + '"';
                    I2 := I2 - Length(tempstr2);
                    tempstr2 := ParseTag(tempstr, tempstr2, '"', I2);
                    // put it into our list only if it's not a gif or jpg
                    if (length(tempstr2) > 0) and not ((AnsiPos('.gif', Tempstr2) > 0) or (AnsiPos('.jpg', Tempstr2) > 0)) then
                    begin
                      TempUrlList.add(tempstr2);
                    end;
                  end;
                  //javascript
                  I2 := 1;
                  while I2 > 0 do
                  begin
                    if PublicShouldStop then break;
                    tempstr2 := ParseTag(tempstr, 'window.open', '''', I2);
                    if I2 = 0 then Break;
                    if Trim(tempstr2) <> '(' then Continue;
                    tempstr2 := 'window.open' + tempstr2 + '''';
                    I2 := I2 - Length(tempstr2);
                    tempstr2 := ParseTag(tempstr, tempstr2, '''', I2);
                    // put it into our list only if it's not a gif or jpg
                    if (length(tempstr2) > 0) and not ((AnsiPos('.gif', Tempstr2) > 0) or (AnsiPos('.jpg', Tempstr2) > 0)) then
                    begin
                      TempUrlList.add(tempstr2);
                    end;
                  end;
                  //javascript
                  I2 := 1;
                  while I2 > 0 do
                  begin
                    if PublicShouldStop then break;
                    tempstr2 := ParseTag(tempstr, 'window.open', '"', I2);
                    if I2 = 0 then Break;
                    if Trim(tempstr2) <> '(' then Continue;
                    tempstr2 := 'window.open' + tempstr2 + '"';
                    I2 := I2 - Length(tempstr2);
                    tempstr2 := ParseTag(tempstr, tempstr2, '"', I2);
                    // put it into our list only if it's not a gif or jpg
                    if (length(tempstr2) > 0) and not ((AnsiPos('.gif', Tempstr2) > 0) or (AnsiPos('.jpg', Tempstr2) > 0)) then
                    begin
                      TempUrlList.add(tempstr2);
                    end;
                  end;
                  //vbscript and javascript
                  I2 := 1;
                  while I2 > 0 do
                  begin
                    if PublicShouldStop then break;
                    tempstr2 := ParseTag(tempstr, 'location.href', '"', I2);
                    if I2 = 0 then Break;
                    if Trim(tempstr2) <> '=' then Continue;
                    tempstr2 := 'location.href' + tempstr2 + '"';
                    I2 := I2 - Length(tempstr2);
                    tempstr2 := ParseTag(tempstr, tempstr2, '"', I2);
                    // put it into our list only if it's not a gif or jpg
                    if (length(tempstr2) > 0) and not ((AnsiPos('.gif', Tempstr2) > 0) or (AnsiPos('.jpg', Tempstr2) > 0)) then
                    begin
                      TempUrlList.add(tempstr2);
                    end;
                  end;
                end;
              end;

             ///Now decide whether to keep these to search later..
              for X := 0 to TempURLList.count - 1 do
              begin
                if PublicShouldStop then break;
                if AnsiPos('mailto:', tempURlList.Strings[x]) > 0 then // ignore mail addresses here..
                  continue;

                if (CountF(tempURlList.Strings[x], '.', 1) = 1) or (copy(tempURlList.Strings[x], 1, 3) = '../') or (copy(tempURlList.Strings[x], 1, 1) = '/') then
                begin
                  if Copy(tempURlList.Strings[x], 1, 1) = '/' then
                    tempURlList.Strings[x] := Url_info.Server + Url_Info.Domain + tempURlList.Strings[x]
                  else
                    tempURlList.Strings[x] := Url_info.Server + Url_Info.Domain + '/' + tempURlList.Strings[x];
                end;

                Temp_Url := Parse_URL(tempURlList.Strings[x]);

                if (Length(Temp_url.domain) > 0) then
                begin
                  AddToSearchLater := True;
                  EnterCriticalSection(work_CS);
                  try
                    if assigned(FOnLinkFound) then
                      FOnLinkFound(Threadnumber, which_Layer, temp_URL, AddToSearchLater);
                  finally
                    LeaveCriticalSection(Work_cs);
                  end;
                  if (AddToSearchLater) and (Which_layer + 1 <= MAX_LAYERS) then
                    LayersURLSToSearch[Which_layer + 1].add(temp_URL.Proto + '://' + temp_Url.Server + Temp_url.Domain + Temp_url.Path + Temp_url.Filename);
                end;
              end;

              Tempstr := '';
              for X := 0 to TempURLList.count - 1 do
              begin
                if PublicShouldStop then break;
                MailPos := AnsiPos('mailto:', tempURlList.Strings[x]); // look only at mail addresses here..
                if (MailPos > 0) and (AnsiPos('@', tempURlList.Strings[x]) > 0) then
                begin
                  Tempstr := copy(tempURlList.Strings[x], mailpos + 7, 500);
                  EnterCriticalSection(work_CS);
                  try
                    if assigned(FOnEmailfound) then
                      FOnEmailfound(Threadnumber, which_Layer, tempstr);
                  finally
                    LeaveCriticalSection(work_cs);
                  end;
                end;
              end;
            end;

          end; // If (Which_layer + 1 <= FMaxLayers) then

          if PublicShouldStop then break;
        end;
      end;
    except
      on E: exception do
      begin
        if assigned(FOnError) then
          FOnError(Threadnumber, E.message);
      end;
    end;
  finally
    TempUrlList.free;
    for x := 1 to MAX_LAYERS do
      LayersURLSToSearch[x].Free;
  end;
end;


procedure Register;
begin
  RegisterComponents('Internet', [Tspider]);
end;

initialization


end.

