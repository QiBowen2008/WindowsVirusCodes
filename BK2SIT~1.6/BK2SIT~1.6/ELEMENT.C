// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
//
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// -*- mode:C++; tab-width:4; c-basic-offset:2; indent-tabs-mode:nil -*- 
//
// $Id: element.C,v 1.18 2000/01/04 15:51:19 jmvidal Exp $
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "element.H"
#include "iwebstream.H"

extern string trim(const string & s);
extern int replaceAll(string & s, string s1, string s2);
extern string myItoa (int n);

//Creates a new element using the first definition it finds in fileContents
// If none, it creates an element of type=""
//Eliminates this definition from fileContents.
element::element(string & fileContents, int idnum) : type(""),
						     printTemplate(""),
						     separator(""),
						     top(""),
						     bottom(""),
						     between(""),
						     daysOld(0),
						     numCols(1),
						     maxNum(0),
						     startNum(0)
{
  string key = placeholder + ":" + BEGIN_TAG;
  string::size_type blockStart = fileContents.find(key);
  if (blockStart == string::npos)
    return; //no more elements there;
  string::size_type endKey= fileContents.find(" ", blockStart);
  string::size_type endDef= fileContents.find(placeholderEnd, blockStart);
  type = fileContents.substr(endKey, endDef - endKey);
  type = trim(type);
  cout << "Found " << type << " block." << endl;
  string endKeyString = placeholder + ":" + END_TAG + placeholderEnd;
  string::size_type blockEnd = fileContents.find(endKeyString, blockStart);


  //Make a copy of the block and search it for all the variables and values.
  //Set the data member values using them.
  key = placeholder + ":";
  string block = fileContents.substr(endDef, blockEnd - endDef);
  string::size_type start =0;
  string::size_type endName= 0;
  while ((start = block.find(key)) != string::npos) {
    endName = block.find(" ", start);
    string varName = block.substr(start + key.size(), endName -( start + key.size()));
    //    cout << "varName=" << varName << endl;
    endDef = block.find(placeholderEnd, endName);
    string varValue = block.substr(endName, endDef - endName);
    //    cout << "varValue=" << varValue << endl;
    trim(varValue);
    if (varName == COLS_VAR)
      numCols = atoi(varValue.c_str());
    else if (varName == TEMPLATE_VAR)
      printTemplate = varValue;
    else if (varName == TOP_VAR)
      top = varValue;
    else if (varName == BOTTOM_VAR)
      bottom = varValue;
    else if (varName == BETWEEN_VAR)
      between = varValue;
    else if (varName == SEPARATOR_VAR)
      separator = varValue;
    else if (varName == MAXNUM_VAR)
      maxNum = atoi(varValue.c_str());
    else if (varName == STARTNUM_VAR)
      startNum = atoi(varValue.c_str());
    else if (varName == DAYSOLD_VAR)
      daysOld = atoi(varValue.c_str());
    else
      cerr << "Did not understand " << key << varName << endl;
    block.replace(start, endDef + placeholderEnd.size() - start, "");
  }

  //Eliminate the block and put a marker in its place
  fileContents.replace(blockStart, blockEnd + endKeyString.size() - blockStart, placeholder + ":" + myItoa(idnum) + placeholderEnd);

}



/**Append the list of refs into ouput using our variable values to guide the output.
  Does not output reference r if r.isPrivate();
  If type == folder then only output if r.isFolder()
  otherwise only output if !(r.isFolder())
*/
string element::sendAsHTML(vector<reference> & refs, const string varValues []){
  string output = "";
  int count = 0;
  int total =0;
  int skipped = 0;
  //  cout << "sendAsHtml refs.size()=" << refs.size() << "maxNum=" << maxNum << " type=" << type << endl;
  if (maxNum == 0) maxNum = 9999;
  time_t currentTime = time(0); //.tv_sec;
  time_t timeCutoff = currentTime - (daysOld * 60 * 60 * 24);
  if (daysOld == 0)
    timeCutoff = -1;
  for (vector<reference>::iterator i = refs.begin(); i != refs.end(); ++i){ 
    //calculate the total we will print out
    reference &r = *i;
    if ( !(r.isPrivate()) && (r.creationTime > timeCutoff) && 
	 (((type == FOLDER_TYPE) && (r.isFolder())) ||
	  ((type != FOLDER_TYPE) && !(r.isFolder())))) {
      if (++skipped < startNum)
	continue;
      total++;
      if (++count >= maxNum) //enough in this.
	break;
    }
  }

  //  cout << "Total leafs = " << total << " type=" << type << endl;
  if (total == 0) return output; //no need to waste time

  //sort refs if they need it (depends on the type)
  if ((type == NEWADD_TYPE) || (type == NEWS_TYPE) || (type == NEWSANDNEWADD_TYPE)){
    sort(refs.begin(), refs.end(), referenceCmpCreation()); //sort based on add date
    //    cout << refs[0] << endl << refs[1] << endl;
  }
  else if (type == TOPHITS_TYPE){
    sort(refs.begin(), refs.end(), referenceCmpHits()); //sort based on hits
  };

  output += top;
  count = 0;
  skipped = 0;
  int numPerRow;
  if (total%numCols == 0)
    numPerRow = total / numCols;
  else
    numPerRow = (total+1)/numCols;
  bool printSeparator = false;
  bool printBetween = false;

  for (vector<reference>::iterator i = refs.begin(); i != refs.end(); ++i){
    reference &r = *i;
    if (!(r.isPrivate()) && (r.creationTime > timeCutoff) &&
	 (((type == FOLDER_TYPE) && (r.isFolder())) ||
	  ((type != FOLDER_TYPE) && !(r.isFolder())))) {
      if (++skipped < startNum)
	continue;
      if (printSeparator){
	output += separator + "\n";
	printSeparator = false;
      }
      else if (printBetween) 
	output += between + "\n";
      else
	printBetween = true;
      output += r.sendAsHTML(printTemplate, varValues);

      if (++count >= maxNum) //enough in this.
	break;
      if (count%numPerRow == 0){ //add separator next time, if there is one!
	printSeparator = true;
      };
    };
  };
  output += bottom;
  return output;
}

/**returns the contents of filename, replacing all includes
   with their contents, recursively. */
string fileView::getContents(const string & fileName){
  //  ifstream ifile(fileName.c_str());
  iwebstream ifile(fileName);
  if (ifile == 0){
    cerr << "Could not open input file " << fileName << endl;
    return "";
  }
  cout << "Reading " << fileName << endl;
  string contents = ifile.getData();
//   while (!ifile.eof()){ 
//     contents += c;
//     c= ifile.get();
//   }
//  ifile.close();
  
  string::size_type includeStart = 0;
  string key = placeholder + ":" + INCLUDE_TAG; //<!--bk2site:include
  string key2 = placeholder + ":" + INCLUDESEARCH_TAG; //<!--bk2site:include:search
  while ((includeStart = contents.find(key,includeStart)) != string::npos) {
    
    string test = contents.substr(includeStart, key2.size()); 

    string::size_type endKey = contents.find(" ", includeStart);
    string::size_type endDef = contents.find(placeholderEnd, includeStart);
    string includeFileName = contents.substr(endKey, endDef - endKey);
    includeFileName = trim(includeFileName);
    string includeFileContents = getContents(includeFileName);
    if (test != key2) {
      replaceAll(includeFileContents,QUERY_DIR,"");
      replaceAll(includeFileContents,NUMBER_DIR,"");
      replaceAll(includeFileContents,ESCQUERY_DIR,"");
    }
    contents.replace(includeStart , endDef + placeholderEnd.size() - includeStart, includeFileContents);
    includeStart += includeFileContents.size();
  }

  return contents;
}

 
/** instantiate fileview from the contents of fileName; */
fileView::fileView(const string & fileName){
  
  fileContents = getContents(fileName);

  if (fileContents == "") {
    cerr << "ERROR: file was empty" << endl;
  }

  int i =0;
  while (1){
    element e(fileContents, i);
    if (e.getType() == "") break; //no more elements
    els.push_back(e);
    i++;
  }

}
/** Returns a string that is the instantiation of this fileview
    using the given contents, allReferences, newsitems, varvalues and channels.
    allReferences contains all the DIRECTORY references only (no news).
    newsItems contains all the news items.
    contents contains all the children (i.e. folders).
    The returned string is a valid HTML page */
string fileView::sendAsHTML(vector<reference> & contents, vector<reference> & allReferences, 
			    vector<reference> & newsItems, const string varValues [],
			    channelContainer & channels) {
  string resultContents = fileContents; //make a copy to change and return.
  
  vector<string> elvalues;

  vector<reference> newsAndReferences;
  vector<reference>::iterator j;
  for ( j = allReferences.begin(); j != allReferences.end(); ++j) //dont push aliases in, avoid duplicates
    if (!((*j).isAliasof()))
      newsAndReferences.push_back(*j);
  for (j = newsItems.begin(); j != newsItems.end(); ++j)
    if (!((*j).isAliasof()))
      newsAndReferences.push_back(*j);
  vector<element>::iterator i;
  for (i = els.begin(); i != els.end(); ++i){
    element &e = *i;
    string type = e.getType();
    if ((type == URL_TYPE) || (type == FOLDER_TYPE)){
      elvalues.push_back(e.sendAsHTML(contents, varValues));
    }
    else if (type == NEWADD_TYPE){
      elvalues.push_back(e.sendAsHTML(allReferences, varValues));
    }
    else if (type == TOPHITS_TYPE) {
      if (varValues[TOPHITSINCLUDENEWS] == "yes")
	elvalues.push_back(e.sendAsHTML(newsAndReferences, varValues));
      else
	elvalues.push_back(e.sendAsHTML(allReferences, varValues));
    }
    else if (type == NEWS_TYPE) {
      elvalues.push_back(e.sendAsHTML(newsItems, varValues));
    }
    else if (type == NEWSANDNEWADD_TYPE) {
      elvalues.push_back(e.sendAsHTML(newsAndReferences, varValues));
    }
    else if (channels.nameExists(type)) {
      channel c;
      channels.getChannel(type, c);
      //      c.readChannel();
      vector<reference> items = c.getItems();
      elvalues.push_back(e.sendAsHTML(items, varValues));
    }
    else{
      cerr << "Did not recognize type " << e.getType() << endl;
      cerr << "Perhaps it is a channel that you neglected " << endl
	   << " to define in ~/.bk2siterc ?" << endl;
    }
      
  }
  //replace all values in resultContents
  int num = 0;
  for (i = els.begin(); i != els.end(); ++i, ++num){
    //    string key = placeholder + ":" + els[i].getType() + placeholderEnd;
    string key = placeholder + ":" + myItoa(num) + placeholderEnd;
    string text;
    if (varValues[COMMENTS] == "yes")
      text = "\n<!--bk2site code begin-->\n" + elvalues[num] + "\n<!--bk2site code end-->\n";
    else
      text = elvalues[num];
    replaceAll(resultContents, key, text);
  };

   return resultContents;
}
