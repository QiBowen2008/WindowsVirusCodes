unit SplitFrm;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Buttons, ComCtrls, ExtCtrls,ShellApi,Registry;

type
  TSplitterForm = class(TForm)
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    SourceFileEdit: TEdit;
    Label1: TLabel;
    SpeedButton1: TSpeedButton;
    Label2: TLabel;
    ResultFileEdit: TEdit;
    SpeedButton2: TSpeedButton;
    GroupBox1: TGroupBox;
    Label3: TLabel;
    FileLengthCmBox: TComboBox;
    TabSheet2: TTabSheet;
    SplitterBtn: TBitBtn;
    CloseBtn1: TBitBtn;
    FileLengthEdit: TEdit;
    OpenDialog1: TOpenDialog;
    SaveDialog1: TSaveDialog;
    Label4: TLabel;
    Label5: TLabel;
    SpeedButton3: TSpeedButton;
    Label6: TLabel;
    EndFileEdit: TEdit;
    SpeedButton4: TSpeedButton;
    ConverBtn: TBitBtn;
    CloseBtn2: TBitBtn;
    FileListBox: TListBox;
    ProgressBar1: TProgressBar;
    ProgressBar2: TProgressBar;
    TabSheet3: TTabSheet;
    Panel1: TPanel;
    GroupBox2: TGroupBox;
    Label9: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    GroupBox3: TGroupBox;
    RegCheckBox: TCheckBox;
    CloseBtn3: TBitBtn;
    OptionBtn: TBitBtn;
    Label12: TLabel;
    RegLengthCmBox: TComboBox;
    Label13: TLabel;
    RegLengthEdit: TEdit;
    SpeedButton5: TSpeedButton;
    procedure FileLengthCmBoxChange(Sender: TObject);
    procedure SplitterBtnClick(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure SpeedButton3Click(Sender: TObject);
    procedure SpeedButton4Click(Sender: TObject);
    procedure ConverBtnClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Label10Click(Sender: TObject);
    procedure Label11Click(Sender: TObject);
    procedure OptionBtnClick(Sender: TObject);
    procedure RegLengthCmBoxChange(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    //procedure EditEnabled();
    procedure PageControl1Change(Sender: TObject);
  private
  public
  end;
  const ShellKey : String = '\*\Shell\直接文件分割';
  const OpenKey : String = '\*\Shell\用文件分割器';
  const SettingKey : String = '\Software\Wangjiong\Splitter\1.0\Settings';

var
  SplitterForm: TSplitterForm;
  BlockSize: Integer;

implementation

{$R *.DFM}

procedure EditEnabled;
begin
  with SplitterForm do
   if  FileLengthCmBox.ItemIndex =7 then
       begin
            FileLengthEdit.Enabled :=True;
            FileLengthEdit.Color :=clWindow;
            RegLengthEdit.Enabled :=True;
            RegLengthEdit.Color :=clWindow;
            if  BlockSize<>-1 then
              begin
                FileLengthEdit.Text:=IntToStr(BlockSize);
                RegLengthEdit.Text:=IntToStr(BlockSize);
              end;  

       end
    else
       begin
            FileLengthEdit.Enabled :=False;
            FileLengthEdit.Color :=clMenu;
            FileLengthEdit.Text :='';
            RegLengthEdit.Enabled :=False;
            RegLengthEdit.Color :=clMenu;
            RegLengthEdit.Text :='';
       end;
end;

procedure TSplitterForm.FileLengthCmBoxChange(Sender: TObject);
begin
   case FileLengthCmBox.ItemIndex of
       0: BlockSize:=1457660;
       1: BlockSize:=2915320;
       2: BlockSize:=1228800;
       3: BlockSize:=711740;
       4: BlockSize:=355870;
       5: BlockSize:=102400000;
       6: BlockSize:=122880000;
       7: BlockSize:=-1;
    end;

    RegLengthCmBox.ItemIndex :=FileLengthCmBox.ItemIndex;
    EditEnabled;
    ProgressBar1.Position:=0;
end;

procedure TSplitterForm.SplitterBtnClick(Sender: TObject);
var
   SourceFile, ResultFile: TFileStream;
   Count,MaxLine,i: Integer;
   TempFile:String;
   BatFile:TextFile;
begin
  try
    begin
       if BlockSize=-1 then
         BlockSize := StrToInt(Trim(FileLengthEdit.Text));
       if BlockSize<=0 then
        begin
         FileLengthCmBox.ItemIndex :=0;
         BlockSize:=1457660;
         EditEnabled;
        end;
       SourceFile := TFileStream.Create(Trim(SourceFileEdit.Text) , fmOpenRead);
    end;

  try
    ProgressBar1.Position :=0;
    MaxLine:= SourceFile.Size div BlockSize ;
    ProgressBar1.Max := MaxLine +1;

    if Trim(ResultFileEdit.Text)='' then
      ResultFileEdit.Text:= SourceFileEdit.Text;

    SourceFile.Position := 0;
    AssignFile(BatFile,Trim(ResultFileEdit.Text)+'.Bat');
    Rewrite(BatFile);
    Write(BatFile, 'Copy /b  ');

    begin
      Count := 0;
      while True do
      begin
        if SourceFile.Position = SourceFile.Size then Break;
        TempFile:=Trim(ResultFileEdit.Text) + '.' + Format('%3.3d',[Count]);
        ResultFile := TFileStream.Create(TempFile , fmCreate);
        if SourceFile.Position + BlockSize > SourceFile.Size then
          i := SourceFile.Size - SourceFile.Position
        else
          i := BlockSize;
        ResultFile.CopyFrom(SourceFile, i);
        ResultFile.Free;
        if count=0 then
           Write(BatFile, TempFile +' ')
        else
           Write(BatFile, '+' + TempFile +' ');

        Inc(Count);

        if ProgressBar1.Position < ProgressBar1.Max-1 then
           ProgressBar1.Position:=ProgressBar1.Position+1;

      end;
    end;

  finally
    SourceFile.Free;
    Write(BatFile, SourceFileEdit.Text);
    CloseFile(BatFile);
    ProgressBar1.Position := ProgressBar1.Max;
  end;
  except
     on E: Exception do MessageBox(Handle, PChar(E.Message), '提示', 48);

  end;


 end;


procedure TSplitterForm.SpeedButton1Click(Sender: TObject);
begin
 ProgressBar1.Position:=0;  
 OpenDialog1.Options :=[];
 if OpenDialog1.Execute then
    SourceFileEdit.Text := OpenDialog1.FileName;
end;

procedure TSplitterForm.SpeedButton2Click(Sender: TObject);
begin
    ProgressBar1.Position:=0;
    if SaveDialog1.Execute then
    ResultFileEdit.Text := SaveDialog1.FileName;
end;

procedure TSplitterForm.SpeedButton3Click(Sender: TObject);
var j:integer;
begin
  ProgressBar2.Position:=0;
  OpenDialog1.Options :=[ofAllowMultiSelect];
  if OpenDialog1.Execute then
        for j:=OpenDialog1.Files.Count-1 downto 0 do
        FileListBox.Items.Add(OpenDialog1.Files.Strings[j]);
end;

procedure TSplitterForm.SpeedButton4Click(Sender: TObject);
begin
  ProgressBar2.Position:=0;
  if SaveDialog1.Execute then
    EndFileEdit.Text := SaveDialog1.FileName;
end;

procedure TSplitterForm.ConverBtnClick(Sender: TObject);
var
  i: Integer;
  SourceFile, EndFile: TFileStream;
begin

  try
    EndFile := TFileStream.Create(EndFileEdit.Text , fmCreate);
    ProgressBar2.Position :=0;
    ProgressBar2.Max := FileListBox.Items.Count +1;
  try
    for i := 0 to FileListBox.Items.Count - 1 do
    begin
      if not FileExists(FileListBox.Items[i]) then
      begin
        MessageBox(Handle, PChar('文件 ' + FileListBox.Items[i] + ' 不存在。'), '提示', 48);
      end else
      begin
          SourceFile := TFileStream.Create(FileListBox.Items[i], fmOpenRead);
          EndFile.CopyFrom(SourceFile, 0);
          SourceFile.Free;
          if ProgressBar2.Position < ProgressBar2.Max-1 then
           ProgressBar2.Position:=ProgressBar2.Position+1;
        end;
     end;

  finally
    EndFile.Free;
    ProgressBar2.Position:=ProgressBar2.Max ;
  end;
  except
    on E: Exception do MessageBox(Handle, PChar(E.Message), '提示', 48);
  end;
end;

procedure TSplitterForm.FormCreate(Sender: TObject);
var
  Reg: TRegistry;
begin
  FileLengthCmBox.ItemIndex :=0;
  BlockSize:=1474560;

  try
    begin
     try
       Reg := TRegistry.Create;
       Reg.RootKey := HKEY_CLASSES_ROOT;
       Reg.OpenKey(Shellkey+'\Command', False);
       if Reg.ReadString('')= Application.ExeName + ' /S "%1"' then
       begin
          RegCheckBox.State:=cbChecked
       end else
          RegCheckBox.State:=cbUnchecked;
       Reg.CloseKey ;

       Reg.RootKey := HKEY_CURRENT_USER;
       Reg.OpenKey(Settingkey, False);
       FileLengthCmBox.ItemIndex:=Reg.readInteger('FileLengthIndex');
       if Reg.readInteger('BlockSize')> 0 then
           BlockSize:= Reg.readInteger('BlockSize');
       Reg.CloseKey ;
     except

     end;
       RegLengthCmBox.ItemIndex :=FileLengthCmBox.ItemIndex;
       EditEnabled;
      end; 
     finally
       Reg.Free;
     end;
end;


procedure TSplitterForm.Label10Click(Sender: TObject);
begin
    ShellExecute(0, Nil,'http://delphix.126.com', Nil, Nil, SW_NORMAL);     
end;

procedure TSplitterForm.Label11Click(Sender: TObject);
begin
   ShellExecute(Handle, 'Open', PChar('mailto:delphix@yeah.net'), '', '', 1);
end;

procedure TSplitterForm.OptionBtnClick(Sender: TObject);
var
  Reg: TRegistry;
begin
  try
    begin
       Reg := TRegistry.Create;
       Reg.RootKey := HKEY_CLASSES_ROOT;

       Reg.OpenKey(Shellkey+'\Command', True);
       if RegCheckBox.State =cbChecked then
       begin
          Reg.WriteString('', Application.ExeName + ' /S "%1"');
       end else
          Reg.DeleteKey(Shellkey);
       Reg.CloseKey ;

       Reg.OpenKey(Openkey+'\Command', True);
       if RegCheckBox.State =cbChecked then
       begin
          Reg.WriteString('', Application.ExeName +' "%1"' );
       end else
          Reg.DeleteKey(Openkey);
       Reg.CloseKey ;

       Reg.RootKey := HKEY_CURRENT_USER;
       SettingKey := '\Software\Wangjiong\Splitter\1.0\Settings';
       Reg.OpenKey(SettingKey, True);
       try
         if RegLengthCmBox.ItemIndex =7 then
           begin
              BlockSize := StrToInt(Trim(RegLengthEdit.Text));
              FileLengthEdit.Text:=IntToStr(BlockSize);
           end;
         Reg.WriteInteger('BlockSize',BlockSize );
         Reg.WriteInteger('FileLengthIndex',RegLengthCmBox.ItemIndex );
       except
          on E: Exception do MessageBox(Handle, PChar(E.Message), '提示', 48);
       end;
       Reg.CloseKey ;
     end;
     finally
       Reg.Free;
     end;
end;

procedure TSplitterForm.RegLengthCmBoxChange(Sender: TObject);
begin
    case RegLengthCmBox.ItemIndex of
       0: BlockSize:=1457660;
       1: BlockSize:=2915320;
       2: BlockSize:=1228800;
       3: BlockSize:=711740;
       4: BlockSize:=355870;
       5: BlockSize:=102400000;
       6: BlockSize:=122880000;
       7: BlockSize:=-1;
    end;

    FileLengthCmBox.ItemIndex :=RegLengthCmBox.ItemIndex;
    EditEnabled;

end;

procedure TSplitterForm.SpeedButton5Click(Sender: TObject);
begin
   FileListBox.Items.Clear;
   ProgressBar2.Position:=0;
end;

procedure TSplitterForm.PageControl1Change(Sender: TObject);
begin
   ProgressBar1.Position:=0;
   ProgressBar2.Position:=0;
end;

end.
