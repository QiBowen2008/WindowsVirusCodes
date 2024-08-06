// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
//
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// $Id: reference.C,v 1.76 2000/12/15 23:45:57 jmvidal Exp $
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "reference.H"

//Table to convert mac chars to ISO-8859-1, starts at 128.
//
const int MACCHAR[128] = {
  196, 197, 199, 201, 209, 214, 220, 225, 224, 226, 228, 227, 229,
  231, 233, 232, 234, 235, 237, 236, 238, 239, 241, 243, 242, 244,
  246, 245, 250, 249, 251, 252, 43, 176, 162, 163, 167, 183, 182, 223,
  174, 169, 32, 180, 168, 32, 198, 216, 32, 177, 32, 32, 165, 181,
  240, 83, 80, 112, 83, 170, 186, 79, 230, 248, 191, 161, 172, 32,
  102, 126, 68, 171, 187, 32, 160, 192, 195, 213, 32, 32, 32, 32,
  32, 32, 96, 39, 247, 183, 255, 255, 47, 164, 60, 62, 32, 32, 32,
  183, 44, 32, 32, 194, 202, 193, 203, 200, 205, 206, 207, 204,
  211, 212, 183, 210, 218, 219, 217, 105, 94, 126, 175, 94, 183, 176,
  184, 32, 184, 94};

/*
string eliminateAccents(const string & s){
  string c;
  string::iterator ci = c.begin();
  for (string::const_iterator i = s.begin(); i != s.end(); ++ i, ++ci){
    char temp = *i;
    if (temp > 128)
      *ci = '_';
    else *ci = *i;
  };
  return c;
}
*/

/** Returns a copy of s where all the accented and other weird characters
    have been replaced with conterparts */
string replaceAccents(const string & s){
  string res = "";
  for (string::const_iterator i = s.begin(); i != s.end(); ++ i){
    char c = *i;
    unsigned short ascval = (unsigned short)c & (unsigned short)255;
    if ((ascval >= 32 ) && (ascval <= 255)){
      //      cout << ascval << "-" << c << " =" << EQUIVCHAR[ascval-32] << "=" << endl;
      res += EQUIVCHAR[ascval-32];
    }
    else{
      res += c;
      //      cout << "not in range=" << ascval << "-" << c << endl;
    }
  }
  return res;
}
	

//remove all HTML, that is, everything between < and >
void removeHTML(string & s){
  string::size_type start = 0;
  string::size_type end = 0;
  while ( (start = s.find("<")) != string::npos){
    end = s.find(">", start);
    s.replace(start, end-start + 1, ""); 
  }
}

//from Stroustrup book
int cmpNoCase(const string& s, const string &s2)
{
  string::const_iterator p = s.begin();
  string::const_iterator p2 = s2.begin();
  
  while (p != s.end() && p2 != s2.end()) {
    if (toupper(*p) != toupper(*p2))
      return (toupper(*p) < toupper(*p2)) ? -1 : 1;
    ++p;
    ++p2;
  }
  return (s2.size() == s.size()) ? 0 : (s.size() < s2.size()) ? -1 : 1;
}

//replace all instances of s1 with s2 on s
int replaceAll(string & s, string s1, string s2)
{
  int c = 0;
  string::size_type f1 = 0;
  while ((f1 = s.find(s1,f1)) != string::npos) {
    c++;
    s.replace(f1,s1.size(), s2);
    f1 += s2.size();
  }
  return c;
}

/** escape all funny characters */
string javaScriptEscape(const string & s){
  string res = "";
  for (unsigned int i = 0; i < s.size(); ++i){
    if (s[i] == '\\'){
      res += '\\';
      res += '\\';
    }
    else if (s[i] == '\'') {
      res += '\\';
      res += '\'';
    }
    else if (s[i] == '\"') {
      res += '\\';
      res += '\'';
    }
    else if (s[i] == '\n') {
      res += '\\';
      res += 'n';
    }
    else if (s[i] == '\t') {
      res += '\\';
      res += 't';
    }
    else if (s[i] == '\r') {
      res += '\\';
      res += 'n';
    }
    else
      res += s[i];
  }
  return res;
}

// get the month
string getMonth(int m)
{
  switch (m) {
  case 1: return "January"; break;
  case 2: return "February"; break;
  case 3: return "March"; break;
  case 4: return "April"; break;
  case 5: return "May"; break;
  case 6: return "June"; break;
  case 7: return "July"; break;
  case 8: return "August"; break;
  case 9: return "September"; break;
  case 10: return "October"; break;
  case 11: return "November"; break;
  case 12: return "December"; break;
  };
  return "Bad Month";
}


string::size_type findNC(const string & source, 
                         const string & search, 
                         const string::size_type startPos)
{
  string::size_type ssize = search.size();
  if (ssize > source.size())
    return string::npos; //because size_type cannot handle negatives!!
  for (string::size_type i = startPos; i <= source.size() - ssize; i++){
    string tu = source.substr(i, ssize);
    if (cmpNoCase(tu, search) == 0)
      return i;
  }
  return string::npos;
  
}

//replace all instances of s1 with s2 on s
int replaceAllNC(string & s, string s1, string s2)
{
  int c = 0;
  string::size_type f1 = 0;
  while ((f1 = findNC(s,s1,f1)) != string::npos) {
    c++;
    s.replace(f1,s1.size(), s2);
    f1 += s2.size();
  }
  return c;
}


string getArgumentValue(const string & str, const string & argument)
{
  int l=0, r=0;
  l = str.find(argument + "=");
  if (l>=0) {
    l = str.find("\"", l);
    l++;
    r = str.find("\"", l);
    string t = str.substr(l,r-l);
    replaceAll(t, "\n", "");
    return t;
  }
  return string("");
}

string name2filename(string const & n){
  string s = n;
  string::size_type p = 0;
  while ((p = s.find(" ")) != string::npos){
    s.replace(p++,1,"_");
  }
  p = 0;
  while ((p = s.find("/")) != string::npos){
    s.replace(p++,1,"-");
  }
  p = 0;
  while ((p = s.find("\"")) != string::npos){
    s.replace(p++,1,"-"); 
  }
  // Escape 8bit characters.
  string res = "";
  char hexbuf[4];
  for (unsigned int i = 0; i < s.size(); ++i){
    if (s[i] & 0x80) {
      ostrstream hexstr(hexbuf, 4);
      hexstr << 'x' << setw(2) << setfill('0') << hex << (int)(s[i] & 0xff);
      res += hexbuf[0];
      res += hexbuf[1];
      res += hexbuf[2];
    }
    else
      res += s[i];
  }
  return res;
}

//make a string usable as url
string name2url(string const & n){
  string s = name2filename(n);
  replaceAll(s, "?", "%3F");
  return s;
}


//RETURN a string that is like s but without any leading or
//  trailing whitespace.
// We also get rid of any \n
string trim(const string & s) 
{
  int l = s.length();
  int left =0, right =l-1;
  for (; left <= right; left++) 
    if (!(isspace(s[left]))) break;
  for (; right >= left; right--)
    if (!(isspace(s[right]))) break;
  string t(s.substr(left, right+1-left));
  replaceAll(t, "\n", "");
  return t;
}

string myItoa (int n)
{
  string         ret;
  strstream      tmp;
  tmp << n << ends;
  ret = tmp.str ();
  tmp.rdbuf()->freeze(0);
  return ret;
}


/** Our time zone*/
extern char *tzname[2];

string padWithZero(string s){
  if (s.length() == 1)
    return "0" + s;
  return s;
}

/** Returns the date and time (down to seconds) in ISO 8601 format as described by
    http://www.w3.org/TR/NOTE-datetime-970915.html */
string getISOTime (time_t li){
  struct tm * t = localtime(&li);
  int year = 1900 + t->tm_year;
  string res = myItoa(year) + "-" + padWithZero(myItoa(t->tm_mon)) + "-" + padWithZero(myItoa(t->tm_mday)) +
    "T" + padWithZero(myItoa(t->tm_hour)) + ":" + padWithZero(myItoa(t->tm_min)) + ":" + padWithZero(myItoa(t->tm_sec)) +
    "+05:00";
  return res;
}


//Autofill the comment, add \n's so that no line
// is bigger than n characters, break only at spaces.
//
void autoFill(string & s, int n = 72){
  int p = 0;
  int last = 0;
  bool set = false;
  replaceAll(s, "\n", " ");
  replaceAll(s, "\t", " ");
  replaceAll(s, "\r", " ");
  s = "    " + s; //indent parragraph
  for (unsigned int i=0; i < s.size(); i++, p++){
    if ((p >= n) && set) {
      s[last] = '\n';
      p = 0;
      set = false;
    };
    if (s[i] == ' '){
      last = i;
      set = true;
    };
  }
  //  cout << "O:" << s << endl;
}

reference::reference(const reference &r) : url(r.url), title(r.title), dirname(r.dirname), 
  comment(r.comment), creationTime(r.creationTime), 
  modifiedTime(r.modifiedTime), visitTime(r.visitTime), aliasDir(r.aliasDir),
  aliasID(r.aliasID), aliasOf(r.aliasOf), children(0), priv(r.priv),
  aliasof(r.aliasof), navBar(r.navBar), folder(r.folder), hits(r.hits)
{
  if (r.children)
    children = new referenceTree(*(r.children));
  else
    children = 0;
}

reference & reference::operator=(const reference & r)
{
  url = r.url;
  title = r.title;
  dirname = r.dirname;
  comment = r.comment;
  creationTime = r.creationTime;
  modifiedTime = r.modifiedTime;
  visitTime = r.modifiedTime;
  aliasDir = r.aliasDir;
  aliasID = r.aliasID;
  aliasOf = r.aliasOf;
  priv = r.priv;
  aliasof = r.aliasof;
  navBar = r.navBar;
  folder = r.folder;
  hits = r.hits;
  delete children;
  if (r.children)
    children = new referenceTree(*(r.children));
  else
    children = 0;
  return *this;
}

// a < b iff a was created more recently than b
int reference::operator<(const reference & r) const
{
  return creationTime > r.creationTime;
}


reference::~reference() 
{ 
  if (!isAliasof())
    delete children; 
}

//given an HTML <H?></H?> or <A></A> it extracts the appropiate values and 
//  sets the reference to these values.
//REQUIRES that value be a properly formed <H> or <A>
void reference::setValues (const string & str){
  int l, r;
  //  cout << "setValues children=" << children << endl;
  if (str[1] == 'H') { //folder
    string addDateValue;
    addDateValue = getArgumentValue(str, addDate);
    creationTime = atoi(addDateValue.c_str());
    l = str.find(">");
    r = str.find("<",l);
    title = str.substr(l+1, r-l-1);
    return;
  }
  else if (str[1] == 'A') { //leaf (URL)
    url = getArgumentValue(str, href);
    aliasID = getArgumentValue(str, ALIASID);
    aliasOf = getArgumentValue(str, ALIASOF);
    if (aliasOf != "")
      aliasof = true;
    string addDateValue, lastModifiedValue, lastVisistValue;
    addDateValue = getArgumentValue(str, addDate);
    creationTime = atoi(addDateValue.c_str());
    lastModifiedValue = getArgumentValue(str, lastModified);
    modifiedTime = atoi(lastModifiedValue.c_str());
    lastVisistValue = getArgumentValue(str, lastVisit);
    visitTime = atoi(lastVisistValue.c_str());
    l = str.find(">");
    r = str.find("<",l);
    title = str.substr(l+1, r-l-1);
    return;
  }
}

int reference::search(const string & s)
  // returns the number of times s appears in 
  //    title and comment
  // also adds <B> </B> around matches
{
  int c = 0;;
  c += replaceAllNC(title, s, "<B>" + s + "</B>");
  c += replaceAllNC(comment, s, "<B>" + s + "</B>");
  return c;
}

bool reference::isPrivate() const
{
  return priv;
}

bool reference::isAliasof() const
{
  return aliasof;
}

bool reference::isFolder() const
{
  return folder;
}


// return length of first sentence until first occurance of p.
// skip one letter abbreviations like in names, for example "O. Obst"
string::size_type reference::fullstop(const string &s, const string &p) const 
{
  string::size_type i, end;
  i = 0;
  end = 0;
  string t;
  
  while (end == 0) {
    t = s.substr(i,s.length());
    end = t.find(p); // search for an occurance of p
    if (end != string::npos) {
      i = i + end + 1;
      if (end <= 1 ||
          t[end - 2] == '.' ||
          t[end - 2] == '!' ||
          t[end - 2] == '?' ||
          t[end - 2] == ' ' ||
          t[end - 2] == '\t' ||
          t[end - 2] == '\r' ||
          t[end - 2] == '\n') 
        end = 0;
    }
    else i = s.length();
  }
  return i;
}

//this is not used anymore (except for debugging)
void reference::sendAsHTML(ostream & out)
{
  string t = title;
  if (isFolder()){
    out << navBar << " <A HREF=\"" << "../" << url << "\">" 
        << t << "</A><BR>" << endl;
  }
  else{
    //    out << navBar << "<BR>" << endl
    out << "<A HREF=\"" << url << "\">" << t << "</A>";
    if (comment != "")
      out << " - " << comment;
    out << endl;
  }
}

/**Format the reference in html and return it.
 Make sure all the %VARS are replaced by their values.
 templ is is template. */
string reference::sendAsHTML(const string & templ, 
                             const string varValues []) const
{
  string outputString = templ;
  time_t currentTime = time(0); //.tv_sec;
  static int timeCutoff = atoi(varValues[TIMECUTOFF].c_str()) * 60 * 60 * 24;
  string localComment = trim(comment);
  for (int i = 0; i < NUMFORMATVARS; i++){

    //if this %VAR is not in template, try next.
    if (outputString.find(FORMATVARS[i]) == string::npos)
      continue;

    if ((FORMATVARS[i] == "%URL") || (FORMATVARS[i] == "%NOACCENTURL")){
      if (isFolder()){
        string myURL;
        if (isAliasof()) 
          myURL = url;
        else if (varValues[SUBDIRSEP] == "/") 	  
          myURL =  name2url(dirname) + varValues[SUBDIRSEP] + varValues[INDEX];
        else {//flat file system.
          myURL = url.substr(varValues[SEARCHTOROOTPATH].size(), url.size()- varValues[SEARCHTOROOTPATH].size());
        }
        if (FORMATVARS[i] == "%URL")
          replaceAll(outputString, "%URL", myURL);
        else {
          string newurl = replaceAccents(myURL);
          replaceAll(outputString, "%NOACCENTURL", newurl);
        }
      } 
      else{ //not a folder
        if (FORMATVARS[i] == "%URL")
          replaceAll(outputString, "%URL", url);
        else {
          string newurl = replaceAccents(url);
          replaceAll(outputString, "%NOACCENTURL", newurl);
        };
      }
    }
    else if (FORMATVARS[i] == "%TITLE"){
      string pt = title;
      if (children && isAliasof())
        pt += "@";
      replaceAll(outputString, "%TITLE", pt);
    }
    else if (FORMATVARS[i] == "%NOACCENTTITLE"){
      string pt = title;
      if (children && isAliasof())
        pt += "@";
      string ptn = replaceAccents(pt);
      replaceAll(outputString, "%NOACCENTTITLE", ptn);
    }
    else if (FORMATVARS[i] == "%COMMENT"){
      replaceAll(outputString, "%COMMENT", localComment); 
    }
    else if (FORMATVARS[i] == "%FILLCOMMENT"){
      string copylocalComment = localComment;
      replaceAll(copylocalComment, "<BR>", " ");
      autoFill(copylocalComment);
      replaceAll(outputString, "%FILLCOMMENT", copylocalComment); 
    }
    else if (FORMATVARS[i] == "%LONGCOMMENT"){
      string copylocalComment = localComment;
      replaceAll(copylocalComment, "<BR>", " ");
      replaceAll(copylocalComment, "\n", " ");
      replaceAll(copylocalComment, "\t", " ");
      replaceAll(copylocalComment, "\r", " ");
      replaceAll(outputString, "%LONGCOMMENT", copylocalComment); 
    }
    else if (FORMATVARS[i] == "%NOACCENTCOMMENT"){
      autoFill(localComment);
      string newComment = replaceAccents(localComment);
      replaceAll(newComment, "<BR>", " ");
      replaceAll(outputString, "%NOACCENTCOMMENT", newComment); 
    }
    else if (FORMATVARS[i] == "%NHCOMMENT"){
      string commentnohtml = localComment;
      replaceAll(commentnohtml, "<BR>", " ");
      removeHTML(commentnohtml);
      autoFill(commentnohtml);
      replaceAll(outputString, "%NHCOMMENT", commentnohtml); 
    }
    else if (FORMATVARS[i] == "%SHORTCOMMENT("){
      string::size_type start;
      if ((start = outputString.find("%SHORTCOMMENT(")) != string::npos){
        string::size_type end = outputString.find(")", start);
        string fullmatch = outputString.substr(start, end-start+1);
        string match = outputString.substr(start+14,end - (start+14));
        string::size_type numChars = atoi(match.c_str());
        string comm = localComment.substr(0,numChars);
        replaceAll(comm, "<BR>", " ");
        replaceAll(comm, "\n", " ");
        replaceAll(comm, "\t", " ");
        replaceAll(comm, "\r", " ");
        //	autoFill(comm);
        replaceAll(outputString, fullmatch, comm);
      }
    }
    else if (FORMATVARS[i] == "%FSCOMMENT"){
      string::size_type end;
      string mycomment = localComment;
      replaceAll(mycomment, "<BR>", " ");
      if ((end = fullstop(mycomment, ". ")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("? ")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("! ")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = fullstop(mycomment, ".\t")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = fullstop(mycomment, ".\n")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = fullstop(mycomment, ".\r")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("?\t")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("?\n")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("?\r")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("!\t")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("!\n")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("!\r")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      };
      replaceAll(outputString, "%FSCOMMENT", mycomment); 
    }
    else if (FORMATVARS[i] == "%JSCOMMENT"){
      string copylocalComment = javaScriptEscape(localComment);
      replaceAll(copylocalComment, "<BR>", " ");
      replaceAll(outputString, "%JSCOMMENT", copylocalComment); 
    }
    else if (FORMATVARS[i] == "%JSFSCOMMENT"){
      string::size_type end;
      string mycomment = localComment;
      replaceAll(mycomment, "<BR>", " ");
      if ((end = fullstop(mycomment, ". ")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("? ")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("! ")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = fullstop(mycomment, ".\t")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = fullstop(mycomment, ".\n")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = fullstop(mycomment, ".\r")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("?\t")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("?\n")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("?\r")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("!\t")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("!\n")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      }
      else if ((end = mycomment.find("!\r")) != string::npos){
        mycomment = mycomment.substr(0,end+1);
      };
      mycomment = javaScriptEscape(mycomment);
      replaceAll(outputString, "%JSFSCOMMENT", mycomment); 
    }
    else if (FORMATVARS[i] == "%HITS"){
      string t = myItoa(hits);
      replaceAll(outputString, "%HITS", t);
    }
    else if (FORMATVARS[i] == "%DAYCRE"){
      time_t li = creationTime;
      struct tm * t = localtime(&li);
      string ti = myItoa(t->tm_mday);
      replaceAll(outputString, "%DAYCRE", ti );
    }
    else if (FORMATVARS[i] == "%DAYVIS"){
      time_t li = visitTime;
      struct tm * t = localtime(&li);
      string ti = myItoa(t->tm_mday);
      replaceAll(outputString, "%DAYVIS", ti );
    }
    else if (FORMATVARS[i] == "%DAYMOD"){
      time_t li = modifiedTime;
      struct tm * t = localtime(&li);
      string ti = myItoa(t->tm_mday);
      replaceAll(outputString, "%DAYMOD", ti );
    }
    else if (FORMATVARS[i] == "%YEARCRE"){
      time_t li = creationTime;
      struct tm * t = localtime(&li);
      int year = t->tm_year;
      if (year >= 100) 
        year -= 100;
      string ti = myItoa(year);
      if (ti.size() == 1)
        ti = "0" + ti;
      replaceAll(outputString, "%YEARCRE", ti );
    }
    else if (FORMATVARS[i] == "%YEARFCRE"){
      time_t li = creationTime;
      struct tm * t = localtime(&li);
      int year = t->tm_year;
      year += 1900;
      string ti = myItoa(year);
      replaceAll(outputString, "%YEARFCRE", ti );
    }
    else if (FORMATVARS[i] == "%YEARMOD"){
      time_t li = modifiedTime;
      struct tm * t = localtime(&li);
      int year = t->tm_year;
      if (year >= 100) 
        year -= 100;
      string ti = myItoa(year);
      if (ti.size() == 1)
        ti = "0" + ti;
      replaceAll(outputString, "%YEARMOD", ti );
    }
    else if (FORMATVARS[i] == "%YEARFMOD"){
      time_t li = modifiedTime;
      struct tm * t = localtime(&li);
      int year = t->tm_year;
      year += 1900;
      string ti = myItoa(year);
      replaceAll(outputString, "%YEARFMOD", ti );
    }
    else if (FORMATVARS[i] == "%YEARVIS"){
      time_t li = visitTime;
      struct tm * t = localtime(&li);
      int year = t->tm_year;
      if (year >= 100) 
        year -= 100;
      string ti = myItoa(year);
      if (ti.size() == 1)
        ti = "0" + ti;
      replaceAll(outputString, "%YEARVIS", ti );
    }
    else if (FORMATVARS[i] == "%YEARFVIS"){
      time_t li = visitTime;
      struct tm * t = localtime(&li);
      int year = t->tm_year;
      year += 1900;
      string ti = myItoa(year);
      replaceAll(outputString, "%YEARFVIS", ti );
    }
    else if (FORMATVARS[i] == "%MONTHCRE1"){
      time_t li = creationTime;
      struct tm * t = localtime(&li);
      int j = t->tm_mon;
      j++;
      string ti = myItoa(j);
      replaceAll(outputString, "%MONTHCRE1", ti );
    }
    else if (FORMATVARS[i] == "%MONTHCRE2"){
      time_t li = creationTime;
      struct tm * t = localtime(&li);
      int j = t->tm_mon;
      j++;
      string m = getMonth(j);
      replaceAll(outputString, "%MONTHCRE2", m );
    }
    else if (FORMATVARS[i] == "%MONTHVIS1"){
      time_t li = visitTime;
      struct tm * t = localtime(&li);
      int j = t->tm_mon;
      j++;
      string ti = myItoa(j);
      replaceAll(outputString, "%MONTHVIS1", ti );
    }
    else if (FORMATVARS[i] == "%MONTHVIS2"){
      time_t li = visitTime;
      struct tm * t = localtime(&li);
      int j = t->tm_mon;
      j++;
      string m = getMonth(j);
      replaceAll(outputString, "%MONTHVIS2", m );
    }
    else if (FORMATVARS[i] == "%MONTHMOD1"){
      time_t li = modifiedTime;
      struct tm * t = localtime(&li);
      int j = t->tm_mon;
      j++;
      string ti = myItoa(j);
      replaceAll(outputString, "%MONTHMOD1", ti );
    }
    else if (FORMATVARS[i] == "%MONTHMOD2"){
      time_t li = modifiedTime;
      struct tm * t = localtime(&li);
      int j = t->tm_mon;
      j++;
      string m = getMonth(j);
      replaceAll(outputString, "%MONTHMOD2", m );
    }
    else if (FORMATVARS[i] == "%LEAFS")
      if (children){
        string c = myItoa(children->getNumLeafs());
        replaceAll(outputString, "%LEAFS", c);
      }
      else
        replaceAll(outputString, "%LEAFS", "");
    else if (FORMATVARS[i] == "%NEW")
      if (creationTime + timeCutoff > currentTime){
        //	time_t li = creationTime;
        //	struct tm * t = localtime(&li);
        //	string dayCreated = myItoa(t->tm_mday);
        //	int j = t->tm_mon;
        //	j++;
        //	string monthCreated = myItoa(j);
        //	string yearCreated = myItoa(t->tm_year);
        string newgifTemplate = varValues[NEWGIF];
        if (newgifTemplate.find("%NEW") != string::npos) 
          replaceAll(newgifTemplate, "%NEW", ""); //nip those infinite loops in the bud.

        string ng = sendAsHTML(newgifTemplate, varValues);
        //	string ng =  "<IMG SRC=\"" + varValues[NEWGIF] + "\" align=middle alt=\"Added " 
        //	  + monthCreated +"/" + dayCreated + "/" + yearCreated + "\">&nbsp;";
        replaceAll(outputString, "%NEW", ng);
      }
      else
        replaceAll(outputString, "%NEW", "");
    else if (FORMATVARS[i] == "%TIMEFCRE"){
      long int l = creationTime;
      string c (ctime(&l));
      c = c.substr(0,c.size()-1);
      replaceAll(outputString, "%TIMEFCRE", c);
    }
    else if (FORMATVARS[i] == "%TIMEFMOD"){
      long int l = modifiedTime;
      string c(ctime(&l));
      c = c.substr(0,c.size()-1);
      replaceAll(outputString, "%TIMEFMOD", c);
    }
    else if (FORMATVARS[i] == "%TIMEFVIS"){
      long int l = visitTime;
      string c(ctime(&l));
      c = c.substr(0,c.size()-1);
      replaceAll(outputString, "%TIMEFVIS", c);
    }
    else if (FORMATVARS[i] == "%CONDDASH"){
      if (comment != "")
        replaceAll(outputString, "%CONDDASH", "-");
      else
        replaceAll(outputString, "%CONDDASH", "");
    }
    else if (FORMATVARS[i] == "%IFCOMHAS("){
      string::size_type ifstart;
      while ((ifstart = outputString.find("%IFCOMHAS(")) != string::npos){
        string::size_type ifmid = outputString.find(")(",ifstart);
        string::size_type ifend = outputString.find(")",ifmid+1);
        string fullif = outputString.substr(ifstart, ifend-ifstart+1);
        string match = outputString.substr(ifstart+10, ifmid - (ifstart+10));
        string htmltoadd = outputString.substr(ifmid+2 , ifend - (ifmid+2));
        replaceAll(outputString, fullif, "");
        if (comment.find(match) != string::npos){
          replaceAll(localComment, match, "");
          outputString.insert(ifstart,htmltoadd);
        }
        i = 0; //so html can contain %stuff
      }
    }
  }

  funstring finalanswer(outputString);
  return finalanswer.eval();
}


/** write as bookmark file */
void reference::writeAsBookHelper(ostream& output, string & prepend) const
{
  if (!isPrivate()){
    output << prepend << "<DT>";
    if (children == 0){ //its a leaf
      output << "<A HREF=\"" << url << "\" ";
      if (aliasID != "")
        output << ALIASID << "=\"" << aliasID << "\" ";
      if (aliasOf != "")
        output << ALIASOF << "=\"" << aliasOf << "\" ";
      output << addDate << "=\"" << creationTime << "\" "
             << lastVisit <<  "=\"" << visitTime << "\" "
             << lastModified <<  "=\"" << modifiedTime << "\">"
             << title << "</A>" << endl;
      if (comment != ""){
        string comt(comment);
        replaceAll(comt,"<","&lt;");
        output << "<DD>" << comt << endl;
      };
    }
    else { //its a folder
      output << "<H3 FOLDED " << addDate << "=\"" << creationTime << "\">"
             << title << "</H3>" << endl;
      if ((comment != "") || isAliasof() || (commentCommands != "")){
        string comt = comment + commentCommands;
        replaceAll(comt,"<","&lt;");
        if (isAliasof())
          comt = ALIAS + comt;
        output << "<DD>" << comt << endl; //netscape does not indent folder comments, why?
      };
      output << prepend << "<DL><p>" << endl;
      if (!isAliasof()){
        string np = prepend + "    ";
        children->writeAsBookHelper(output, np);
      }
      output << prepend << "</DL><p>" << endl;

    }
  }
};


/** write as xbel file */
void reference::writeAsXBELHelper(ostream& output, string & prepend) const
{
  if (!isPrivate()){ 
    if (children == 0){ //its a bookmark
      output << prepend << "<bookmark href=\"" << url << "\" ";
      //The times should be wrapped in a getISOTime() to fully conform to the standard,
      //but I prefer to deal with longs.
      output << "added=\"" <<  creationTime << "\" "
             << "visited=\"" << visitTime << "\" "
             << "modified=\"" << modifiedTime << "\" ";
      if (aliasID != "")
        output << "id=\"" << aliasID << "\" ";
      output << ">" << endl;
      if (aliasOf != "")
        output <<  prepend << "<alias ref=\"" << aliasOf << "\"/>" << endl;
      output << prepend << "<title>" << title << "</title>" << endl;
      if (comment != ""){
        string comt(comment);
        //         replaceAll(comt,"<","&lt;");
        replaceAll(comt,"<BR>","");        
        output << prepend << "<desc>" << comt << "</desc>" << endl;
      };
      output << prepend << "</bookmark>" << endl;
    }
    else { //its a folder
      output << prepend << "<folder added=\"" << creationTime << "\">" << endl;
      output << prepend << "<title>" << title << "</title>" << endl;
      if ((comment != "") || isAliasof() || (commentCommands != "")){
        string comt = comment + commentCommands;
        //         replaceAll(comt,"<","&lt;");
        replaceAll(comt,"<BR>","");
        if (isAliasof())
          comt = ALIAS + comt;
        output << prepend << "<desc>" << comt << "</desc>" << endl; //netscape does not indent folder comments, why?
      };
      if (!isAliasof()){
        string np = prepend + "  ";
        children->writeAsXBELHelper(output, np);
      }
      output << prepend << "</folder>" << endl;
    }
  }
};



// //write as bookmark file
// ostream &operator<<(ostream& output, const reference & r) 
// {
//   if (!r.isPrivate()){
//     output << "<DT>";
//     if (!r.isFolder()){
//       output << "<A HREF=\"" << r.url << "\" ";
//       if (r.aliasID != "")
// 	output << ALIASID << "=\"" << r.aliasID << "\" ";
//       if (r.aliasOf != "")
// 	output << ALIASOF << "=\"" << r.aliasOf << "\" ";
//       output << addDate << "=\"" << r.creationTime << "\" "
// 	     << lastVisit <<  "=\"" << r.visitTime << "\" "
// 	     << lastModified <<  "=\"" << r.modifiedTime << "\">"
// 	     << r.title << "</A>" << endl;
//       if (r.comment != ""){
// 	string comt(r.comment);
// 	replaceAll(comt,"<","&lt;");
// 	output << "<DD>" << comt << endl;
//       };
//     }
//     else {
//       output << "<H3 FOLDED " << addDate << "=\"" << r.creationTime << "\">"
// 	     << r.title << "</H3>" << endl;
//       if (r.comment != ""){
// 	string comt = r.comment;
// 	replaceAll(comt,"<","&lt;");
// 	output << "<DD>" << comt << endl;
//       };
//       output << "<DL><p>" << endl
// 	     << *(r.children)
// 	     << "</DL><p>" << endl;
//     }
//   }
//   return output;
// };

//outputs in flat list format
void reference::writeAsFlatFile(ostream& output) const
  // It assumes that all \n's have been removed from the strings (for >> to work)
{
  if (!isPrivate() && !isAliasof()){
    if (children == 0)
      output << "LEAF" << endl;
    else
      output << "FOLDER" << endl;
    string ptitle = title;
    output << navBar << endl << url << endl
           << title << endl << comment << endl
           << creationTime << endl << modifiedTime << endl
           << visitTime << endl << hits << endl;
    if (children != 0)
      children->writeAsFlatFile(output);
  }
}

//read a reference from a flat file.
iwebstream &operator>>(iwebstream& is, reference & r)
{
  string s1;
  //  getline (is, s1);
  is.getline(s1);
  string s;
  s = trim(s1);
  if (s == "") return is; //done.
  r.children = 0;
  if (s == "FOLDER"){
    r.folder = true;
  }
  else if (s == "LEAF"){
    r.folder = false;
  }
  else {
    cout << "Could not parse input on symbol===" << s << "===" << endl;
    return is;
  };
  //  getline(is,r.navBar);
  is.getline(r.navBar);
  r.navBar = trim(r.navBar);
  //  getline (is, r.url);
  is.getline(r.url);
  r.url = trim(r.url);
  //  getline (is, r.title);
  is.getline(r.title);
  r.title = trim(r.title);
  //  getline (is, r.comment);
  is.getline(r.comment);
  is >> r.creationTime >> r.modifiedTime >> r.visitTime >> r.hits;
  //  getline (is, s); //get the last /n....not needed with iwebstream
  return is;
}

/**set the contents of this reference based on the next <item> in is
 return true if an item was read, false otherwise
*/
bool reference::readXml(iwebstream & is) {
  string s = is.getData(); //easier to do with the whole string
  const string itemT = "<item>";
  const string itemET = "</item>";
  const string titleT = "<title>";
  const string titleET = "</title>";
  const string linkT = "<link>";
  const string linkET = "</link>";
  const string descriptionT = "<description>";
  const string descriptionET = "</description>";

  string temp;
  //  cout << "readXML:";
  if (!is.find(itemT))
    return false;
  title = is.findTag(titleT, titleET, itemT, itemET);
  title = trim(title);
  if (title == "" )
    return false;
  url = is.findTag(linkT, linkET, itemT, itemET);
  url = trim(url);
  if (url == "" )
    return false;
  comment = is.findTag(descriptionT, descriptionET, itemT, itemET);
  comment = trim(comment);
  is.findAndPass(itemET);
  //  cout << title << url << comment << endl;
  return true;
}

string referenceTree::getToken(iwebstream & is)
{
  string token;
  char c,c1,c2, c3;
  while (!is.eof()) {
    c = is.get();
    if (c != '<')
      continue;
    c1 = is.get();
    if (c1 == 0) return "";
    c2 = is.get();
    if (c2 == 0) return "";
    if (c1 == 'A') {
      token += c;
      token += c1;
      token += c2;
      while (!is.eof()) {
        c = is.get();
        token +=c;
        if (c == '/'){
          c = is.get();
          token += c;
          if (c == 'A'){
            c = is.get();
            token += c;
            if (c == '>'){
              token +=c;
              return token; // <A HREF=.... </A>
            }
          }
        }
      }
      return ""; //if we exited its because the file ended
    }
    else if ((c1 == 'H') && (c2 == '3')) {
      token += c;
      token += c1;
      token += c2;
      while (!is.eof()) {
        c = is.get();
        if (is.eof()) return "";
        token +=c;
        if (c == '/'){
          c = is.get();
          token +=c;
          if (c == 'H'){
            c = is.get();
            token +=c;
            if (c == '3'){
              c = is.get();
              token +=c;
              if (c == '>'){
                token +=c;
                return token; // <H3> ... </H3>
              }
            }
          }
        }
      }
      return "";
    }
    else if ((c1 == 'D') && (c2 == 'L')) {
      token += c;
      token += c1;
      token += c2;
      while ((c = is.get()) != '>'){
        token +=c;
        if (is.eof()) return "";
      };
      token +=c;
      return token; // <DL>
    }
    else if ((c1 == 'D') && (c2 == 'D')) {
      token += c;
      token += c1;
      token += c2;
      c1 = is.get();
      c2 = is.get();
      while ((!((c1 == '<') && (c2 == 'D'))) && (!((c1 == '<') && (c2 == '/'))))
      { //end only when find <D or </ ..this ignores <BR>
        //	  cout << c1;
        //eliminate <BR>s
        if (is.eof()) return "";
        if ((c1 == '<') && (c2 == 'B')) { //assume this is a <BR>
          token += c1;
          token += c2;
          c3 = is.get(); // R
          token += c3;
          c3 = is.get(); // >
          token += c3;
          c1 = is.get(); //' '; // insert a space instead
          c2 = is.get();
          continue;
        }
        if ((c1 == '&') && (c2 == 'l')) { //assume this is &lt; replace with <
          c3 = is.get(); //t
          c3 = is.get(); //;
          token += '<';
          c1 = is.get();
          c2 = is.get();
          continue;
        }
        token +=c1;
        c1 = c2;
        c2 = is.get();
      }
      is.putback(c2);
      is.putback(c1);
      return token; // <DD> comment... (up to next '<')
    }
    else if ((c1 == '/') && (c2 == 'D') && ((c3 = is.get()) == 'L')) {
      token += c;
      token += c1;
      token += c2;
      token += c3;
      while ((c = is.get()) != '>'){
        token +=c;
        if (is.eof()) return "";
      };
      token +=c;
      return token; // </DL>
    }
  }
  return "";
}

//read a referenceTree from a bookmarks file.
iwebstream &operator>>(iwebstream& is, referenceTree & rt)
{
  string token;
  //  cout << "Calling >> " << endl;
  //  cout << "Calling >> numl=" << rt.numLeafs << endl;
  while ((token = rt.getToken(is)) != ""){
    //    cout << token << endl;
    if (token.substr(0,2) == "<A") {
      reference r(token);
      (rt.contents).push_back(r);
      rt.numLeafs++;
    }
    else if (token.substr(0,4) == "<DD>") {
      int len = token.size();
      if (rt.contents.empty()) { //if its a comment to the folder, add it.
        string temp = token.substr(4,len-4);
        rt.comment = trim(temp);
      }
      else {//if a comment to a leaf then get the leaf and add a comment to it.
        reference & last = rt.contents.back();
        string temp = token.substr(4,len-4);
        last.comment = trim(temp);
        if (last.comment == PRIVATE){
          last.priv = true;
          rt.numLeafs--; //do not count this one
        }
      }
    }
    else if (token.substr(0,2) == "<H") { //a new (sub)folder
      reference r(token); 
      referenceTree * rtc = new referenceTree;
      is >> (*rtc);
      if (rtc->comment == PRIVATE){
        r.priv = true;
        rt.numLeafs -= rtc->numLeafs; //do not count these
      }

      if (rtc->comment.substr(0,5) == ALIAS) {
        r.aliasof = true;
        string::size_type endDirName = rtc->comment.find("\n", 0);
        if (endDirName == string::npos)
          endDirName = rtc->comment.length();
        r.aliasDir = rtc->comment.substr(5,endDirName - 5);
        r.aliasDir = trim(r.aliasDir);
      }

      r.children = rtc;
      //      rtc->doSort(); //sort, if needed (as per comment)

      if (cmpNoCase(rtc->comment.substr(0,7), INCLUDE) == 0){ //INCLUDE found, read file and merge with rtc.
        string::size_type endFileName = rtc->comment.find("\n", 0);
        if (endFileName == string::npos)
          endFileName = rtc->comment.length();
        string fileName = rtc->comment.substr(7,endFileName - 7);
        fileName = trim(fileName);
        iwebstream istream(fileName.c_str());
        cout << "Found INCLUDE " << fileName << " in folder " << r.title << endl;
        if (istream == 0){
          cout << "Could not open " << fileName << endl;
        }
        else {
          cout << "Reading " << fileName << endl;
          referenceTree rtc2;
          istream >> rtc2;
          rtc->addTree(rtc2);
        }
      }
	
      if (cmpNoCase(rtc->comment.substr(0,7), DIRNAME) == 0){ //DIRNAME found, set to dirname.
        string::size_type beginDirName = rtc->comment.find_first_not_of(" tn", 7);
        if (beginDirName == string::npos)
          beginDirName = 7;
        string::size_type endDirName = rtc->comment.find_first_of(" tn", beginDirName);
        if (endDirName == string::npos)
          endDirName = rtc->comment.length();
        string dirName = rtc->comment.substr(beginDirName, endDirName - beginDirName);
        rtc->comment.erase(0, endDirName);
        dirName = trim(dirName);
        r.dirname = dirName;
        cout << "Found DIRNAME \"" << r.dirname << "\" in folder " << r.title << endl;
      }
      else {
        r.dirname = r.title;
      }
	
      r.comment = rtc->comment; //make a copy, waste memory.
      r.folder = true;
      rt.contents.push_back(r);
      rt.numLeafs += rtc->numLeafs;
    }
    else if (token == "</DL>"){
      //      cout << "Returning from >> numl=" << rt.numLeafs << endl;
      return is;
    }
  };
  cerr << "Bookmark file is not well formed. Trying anyway...(crash may ensue)." << endl;
  //  rt.contents.erase(rt.contents.begin(), rt.contents.end());
  //  rt.numLeafs = 0;
  //  rt.comment = "";
  return is; //this should never happen with a well formed bookmark file
}


// //write as bookmarkfile ---does not add indenting spaces
// ostream &operator<<(ostream& out, const referenceTree & rt) 
// {
//   for (vector<reference>::const_iterator i = rt.contents.begin(); i != rt.contents.end(); ++i){
//     const reference& r = *i;
//     out << r;
//   }
//   return out;
// }

/** write as bookmarkfile */
void referenceTree::writeAsBookHelper(ostream& out, string & prepend) const
{
  for (vector<reference>::const_iterator i = contents.begin(); i != contents.end(); ++i){
    const reference& r = *i;
    //    out << prepend << "<DT>";
    r.writeAsBookHelper(out, prepend);
  }
}

/** write as bookmarkfile */
void referenceTree::writeAsBookmarkFile(ostream & out, string title) const
{

  out << "<!DOCTYPE NETSCAPE-Bookmark-file-1>" << endl
      << "<!--This file was generated by bk2site-->" << endl
      << "<TITLE>" << title << "</TITLE>" << endl
      << "<H1>" << title << "</H1>" << endl << endl
      << "<DL><p>" << endl;
  //      << *this
  string prepend = "";
  writeAsBookHelper(out, prepend);
  out << "</DL><p>" << endl;
}


/** write as xbel */
void referenceTree::writeAsXBELHelper(ostream& out, string & prepend) const
{
  for (vector<reference>::const_iterator i = contents.begin(); i != contents.end(); ++i){
    const reference& r = *i;
    //    out << prepend << "<DT>";
    r.writeAsXBELHelper(out, prepend);
  }
}

/** write as xbel */
void referenceTree::writeAsXBELFile(ostream & out, string title) const
{

  out << "<?xml version=\"1.0\"?>" << endl
      << "<!DOCTYPE xbel PUBLIC \"+//IDN python.org//DTD XML Bookmark Exchange Language 1.0//EN//XML\" \"http://www.python.org/topics/xml/dtds/xbel-1.0.dtd\">" << endl
      << "<xbel>" << endl << "<info>File generated by bk2site</info>" << endl
      << "<title>" << title << "</title>" << endl;
  string prepend = "  ";
  writeAsXBELHelper(out, prepend);
  out << "</xbel>" << endl;
}

/** Outputs in flat list format */
void referenceTree::writeAsFlatFile(ostream& out) const
{
  for (vector<reference>::const_iterator i = contents.begin(); i != contents.end(); ++i){
    const reference& r = *i;
    r.writeAsFlatFile(out);
  };
}


/** Returns the path to a directory named "title" whose comment is
 *  NOT ALIAS. The result is prepended by "dir". */
string referenceTree::getPath(const string & dirname, const string & dir, const string & subdirsep) const
{
  
  for (vector<reference>::const_iterator i = contents.begin(); i != contents.end(); ++i){
    const reference& r = *i;
    if ((r.dirname == dirname) && (r.children != 0) && !(r.isAliasof())) {
      return dir + name2filename(r.dirname) + subdirsep;
    }
    else if (r.children){
      string newDir = dir + name2filename(r.dirname) + subdirsep;
      string res = (r.children)->getPath(dirname, newDir, subdirsep);
      if (res != "")
        return res;
    }
  }
  return "";
}

/** Increase the number of hits for url by 1. */
void referenceTree::increaseHits(string & url, int x)
{
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.url == url)
      r.hits+=x;
    if (r.children)
      r.children->increaseHits(url, x);
  }
}

/** Set the creationtime of all folders to be the max of all
    descendants. Returns the max Creationtime of this tree */
time_t referenceTree::setFolderCreationToMaxDescendant() {
  time_t maxTime  =0;
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.isFolder())
      r.creationTime = r.children->setFolderCreationToMaxDescendant();
    if ((r.creationTime > maxTime) && !r.isAliasof())
      maxTime = r.creationTime;
  }
  return maxTime;
}


/** Set the creationtime of all folders to be the max of all
    its children. */
time_t referenceTree::setFolderCreationToMaxChildren() {
  time_t maxTime  =0;
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.isFolder())
      r.creationTime = r.children->setFolderCreationToMaxDescendant();
    else if (r.creationTime > maxTime)
      maxTime = r.creationTime;
  }
  return maxTime;
}
      


/** replaces the .url in all directory references with ALIAS
    with the relative path of the other directory with the same title.
    Also sets the children pointer to point to the correct place.
 *  dir is the number of "../../" needed to get back to the top.
 *  index is the varvalues[INDEX]
 *  backdirsep is varvalues[BACKDIRSEP]
 *
 *  It also sets the .url of all other folders to point
 *  to SEARCHTOROOTPATH + path to folder + index.html.
 *  
 * It also sorts all folders, as per the comment section in the folder.
 */
void referenceTree::fixAliases(referenceTree * rootRT, const string & dir, const string varValues [],
                               const string & path)
{
  doSort();  //sort, if needed (as per comment)

  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.isFolder() && r.isAliasof()){
      string realPath;
      string title;
      if (r.aliasDir == "") {
        realPath = rootRT->getPath(r.dirname, dir, varValues[SUBDIRSEP]);
        title = r.title;
      } else {
        realPath = rootRT->getPath(r.aliasDir, dir, varValues[SUBDIRSEP]);
        title = r.aliasDir;
      }

      if (realPath == "") {
        cerr << "ERROR: Folder " << title 
             << " has an ALIAS on it, but I cannot find the folder it points to." << endl;
        cerr << "       I am ignoring it." << endl;
        continue;
      };
      r.url = realPath + varValues[INDEX];
      r.children = rootRT->getSubtree(title);
      r.comment = (r.children)->comment; 
    }
    else if (r.isFolder()) {
      r.url = varValues[SEARCHTOROOTPATH] + path + name2filename(r.dirname) + varValues[SUBDIRSEP] + varValues[INDEX];
    }
  };
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.isFolder() && !(r.isAliasof())){
      string newpath = path + name2filename(r.dirname) + varValues[SUBDIRSEP];
      string newdir = dir +  varValues[BACKDIRSEP];
      (r.children)->fixAliases(rootRT, newdir , varValues, newpath);
      r.comment = (r.children)->comment; //since the doSort changes the comments.
    }
			       
  };
}



/** returns a subtree of this rt which has topName as its title
 * does a BFS. topName MUST be a directory (i.e. have some children) 
 *  otherwise it will not be found. */
referenceTree * referenceTree::getSubtree(const string & topName)
{
  if (topName == "")
    return this;
  for (vector<reference>::const_iterator i = contents.begin(); i != contents.end(); ++i){
    const reference& r = *i;
    if ((r.title == topName) && (r.children !=0) && !(r.isAliasof())) {
      return r.children;
    }
  }
  for (vector<reference>::const_iterator i = contents.begin(); i != contents.end(); ++i){
    const reference& r = *i;
    if (r.children){
      referenceTree * res = (r.children)->getSubtree(topName);
      if (res != 0)
        return res;
    }
  }
  return 0;
}

/**Fix the numLeafs variable for all nodes
 * returns the number of leafs below me. */
int referenceTree::fixNumChildren() 
{
  int num = 0;
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.children != 0 )
      num += r.children->fixNumChildren();
    else
      num++;
  }
  numLeafs = num;
  return num;
}

/** Add rt to me (i.e. this referenceTree).
 *  if folder == "" then add it here */
void referenceTree::addTree(const referenceTree & rt)
{
  for (vector<reference>::const_iterator i = rt.contents.begin(); i != rt.contents.end(); ++i){
    const reference &r = *i;
    if (!r.isFolder()){
      int alreadyThere = 0;
      for (vector<reference>::const_iterator j = contents.begin(); j != contents.end(); ++j){
        if ((*j).url == r.url) {
          alreadyThere = 1;
          break;
        }
      };
      if (!alreadyThere){
        reference r2(r);
        if (r2.aliasOf != ""){
          int x = atoi(r.aliasOf.c_str()) + numMerges;
          r2.aliasOf = myItoa(x);
        };
        if (r2.aliasID != ""){
          int x = atoi(r.aliasID.c_str()) + numMerges;
          r2.aliasID =  myItoa(x);
        };
        cout << "Adding " << r2.title << "-" << r2.url << " to main tree." << endl;
        contents.push_back(r2);//add a leaf
      }
    }
    else { //its a folder
      referenceTree * rtchild = 0;
      for (vector<reference>::iterator j = contents.begin(); j != contents.end(); ++j){
        reference &rc = *j;
        if (rc.isFolder() && (rc.title == r.title)) {
          rtchild = rc.children;
          break;
        };
      }
      if (rtchild == 0) { //did not find folder, so create new one.
        reference newref(r);
        contents.push_back(newref);
      }
      else
        rtchild->addTree(*(r.children)); //add a folder
    }
  }
  fixNumChildren();
}

/** Merge me with rt. A merge of two trees is done by taking
 *  the second tree (rt) and findin a folder under it with the
 *  comment PUBLISH. The title of that folder is then found on 
 *  this tree, thereby matching the two roots of the trees. All
 *  the descendants of these root nodes are merged together. If
 *  rt has folders that do not appear under "this" tree, the folders
 *  are created. */
void referenceTree::merge(const referenceTree & rt)
{
  numMerges +=100; //increase by 100, we add this number to all the aliases
  for (vector<reference>::const_iterator i = rt.contents.begin(); i != rt.contents.end(); ++i){
    const reference& r = *i;
    if (r.isFolder() && (r.comment == PUBLISH)) {
      cout << "Found PUBLISH in " << r.title << endl;
      referenceTree * myNode = getSubtree(r.title);  
      if (myNode == 0){
        cerr << "ERROR: Tried to merge folder " << r.title << " but it was not found in main tree" << endl;
        continue;
      };
      myNode->addTree(*(r.children));
      fixNumChildren();
    }
    else if (r.isFolder()) { //try the subtrees
      merge(*(r.children));
    }
  }
}

/** Flattens out rt and adds all the LEAFS (urls)(only) to the list lr.
    returns the number of elements added.
    Only adds URLS that are not Private and not Aliasof another one and
    are not under avoidFolder. */
void referenceTree::makeVector(vector<reference> & vr, const string & avoidFolder = "")
{
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference & r = *i;
    if (!(r.isFolder()) && !(r.isPrivate()) && !(r.isAliasof()))
      vr.push_back(r);
    else if (r.isFolder() && !(r.isPrivate()) && !(r.isAliasof()))
      if (r.title != avoidFolder)
        (r.children)->makeVector(vr, avoidFolder);
  }
}

/**Generates the website.
 */
void referenceTree::createSite(const string varValues[], channelContainer & channels) {

  vector<reference> allReferences;
  makeVector(allReferences, varValues[NEWSTOPFOLDER]);

  int numReferences = allReferences.size();
  cout << "We are using " << numReferences << " unique bookmarks." << endl;
  
  string ifileName;
  ifileName = varValues[INDEXFILENAME];

  fileView baseView(ifileName);

  ifileName = varValues[OTHERFILENAME];
  fileView otherView(ifileName);

  vector<reference> newsItems;
  if (varValues[NEWSTOPFOLDER] != ""){
    referenceTree * rt = getSubtree(varValues[NEWSTOPFOLDER]);
    if (rt == 0) {
      cerr << "ERROR: News folder named " << varValues[NEWSTOPFOLDER] << " was not found in " << varValues[BOOKMARKFILE] << endl;
      return;
    }
    newsItems = rt->contents;
  }
  //  cout << "newsItems.size()=" << newsItems.size() << endl;

  createSiteH(varValues,allReferences, channels, baseView, otherView,
              newsItems, 0, "","", varValues[TOP], "", "");

  //write out the urls.db for the search program (if needed)
  if (varValues[SEARCH] != ""){
    ofstream dbase(varValues[SEARCH].c_str(), ios::out);
    if (dbase == 0){
      cout << "Could not open " << varValues[SEARCH] << " for output." << endl;
      return;
    }
    cout << "Writing " << varValues[SEARCH] << endl;
    dbase << urlsDBHeader << endl;
    dbase << varValues[SEARCHURLTEMPLATE] << endl
          << varValues[NEWGIF] << endl
          << varValues[TIMECUTOFF] << endl;
    //    dbase << *this;
    writeAsFlatFile(dbase);
  }

}

/**
   createsiteHelper, recursively calls itself for each page.
*/
void referenceTree::createSiteH(const string varValues[], vector<reference> & allReferences,
                                channelContainer & channels,
                                fileView & baseView, fileView & otherView,
                                vector<reference> & newsItems, int depth = 0, 
                                string navigateBar = "", string searchNavBar = "",
                                string parentTitle = "", string folderTitle="",
                                string filePath = "") {
  string ofileName;
  fileView & fv = baseView;
  string fileContents;

  if (depth == 0){
    ofileName = varValues[DESTDIR] + varValues[INDEX];
    //    els = elsBase;
    //    fileContents = fileContentsB;
    fv = baseView;
  }
  else {
    unsigned short mode = 493;
    if (varValues[SUBDIRSEP]=="/") {  //hierarchical filesystem
      string dirName = varValues[DESTDIR] + filePath;
      mkdir(dirName.c_str(), mode);
    };
    ofileName = varValues[DESTDIR] + filePath + varValues[INDEX];
    //    els =  elsOther;
    //    fileContents = fileContentsO;
    fv = otherView;
  }

  //replace all the comments for their values.
  fileContents = fv.sendAsHTML(contents, allReferences, newsItems, varValues, channels);

  //replace title and navigatebar in fileContents
  string key = placeholder + ":" + NAVIGATEBAR_TAG + placeholderEnd;
  string fullNavBar = navigateBar + " " + parentTitle;
  replaceAll(fileContents, key, fullNavBar);
  key= placeholder + ":" + TITLE_TAG + placeholderEnd;
  string newTitle =  varValues[TITLE] + ": " + parentTitle;
  replaceAll(fileContents, key, newTitle);

  // replace search in fileContents - btb@debian.org
  key = placeholder + ":" + SEARCH_TAG + placeholderEnd;
  replaceAll(fileContents, key, varValues[SEARCH]);

  // replace the path
  key = placeholder + ":" + PATH_TAG + placeholderEnd;
  replaceAll(fileContents, key, filePath);

  // replace current date - august.hoerandl@gmx.at
  key= placeholder + ":" + DATE_TAG + placeholderEnd;
  time_t now = time(0);
  string newDate =  asctime( localtime(&now) );
  replaceAll(fileContents, key, newDate);


  //write out the file
  cout << "Writing " << ofileName << endl;
  ofstream ofile(ofileName.c_str(), ios::out);
  if (!(ofile)){
    cerr << "Could not open output file " << ofileName << endl;
    return;
  }
  ofile << fileContents;
  ofile.close();
  //  time_t currentTime2 = time(0); //.tv_sec;
  //  cout << "Time = " << currentTime2 - currentTime << endl;

  //determine the naviatebar for this folder.
  string newNavigateBar;
  string newSearchNavBar; //for navBar, which is only used by search.pl.
  if (varValues[SUBDIRSEP] == "/") {
    replaceAll(navigateBar, "href=\"" + varValues[BACKDIRSEP], "href=\"" +varValues[BACKDIRSEP] + varValues[BACKDIRSEP]);
    newNavigateBar= navigateBar + "<a href=\"" + varValues[BACKDIRSEP] + varValues[INDEX] + "\"><B>" +
      parentTitle + "</B></A><B>:</B> ";
  }
  else //flat file system
    newNavigateBar= navigateBar + "<a href=\"" + filePath + varValues[INDEX] + "\"><B>" +
      parentTitle + "</B></A><B>:</B> ";
  newSearchNavBar = searchNavBar + "<a href=\"" + varValues[SEARCHTOROOTPATH] + filePath + varValues[INDEX] + "\"><B>" +
    parentTitle + "</B></A><B>:</B> ";

  //Now, do a page for each of the directories.
  for (vector<reference>::iterator j = contents.begin(); j != contents.end(); ++j){
    reference & r = *j;
    r.navBar = newSearchNavBar;
    if ((r.children != 0) && !(r.isAliasof()) && !(r.isPrivate())){
      string newFilePath = filePath + name2filename(r.dirname) + varValues[SUBDIRSEP];
      r.children->createSiteH(varValues, allReferences, channels, baseView, otherView, newsItems,
                              depth+1, newNavigateBar, newSearchNavBar, r.title, folderTitle, newFilePath);
    }
  }
}

/**
   Creates a single page. This implements the extrafilebase, extrafilename feature.
*/

void referenceTree::createPage(const string indexFileName, const string ofileName, 
                               const string varValues [], channelContainer & channels)
{
  vector<reference> allReferences;
  makeVector(allReferences, varValues[NEWSTOPFOLDER]);
  //   for (vector<reference>::iterator i = allReferences.begin(); i != allReferences.end(); ++i){
  //     reference &r = *i;
  //     cout << r.title << endl;
  //   };

  vector<reference> newsItems;
  if (varValues[NEWSTOPFOLDER] != ""){
    referenceTree * rt = getSubtree(varValues[NEWSTOPFOLDER]);
    if (rt == 0) {
      cerr << "ERROR: News folder named " << varValues[NEWSTOPFOLDER] << " was not found in " << varValues[BOOKMARKFILE] << endl;
      return;
    }
    newsItems = rt->contents;
  }

  fileView fv(indexFileName);
  vector<reference> contents;
  string fileContents = fv.sendAsHTML(contents, allReferences, newsItems, varValues, channels);

  string key= placeholder + ":" + TITLE_TAG + placeholderEnd;
  replaceAll(fileContents, key, varValues[TITLE]);

  // replace search in fileContents - btb@debian.org
  key = placeholder + ":" + SEARCH_TAG + placeholderEnd;
  replaceAll(fileContents, key, varValues[SEARCH]);


  // replace the path with nothing, in case they left it by mistake.
  key = placeholder + ":" + PATH_TAG + placeholderEnd;
  replaceAll(fileContents, key, "");

  // replace current date - august.hoerandl@gmx.at
  key= placeholder + ":" + DATE_TAG + placeholderEnd;
  time_t now = time(0);
  string newDate =  asctime( localtime(&now) );
  replaceAll(fileContents, key, newDate);


  cout << "Writing " << ofileName << endl;
  ofstream ofile(ofileName.c_str(), ios::out);
  if (!(ofile)){
    cerr << "Could not open output file " << ofileName << endl;
    return;
  }
  ofile << fileContents;
  ofile.close();
}



/** 
    Does sort of all folders, as per comment on folder, 
    then eliminates the instructions from the comment.
*/
void referenceTree::doSort()
{
  string::size_type start = comment.find("sort");

  //  if (cmpNoCase(comment.substr(0,4), "sort") == 0) { 

  //do we need to sort this folder?
  if (start != string::npos){ //yes, need to sort
    //    cout << "Found Sort: " << comment <<  endl;
    if (cmpNoCase(comment.substr(start,20), "sort creation normal") == 0){
      sort(contents.begin(), contents.end(), referenceCmpCreation());
      replaceAllNC(comment, "sort creation normal", "");
    }
    else if (cmpNoCase(comment.substr(start,21), "sort creation inverse") == 0){
      sort(contents.begin(), contents.end(), referenceCmpCreationI());
      replaceAllNC(comment, "sort creation inverse", "");
    }
    else if (cmpNoCase(comment.substr(start,17), "sort title normal") == 0){
      sort(contents.begin(), contents.end(), referenceCmpTitle());
      replaceAllNC(comment, "sort title normal", "");
    }
    else if (cmpNoCase(comment.substr(start,18), "sort title inverse") == 0){
      sort(contents.begin(), contents.end(), referenceCmpTitleI());
      replaceAllNC(comment, "sort title inverse", "");
    }
    else if (cmpNoCase(comment.substr(start,20), "sort modified normal") == 0){
      sort(contents.begin(), contents.end(), referenceCmpModified());
      replaceAllNC(comment, "sort modified normal", "");
    }
    else if (cmpNoCase(comment.substr(start,21), "sort modified inverse") == 0){
      sort(contents.begin(), contents.end(), referenceCmpModifiedI());
      replaceAllNC(comment, "sort modified inverse", "");
    }
    else if (cmpNoCase(comment.substr(start,17), "sort visit normal") == 0){
      sort(contents.begin(), contents.end(), referenceCmpVisit());
      replaceAllNC(comment, "sort visit normal", "");
    }
    else if (cmpNoCase(comment.substr(start,18), "sort visit inverse") == 0){
      sort(contents.begin(), contents.end(), referenceCmpVisitI());
      replaceAllNC(comment, "sort visit inverse", "");
    }
    else if (cmpNoCase(comment.substr(start,18), "sort hits normal") == 0){
      sort(contents.begin(), contents.end(), referenceCmpHits());
      replaceAllNC(comment, "sort hits normal", "");
    }
    else if (cmpNoCase(comment.substr(start,18), "sort hits inverse") == 0){
      sort(contents.begin(), contents.end(), referenceCmpHitsI());
      replaceAllNC(comment, "sort hits inverse", "");
    }
    else 
      cerr << "Did not understand sort comment: " << comment << endl;
  }

  //recursively sort all children
  for (vector<reference>::iterator i = contents.begin(); i != contents.end(); ++i){
    reference& r = *i;
    if (r.isFolder() && !r.isAliasof()) {
      r.commentCommands = r.children->comment;
      r.children->doSort();
    }
  }
  
}

// -*-
// Local Variables:
// mode: C++
// tab-width: 4
// c-basic-offset:2
// indent-tabs-mode: nil 
// End:
