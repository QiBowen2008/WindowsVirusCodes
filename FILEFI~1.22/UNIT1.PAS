unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, StdCtrls,filectrl, ComCtrls,shellapi;

type
  TForm1 = class(TForm)
    Panel1: TPanel;
    Panel2: TPanel;
    Label1: TLabel;
    Label2: TLabel;
    Edit1: TEdit;
    Button1: TButton;
    Label4: TLabel;
    Edit2: TEdit;
    Panel3: TPanel;
    Label3: TLabel;
    Bevel1: TBevel;
    StatusBar1: TStatusBar;
    Button2: TButton;
    Button3: TButton;
    drivelist: TListBox;
    Label5: TLabel;
    Edit3: TEdit;
    Button4: TButton;
    files: TListView;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure drivelistDrawItem(Control: TWinControl; Index: Integer;
      Rect: TRect; State: TOwnerDrawState);
    procedure Button4Click(Sender: TObject);
  private
    { Private-Deklarationen }
  public
    { Public-Deklarationen }
    procedure find(a:string);
    procedure myhint (sender:tobject);
  end;

  function volname(drivechar: char):string;

var
  Form1: TForm1;
  breaked : boolean;
  filemask,namemask : tstringlist;
implementation

uses masksearch;

{$R *.DFM}

procedure tform1.myhint (sender:tobject);
begin
     statusbar1.simpletext := application.hint;
end;

procedure TForm1.Button1Click(Sender: TObject);
var ct : integer;
    sr : string;
