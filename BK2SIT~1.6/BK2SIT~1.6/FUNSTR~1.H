// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
//
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// -*- mode:C++; tab-width:4; c-basic-offset:2; indent-tabs-mode:nil -*- 
//
// $Id: funstring.H,v 1.2 2000/02/06 19:49:45 jmvidal Exp $


#ifndef FUNSTRING_H 
#define FUNSTRING_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

//Maps asci from "ISO-8859-1 dec. code" (starting at 32) to
// an equivalent character that can be used when we don't want
// accents.
const string EQUIVCHAR[224] = {
  " ", "!","\"", "#", "$", "%", "&","\'", "(", ")", 
  "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", 
  "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", 
  ">", "?", "@", "A", "B", "C", "D", "E", "F", "G", 
  "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", 
  "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", 
  "_", "_", "_", "_", "_", "a", "b", "c", "d", "e",
  "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
  "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", 
  "z", "{", "|", "}", "~", " ", "_", "_", "_", "_", 
  "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", 
  "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", 
  "_", "_", "_", "_", "_", "_", "_", "A", "A", "A", 
  "A", "A", "A", "AE", "C", "E", "E","E", "E", "I", 
  "I", "I", "I", "D", "N", "O", "O", "O", "O", "O",
  "x", "O", "U", "U", "U", "U", "Y", "_","ss", "a", 
  "a", "a", "a", "a", "a","ae", "c", "e", "e", "e", 
  "e", "i", "i", "i", "i", "d", "n", "o", "o", "o", 
  "o", "o", "_", "o", "u", "u", "u", "u", "y", "_", 
  "y", "ss","a", "a", "a", "a", "a", "a","ae", "c", 
  "e", "e", "e", "e", "i", "i", "i", "i", "d", "n", 
  "o", "o", "o", "o", "o", "_", "o", "u", "u", "u", 
  "u", "y", "_", "y"
 };


/** This class defines a string with a lot of functions in it. The 
    functions are represented in the string and by an actual member
    function of this class (of the same name). The functions in the
    string start with an & character and their parameters are
    separated with a |, like &fun[par1|par2]  */
class funstring {
  /** The string */
  string fs; 
public:
  funstring(const string & s) : fs(s){};

  string::size_type getEndParen(const string & s, string::size_type start);
  string::size_type getNextArgument(const string & s, string::size_type pos, string & res);
  string getEvaledArgument(string::size_type start, string::size_type & end);

  /** evaluates all the functions in fs and returns the result */
  string eval(); 

  /** Helper function */
  int replaceAll(string & s, string s1, string s2);

  /** The functions */
  string fill(const string & s);
  string nohtml(const string & s);
  string noLineBreaks(const string & s);
  string noAccents(const string & s);
  string firstSentence(const string & s);
  string replace(const string & s);
  string cut(const string & s);
  string rss(const string & s);
  
};


#endif
