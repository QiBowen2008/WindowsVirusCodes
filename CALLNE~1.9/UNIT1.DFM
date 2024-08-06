object Form1: TForm1
  Left = 305
  Top = 99
  AutoScroll = False
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = '稚伟,你好吗,我想你!我不想失去你!'
  ClientHeight = 363
  ClientWidth = 296
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = '宋体'
  Font.Style = [fsBold]
  Icon.Data = {
    0000010001001010100000000000280100001600000028000000100000002000
    00000100040000000000C0000000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    000000000000000000A000000000000000AA000000AA0AAA00A0A0000AA00000
    AAA00A00A000000A00AA00A0A000000AA0A000AA0000000A0AA00AA00000000A
    00A0A0A0A0000000AAA000A00A00000A00A000A00A00000AA0A000A000000000
    0AAA000A00000000000000A0000000000000000000000000000000000000FFFF
    0000FDFF0000FCFC00008D790000F1B70000ECD70000E5CF0000E99F0000ED57
    0000F1DB0000EDDB0000E5DF0000F8EF0000FFDF0000FFFF0000FFFF0000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 16
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 56
    Height = 16
    Caption = '寻呼台:'
    Color = clGray
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
  end
  object Label2: TLabel
    Left = 8
    Top = 104
    Width = 72
    Height = 16
    Caption = '寻呼号码:'
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label3: TLabel
    Left = 8
    Top = 200
    Width = 40
    Height = 16
    Caption = '性别:'
    Color = clNone
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
  end
  object Label4: TLabel
    Left = 8
    Top = 152
    Width = 40
    Height = 16
    Caption = '贵姓:'
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label5: TLabel
    Left = 144
    Top = 8
    Width = 72
    Height = 16
    Caption = '呼叫内容:'
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label6: TLabel
    Left = 8
    Top = 248
    Width = 40
    Height = 16
    Caption = '机型:'
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label7: TLabel
    Left = 8
    Top = 56
    Width = 40
    Height = 16
    Caption = '台号:'
    Font.Charset = GB2312_CHARSET
    Font.Color = clAqua
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object bbtCall: TBitBtn
    Left = 147
    Top = 264
    Width = 62
    Height = 25
    Caption = '寻呼'
    Font.Charset = GB2312_CHARSET
    Font.Color = clGreen
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnClick = bbtCallClick
    Kind = bkOK
  end
  object cbServer: TComboBox
    Left = 8
    Top = 24
    Width = 129
    Height = 22
    Style = csOwnerDrawFixed
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -14
    Font.Name = '宋体'
    Font.Style = []
    ItemHeight = 16
    ParentFont = False
    TabOrder = 1
    OnClick = cbServerClick
  end
  object cbSex: TComboBox
    Left = 8
    Top = 216
    Width = 129
    Height = 24
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ItemHeight = 16
    MaxLength = 12
    ParentFont = False
    TabOrder = 2
  end
  object mmWord: TMemo
    Left = 144
    Top = 24
    Width = 145
    Height = 233
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    Lines.Strings = (
      '万事如意')
    MaxLength = 150
    ParentFont = False
    TabOrder = 4
  end
  object cbType: TComboBox
    Left = 8
    Top = 264
    Width = 129
    Height = 22
    Style = csOwnerDrawFixed
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ItemHeight = 16
    ParentFont = False
    TabOrder = 3
    Items.Strings = (
      '中文寻呼'
      '数字寻呼')
  end
  object mmStatus: TMemo
    Left = 0
    Top = 296
    Width = 296
    Height = 67
    Align = alBottom
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clNavy
    Font.Height = -14
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 5
  end
  object cbCallNum: TComboBox
    Left = 8
    Top = 120
    Width = 129
    Height = 24
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ItemHeight = 16
    ParentFont = False
    TabOrder = 6
  end
  object cbName: TComboBox
    Left = 8
    Top = 168
    Width = 129
    Height = 24
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ItemHeight = 16
    MaxLength = 12
    ParentFont = False
    TabOrder = 7
  end
  object cbServerCode: TComboBox
    Left = 8
    Top = 72
    Width = 129
    Height = 22
    Style = csOwnerDrawFixed
    Color = clAqua
    Font.Charset = GB2312_CHARSET
    Font.Color = clMaroon
    Font.Height = -14
    Font.Name = '宋体'
    Font.Style = []
    ItemHeight = 16
    ParentFont = False
    TabOrder = 8
  end
  object BitBtn1: TBitBtn
    Left = 232
    Top = 264
    Width = 59
    Height = 25
    Caption = '关于'
    Font.Charset = GB2312_CHARSET
    Font.Color = clGreen
    Font.Height = -16
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
    OnClick = BitBtn1Click
    Kind = bkRetry
  end
end
