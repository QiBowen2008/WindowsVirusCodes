// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
//
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// -*- mode:C++; tab-width:4; c-basic-offset:2; indent-tabs-mode:nil -*- 
// $Id: element.H,v 1.12 2000/01/03 03:23:52 jmvidal Exp $

#ifndef ELEMENT_H
#define ELEMENT_H

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
#include "reference.H"
//#include "channel.H"

class channelContainer;

//the element types
const string FOLDER_TYPE = "folder";
const string NEWADD_TYPE = "newadd";
const string NEWS_TYPE = "news";
const string TOPHITS_TYPE = "tophits";
const string URL_TYPE = "url";
const string NEWSANDNEWADD_TYPE = "newsandnewadd";

//the variable names for an element
const string COLS_VAR = "cols";
const string TEMPLATE_VAR = "template";
const string TOP_VAR = "top";
const string BOTTOM_VAR = "bottom";
const string BETWEEN_VAR = "between";
const string SEPARATOR_VAR = "separator";
const string MAXNUM_VAR = "maxnum";
const string STARTNUM_VAR = "startnum";
const string DAYSOLD_VAR = "daysold";

//special tags
const string BEGIN_TAG = "begin";
const string END_TAG = "end";
const string TITLE_TAG = "title";
const string NAVIGATEBAR_TAG = "navigatebar";
const string INCLUDE_TAG = "include";
const string INCLUDESEARCH_TAG = "include:search";
const string SEARCH_TAG = "search"; // path to urls.db - btb@debian.org
const string DATE_TAG = "date";
const string PATH_TAG = "path";

//directives used for search.pl. these are elimininated from the input file by
//  include, but not by include:search
const string QUERY_DIR = "%QUERY";
const string NUMBER_DIR = "%NUMBER";
const string ESCQUERY_DIR = "%ESCQUERY";

using namespace std;
class reference; 

//An element is the "view" part of the data-view-modify abstraction.
//Each element is a group of items, displayed as a group, possibly in columns.
//
//The element does NOT hold the actual data, only the instructions for how to
//turn a vector<reference> into HTML.
//
class element {
  string type; //the types=newadd, tophits, news, folder, url
  string printTemplate; //in HTML + %URL and other directives
  string separator; //what separates columns
  string top; //html to add before the items, IFF there are items to print
  string bottom; //html to add after the items, IFF there are items to print
  string between; //html to add in between elements, NOT at the head or tail.
  int daysOld; //if entry was created before these many days, do not print it. If 0, print all.
  int numCols; //the number of columns
  int maxNum; //maximum number of items to printout (if 0, print all)
  int startNum; //start with item number startNum (where, the first one is number 1)
public:
  element() : type(""),
	      printTemplate(""),
	      separator(""),
	      top(""),
	      bottom(""),
	      between(""),
	      daysOld(0),
	      numCols(1),
	      maxNum(0),
	      startNum(0)
  {};
  element(string & fileContents, int idnum);

  element(const element & el) : type(el.type),
				printTemplate(el.printTemplate),
				separator(el.separator),
				top(el.top),
				bottom(el.bottom),
				between(el.between),
				daysOld(el.daysOld),
				numCols(el.numCols),
				maxNum(el.maxNum),
				startNum(el.startNum)
  {};
  ~element(){};
  string sendAsHTML(vector<reference> & refs, const string varValues []);
  string getType() {return type;};
  string getTemplate() {return printTemplate;};
};



class fileView {
  vector<element> els;
  vector<string> eltypes;
  string fileContents;

public:
  fileView(){};
  ~fileView(){};
  fileView(const string & fileName); //instantiate fileview from the contents of fileName;
  string getContents(const string & fileName);
  //return a string which is the printout of refs using fileview.
 string sendAsHTML(vector<reference> & contents, vector<reference> & allReferences, 
		   vector<reference> & newsItems, const string varValues [],
		   channelContainer & channels); 
};
#endif
