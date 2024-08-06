// bk2site by Jose M. Vidal
// http://jmvidal.ece.sc.edu  vidal@sc.edu
//
// Converts the Netscape bookmark.html file into a yahoo-like
// web site.
//
// Copyright 1999 Jose M. Vidal
// Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
// This program is free software.  You can redistribute it and/or modify
// it under the terms of the GNU General Public License
//
// CONTRIBUTORS:
//  See the AUTHORS file
//
//
// $Id: bk2site.C,v 1.49 2000/12/15 23:52:56 jmvidal Exp $
// -*- mode:C++; tab-width:4; c-basic-offset:2; indent-tabs-mode:nil -*- 

//TO DO:
// - Escape accented charactes in name2filename (name2url)
// - Add ALIASID= and ALIASOF= options to directory comments, so as
//   to allow more flexible directory aliasing.
// - Add support for separators <HR>
// - urllog should have "time url IP" and bk2site should count only one hit
///   per IP.
//
// BUGS
// - Make search.pl ignore html marking....how????

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>

// #if TIME_WITH_SYS_TIME
// # include <sys/time.h>
// # include <time.h>
// #else
// # if HAVE_SYS_TIME_H
// #  include <sys/time.h>
// # else
// #  include <time.h>
// # endif
// #endif 
#include <ctime>

#include "channel.H" //this include must appear before #include "reference.H"
#include "reference.H"


//defined in reference.C
extern string trim(const string & s);
extern int cmpNoCase(const string& s, const string &s2);
extern int replaceAllNC(string & s, string s1, string s2);
extern int replaceAll(string & s, string s1, string s2);

const string version = VERSION;
using namespace std;

void printUsage(const string & s)
{
  cout << "Usage: " << s << ": [OPTIONS] [main-bookmark-file] [bookmarkfile]*" << endl 
       << "\t If main-bookmark-file is omitted we use ~/.netscape/bookmarks.html" << endl
       << "\t Any extra bookmark files given are merged with main, see the README" << endl << endl;
  cout << "Builds a yahoo-like website based on one or more Navigator bookmark files" << endl << endl
       << "-nd\t\t - don't use a subdirectory hierachy. Use ugly long filenames" << endl
       << "\t\t  it's useful for tree depth limited providers like geocities" << endl
    //	 << "-bf <book file>\t - File with bookmarks (~/.netscape/bookmarks.html)" << endl
       << "-f <config file> - Name of configuration file (~/.bk2siterc)" << endl
       << "-d <dest dir>\t - Local directory for generated files." << endl
       << "\t\t   Default is /home/httpd/html/" << endl
       << "-t <name>\t - Name of top-level bookmark folder to use." << endl
       << "\t\t   If ommited will convert whole bookmark file." << endl
       << "-f1 <file>\t - Index template file for top index. " << endl
       << "-f2 <file>\t - Template for the other pages. " << endl 
       << "-o <file>\t - Output bookmark file." << endl
			 << "-xbel <file>\t - Output an xbel file." << endl 
       << "-ns\t\t - Do NOT generate the website. " << endl 
       << "-nc\t\t - Do NOT put HTML comments in the output." << endl
       << "-old <int>\t - Number of days (30) after which a bookmark is no longer new." << endl << endl
       << "Version " << version << endl << endl;
  exit(1);
}

void readConfigStream(istream & configStream, string varValues[],
											vector<string> & extraFileBase, vector<string> & extraFileName,
											channelContainer & channels)
{
	string curVarName;
	string curVarValue;

	string channelName ="";
	string channelUrl;
	string channelFile;
	while (configStream >> curVarName) {
		getline(configStream, curVarValue);
		curVarValue = trim(curVarValue);
		if (curVarName[0] == '#') continue;
		if (curVarValue[0] == '"') //get rid of surrounding "'s
	    curVarValue.erase(0,1);
		if (curVarValue[curVarValue.size() -1] == '"')
	    curVarValue.erase(curVarValue.size() - 1,1);

		if (getenv("HOME") != NULL)
			replaceAll(curVarValue,"$HOME", getenv("HOME"));
		else
			replaceAll(curVarValue,"$HOME", "");

		if (getenv("USER") != NULL)
			replaceAll(curVarValue,"$USER", getenv("USER"));
		else
			replaceAll(curVarValue,"$USER", "");

		if (getenv("LOGNAME") != NULL)
			replaceAll(curVarValue,"$LOGNAME", getenv("LOGNAME"));
		else
			replaceAll(curVarValue,"$LOGNAME", "");

		int i, found = 0;
		for (i = 0; i < NUMVARS; i++){
	    if (cmpNoCase(varNames[i],curVarName) == 0){
				varValues[i] = curVarValue;
				//	cout << varNames[i] << ":" << varValues[i] << endl;
				found = 1;
				break;
	    }
		};
		//these two are special, since they can appear many times.
		if (cmpNoCase("EXTRAFILEBASE",curVarName) == 0){
			extraFileBase.push_back(curVarValue);
			found = 1;
		}
		else if (cmpNoCase("EXTRAFILENAME",curVarName) == 0){
			extraFileName.push_back(curVarValue);
			found = 1;
		};

		//these can also appear many times
		if (cmpNoCase("CHANNELNAME",curVarName) == 0){
			if (channelName != ""){ //push back old one
				channels.addNewChannel(channelName, channelUrl, channelFile);
			}
			else
				channelName = curVarValue;
			found = 1;
		}
		else if (cmpNoCase("CHANNELURL",curVarName) == 0){
			channelUrl = curVarValue;
			found = 1;
		}
		else if (cmpNoCase("CHANNELFILE",curVarName) == 0){
			channelFile = curVarValue;
			channels.addNewChannel(channelName, channelUrl, channelFile);
			channelName = channelFile = channelUrl = "";
			found = 1;
		};
		if (found == 0)
	    cerr << "ERROR: variable " << curVarName << " is not a recognized name." << endl;
	}
	if (extraFileName.size() != extraFileBase.size()){
		cerr << "ERROR: the number of extrafilename declarations is not the same as the " << endl
				 << "       number of extrafilebase declarations. Exiting." << endl;
		exit(1);
	}
}