begin
     if drivelist.itemindex = -1 then exit;
     breaked := false;
     screen.cursor := crhourglass;
     files.items.clear;
     files.items.beginupdate;
     files.viewstyle := vslist; //enhances addition of items
     sr := edit3.text;
     if length(sr) > 2 then if sr[2] = ':' then sr := copy(sr,4,maxint);
     if sr <> '' then if sr[length(sr)]<>'\' then sr:=sr+'\';
     panel2.enabled := false;
     setfilters(edit1.text,filemask,false);
     setfilters(edit2.text,namemask,true);
     with drivelist do
          for ct := 0 to items.count-1 do if selected[ct] then
              find(items[ct][1]+':\'+sr);
     screen.cursor := crdefault;
     label3.caption := '';
     breaked := false;
     panel2.enabled:=true;
     files.hint := inttostr(files.items.count)+' file(s) found.';
     files.viewstyle := vsreport;
     files.items.endupdate;
end;

procedure tform1.find(a:string);
var sr : tsearchrec;
    fl : tstringlist;
    rs : integer;

    procedure addfile;
    // adds a file to the listview;
    var li : tlistitem;
    begin
         li := files.items.add;
         li.caption := a+sr.name;
         li.subitems.add(inttostr(sr.size));
         li.subitems.add(formatdatetime('c',filedatetodatetime(sr.time)));
    end;

begin
     if (getasynckeystate(vk_escape)) <> 0 then breaked := true;
     if breaked then exit;
     label3.caption := 'scanning : '+a;
     application.hint:='press ESC to cancel';
     application.processmessages;
     try
        fl := tstringlist.create;
        fl.sorted := true;
        fillchar(sr,sizeof(tsearchrec),0);
        rs := findfirst(a+'*.*',faanyfile-favolumeid,sr);
        while rs = 0 do with sr do begin
              if (getasynckeystate(vk_escape)) <> 0 then breaked := true;
              if breaked then exit;
              if ((attr and fadirectory) <> 0) then begin
                 if (name <> '.') and (name <> '..') then
                    fl.add(a+name+'\')
              end
              else try
                   if cmpmask(sr.name,namemask,true) then
                   if cmpfile(a+name,filemask) then addfile;
              finally
              end;
              rs := findnext(sr);
        end;
        findclose(sr);
        if fl.count > 0 then for rs := 0 to pred(fl.count) do begin
           if breaked then exit;
           find(fl[rs]);
        end;
     finally
            fl.free;
     end;
end;

procedure TForm1.FormCreate(Sender: TObject);
var ct,vi : integer;
    bits : set of 0..25;
    chr : char;
begin
     filemask := tstringlist.create;
     namemask := tstringlist.create;
     application.onhint := myhint;
     //set the drive-listbox
     integer(bits) := getlogicaldrives;
     for ct := 0 to 25 do
         if ct in bits then begin
            chr := char(ct+ord('A'));
            vi := getdrivetype(pchar(chr+':\'));
            drivelist.items.add(chr+char(vi+ord('A'))+volname(chr));
         end;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
     filemask.free;
     namemask.free;

end;

procedure TForm1.Button2Click(Sender: TObject);
begin
     shellabout(handle,pchar(caption),pchar('© 1997 by MirBir.St@T-Online.de'),application.icon.handle);
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
     close;
end;


// subroutines to get a drive's volume-label
// these routines are a from BORLAND, just a bit modified

function Vol(DriveChar: Char): string;
var
  OldErrorMode: Integer;
  NotUsed, VolFlags: Integer;
  Buf: array [0..MAX_PATH] of Char;
begin
  OldErrorMode := SetErrorMode(SEM_FAILCRITICALERRORS);
  try
    GetVolumeInformation(PChar(DriveChar + ':\'), Buf, sizeof(Buf),
      nil, NotUsed, VolFlags, nil, 0);
    SetString(Result, Buf, StrLen(Buf));
    Result := Format('[%s]',[Result]);
  finally
    SetErrorMode(OldErrorMode);
  end;
end;

function NetVol(DriveChar: Char): string;
var
  Buf: Array [0..MAX_PATH] of Char;
  DriveStr: array [0..3] of Char;
  BufferSize: Integer;
begin
  BufferSize := sizeof(Buf);
  DriveStr[0] := DriveChar;
  DriveStr[1] := ':';
  DriveStr[2] := #0;
  if WNetGetConnection(DriveStr, Buf, BufferSize) <> WN_SUCCESS then
    Result := VolumeID(DriveChar)
  else
    SetString(Result, Buf, BufferSize);
end;

function volname(drivechar: char):string;
// gets the volume-label of the specified drive
begin
    if getdrivetype(pchar(drivechar+':\')) =
       drive_remote  then  result := NetVol(DriveChar)
    else result := vol(DriveChar);
    if not directoryexists(pchar(drivechar+':\')) then result := '[...]';
end;

procedure TForm1.drivelistDrawItem(Control: TWinControl; Index: Integer;
  Rect: TRect; State: TOwnerDrawState);
var sr : string;
    tbmp : tbitmap;

// constants for drivebitmaps they are coming from the compiled filectrls-unit-resource
// and are linked automatic cause we are using that unit

const drt : array ['A'..'G'] of string = (
     'UNKNOWNFILE','UNKNOWNFILE','FLOPPY','HARD','NETWORK','CDROM','RAM');

     
begin
     sr := drivelist.items[index];
     with drivelist.canvas do begin
          fillrect(rect);
          // draw the bitmap
          try
             tbmp:=tbitmap.create;
             tbmp.loadfromresourcename(hinstance,drt[sr[2]]);
             BrushCopy(Bounds(Rect.Left + 2,
               (Rect.Top + Rect.Bottom - tbmp.Height) div 2,
               tbmp.Width, tbmp.Height),
               tbmp, Bounds(0, 0, tbmp.Width, tbmp.Height),
               tbmp.Canvas.Pixels[0, tbmp.Height - 1]);
          finally
                 tbmp.free;
          end;
          textout(rect.left+20,rect.top+1,sr[1]+' : '+copy(sr,3,maxint));
     end;
end;

procedure TForm1.Button4Click(Sender: TObject);
var sr : string;
begin
     // browse for a folder
     sr := edit3.text;
     if selectdirectory(sr,[],0) then begin
        if sr[length(sr)] <> '\' then sr := sr+'\';
        edit3.text := copy(sr,4,maxint);
     end;

end;

end.
