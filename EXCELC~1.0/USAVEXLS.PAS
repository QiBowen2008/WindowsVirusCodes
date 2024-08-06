
{$I-,N+}

{General purpose save library - (c) J. Gomes Ferreira 1994
 Writes records in Excel v.2.1, v.3, v.4,
 ASCII comma separated text,
 and tab-delimited excel text files

 Excel BIFF: parts translated from C by Todd Lucas - Microsoft corp.}

Unit USaveXls;

Interface

uses SysUtils;

Const

  Space : char = chr(32);
  Tab   : char = chr(9);
  CR    : char = chr(13);
  LF    : char = chr(10);

  {BOF}
  BOF       = $0009;
  BIT_BIFF5 = $0800;
  BIT_BIFF4 = $0400;
  BIT_BIFF3 = $0200;
  BOF_BIFF5 = BOF or BIT_BIFF5;
  BOF_BIFF4 = BOF or BIT_BIFF4;
  BOF_BIFF3 = BOF or BIT_BIFF3;
  {EOF}
  BIFF_EOF = $000a;
  {Dimensions}
  DIMENSIONS = $0000;
  DIMENSIONS_BIFF4 = DIMENSIONS or BIT_BIFF3;
  DIMENSIONS_BIFF3 = DIMENSIONS or BIT_BIFF3;
  {Document types}
  DOCTYPE_XLS = $0010;
  DOCTYPE_XLC = $0020;
  DOCTYPE_XLM = $0040;
  DOCTYPE_XLW = $0100;
  {Use with output functions}
  VER_BIFF4 = $04;
  VER_BIFF3 = $03;
  VER_BIFF2 = $02;
  {Structures}
  LEN_RECORDHEADER = 4;
  {Data types }
  CellBlank   = 1;
  CellInteger = 2;
  CellDouble  = 4;
  CellLabel   = 8;
  CellBoolean = 16; { or error }

Type

  string10 = String[10]; String255 = string[255];
  chartype = array[0..255] of char;

  PBaseSave = ^TBaseSave;
  TBaseSave = object
    Charfile : file of char;
    DataString : String255; Separator : char;
    MinSaveRecs, MaxSaveRecs, MinSaveCols, MaxSaveCols : word;
    CellType, Row, Col : integer;
    Data : pointer;
    EndOfLine : boolean;

    Constructor Init(SaveFileName : TFileName );
    procedure WriteBlank; virtual;
    procedure WriteInteger; virtual;
    procedure WriteDouble; virtual;
    procedure WriteLabel (var w : word); virtual;
    procedure WriteData(AType, ARow, ACol: Integer; AData: Pointer); virtual;
    Destructor Done; virtual;
  end;

  PASCII = ^TASCII;
  TASCII = object(TBaseSave)
    Constructor Init( SaveFileName : TFileName );
    Destructor Done; virtual;
  end;

  PExcelTab = ^TExcelTab;
  TExcelTab = object(TBaseSave)
    Constructor Init(SaveFileName : TFileName );
    Destructor Done; virtual;
  end;

  PBIFF2 = ^TBIFF2;
  TBIFF2 = object(TBaseSave)
    {BIFFtime, BIFFdata : double;} BIFFColumn : byte;
    ExcelFile : File;
    VerBIFF, TypeDOC : word;
    typerec, lendata : word;

    constructor Init(AFileName : TFileName);
    destructor Done; virtual;
    procedure BIFFBOF; virtual;
    procedure BIFFDIM; virtual;
    procedure WriteBOF; virtual;
    procedure WriteRecordHeader; virtual;
    procedure WriteDimensions; virtual;
    procedure WriteEOF; virtual;
    procedure WriteData(AType, ARow, ACol: Integer; AData: Pointer); virtual;
    procedure WriteBlank; virtual;
    procedure WriteInteger; virtual;
    procedure WriteDouble; virtual;
    procedure WriteLabel (var w : word); virtual;
    procedure WriteBoolean; virtual;
  end;

  PBIFF3 = ^TBIFF3;
  TBIFF3 = object(TBIFF2)
    procedure BIFFBOF; virtual;
    procedure BIFFDIM; virtual;
  end;

  PBIFF4 = ^TBIFF4;
  TBIFF4 = object(TBIFF3)
    procedure BIFFBOF; virtual;
  end;

  PBIFF5 = ^TBIFF5;
  TBIFF5 = object(TBIFF4)
    procedure BIFFBOF; virtual;
  end;

