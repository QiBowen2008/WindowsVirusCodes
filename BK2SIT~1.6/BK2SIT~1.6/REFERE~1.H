// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
//
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// -*- mode:C++; tab-width:4; c-basic-offset:2; indent-tabs-mode:nil -*- 
//
// $Id: reference.H,v 1.44 2000/12/15 23:45:57 jmvidal Exp $

#ifndef REFERENCE_H 
#define REFERENCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include <string>
#include <strstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>

#include "element.H"
#include "iwebstream.H"
#include "funstring.H"

using namespace std;

const string urlsDBHeader = "#bk2site urls.db generated file";

const string addDate = "ADD_DATE";
const string lastModified = "LAST_MODIFIED";
const string lastVisit = "LAST_VISIT";
const string href = "HREF";
const string ALIASID = "ALIASID";
const string ALIASOF = "ALIASOF";
const string ALIAS = "ALIAS";
const string PRIVATE = "PRIVATE";
const string PUBLISH = "PUBLISH";
const string INCLUDE = "INCLUDE";
const string DIRNAME = "DIRNAME";

const string placeholder = "<!--bk2site";
const string placeholderEnd = "-->";

const int NUMVARS = 22;
const string varNames[NUMVARS] = 
{"NEWGIF", "DESTDIR", 
 "TOPFOLDER","BOOKMARKFILE","INDEXFILENAME","OTHERFILENAME",
 "TIMECUTOFF","TOP", "INDEX", "SEARCH", "SEARCHTOROOTPATH", "URLLOGFILE",
 "HITSTIMECUTOFF", "SUBDIRSEP", "BACKDIRSEP", "SEARCHURLTEMPLATE", 
   "NEWSTOPFOLDER", "TITLE", "OUTPUTBOOKMARKFILE", "COMMENTS", "FOLDERCREATION",
   "TOPHITSINCLUDENEWS"};

const int NEWGIF = 0;
const int DESTDIR = 1;
const int TOPFOLDER = 2;
const int BOOKMARKFILE = 3;
const int INDEXFILENAME = 4;
const int OTHERFILENAME = 5;
const int TIMECUTOFF = 6;
const int TOP = 7;
const int INDEX = 8;
const int SEARCH = 9;
const int SEARCHTOROOTPATH = 10;
const int URLLOGFILE = 11;
const int HITSTIMECUTOFF = 12;
const int SUBDIRSEP = 13;
const int BACKDIRSEP = 14;
const int SEARCHURLTEMPLATE = 15;
const int NEWSTOPFOLDER = 16;
const int TITLE = 17;
const int OUTPUTBOOKMARKFILE = 18;
const int COMMENTS = 19;
const int FOLDERCREATION = 20;
const int TOPHITSINCLUDENEWS = 21;

const int NUMFORMATVARS = 36;
const string FORMATVARS[NUMFORMATVARS] = { //they are all prepended by %
    "%IFCOMHAS(", //%IFCOMHAS(string)(html) MUST appear before comment on this list.
    "%URL", //the URL
    "%NOACCENTURL", //the URL
    "%TITLE", //the title
    "%NOACCENTTITLE", //the title
    "%NHCOMMENT", //the comment without HTML in it.
    "%NOACCENTCOMMENT", //the comment without HTML in it.
    "%COMMENT", //the comment
    "%FILLCOMMENT", //the comment, autofilled
    "%FSCOMMENT", //the first sentence in the comment
    "%LONGCOMMENT", //the comment in one long line
    "%SHORTCOMMENT(",
    "%JSCOMMENT", //comment suitable for use as a javascript string
    "%JSFSCOMMENT", //first sentence for js
    "%DAYCRE", //the day created
    "%DAYVIS", //the day last visited
    "%DAYMOD", //the day last Modified
    "%MONTHCRE1", //the month created, in number
    "%MONTHCRE2", //the month created, in English
    "%MONTHVIS1", //the month created, in number
    "%MONTHVIS2", //the month created, in English
    "%MONTHMOD1", //the month created, in number
    "%MONTHMOD2", //the month created, in English
    "%YEARCRE",
    "%YEARFCRE",
    "%YEARVIS",
    "%YEARFVIS",
    "%YEARMOD",
    "%YEARFMOD",
    "%LEAFS", //the total number of leafs under it (for folders only)
    "%NEW", //the new.gif, if the time is right.
    "%TIMEFCRE", //the time created in full "Wed Jun 30 21:49:03 1993"
    "%TIMEFMOD", //the time modified in full "Wed Jun 30 21:49:03 1993"
    "%TIMEFVIS", //the time visited in full "Wed Jun 30 21:49:03 1993"
    "%CONDDASH", //prints a dash "-" if the comment is not null
    "%HITS" //the number of hits it has received.
};


class referenceTree;
class element;
class fileView;
class channelContainer;

class reference {
  
public: 
  string url;
  string title;
  string dirname;
  string comment;
  string commentCommands; //sort commands that used to be in the comment.
  time_t creationTime;
  time_t modifiedTime;
  time_t visitTime;
  string aliasID; //contains the reference's IDNUM (as given by Navigator)
  string aliasOf; //contains the IDNUM of the bookmark it refers to. We ignore these.
  string aliasDir;
  referenceTree * children;
  bool priv;
  bool aliasof;
  //the navigation bar (HTML) under which we reside
  //this is used when doing << and then >>, for search.
  //The urls are prepended with SEARCHTOROOTPATH and then the path from there.
  string navBar;
  bool folder;
  int hits;
  
