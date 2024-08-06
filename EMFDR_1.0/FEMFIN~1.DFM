object fmURL: TfmURL
  Left = 192
  Top = 107
  Width = 573
  Height = 408
  Caption = 'Email Finder'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object pnSetup: TPanel
    Left = 0
    Top = 0
    Width = 565
    Height = 116
    Align = alTop
    TabOrder = 0
    object lbKey: TLabel
      Left = 8
      Top = 92
      Width = 58
      Height = 13
      Caption = 'Key Words: '
    end
    object editKey: TEdit
      Left = 70
      Top = 88
      Width = 224
      Height = 21
      TabOrder = 0
      OnKeyPress = editKeyKeyPress
    end
    object btnTran: TBitBtn
      Left = 297
      Top = 86
      Width = 116
      Height = 25
      Caption = 'Translate To URL'
      TabOrder = 1
      OnClick = btnTranClick
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333033333
        33333333373F33333333333330B03333333333337F7F33333333333330F03333
        333333337F7FF3333333333330B00333333333337F773FF33333333330F0F003
        333333337F7F773F3333333330B0B0B0333333337F7F7F7F3333333300F0F0F0
        333333377F73737F33333330B0BFBFB03333337F7F33337F33333330F0FBFBF0
        3333337F7333337F33333330BFBFBFB033333373F3333373333333330BFBFB03
        33333337FFFFF7FF3333333300000000333333377777777F333333330EEEEEE0
        33333337FFFFFF7FF3333333000000000333333777777777F33333330000000B
        03333337777777F7F33333330000000003333337777777773333}
      NumGlyphs = 2
    end
    object GroupBox2: TGroupBox
      Left = 205
      Top = 4
      Width = 145
      Height = 69
      Caption = 'Proxy Setup'
      TabOrder = 3
      object Label6: TLabel
        Left = 8
        Top = 20
        Width = 31
        Height = 13
        Caption = 'Server'
      end
      object Label7: TLabel
        Left = 8
        Top = 44
        Width = 19
        Height = 13
        Caption = 'Port'
      end
      object edProxyServer: TEdit
        Left = 48
        Top = 16
        Width = 89
        Height = 21
        TabOrder = 0
      end
      object edProxyPort: TEdit
        Left = 48
        Top = 40
        Width = 89
        Height = 21
        TabOrder = 1
      end
    end
    object GroupBox1: TGroupBox
      Left = 8
      Top = 4
      Width = 185
      Height = 69
      Caption = 'Search Setup'
      TabOrder = 2
      object lbLayer: TLabel
        Left = 8
        Top = 20
        Width = 108
        Height = 13
        Caption = 'Max Layer To Search: '
      end
      object lbThread: TLabel
        Left = 8
        Top = 44
        Width = 101
        Height = 13
        Caption = 'Max Thread To Use: '
      end
      object speLayer: TSpinEdit
        Left = 122
        Top = 15
        Width = 47
        Height = 22
        MaxValue = 100
        MinValue = 1
        TabOrder = 0
        Value = 1
      end
      object speThread: TSpinEdit
        Left = 122
        Top = 39
        Width = 47
        Height = 22
        MaxValue = 100
        MinValue = 1
        TabOrder = 1
        Value = 1
      end
    end
    object RadioGroup1: TRadioGroup
      Left = 361
      Top = 4
      Width = 145
      Height = 69
      Caption = 'Line Setup'
      TabOrder = 4
    end
    object rdb56K: TRadioButton
      Left = 376
      Top = 19
      Width = 113
      Height = 17
      Caption = '28.8K To 56K'
      Checked = True
      TabOrder = 5
      TabStop = True
    end
    object rdb2M: TRadioButton
      Left = 376
      Top = 36
      Width = 113
      Height = 17
      Caption = '56K To 2M'
      TabOrder = 6
    end
    object rdb10M: TRadioButton
      Left = 376
      Top = 53
      Width = 113
      Height = 17
      Caption = '2M To 100M'
      TabOrder = 7
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 362
    Width = 565
    Height = 19
    Panels = <
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 60
      end>
    SimplePanel = False
  end
  object MemoURL: TMemo
    Left = 0
    Top = 116
    Width = 565
    Height = 205
    Align = alClient
    Lines.Strings = (
      '                                                       Help'
      ''
      
        '  emFinder is an email address automatic finder on internet, it ' +
        'can be used to'
      
        'comb through web pages, looking for email address as it goes. It' +
        ' will automaticly'
      
        'follow links as many layers deep as you wish, and you can use ma' +
        'ny threads to'
      'follow many URLs simultaneously.'
      ''
      
        '  You can use it very simply, first of all you must connect to i' +
        'nternet, then type'
      'some URLs in the text box line by line like this:'
      ''
      'http://www.yahoo.com/'
      'http://www.sohu.com/'
      '......'
      '......'
      
        '  Then click the RUN button, this program will do all work for y' +
        'ou, it will write'
      
        'email address it found to a file named FOUNDEMAILS.LOG in the cu' +
        'rrent directory.'
      ''
      
        '  If you are using a proxy server to connect to internet, you mu' +
        'st fill Server and'
      'Port parameter, otherwise you should leave them blank.'
      ''
      
        '  Max Layer To Search parameter can be setup from 1 to 100, defa' +
        'ult value is 1. You can'
      'increase the value if you wish to go deep.'
      ''
      
        '  Max Thread To Use parameter can be setup from 1 to 100, defaul' +
        't value is 1. On a dialup,'
      
        'usally 3 is good. Over good bandwidth, try 3-6. More than 10 rea' +
        'lly has little use.'
      ''
      
        '  You can also setup the line speed, if you have a high speed li' +
        'ne, you can select any'
      
        'one which speed equals or lower than your line speed. For exampl' +
        'e, if you have 100M'
      
        'bandwidth, you may select 28.8-56K, or 56K-2M or 2M-100M. But if' +
        ' you have 56K bandwidth, you'
      'should not select 56K-2M and 2M-100M.'
      ''
      
        '  Sometimes you may want to search email address from some relat' +
        'e keywords, like:'
      
        #39'email'#39', '#39'mailbox'#39', '#39'mailto'#39', '#39'contact us'#39', '#39'lawyer'#39', '#39'white hou' +
        'se'#39', ...'
      
        'you can type your keywords in the edit box, separate each keywor' +
        'd from a space letter,'
      
        'press enter or click Translate To URL button, the URL will be ad' +
        'ded to the text box.'
      ''
      '  Wish you like this program, send email nicely will be good.'
      ''
      '  !!! CLEAR ALL HELP CONTENT BEFORE YOU ADD ANY URLS !!!'
      ''
      '  Last update 4/4/2001')
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object pnCommand: TPanel
    Left = 0
    Top = 321
    Width = 565
    Height = 41
    Align = alBottom
    TabOrder = 2
    object btnRun: TBitBtn
      Left = 189
      Top = 8
      Width = 84
      Height = 25
      Caption = 'Run'
      TabOrder = 0
      OnClick = btnRunClick
      Glyph.Data = {
        96020000424D960200000000000036000000280000000F000000130000000100
        1000000000006002000000000000000000000000000000000000F75EF75EF75E
        F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EC6FEF75EF75EF75E
        F75EEF3DF75EF75EF75EF75EF75EF75EF75EF75EF75EF75E23F6F75EF75EF75E
        0000EF3DEF3DF75EF75EF75EF75EF75EF75EF75EF75EF75E44F6F75EF75EF75E
        00000000EF3DEF3DF75EF75EF75EF75EF75EF75EF75EF75EE8FEF75EF75EF75E
        0000E0030000EF3DEF3DF75EF75EF75EF75EF75EF75EF75EC2FEF75EF75EF75E
        0000E003E0030000EF3DEF3DF75EF75EF75EF75EF75EF75EC3FEF75EF75EF75E
        0000F75EE003F75E0000EF3DEF3DF75EF75EF75EF75EF75EC3FEF75EF75EF75E
        0000E003E003E003E0030000EF3DEF3DF75EF75EF75EF75EA0FEF75EF75EF75E
        0000E003F75EE003F75EE0030000EF3DEF3DF75EF75EF75EA0FEF75EF75EF75E
        0000E003E003E003E003E003E0030000F75EF75EF75EF75EA0FEF75EF75EF75E
        0000F75EE003F75EE003F75E0000F75EF75EF75EF75EF75EA0FEF75EF75EF75E
        0000E003E003E003E0030000F75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        0000E003F75EE0030000F75EF75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        0000E003E0030000F75EF75EF75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        0000F75E0000F75EF75EF75EF75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        00000000F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        0000F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EA0FEF75EF75EF75E
        F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EA0FE}
    end
    object btnStop: TBitBtn
      Left = 296
      Top = 8
      Width = 84
      Height = 25
      Caption = 'Stop'
      Enabled = False
      TabOrder = 1
      OnClick = btnStopClick
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333FFFFF3333333333999993333333333F77777FFF333333999999999
        33333337777FF377FF3333993370739993333377FF373F377FF3399993000339
        993337777F777F3377F3393999707333993337F77737333337FF993399933333
        399377F3777FF333377F993339903333399377F33737FF33377F993333707333
        399377F333377FF3377F993333101933399377F333777FFF377F993333000993
        399377FF3377737FF7733993330009993933373FF3777377F7F3399933000399
        99333773FF777F777733339993707339933333773FF7FFF77333333999999999
        3333333777333777333333333999993333333333377777333333}
      NumGlyphs = 2
    end
  end
  object spdEmail: TSpider
    ProxyPort = 80
    RecvBufferSize = 7504
    MaxLayers = 1
    ThreadsToUse = 1
    OnAfterConnect = spdEmailAfterConnect
    OnBeforeConnect = spdEmailBeforeConnect
    OnFailConnect = spdEmailFailConnect
    OnLinkFound = spdEmailLinkFound
    OnPageLoaded = spdEmailPageLoaded
    OnEmailFound = spdEmailEmailFound
    OnGetNextUrlSeed = spdEmailGetNextUrlSeed
    OnError = spdEmailError
    Left = 520
    Top = 24
  end
end