int main (int argc, char **argv)
{
	//initiallize default values.
	string varValues[NUMVARS];
	varValues[NEWGIF] = "<img src=\"/icons/new.gif\" alt=\"Added %MONTHCRE1/%DAYCRE/%YEARCRE\">";
	varValues[DESTDIR] = "";
	varValues[TOPFOLDER] = ""; 
	varValues[BOOKMARKFILE] = "bookmarks.html";
	varValues[INDEXFILENAME] = "indexbase.html";
	varValues[OTHERFILENAME] = "otherbase.html";
	varValues[TIMECUTOFF] = "30";
	varValues[TOP] = "Top";
	varValues[INDEX] = "index.html";
	varValues[SEARCH] = "";
	varValues[SEARCHTOROOTPATH] = "../";
	varValues[URLLOGFILE] = "";
	varValues[SUBDIRSEP] = "/";
	varValues[BACKDIRSEP] ="../";
	varValues[SEARCHURLTEMPLATE] = "<A HREF=\"%URL\">%TITLE</A> %NEW %CONDDASH %COMMENT";
	varValues[NEWSTOPFOLDER] = "";
	varValues[TITLE] = "My Bookmarks";
	varValues[OUTPUTBOOKMARKFILE] = "";
	varValues[COMMENTS] = "yes";
	varValues[FOLDERCREATION] = "maxdescendants";
	varValues[TOPHITSINCLUDENEWS] = "no";

	string xbelFileName = "";

	string homeDir = getenv("HOME");

	vector<string> extraFileBase;
	vector<string> extraFileName;

	vector<string> bookmarkfiles;
	int numBookmarkfiles = 0;
	int generateSite = 1;

    //read /etc/bk2site/dot.bk2siterc
    //    string globalConfigFile("CONFIGFILE");
    //    cout << "Checking " << "CONFIGFILE" << endl; //the -D on the compiler does not work. (egcs?)
	string globalConfigFile(CONFIGFILE);
	// cout << "Checking " << CONFIGFILE << endl;
	//the -D on the compiler does not work. (egcs?)
	// string globalConfigFile("/etc/bk2site/dot.bk2siterc");
	//string globalConfigFile("/etc/bk2site/dot.bk2siterc");
	ifstream globalConfigStream(globalConfigFile.c_str(), ios::in);
	channelContainer channels;
	if (globalConfigStream) 
		readConfigStream(globalConfigStream, varValues, extraFileBase, extraFileName, channels);

	int readHomeConfig = 1;
	for (int i = 1; i < argc; i++){
		string s(argv[i]);
		if (s == "-f")
			readHomeConfig = 0;
	}

	//read ~/.bk2siterc
	string configFile = homeDir + "/.bk2siterc";
	if (readHomeConfig == 1) {
		cout << "Reading " << configFile << endl;
		ifstream configStream(configFile.c_str(), ios::in);
		if (configStream) 
			readConfigStream(configStream, varValues, extraFileBase, extraFileName, channels);
	};

	//read the file  in -f
	for (int i = 1; i < argc; i++){
		string s(argv[i]);
		if (s == "-f")
	    if (++i < argc){
				configFile = argv[i];
				cout << "Reading " << configFile << endl;
				ifstream configStream2(configFile.c_str(), ios::in);
				if (configStream2)
					readConfigStream(configStream2, varValues, extraFileBase, extraFileName, channels);
				else {
					cerr << "ERROR: Could not open " << configFile << endl;
					return 1;
				};
	    }
	}

	//now read the command line arguments.
	for (int i = 1; i < argc; i++){
		string s(argv[i]);
		if (s == "-nd") {
	    varValues[SUBDIRSEP] = "_";
	    varValues[BACKDIRSEP] ="";
		}
		else if (s == "-ns"){
			generateSite = 0;
		}
		else if (s == "-nc"){
			varValues[COMMENTS] = "no";
		}
		else if (s == "-d")
	    if (++i < argc)
				varValues[DESTDIR] = argv[i];
	    else 
				printUsage(argv[0]);
		else if (s == "-t")
	    if (++i < argc)
				varValues[TOPFOLDER] = argv[i];
	    else
				printUsage(argv[0]);
		else if (s == "-f1")
	    if (++i < argc)
				varValues[INDEXFILENAME] = argv[i];
	    else
				printUsage(argv[0]);
		else if (s == "-f2")
	    if (++i < argc)
				varValues[OTHERFILENAME] = argv[i];
	    else
				printUsage(argv[0]);
		else if (s == "-old")
	    if (++i < argc){
				varValues[TIMECUTOFF] = argv[i];
	    }
	    else
				printUsage(argv[0]);
		else if (s == "-o")
	    if (++i < argc){
	      varValues[OUTPUTBOOKMARKFILE] = argv[i];
	    }
	    else
				printUsage(argv[0]);
		else if (s == "-xbel")
	    if (++i < argc){
				xbelFileName = argv[i];
	    }
	    else
				printUsage(argv[0]);
		else if (s == "-f") {
	    ++i;
	    continue;
		}
		else if (s == "-bf") {
	    if (++i < argc)
				varValues[BOOKMARKFILE] = argv[i];
	    else
				printUsage(argv[0]);
		}
		else if ((s == "-h") || (s == "--help") || (s == "-help") || (s == "-v")) {
			printUsage(argv[0]);
		}
		else {
			if (numBookmarkfiles == 0){
				varValues[BOOKMARKFILE] = s;
			}
			else
				bookmarkfiles.push_back(s);
			numBookmarkfiles++;
		}
	}
	int last;
	if (varValues[DESTDIR] == "")  
		varValues[DESTDIR] = "/home/httpd/html/";
	last = varValues[DESTDIR].length() -1;
	if (varValues[DESTDIR][last] != '/') varValues[DESTDIR] = varValues[DESTDIR] + "/";
	if (varValues[BOOKMARKFILE] == "")
		varValues[BOOKMARKFILE] = homeDir + "/.netscape/bookmarks.html";
	last =varValues[SEARCHTOROOTPATH].length() -1;
	if (varValues[SEARCHTOROOTPATH][last] != '/') 
		varValues[SEARCHTOROOTPATH] =  varValues[SEARCHTOROOTPATH] + "/";
	referenceTree rt;
    
	//    for (int i = 0; i <= NUMVARS; i++)
	//	cout << varNames[i] << "\t" << varValues[i] << endl;
	if (varValues[BOOKMARKFILE] != "") {
		iwebstream istream(varValues[BOOKMARKFILE].c_str(), ios::in);
		if (istream == 0){
			cout << "Could not open " << varValues[BOOKMARKFILE] << endl;
			return 1;
		}
		cout << "Reading " << varValues[BOOKMARKFILE] << endl;
		istream >> rt;
		cout << "Total Bookmarks in file= " << rt.getNumLeafs() << endl;
	}

	//    cout << rt;
	referenceTree * myrt = rt.getSubtree(varValues[TOPFOLDER]);
	if (myrt == 0) {
		cerr << "ERROR: " << varValues[TOPFOLDER] << " not found in " << varValues[BOOKMARKFILE] << endl;
		return 1;
	}


	//ugly hack to append the top folder title to the tree.
	// we must undo this hack after all the merges.
	referenceTree myrtTemp;
	reference refTemp;
	if (varValues[TOPFOLDER] != ""){
		refTemp.title = varValues[TOPFOLDER];
		refTemp.children = myrt;
		refTemp.folder = true;
		refTemp.aliasOf = "";
		refTemp.priv = false;
		myrtTemp.addReference(refTemp); //adds a copy of refTemp?, myrtTemp is a copy of myrt with new root.
		myrt = &myrtTemp;
		myrt->fixNumChildren();
	}

	//Merge all the other trees in
	for (vector<string>::const_iterator fn = bookmarkfiles.begin(); fn != bookmarkfiles.end(); fn++){
		referenceTree ort;
		const string & bfname = * fn;
		iwebstream obf(bfname.c_str(), ios::in);
		if (obf == 0){
			cout << "Could not open " << bfname << endl;
			return 1;
		}
		cout << "Reading " << bfname << endl;
		obf >> ort;
		cout << "Total Bookmarks in file = " << ort.getNumLeafs()<< endl;
		cout << "Merging " << bfname << " into main tree." << endl;
		myrt->merge(ort);
		cout << "Total Bookmarks in merged file = " << myrt->getNumLeafs() << endl;
	} 

	//undo the ugly hack
	if (varValues[TOPFOLDER] != ""){
		myrt = myrt->getFirstGrandchild();
		refTemp.children = 0;
	}

	//read the urllog file if needed
	if (varValues[URLLOGFILE] != ""){ 
		iwebstream istream(varValues[URLLOGFILE].c_str(), ios::in);
		vector<string> theURLS;
		vector<time_t> theTimes;
		if (istream == 0){ //If file exists but is empty
			cout << "Could not open " << varValues[URLLOGFILE] << " for reading." << endl;
		}
		else {
			cout << "Reading " << varValues[URLLOGFILE] << endl;
			time_t curTime;
			time_t cutoffTime = time(0) - atoi(varValues[HITSTIMECUTOFF].c_str());
			string currentURL = "";
			while (!(istream.eof())){
				istream >> curTime;
				istream >> currentURL;
				//	    cout << curTime << "-" << currentURL << endl;
				//       if (('0' <= currentURL[0] ) && ('9' >= currentURL[0])) { //starts with a num, then its not a URL
				// 	int numhits = atoi(currentURL.c_str());
				// 	myrt->increaseHits(oldURL, numhits - 1); //we already added 1
				//       }
				//       else
				// 	myrt->increaseHits(currentURL, 1);
				//       oldURL = currentURL;

				//if url starts with a number (its a time) or url is null
				//  keep trying to read a time/url pair.
				while (((currentURL[0] >= '0' && currentURL[0] <= '9') || currentURL == "") && !istream.eof()){
					curTime = atoi(currentURL.c_str());
					istream >> currentURL;
				}
	    
				if (curTime >= cutoffTime){
					channels.increaseHits(currentURL, 1);
					myrt->increaseHits(currentURL, 1);
					theURLS.push_back(currentURL);
					theTimes.push_back(curTime);
				}
			}
			//write out only the ones after cutoffTime;
			istream.close();
		}
		ofstream ostream(varValues[URLLOGFILE].c_str(), ios::out);
		cout << "Opening " << varValues[URLLOGFILE] << " for writing" << endl;
		if (ostream == 0){
	    cout << "Could not open " << varValues[URLLOGFILE] << " for writing" << endl;
	    return 1;
		}
		for (unsigned int i=0; i < theURLS.size(); i++)
	    ostream << theTimes[i] << "\t" << theURLS[i] << endl;
		ostream.close();
	}

	//Fix Aliases.
	myrt->fixAliases(myrt, "", varValues, "");

	//Set the creationtime for the folders to be the max of creation time of descendants
	// or children.
	if (varValues[FOLDERCREATION] == "maxdescendants")
		myrt->setFolderCreationToMaxDescendant();
	else if (varValues[FOLDERCREATION] == "maxchildren")
		myrt->setFolderCreationToMaxChildren();

	if (generateSite){
		myrt->createSite(varValues, channels);

		for (unsigned int ne = 0; ne < extraFileName.size(); ne++) 
			myrt->createPage(extraFileBase[ne], varValues[DESTDIR] + extraFileName[ne], varValues, channels);
	}

	//write out the bookmark file    
	if (varValues[OUTPUTBOOKMARKFILE] != "") {
		ofstream otstream(varValues[OUTPUTBOOKMARKFILE].c_str(), ios::out);
		cout << "Opening " << varValues[OUTPUTBOOKMARKFILE] << " for writing" << endl;
		if (otstream == 0){
			cout << "Could not open " << varValues[OUTPUTBOOKMARKFILE] << " for writing" << endl;
			return 1;
		}
		myrt->writeAsBookmarkFile(otstream, varValues[TITLE]);
	}


	//write out the xbel file    
	if (xbelFileName != "") {
		ofstream otstream(xbelFileName.c_str(), ios::out);
		cout << "Opening " << xbelFileName << " for writing" << endl;
		if (otstream == 0){
			cout << "Could not open " << xbelFileName << " for writing" << endl;
			return 1;
		}
		myrt->writeAsXBELFile(otstream, varValues[TITLE]);
	}


	return 0;
}
