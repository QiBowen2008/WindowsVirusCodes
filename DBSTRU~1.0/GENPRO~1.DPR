program GenProcToCreateDB;

(*****************************************************************************************

 This program can read the structure of simple database tables and generate code that
 can be "included" or copied into programs. This code, when executed, will produce
 empty copies of the original database tables (with the fields and indexes preserved).

 The program will also create a text decription of the fields and indexes.  This can be
 used to document the tables of a database.

 Pgm. 02/23/1998 by John Herbster for creating new empty databases for the EYH pgm.
     (Tel. 281-558-2708 or e-mail JHerbster@compuserve.com)

 This program is copyrighted and licensed as shareware.  You may distribute it intact
     freely.  Try it before you buy, but if you continue to use it, the author requests
     US$10 registration payment be sent to Herbster Scientific, 12807 Park Royal,
     Houston, TX 77077-2249, USA.  In return, you will get a confirmation and notice of
     any corrections, enhancements, and free updates for a year. Further, if enough
     people register and suggest improvements, I will add other features such as handling
     of constraints and referential integrity.

 For your reference and education, the source code is included.  If you would like to
     make derivatives of it for further distribution or sale, the author requests
     recognition and a modest share of the profit.  Call or write to negotiate.

*****************************************************************************************)

uses
  Forms,
  GenProcToCreateDB_MainFrm in 'GenProcToCreateDB_MainFrm.pas' {MainFrm};

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TMainFrm, MainFrm);
  Application.Run;
end.