var PSaveFile : PBaseSave;

Implementation

{Generic save object}

Constructor TBaseSave.Init;
begin
  MinSaveRecs := 0; MaxSaveRecs := 65535;
  MinSaveCols := 0; MaxSaveCols := 65535;
  EndOfLine := false;
end;

Procedure TBaseSave.WriteBlank;
begin
  write( CharFile, separator );
end;

Procedure TBaseSave.WriteInteger;
var AIntegerP : ^integer; AInteger : integer;
begin
  AIntegerP := Data; AInteger := AIntegerP^;
  str(AInteger, DataString );
end;

Procedure TBaseSave.WriteDouble;
var ADoubleP : ^double; ADouble : double;
begin
  ADoubleP := Data; ADouble := ADoubleP^;
  str(ADouble, DataString );
end;

Procedure TBaseSave.WriteLabel;
var ALabelP : ^CharType; ALabel : CharType;
begin
  ALabelP := Data; ALabel := ALabelP^;
  DataString  := StrPas( ALabel );
  w := length(DataString); {unused by calling method}
end;

Procedure TBaseSave.WriteData;
var i : integer; AWordLength : word;
begin
  CellType := AType;
  if Row <> -1 then if Row <> ARow then EndOfLine := true else EndOfLine := false;
  Row := ARow;
  Col := ACol;
  Data := AData;

  case CellType of
    CellBlank   : WriteBlank;
    CellInteger : WriteInteger;
    CellDouble  : WriteDouble;
    CellLabel   : WriteLabel(AWordLength);
    CellBoolean : exit; {No boolean types in text files}
    else exit;
  end;
  
  if EndOfLine then begin write ( CharFile, CR ); write ( CharFile, LF ) end;
  for i := 1 to length(DataString) do write( CharFile, DataString[i] );
  write( CharFile, separator );
  
end;

Destructor TBaseSave.Done;
begin
end;

{ASCII files object}

Constructor TASCII.Init;
begin
  TBaseSave.Init( SaveFileName );
  Separator := Space;
  assign( CharFile, SaveFileName );
  Row := -1; col := -1;
  rewrite ( CharFile ); 
end;

Destructor TASCII.Done;
begin
  TBaseSave.Done; close( CharFile );
end;

{Excel tab-delimited files object}

Constructor TExcelTab.Init;
begin
  TBaseSave.Init( SaveFileName );
  Separator := tab; 
  assign( CharFile, SaveFileName );
  Row := -1; col := -1;
  rewrite ( CharFile ); 
end;

Destructor TExcelTab.Done;
begin
  TBaseSave.Done; close( CharFile );
end;

{Excel BIFF2 object}

Constructor TBIFF2.Init;
begin
  TBaseSave.Init( AFileName );
  Assign( ExcelFile, AFileName); Rewrite( ExcelFile, 1 );
  WriteBOF;
  WriteDimensions; 
end;

Destructor TBIFF2.Done;
begin
  TBaseSave.Done;
  WriteEOF;
  Close (ExcelFile);
end; 

procedure TBIFF2.BIFFBOF;
begin
  typerec := BOF;
  lendata := 4;
end;

procedure TBIFF2.BIFFDIM;
begin
  typerec := DIMENSIONS;
  lendata := 8;
end;

procedure TBIFF2.WriteBOF;
var awBuf : array[0..2] of word; 
begin
  awBuf[0] := 0;
  awBuf[1] := DOCTYPE_XLS;
  awBuf[2] := 0;
  BIFFBOF;
  WriteRecordHeader; 
  Blockwrite(Excelfile, awbuf, lendata);
