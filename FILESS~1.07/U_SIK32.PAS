UNIT U_SIK32;

INTERFACE

USES
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, StdCtrls, Buttons, FileCtrl, Grids, Outline,
  DirOutln, ExtCtrls;

TYPE
  TfrmSynchro = CLASS(TForm)
    QuellPathSelector        : TDirectoryListBox;  // Auswahl Quell-Laufwerk
    QuellDriveSelector       : TDriveComboBox;     // Auswahl Quell-Pfad
    ZielDriveSelector        : TDriveComboBox;     // Auswahl Ziel-Laufwerk
    ZielPathSelector         : TDirectoryListBox;  // Auswahl Ziel-Pfad
    lblQuelle                : TLabel;             // Überschrift Quelle
    lblZiel                  : TLabel;             // Überschrift Ziel

    bxKopierOpt              : TGroupBox;          // Kopieroptionen
      optDoNurVorh           : TCheckBox;          // Option nur vorhandene kopieren
      optDoUnterVerz         : TCheckBox;          // Option inkl. Unterverzeichnisse
      optDoQuellUpdate       : TCheckBox;          // Option Update älterer Quelldateien
      optDoDelSrc            : TCheckBox;          // Option Löschen Quelldatei
      Praefix                : TEdit;              // Quelldateiselektion
      Suffix                 : TEdit;              // Quelldateiselektion

    bxAnzeigeOpt             : TGroupBox;          // Ergebnisanzeigeoptionen
      optShowSuchpfade       : TCheckBox;          // Suchpfade protokollieren
      optShowKopiert         : TCheckBox;          // Neu kopierte Zieldateien protokollieren
      optShowUpdate          : TCheckBox;          // Upgedatete Zieldateien protokollieren
      optShowKilled          : TCheckBox;          // Gelöschte Quelldateien protokollieren
      optShowQuellUpdate     : TCheckBox;          // Upgedatete Quelldateien protokollieren
      optShownichtVorh       : TCheckBox;          // nicht gefundene Zieldateien protokollieren
      optShowIdent           : TCheckBox;          // identische Zieldateien protokollieren

    btnStart                 : TBitBtn;            // Start der Synchronisation
    FileList                 : TListBox;           // Fenster für Protokollanzeige
    btnClose                 : TBitBtn;            // Programmende
    btnSwap                  : TButton;
    imgAbout: TImage;            // Austausch Quell- und Zielpfad

    PROCEDURE FormCreate(Sender: TObject);
    PROCEDURE btnStartClick(Sender: TObject);      // Starten Synchronisation
    PROCEDURE btnCloseClick(Sender: TObject);      // Schließen der Anwendung
    PROCEDURE btnSwapClick(Sender: TObject);       // Austauschen Quell- und Zielpfad
    PROCEDURE optDoQuellUpdateClick(Sender: TObject); // Änderung Option -> Ändert Anzeigeopt.
    PROCEDURE optDoDelSrcClick(Sender: TObject);      // Änderung Option -> Ändert Anzeigeopt.
    PROCEDURE optDoNurVorhClick(Sender: TObject);     // Änderung Option -> Ändert Anzeigeopt.
    PROCEDURE QuellPathSelectorChange(Sender: TObject); // Setzen der Quellpfaddaten
    PROCEDURE ZielPathSelectorChange(Sender: TObject);  // Setzen der Zielpfaddaten
    PROCEDURE FileListClose(Sender: TObject);
    PROCEDURE ShowAbout(Sender: TObject);       // Schließen Ergebnisausgabe

  PRIVATE
    SrcFound    : LONGINT;    // Anzahl gefundener Quelldateien
    SrcEqualDst : LONGINT;    // Anzahl gleicher Quell-/Zieldateien
    SrcCopied   : LONGINT;    // Anzahl neu kopierter Quelldateien
    DstUpdated  : LONGINT;    // Anzahl upgedateter Zieldateien
    DstFound    : LONGINT;    // Anzahl gefundener Zieldateien
    SrcKilled   : LONGINT;    // Anzahl gelöschter Quelldateien
    QuellPfad   : STRING;     // Quellpfad, ab dem gesucht wird
    ZielPfad    : STRING;     // Zielpfad, ab dem synchronisiert wird

    PROCEDURE ScanTree(abWo:STRING);  // Durchsuchen inkl. Unterverzeichnisse
    PROCEDURE ScanDir (abWo:STRING);  // Durchsuchen eines Verzeichnisses
    PROCEDURE processFile(CONST FileName:STRING);  // Handling der Synchronisation

  PUBLIC
    { Public-Deklarationen }

  END;

VAR
  frmSynchro: TfrmSynchro;

IMPLEMENTATION

{$R *.DFM}

USES
  U_KOPI32, ABOUT95;

PROCEDURE TfrmSynchro.QuellPathSelectorChange(Sender: TObject);
BEGIN
  QuellPfad := QuellPathSelector.Directory;  // Setzen des Quellpfades
END;

PROCEDURE TfrmSynchro.ZielPathSelectorChange(Sender: TObject);
BEGIN
  ZielPfad := ZielPathSelector.Directory;    // Setzen des Zielpfades
END;

PROCEDURE TfrmSynchro.FormCreate(Sender: TObject);
BEGIN
  Praefix.TEXT := '*';        // Defaultvorgabe
  Suffix.TEXT:='*';           // Defaultvorgabe
  optShowQuellupDate.Enabled := optDoQuellUpdate.Checked;
  optShowKilled.Enabled      := optDoDelSrc.Checked;
  optShowKopiert.Enabled     := NOT optDoNurVorh.Checked;
  QuellPfad                  := QuellPathSelector.Directory;
  ZielPfad                   := ZielPathSelector.Directory;
END;


PROCEDURE TfrmSynchro.btnStartClick(Sender: TObject);
BEGIN
  IF AnsiCompareText(Quellpfad,Zielpfad) = 0
    THEN ShowMessage('Dateien können nicht auf sich selbst kopiert werden.')
    ELSE BEGIN
           SrcFound    := 0;
           SrcEqualDst := 0;
           SrcCopied   := 0;
           DstUpdated  := 0;
           DstFound    := 0;
           SrcKilled   := 0;
           QuellPathSelector.Hide;
           ZielPathSelector.Hide;
           QuellDriveSelector.Hide;
           ZielDriveSelector.Hide;
           FileList.Clear;
           FileList.Show;
           Screen.Cursor:=crHourGlass;
           IF optDoUnterVerz.checked
             THEN ScanTree(Quellpfad)
             ELSE ScanDir (Quellpfad+'\');
           Screen.Cursor:=crDefault;
           FileList.Items.Add('===========================================');
           FileList.Items.Add('gefundene Quelldateien     '+IntToStr(SrcFound));
           FileList.Items.Add('gefundene  Zieldateien     '+IntToStr(DstFound));
           FileList.Items.Add('nicht gef. Zieldateien     '+IntToStr(SrcFound-DstFound));
           FileList.Items.Add('gleiche Zieldateien        '+IntToStr(SrcEqualDst));
           FileList.Items.Add('neu kopierte Quelldateien  '+IntToStr(SrcCopied));
           FileList.Items.Add('upgedatete Zieldateien     '+IntToStr(DstUpdated));
           FileList.Items.Add('gelöschte Quelldateien     '+IntToStr(SrcKilled))
         END
END;


// Zwei Suchroutinen: rekursive und lineare Verzeichnisstrukturen
// abWo endet dabei immer mit \

PROCEDURE TfrmSynchro.ScanTree(abWo:STRING);
VAR
  SR: TSearchRec;
  Result : WORD;

BEGIN
  IF abWo[LENGTH(abWo)] <> '\'
    THEN abWo := abWo + '\';
  Result := FindFirst(abWo+'*.*',faAnyFile,SR);   // Suche nach allen Eintraegen
  WHILE (Result=0) DO
  BEGIN
    IF (SR.Name[1]<>'.') AND (SR.Attr AND faDirectory <> 0) // falls echtes Verzeichnis
      THEN ScanTree(abWo+SR.NAME); // Von vorn mit neuem Verzeichnis
    Result := FINDNEXT(SR);
  END;
  ScanDir(abWo);     // auf dem Rückwege Durchsuchen aller gefundenen Verzeichnisse
  FindClose(SR);
END;

PROCEDURE TfrmSynchro.ScanDir(abWo:STRING);
VAR
  SR: TSearchRec;
  Result : WORD;

BEGIN
  Result := FindFirst(abWo+Praefix.TEXT+'.'+Suffix.TEXT,faAnyFile AND NOT faDirectory,SR);   {Suche Nach allen Eintraegen}
  IF optShowSuchpfade.Checked
    THEN FileList.Items.Add('Durchsuche '+abWo);
  WHILE (Result=0) DO
  BEGIN
    processFile(abWo+SR.Name);
    Result := FINDNEXT(SR);
  END;
  FindClose(SR);
END;


PROCEDURE TfrmSynchro.processFile(CONST FileName:STRING);
VAR
  SrcFile  : STRING;
  DstFile  : STRING;
  DstExist : BOOLEAN;
  B        : BYTE;

BEGIN
  FileList.Update;                  // Ausgabefenster aktualisieren, dauert sonst so lange
  INC (SrcFound);                   // zähle alle gefundenen Quelldateien
  SrcFile := FileName;              // Sourcefilename: Y:\PS1\21-2_1\Beispiel\test.pas
  DstFile := SrcFile;               // Zielfilename  : Y:\PS1\21-2_1\Beispiel\test.pas
  FOR B := 1 TO LENGTH(Quellpfad) DO  // herausnehmen Quellpfad (z.B. Y:\PS1)
    DELETE (DstFile,1,1);           // aus Zielfilename: \21-2_1\beispiel\test.pas

  DstFile := ZielPfad+'\'+DstFile; // neues Zielfile aufbauen

{ Beachtung, daß Roots (Y:\) mit \ enden: Folgende DstFiles können auftreten:
  QPfad   ZPfad       Srcfile           Dstfile       Dstfile wie folgt zu konkatinieren
  Y:\PS1  K:\mueller  Y:\PS1\21-2\...   \21-2\...     ZPfad+Dstfile: k:\mueller\21-2\...
  Y:\     K:\mueller  Y:\PS1\21-2\...   PS1\21-2\...  ZPfad+'\'+Dstfile: K:\mueller\21-2\...
  Y:\PS1  K:\         Y:\PS1\21-2\...   \21-2\...     ZPfad+Dstfile: k:\\21-2\...
  Y:\     K:\         Y:\PS1\21-2\...   PS1\21-2\...  ZPfad+DstFile: K:\PS1\21-2\...
  Daher: nach Konkatinierung evtl. doppelt vorhandene \ herauslöschen }

  B := 1;    // doppelte \\ herausnehmen
  WHILE B < LENGTH(DstFile)-1 DO
    IF (DstFile[B] = '\') AND (Dstfile[B+1] = '\')
      THEN DELETE (DstFile,B,1)
      ELSE INC(B);

  DstExist := FileExists(DstFile);  // Gibt's das Zielfile
  IF DstExist THEN INC (DstFound);  // zähle gefundene Zieldateien

  IF NOT DstExist                   // Zielfile nicht da
    THEN IF optDoNurVorh.Checked         // und nur vorhandene gewählt
           THEN BEGIN
                  IF optShowNichtVorh.Checked
                    THEN FileList.Items.Add(DstFile + ' nicht vorhanden');
                END
           ELSE BEGIN               // Zielfile nicht da, aber nicht nur vorh. gewählt
                  ForceDirectories(ExtractFileDir(DstFile));
                  IF Quickcopy (SrcFile,DstFile)
                    THEN BEGIN
                           IF optShowKopiert.Checked
                             THEN FileList.Items.Add(DstFile+' neu angelegt');
                           INC (SrcCopied);
                           IF OptDoDelSrc.Checked
                             THEN BEGIN
                                    IF DeleteFile(SrcFile)
                                      THEN BEGIN
                                             INC(SrcKilled);
                                             IF optShowKilled.Checked
                                               THEN FileList.Items.Add(SrcFile+' gelöscht')
                                            END
                                       ELSE FileList.Items.Add(SrcFile+' konnte nicht gelöscht werden')
                                  END
                         END
                END                 // Zielfile nicht da, aber nicht nur vorh. gewählt
    ELSE BEGIN                      // Zielfile da
           IF QuickFresh (SrcFile,DstFile)
             THEN BEGIN             // Zielfile wurde aktualisiert
                    IF optShowUpdate.Checked
                      THEN FileList.Items.Add(DstFile+' aktualisiert');
                    INC(DstUpdated);
                  END
             ELSE BEGIN             // Zielfile wurde nicht aktualisiert
                    IF optShowIdent.Checked
                      THEN FileList.Items.Add(DstFile+' ist identisch');
                    INC(SrcEqualDst)
                  END;
            IF optDoQuellUpdate.Checked
              THEN IF QuickFresh(DstFile, SrcFile)  // Quellfile zu aktualisieren?
                     THEN IF optShowQuellUpdate.Checked
                            THEN FileList.Items.Add(SrcFile+' ! upgedatet');
            IF optDoDelSrc.Checked
              THEN BEGIN
                     IF DeleteFile(SrcFile)
                       THEN BEGIN
                              INC(SrcKilled);
                              IF optShowKilled.Checked
                                THEN FileList.Items.Add(SrcFile+' gelöscht')
                            END
                       ELSE FileList.Items.Add(SrcFile+' konnte nicht gelöscht werden')
                    END
         END
END;


PROCEDURE TfrmSynchro.btnCloseClick(Sender: TObject);
BEGIN
  CLOSE;
END;


PROCEDURE TfrmSynchro.btnSwapClick(Sender: TObject);
VAR
  Drive : Char;
  Pfad  : STRING;

BEGIN
// Merken der Einstellungen Quellpfad
  Drive := QuellDriveSelector.Drive;
  Pfad  := QuellPathSelector.Directory;
// Quelle = Ziel
  QuellDriveSelector.Drive := ZielDriveSelector.Drive;
  QuellPathSelector.Directory := ZielPathSelector.Directory;
// Ziel = gemerkte Einstellungen
  ZielDriveSelector.Drive := Drive;
  ZielPathSelector.Directory := Pfad;
END;


PROCEDURE TfrmSynchro.optDoQuellUpdateClick(Sender: TObject);
BEGIN
  optShowQuellUpdate.Enabled := optDoQuellUpdate.Checked;
END;

PROCEDURE TfrmSynchro.optDoDelSrcClick(Sender: TObject);
BEGIN
  optShowKilled.Enabled := optDoDelSrc.Checked;
END;

PROCEDURE TfrmSynchro.optDoNurVorhClick(Sender: TObject);
BEGIN
  optShowKopiert.Enabled := NOT optDoNurVorh.Checked;
END;


PROCEDURE TfrmSynchro.FileListClose(Sender: TObject);
BEGIN
  QuellPathSelector.Show;
  ZielPathSelector.Show;
  QuellDriveSelector.Show;
  ZielDriveSelector.Show;
  FileList.Hide;
END;

PROCEDURE TfrmSynchro.ShowAbout(Sender: TObject);
BEGIN
  Application.CreateForm(TfrmAbout, frmAbout);
  frmAbout.ShowModal;
END;

END.
