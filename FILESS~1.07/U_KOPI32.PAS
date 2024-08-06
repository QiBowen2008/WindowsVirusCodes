UNIT U_KOPI32;

INTERFACE

  FUNCTION QuickCopy  (Quelle, Ziel: STRING) : BOOLEAN;
  FUNCTION QuickFresh (Quelle, Ziel: STRING) : BOOLEAN;

IMPLEMENTATION

USES
  Messages, SysUtils, Classes, Controls, Forms,
  Dialogs;


{ ++++ Kopierroutinen ++++ }

{ Eine einfache Kopierroutine }
{ --------------------------- }
{ Diese Funktion kopiert eine Quell- in eine Zieldatei hinein.
  Dabei wird zugleich der Time-Stamp der Zieldatei mit dem der
  Quelldatei gleichgesetzt.
  Es erfolgt keine Prüfung auf Überschreiben der Zieldatei!
  Als Vorbedingung gilt: Die Quelldatei muß vorhanden sein.
  Ergebnis der Funktion:
    TRUE       Kopiervorgang gelungen
    FALSE      Es trat ein Fehler auf
               mögliche Fehler
                        Ziellaufwerk nicht bereit
                        Zieldatenträger ist voll
                        Zieldirectory ist voll
                        Zieldatei ist schreibgeschützt }

FUNCTION QuickCopy ( Quelle, Ziel : STRING ) : BOOLEAN;
VAR
  S, T: TFileStream;

BEGIN
  Result := FALSE;
  IF FileExists(Ziel) AND ((FileGetAttr (Ziel) AND faReadOnly) > 0)
    THEN MessageDlg(InttoStr(FileGetAttr(Ziel) AND faReadOnly)+'; Zieldatei '+Ziel+' ist schreibgeschützt.'+#13+#13+
                    'Heben Sie den Schreibschutz auf und '+
                    'führen Sie das Programm nochmals aus',
                    mtInformation, [mbOk],0)
    ELSE BEGIN
           Result := TRUE;
           S := TFileStream.Create( Quelle, fmOpenRead );
           TRY
             TRY
               T := TFileStream.Create( Ziel, fmOpenWrite OR fmCreate );
             EXCEPT
               Screen.Cursor := crDefault;
               MessageDlg('Fehler beim Erzeugen der Zieldatei'+#13+Ziel, mtError, [mbOK], 0);
               Result := FALSE;
             END;
             TRY
               TRY
                 T.CopyFrom( S, S.Size ) ;
                 FileSetDate( T.Handle, FileGetDate( S.Handle ) );
                 { Dateizeit setzen }
               EXCEPT
                 Screen.Cursor := crDefault;
                 MessageDlg('Fehler beim Kopieren der Zieldatei'+#13+Ziel, mtError, [mbOK], 0);
                 Result := FALSE
               END;
             FINALLY
               T.Free
             END;
           FINALLY
             S.Free
           END
         END
END;

{ Kopierroutine, die nur kopiert, wenn
   1. die Zieldatei da ist
   2. die Zieldatei älter als die Quelldatei ist }

FUNCTION QuickFresh (Quelle, Ziel: STRING) : BOOLEAN;
BEGIN
  IF FileAge(Quelle) > FileAge(Ziel)
    THEN Quickfresh := QuickCopy (Quelle, Ziel)
    ELSE Quickfresh := FALSE
END;


END.