  reference() : url(""), title(""), dirname(""), comment (""), commentCommands(""), creationTime(0), 
    modifiedTime(0), visitTime(0), aliasDir(""), aliasID(""), aliasOf(""), 
    children(0), priv(false), aliasof(false), navBar(""), folder(false),
    hits(0){
  };
  reference(const reference &r);
  reference(const string & str) : url(""), title(""), dirname(""), comment(""), commentCommands(""), creationTime(0),
                                  modifiedTime(0), visitTime(0), aliasDir(""), aliasID(""), aliasOf(""), children(0), 
                                  priv(false),aliasof(false),navBar(""), folder(false), hits(0){ 
    setValues(str);};
  reference& operator=(const reference & r);
  void setValues (const string & str); 
  
  int operator<(const reference & r) const;
  ~reference();
  int search(const string & s);
  void sendAsHTML(ostream & out); //for search
  string sendAsHTML(const string & templ, const string varValues []) const;
  bool isFolder() const;
  bool isPrivate() const;
  bool isAliasof() const;
  string::size_type fullstop(const string &s, const string &p) const;
  //  friend ostream &operator<<(ostream&, const reference &);
  friend iwebstream &operator>>(iwebstream&, reference &);
  void writeAsFlatFile(ostream& output) const;
  void writeAsBookHelper(ostream& output, string & prepend) const;
  void writeAsXBELHelper(ostream& output, string & prepend) const;
  bool readXml(iwebstream & is);
};

#include "channel.H"

class referenceCmpCreation {
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.creationTime > r2.creationTime;}
};

class referenceCmpCreationI { //the inverse
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.creationTime < r2.creationTime;}
};

class referenceCmpTitle {
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.title < r2.title;}
};

class referenceCmpTitleI {
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.title > r2.title;}
};

class referenceCmpModified { //latest modified is first (i.e. "less")
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.modifiedTime > r2.modifiedTime;}
};

class referenceCmpModifiedI { 
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.modifiedTime < r2.modifiedTime;}
};

class referenceCmpVisit { 
public:
  int operator()(const reference&r1, const reference&r2) const
  { return r1.visitTime > r2.visitTime;}
};

class referenceCmpVisitI { 
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.visitTime < r2.visitTime;}
};

class referenceCmpHits {
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.hits > r2.hits;}
};

class referenceCmpHitsI {
public:
  int operator()(const reference&r1, const reference&r2) const
    { return r1.hits < r2.hits;}
};



static int numMerges = 0; //we add this number to all the aliases that we merge into the rt.

class referenceTree {
    
  vector<reference> contents;
  int numLeafs; //below me.
  string getToken(iwebstream & is);
public:
  string comment; //The comment for a folder appears here AND in the reference.
  referenceTree() : numLeafs(0), comment(""){};
  referenceTree(const referenceTree &rt): numLeafs(rt.numLeafs) {
    contents = rt.contents;
    comment = rt.comment;};
  referenceTree& operator=(const referenceTree &rt){
    contents = rt.contents; 
    numLeafs = rt.numLeafs;
    comment = rt.comment; 
    return *this;};
  
  ~referenceTree() {};
  void addReference(const reference & r){
    contents.push_back(r);};
  int getNumLeafs(){return numLeafs;};
  string getPath(const string & title, const string & dir, const string & subdirsep) const;
  void increaseHits(string & url, int x);

  referenceTree * getFirstGrandchild(){
    vector<reference>::iterator iter = contents.begin();
    return (*iter).children;
  }

  time_t setFolderCreationToMaxDescendant();
  time_t setFolderCreationToMaxChildren();
  void fixAliases(referenceTree * rootRT, const string & dir, const string varValues [],
		  const string & path);
  referenceTree * getSubtree(const string & topName);
  void createSite(const string varValues[], channelContainer & channels);
  void createSiteH(const string varValues[], vector<reference> & allReferences, channelContainer & channels,
				fileView & baseView, fileView & otherView,
				vector<reference> & newsItems, int depth = 0, 
				string navigateBar = "", string searchNavBar = "",
				string parentTitle = "", string folderTitle ="",
		   		string filePath = "");
  void createPage(const string indexFileName, const string ofileName, const string varValues [],
		  channelContainer & channels);
  void makeVector(vector<reference> & vr, const string & avoidFolder = "");
  friend iwebstream &operator>>(iwebstream&, referenceTree &);
  //  friend ostream &operator<<(ostream&, const referenceTree &);
  void doSort(); //checks the comment and does appropiate sort.
  void writeAsFlatFile(ostream& out) const;
  void writeAsBookmarkFile(ostream & out, string title) const;
  void writeAsBookHelper(ostream& out, string & prepend) const;
  void writeAsXBELHelper(ostream& out, string & prepend) const;
  void writeAsXBELFile(ostream & out, string title) const;
  int fixNumChildren();
  void addTree(const referenceTree & rt);
  void merge(const referenceTree & rt);

};

#endif

// -*-
// Local Variables:
// mode: C++
// tab-width: 4
// c-basic-offset:2
// indent-tabs-mode: nil 
// End:
