// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
//
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// -*- mode:C++; tab-width:4; c-basic-offset:2; indent-tabs-mode:nil -*- 
//
// $Id: funstring.C,v 1.3 2000/02/06 19:49:45 jmvidal Exp $
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "funstring.H"


string::size_type funstring::getEndParen(const string & s, string::size_type start){
  int openCount = 0;
  for (string::size_type i = start; i < s.size(); ++i){
    if (s[i] == '[')
      openCount++;
    if (s[i] == ']')
      if (openCount == 0)
	return i;
      else openCount--;
  }
  cerr << "Mismatched [] parentheses" << endl;
  return string::npos;
}

string::size_type funstring::getNextArgument(const string & s, string::size_type pos, string & res){
  int openCount = 0;
  for (string::size_type i = pos; i < s.size(); ++i){
    if (s[i] == '[')
      openCount++;
    if ((s[i] == '|') && (openCount == 0)){
      res = s.substr(pos, i-pos);
      return i+1;
    }
    if (s[i] == ']')
      if (openCount == 0) {
	res = s.substr(pos, i-pos);
	return i+1;
      }
      else
	openCount--;
  }
  cerr << "Mismatched [] parentheses, or missing/extra |." << endl;
  return string::npos;
}

string funstring::getEvaledArgument(string::size_type start, string::size_type & end){
  if ((end = getEndParen(fs, start)) == string::npos) {
    cerr << "Mismatch Parenthesis." << endl;
    return "";
  }
  funstring argument(fs.substr(start, end-start));
  return argument.eval();
}
  
  
string funstring::fill(const string & s){

  string st;
  string::size_type p = getNextArgument(s,0,st);
  funstring firstArg(st);
  st = firstArg.eval();

  string col;
  p = getNextArgument(s,p ,col);
  unsigned int numcols = atoi(col.c_str());
  
  string res = "    " + st; //indent
  bool set = false;
  unsigned int last = 0;
  p=0;
  for (unsigned int i=0; i < res.size(); i++, p++){
    if ((p >= numcols) && set) {
      res[last] = '\n';
      p = 0;
      set = false;
    };
    if (res[i] == ' '){
      last = i;
      set = true;
    };
  }
  return res;
}

string funstring::nohtml(const string & s){
  string st;
  getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();

  string::size_type start = 0;
  string::size_type end = 0;

  while ( (start = st.find("<")) != string::npos){
    end = st.find(">", start);
    st.replace(start, end-start + 1, ""); 
  };

  return st;
}

string funstring::noLineBreaks(const string & s){
  string st;
  getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();

  replaceAll(st, "\n", " ");
  replaceAll(st, "\t", " ");
  replaceAll(st, "\r", " ");
  return st;
}

string funstring::noAccents(const string & s){
  string st;
  getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();
  //  cout << "noaccents:" << st << ":" << endl;

  string res = "";
  for (string::const_iterator i = st.begin(); i != st.end(); ++ i){
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

/** Do a &CUT[&NOACCENTS[&NOHTML[s]]|450] */
string funstring::rss(const string & s){
  string st;
  getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();
  st = nohtml(st);
  st = noAccents(st);
  st = cut(st + "|450");
  return st;
}

string funstring::firstSentence(const string & s){
  string st;
  getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();

  string::size_type end;
  string res = st;
  if ((end = st.find(". ")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("? ")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("! ")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find(".\t")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find(".\n")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find(".\r")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("?\t")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("?\n")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("?\r")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("!\t")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("!\n")) != string::npos){
    res = st.substr(0,end+1);
  }
  else if ((end = st.find("!\r")) != string::npos){
    res = st.substr(0,end+1);
  };
  return res;
}

int funstring::replaceAll(string & s, string s1, string s2)
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

string funstring::replace(const string & s){
  string st;
  string::size_type p = getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();

  string from, to;
  p = getNextArgument(s,p,from);
  p = getNextArgument(s,p,to);
  replaceAll(st,from, to);
  return st;
}


string funstring::cut(const string & s){
  string st;
  string::size_type p = getNextArgument(s,0,st);
  funstring arg(st);
  st = arg.eval();

  string length;
  p = getNextArgument(s,p ,length);
  unsigned int len = atoi(length.c_str());
  
  return st.substr(0,len);
}

  
  
string funstring::eval() {
  string::size_type end;
  string::size_type start;
  string temp;
  //  cout << "eval==" << fs << "==" << endl;
  for (string::size_type i = 0; i < fs.size(); ++i){
    if (fs[i] == '&')
      if (fs.substr(i+1, 5) == "FILL[") {
	start = i;
	end = getEndParen(fs, i+6);
	temp = fill(fs.substr(i+6, end - (i+6) + 1));
	fs.replace(start, end-start+1, temp);
      }
      else if (fs.substr(i+1, 7) == "NOHTML[") {
	start = i;
	end = getEndParen(fs, i+8);
	temp = nohtml(fs.substr(i+8, end - (i+8) + 1));
	fs.replace(start, end-start+1, temp);
      }
      else if (fs.substr(i+1, 8) == "REPLACE[") {
	start = i;
	end = getEndParen(fs, i+9);
	temp = replace(fs.substr(i+9, end - (i+9) + 1));
	fs.replace(start, end-start+1, temp);
      }
      else if (fs.substr(i+1, 4) == "CUT[") {
	start = i;
	end = getEndParen(fs, i+5);
	temp = cut(fs.substr(i+5, end - (i+5) + 1));
	fs.replace(start, end-start+1, temp);
      }
      else if (fs.substr(i+1, 13) == "NOLINEBREAKS[") {
	start = i;
	end = getEndParen(fs, i+14);
	temp = noLineBreaks(fs.substr(i+14, end - (i+14) + 1));
	fs.replace(start, end-start+1, temp);
      }
      else if (fs.substr(i+1, 10) == "NOACCENTS[") {
	start = i;
	end = getEndParen(fs, i+11);
	temp = noAccents(fs.substr(i+11, end - (i+11) + 1));
	fs.replace(start, end-start+1, temp);
      }
      else if (fs.substr(i+1, 3) == "FS[") {
	start = i;
	end = getEndParen(fs, i+4);
	temp = firstSentence(fs.substr(i+4, end - (i+4) + 1));
	fs.replace(start, end-start+1, temp);
      }
     else if (fs.substr(i+1, 4) == "RSS[") {
	start = i;
	end = getEndParen(fs, i+5);
	temp = rss(fs.substr(i+5, end - (i+5) + 1));
	fs.replace(start, end-start+1, temp);
      }
 
  }
  return fs;
}
	

	


