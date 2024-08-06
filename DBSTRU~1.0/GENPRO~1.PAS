unit GenProcToCreateDB_MainFrm;

(*****************************************************************************************

 This program can read the structure of simple database tables and generate code that
 can be "included" or copied into programs. This code, when executed, will produce
 empty copies of the original database tables (with the fields and indexes preserved).

 The program will also create a text decription of the fields and indexes.  This can be
 used to document the tables of a database.

 Credit: It was inspired by Bill Fortney's "Award Winning Program" TblDoc and the need
     to allow a program under development to create new, empty databases.

 For information about copyright and licensing, see the DPR file.

 Pgm. 02/23/1998 by John Herbster for creating new empty databases for the EYH pgm.
     (Tel. 281-558-2708 or e-mail JHerbster@compuserve.com)

 Potential Enhancements:
   Could make it command-line driven so that it could be used in a "make file" script.
       The usage could be "GenProcToCreateDB ProcName=<code filename> Alias=<alias>".

*****************************************************************************************)

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ComCtrls, ExtCtrls, StdCtrls, Buttons, Db, DBTables;

type
  TMainFrm = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    Label2: TLabel;
    Alias_cb: TComboBox;
    SelectDbPath_b: TButton;
    DbPath_eb: TEdit;
    Panel2: TPanel;
    Splitter1: TSplitter;
    PageControl1: TPageControl;
    DbDoc_ts: TTabSheet;
    CreationCode_ts: TTabSheet;
    Splitter2: TSplitter;
    Panel3: TPanel;
    Panel4: TPanel;
    TblStructure_m: TMemo;
    Panel5: TPanel;
    Panel6: TPanel;
    TblsList_lb: TListBox;
    DBCode_m: TMemo;
    DbDoc_m: TMemo;
    Table1: TTable;
    SaveCodeDialog: TSaveDialog;
    OpenDialog1: TOpenDialog;
    Panel7: TPanel;
    GenCode_sb: TSpeedButton;
    ProcName_e: TEdit;
    SaveCode_b: TButton;
    SaveDbDoc_b: TButton;
    SaveDocDialog: TSaveDialog;
    Hints_b: TButton;
    procedure Alias_cbDropDown(Sender: TObject);
    procedure Alias_cbChange(Sender: TObject);
    procedure TblsList_lbClick(Sender: TObject);
    procedure SelectDbPath_bClick(Sender: TObject);
    procedure GenCode_sbClick(Sender: TObject);
    procedure SaveCode_bClick(Sender: TObject);
    procedure SaveDbDoc_bClick(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Hints_bClick(Sender: TObject);
    procedure ProcName_eExit(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
    fDbAlias, fDbPath, fProcName,
    fProcFile, fDocFile,
    fDbType, fDbName, fDbText: string;
    fDesWidth, fDesDbPath_eWidth: integer;
    procedure SetDbAlias  (const NewValue: string);
    procedure SetDbPath   (const NewValue: string);
    procedure SetProcName (const NewValue: string);
  public
    property DbAlias:  string read fDbAlias  write SetDbAlias;
    property DbPath:   string read fDbPath   write SetDbPath;
    property ProcName: string read fProcName write SetProcName;
    property ProcFile: string read fProcFile;
    property DocFile:  string read fDocFile;
  end;

var
  MainFrm: TMainFrm;

Procedure GetTableDoc (Table: tTable; Pfx,Sfx: string; Strings: tStrings);

{ adds table structure to strings.}

procedure GenerateCodeToRecreateTable (Table: tTable; Indent: integer; Strings: tStrings);
{ This routine adds code to strings to create an empty DB table.  The code can be stored
  as part of an include file for a routine to create a new database. }

procedure GenerateProcedureToRecreateTables
    (const SrcAliasOrDir: string; ListBox: tCustomListBox;
     const ProcName: string; Strings: tStrings);
{ This routine generates procedure code to create empty database tables with the same
  structures as those in the SrcAliasOrDir database.  The generated code can be stored
  as part of an "include" file for a routine to create a new empty database. }

procedure GetDatabaseDesc (const DBName: String; var DbType,Name,Text,PhyName: string);
{ Code to call BDE shown here for reference. It was not used. }

CONST {$WRITEABLECONST OFF}
  TableTypeStrs: array [TTableType] of string[10] =
     ('ttDefault', 'ttParadox', 'ttDBase', 'ttASCII');
  FieldTypeStrs: array [TFieldType] of string[17] =
     ('ftUnknown', 'ftString', 'ftSmallint', 'ftInteger', 'ftWord', 'ftBoolean',
      'ftFloat', 'ftCurrency', 'ftBCD', 'ftDate', 'ftTime', 'ftDateTime', 'ftBytes',
      'ftVarBytes', 'ftAutoInc', 'ftBlob', 'ftMemo', 'ftGraphic', 'ftFmtMemo',
      'ftParadoxOle', 'ftDBaseOle', 'ftTypedBinary', 'ftCursor');
  FieldTypeNames: array [TFieldType] of string[15] =
     ('Unknown', 'String', 'Smallint', 'Integer', 'Word', 'Boolean',
      'Float', 'Currency', 'BCD', 'Date', 'Time', 'DateTime', 'Bytes',
      'VarBytes', 'AutoInc', 'Blob', 'Memo', 'Graphic', 'FmtMemo',
      'ParadoxOle', 'DBaseOle', 'TypedBinary', 'Cursor');
  FieldTypeAbbrs: array [TFieldType] of string[9] =
     ('?', 'A', 'Short', 'Integer', 'Word', 'Logical',
      'Float', 'Money', 'BCD', 'Date', 'Time', '@', 'Bytes',
      'vBytes', '+', 'Blob', 'M', 'Graphic', 'FmtMemo',
      'PdoxOLE', 'DBaseOle', 'TypedBin', 'Cursor');
  IndexOptionStrs: array [ixPrimary..ixExpression] of string[20] =
      ('ixPrimary', 'ixUnique', 'ixDescending', 'ixCaseInsensitive', 'ixExpression');
  IndexOptionAbbrs: array [ixPrimary..ixExpression] of string[11] =
      ('Primary', 'Unique', 'Descend', 'CaseInsens', 'Expression');
  NoYesStrs    : array [boolean] of string[5] = ('No','Yes');
  FalseTrueStrs: array [boolean] of string[5] = ('False','True');

implementation (*************************************************************************)

USES BDE;

{$R *.DFM}

{---- Database Setup --------------------------------------------------------------------}

procedure GetDatabaseDesc (const DBName: String; var DbType,Name,Text,PhyName: string);
{ Code to call BDE shown here for reference. It was not used. }
var dbDes: DBDesc;
begin
  Check(DbiGetDatabaseDesc(PChar(DBName), @dbDes));  {Error $2104}
  With dbDes do begin
    DbType := szDbType;
    Name   := szName;
    Text   := szText;
    PhyName:= szPhyName;
    end;
end;

procedure  TMainFrm.SetDbAlias (const NewValue: string);
begin
  If NewValue=''
    then begin fDbType := ''; fDbName := ''; fDbText := ''; fDbPath := '' end
    else GetDatabaseDesc (NewValue,fDbType,fDbName,fDbText,fDbPath);
  fDbAlias := NewValue;
{ Fill in the Table1.DatabaseName. }
  Table1.DatabaseName := DbAlias;
  Caption := 'Database -- ' + DbAlias;
  Alias_cb.Text := DbAlias;
  DbPath_eb.Text := DbPath;
end;

procedure  TMainFrm.SetDbPath (const NewValue: string);
begin
  fDbAlias := ''; fDbType := ''; fDbName := ''; fDbText := '';
  fDbPath := NewValue;
{ Fill in the Table1.DatabaseName. }
  Table1.DatabaseName := DbPath;
  Caption := 'Database -- ' + DbPath;
  Alias_cb.Text := DbAlias;
  DbPath_eb.Text := DbPath;
end;

procedure TMainFrm.SetProcName (const NewValue: string);
begin
  fProcName := NewValue;
  ProcName_e.Text := ProcName;
end;

{---- Table Doc Createion ---------------------------------------------------------------}

procedure GetTableDoc
    (Table: tTable; Pfx,Sfx: string; Strings: tStrings);
{ This routine adds code to strings to document a DB table.  The code can be stored
  as part of an include file for a procedure to create a new database or used in a memo
  box. }
var
  i: Integer; SizeStr, IdxName, IdxOptions: String; iopt: ixPrimary..ixExpression;
begin
  with Table do
  begin
  { Refresh the definitions from the disk table.}
    FieldDefs.Update;
    IndexDefs.Update;
    Strings.Add(Pfx+'Table: ' + TableName+Sfx);
    Strings.Add(Pfx+Format('  %-25s %-10s %-3s',['Fields','Type(Size)',' Required'])+Sfx);
    for i := 0 to FieldDefs.Count - 1 do with FieldDefs.Items[i] {TFieldDef} do
    begin
      if Size>0
        then SizeStr := '('+IntToStr(Size)+')'
        else SizeStr := '';
      Strings.Add(Pfx+Format('    %-25s %-12s %-3s',
            [Name,FieldTypeAbbrs[DataType]+SizeStr,NoYesStrs[Required]])+Sfx);
      end{i-loop};
    Strings.Add(Pfx+'  Indexes'+Sfx);
    for i := 0 to IndexDefs.Count - 1 do with IndexDefs.Items[i] {TIndexDef} do
    begin
        IdxOptions := '[';
        For iopt := ixPrimary to ixExpression do if iopt in Options then begin
          If Name<>'' then IdxName := Name+':' else IdxName := '(no name)';
          If length(IdxOptions)>1 then IdxOptions := IdxOptions + ',';
          IdxOptions := IdxOptions + IndexOptionAbbrs[iopt];
          end;
        IdxOptions := IdxOptions + ']';
        Strings.Add(Pfx+Format('    %-11s %-25s %s',[IdxName,Fields,IdxOptions])+Sfx);
      end;
    end;
end{GetTableDoc};

{---- Code Generation -------------------------------------------------------------------}

procedure GenerateCodeToRecreateTable (Table: tTable; Indent: integer; Strings: tStrings);
{ This routine generates code to create an empty DB table in a specified directory.
  The code can be stored as part of an include file for a routine to create a new
  database. }
var
  FldName, FldSize, FldDataType, FldRequired, IdxName, IdxFields, IdxOptions,
  IdS{indentation string} : string;
  i: integer; iopt: ixPrimary..ixExpression;
begin
  IdS := ''; For i:= 1 to Indent do IdS := IdS + ' ';
  With Table do begin
  { Refresh the definitions from the disk table.}
    FieldDefs.Update;
    IndexDefs.Update;
  { Name the table.}
    Strings.Add(IdS+'IndexDefs.Clear;');
    Strings.Add(IdS+'FieldDefs.Clear;');
    Strings.Add(IdS+'TableType := '+TableTypeStrs[TableType]+';');
    Strings.Add(IdS+'TableName := '''+TableName+''';');
  { Construct the field definitions.}
    Strings.Add(IdS+'With FieldDefs do');
    Strings.Add(IdS+'begin');
    For i := 0 to FieldDefs.Count-1 do with FieldDefs.Items[i] {tFieldDef} do begin
      FldName := Name;
      FldDataType := FieldTypeStrs[DataType];
      FldSize := IntToStr(Size);
      If Required then FldRequired := 'true' else FldRequired := 'false';
      Strings.Add(IdS+'  Add('''+FldName+''','+FldDataType+','+FldSize+','+FldRequired+');');
      end{i-loop};
    Strings.Add(IdS+'end{with FieldDefs};');
  { Construct the index definitions.}
    With IndexDefs do begin
      Strings.Add(IdS+'With IndexDefs do ');
      Strings.Add(IdS+'begin');
      For i := 0 to Count-1 do with IndexDefs.Items[i] {TIndexDef} do begin
        IdxName := Name;
        IdxFields := Fields;
        IdxOptions := '[';
        For iopt := ixPrimary to ixExpression do if iopt in Options then begin
          If length(IdxOptions)>1 then IdxOptions := IdxOptions + ',';
          IdxOptions := IdxOptions + IndexOptionStrs[iopt];
          end;
        IdxOptions := IdxOptions + ']';
        Strings.Add(IdS+'  Add('''+IdxName+''','''+IdxFields+''','+IdxOptions+');');
        end;
      Strings.Add(IdS+'end{with IndexDefs.Items[i]};');
      end{with IndexDefs};
  { Gen. code to create the destination table file.}
    Strings.Add(IdS+'CreateTable;');
  end{with Table};
end{GenerateCodeToRecreateTable};

procedure GenerateProcedureToRecreateTables
    (const SrcAliasOrDir: string; ListBox: tCustomListBox;
     const ProcName: string; Strings: tStrings);
{ This routine generates procedure code to create empty database tables with the same
  structures as those in the SrcAliasOrDir database.  The generated code can be stored
  as part of an "include" file for a routine to create a new empty database. }
var
  i: integer; Table: tTable;
begin
  Table := tTable.Create(nil);
  With Table do Try{Finally}
    DatabaseName := SrcAliasOrDir;
  { First add the code to start the procedure }
    Strings.Add('Procedure '+ProcName+' (const DstAliasOrDir: string);');
    Strings.Add('var Table: tTable; {Uses Delphi units DB and DBTables.}');
    Strings.Add('begin {Code generated '+FormatDateTime('mm/dd/yyyy hh:nn',Now)+'}');
    Strings.Add('  Table := tTable.Create(nil);');
    Strings.Add('  With Table do');
    Strings.Add('  Try{Finally}');
    Strings.Add('    DatabaseName := DstAliasOrDir;');
  { Do for each table to have creation code generated.}
    With ListBox do for i := 0 to Items.Count-1
        do if Selected[i] or (SelCount=0) then begin
      TableName := Items[i];
    { Make the code, including the "Table.CreateTable".}
      Strings.Add('{ Code to create table '+TableName+'.}');
      GenerateCodeToRecreateTable(Table,4,Strings);
      end{i-loop};
  { Finally add the code to end the procedure.}
    Strings.Add('  Finally');
    Strings.Add('    Table.Free;');
    Strings.Add('  End{Finally};');
    Strings.Add('end{'+ProcName+'};');
  Finally
    Table.Free;
  End{Finally};
end{GenerateProcedureToRecreateTables};

{---- Form Controls ---------------------------------------------------------------------}

procedure TMainFrm.FormCreate(Sender: TObject);
begin
{ Save initial sizes for use in FormResize.}
  fDesWidth := Width;
  fDesDbPath_eWidth := DbPath_eb.Width;
end{FormCreate};

procedure TMainFrm.FormResize(Sender: TObject);
begin
  DbPath_eb.Width := fDesDbPath_eWidth + (Width - fDesWidth); {Resize the path box.}
end;

procedure TMainFrm.Alias_cbDropDown(Sender: TObject);
begin
  Screen.Cursor := crHourglass;
  Try
    Session.GetDatabaseNames(Alias_cb.Items);
  Finally
    Screen.Cursor := crDefault;
  End;
end;

procedure TMainFrm.Alias_cbChange(Sender: TObject);
begin
  DbAlias := Alias_cb.Text;
{ Load up the tables list.}
  Alias_cb.Text := DbAlias;
  DbPath_eb.Text := DbPath;
  Session.DropConnections;
  Session.GetTableNames(DbAlias,'*.*',true,false,TblsList_lb.Items);
end;

procedure TMainFrm.SelectDbPath_bClick(Sender: TObject);
var i: integer;
begin
  With OpenDialog1 do begin
    InitialDir := DbPath;
    If Execute then begin
{ Need to try to distinguish here between a single directory and selected files!!}
      DbPath := ExtractFileDir(Filename);
    { Load up the tables list.}
      TblsList_lb.Clear;
      With Files do for i:= 0 to Count-1
          do TblsList_lb.Items.Add(ExtractFileName(Files[i]));
      end;
    end;
end;

procedure TMainFrm.TblsList_lbClick(Sender: TObject);
begin
{ Display structure for this table.}
  Screen.Cursor := crHourglass;
  with Table1 do Try
    TableName := TblsList_lb.Items[TblsList_lb.ItemIndex];
    TblStructure_m.Lines.Clear;
    GetTableDoc(Table1,'','',TblStructure_m.Lines);
    TblStructure_m.Lines.Add('');;
  Finally
    Screen.Cursor := crDefault;
  End;
end;

procedure TMainFrm.GenCode_sbClick(Sender: TObject);
var i: integer;
begin
  DBCode_m.Lines.Clear;
  GenerateProcedureToRecreateTables
      (DbAlias,TblsList_lb,ProcName_e.Text,DBCode_m.Lines);
{ Add the code to write DB structure documentation file.}
  With DbDoc_m, Table1 do begin
    Lines.Add('DATABASE STRUCTURE -- '+DbAlias+' ('+DbPath+') '+
               FormatDateTime('mm/dd/yyyy hh:nn',Now));
    With TblsList_lb
        do for i := 0 to Items.Count-1 do if Selected[i] or (SelCount=0) then begin
      TableName := Items[i];
    { Make the code to write documentation.}
      Lines.Add('');
      GetTableDoc(Table1,'','',Lines);
    end{with ... for i ... if ... loop};
  end;
end;

procedure TMainFrm.SaveCode_bClick(Sender: TObject);
begin
  With SaveCodeDialog do begin
    If Execute then begin
      DBCode_m.Lines.SaveToFile(Filename);
      end;
    end;
end;

procedure TMainFrm.SaveDbDoc_bClick(Sender: TObject);
begin
  With SaveDocDialog do begin
    If Execute then begin
      DbDoc_m.Lines.SaveToFile(Filename);
      end;
    end;
end;

procedure TMainFrm.Hints_bClick(Sender: TObject);
begin
  ShowMessage('The source database may be selected by either alias or path. '+
      ' With the Select DB Path dialog, either a single database directory or'+
      ' multiple files w/in a directory may be selected.  The selected tables'+
      ' will show in the Tables List. Multiple files may be selected'+
      ' in the Tables List.  If none are selected, then when Process List is'+
      ' pressed all tables will be processed into the Procedure and Documentation'+
      ' tabs at bottom of form.');
end;

procedure TMainFrm.ProcName_eExit(Sender: TObject);
begin
  ProcName := ProcName_e.Text;
end;

procedure TMainFrm.FormShow(Sender: TObject);
var pn, i: integer; s,t: string;
begin
{ Read any command line parameters. (This in anticipation of making command-line driven.)}
  For pn := 1 to ParamCount do begin
    s := ParamStr(pn);
    If s[length(s)]=','
      then SetLength(s,length(s)-1);
    i := Pos('=',s);
    If i>0
      then begin t := UpperCase(Copy(s,1,i-1)); s := Copy(s,i+1,255) end
      else t:='';
    If      (t='PROCNAME') or (t='PROCEDURENAME')
      then ProcName := s
    Else if (t='PROCFILE') or (t='PROCEDUREFILE')
      then fProcFile := s
    Else if (t='ALIAS') or (t='ALIASNAME')
      then DBAlias := s
    Else if (t='DBPATH') or ((t='DATABASEPATH'))
      then DBPath := s
    Else if (t='DOCFILE') or ((t='DOCUMENTATIONFILE'))
      then fDocFile := s
    Else raise Exception.Create
         ('Program parameter('+IntToStr(pn)+') '+ParamStr(pn)+' cannot be interpreted.');
    end{pn-loop};
  If (DbPath<>'') and ((ProcFile<>'') or (DocFile<>'')) then begin
    GenCode_sbClick(Sender);
    If ProcFile<>''
      then DBCode_m.Lines.SaveToFile(ProcFile);
    If DocFile<>''
      then DbDoc_m.Lines.SaveToFile(DocFile);
    Close;
    end;
end;

end. (***********************************************************************************)