end;

procedure TBIFF2.WriteRecordHeader;
var awBuf : array[0..1] of word;
begin
  awBuf[0] := typerec;
  awBuf[1] := lendata;
  Blockwrite(Excelfile, awbuf, LEN_RECORDHEADER);
end;

procedure TBIFF2.WriteDimensions;
var awBuf : array[0..4] of word;
begin
  awBuf[0] := MinSaveRecs;
  awBuf[1] := MaxSaveRecs;
  awBuf[2] := MinSaveCols;
  awBuf[3] := MaxSaveCols;
  awBuf[4] := 0;
  BIFFDIM;
  WriteRecordHeader;
  Blockwrite(Excelfile, awbuf, lendata);
end;

procedure TBIFF2.WriteEOF;
begin
  typerec := BIFF_EOF;
  lendata := 0;
  WriteRecordHeader;
end;

Procedure TBIFF2.WriteBlank;
begin
  typerec := 1;
  lendata := 7;
  WriteRecordHeader;
  lendata := 0;
end;

Procedure TBIFF2.WriteInteger;
begin
  typerec := 2;
  lendata := 9;
  WriteRecordHeader;
  lendata := 2;
end;

Procedure TBIFF2.WriteDouble;
begin
  typerec := 3;
  lendata := 15;
  WriteRecordHeader;
  lendata := 8;
end;

Procedure TBIFF2.WriteLabel(var w : word);
begin
  w := strlen(Data);
  typerec := 4;
  lendata := 8+w;
  WriteRecordHeader;
  lendata := w;
end;

Procedure TBIFF2.WriteBoolean;
begin
  typerec := 5;
  lendata := 9;
  WriteRecordHeader;
  lendata := 0;
end;

Procedure TBIFF2.WriteData;
const
  Attribute: Array[0..2] Of Byte = (0, 0, 0); { 24 bit bitfield }
var
  awBuf : array[0..1] of word;
  AWordLength : word; ABoolByte : byte;
begin
  CellType := AType;
  Row := ARow;
  Col := ACol;
  Data := AData;

  case CellType of
    CellBlank   : WriteBlank;
    CellInteger : WriteInteger;
    CellDouble  : WriteDouble;
    CellLabel   : WriteLabel(AWordLength);
    CellBoolean : WriteBoolean; { or error }
    else exit;
  end;
  awBuf[0] := Row;
  awBuf[1] := Col;
  Blockwrite(Excelfile, awbuf, sizeof(awBuf));
  BlockWrite(Excelfile, Attribute, SizeOf(Attribute));
  
  if CellType = CellLabel then begin
    ABoolByte := AWordLength;
    BlockWrite(Excelfile, ABoolByte, SizeOf(ABoolByte))
  end else if CellType = CellBoolean then begin
    if byte(Data^) <> 0 then ABoolByte := 1 else ABoolByte := 0;
    BlockWrite(Excelfile, ABoolByte, SizeOf(ABoolByte));
    ABoolByte := 0;
    BlockWrite(Excelfile, ABoolByte, SizeOf(ABoolByte));
  end;
  if lendata <> 0 then BlockWrite(Excelfile, Data^, lendata);
end;

{Excel BIFF3 object}

procedure TBIFF3.BIFFBOF;
begin
  typerec := BOF_BIFF3;
  lendata := 6;
end;

procedure TBIFF3.BIFFDIM;
begin
  typerec := DIMENSIONS_BIFF3;
  lendata := 10;
end;

{Excel BIFF4 object}

procedure TBIFF4.BIFFBOF;
begin
  typerec := BOF_BIFF4;
  lendata := 6;
end;

{Excel BIFF5 object}

procedure TBIFF5.BIFFBOF;
begin
  typerec := BOF_BIFF5;
  lendata := 6;
end;

end.


