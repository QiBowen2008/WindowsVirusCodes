// Author: Jose M. Vidal
// $Id: iwebstream.H,v 1.5 2000/02/14 23:00:06 jmvidal Exp $
// This code is copyright of Jose M. Vidal and released under
// the GNU General Public License
//
// A class thats is meant to look like an istream, regardless of
//  whether it opens a local file or an http object.
//  Sometime later I might also add ftp.
//
#ifndef IWEBSTREAM_H
#define IWEBSTREAM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cctype>
//#include <ctime>
#include <sys/time.h>
#include <sys/socket.h>
#include <string>
#include <iostream>

// Objects of the http class can connect to WWW servers and get pages from
// those servers

using namespace std;

class iwebstream {
private:
  int socketDesc; // socket descriptor used in system calls
  fd_set read_map; // selects socket descriptors that are ready for reading
  struct timeval tval; // timeout for reading
  int defaultTimeout; // default value of tval
  string data; //the contents of the file
  unsigned int position; //the current position on data

  void readFile(string & filename);
  int connect(string hostName, unsigned short int port, int timeOut);
  int get(const string hostname, const string port, const string path, string& pageContents);
  unsigned int findNC(const string & s, unsigned int p);

public:
  iwebstream(string url, int timeout = 30);
  iwebstream & operator>>(long int & t);
  iwebstream & operator>>(int & t);
  iwebstream & operator>>(string & s);
  string getData() {
    return data;
  }
  char get();
  void putback(char & c);
  void getline(string & s);
  bool eof();
  void close(){};
  bool operator==(int x);
  bool operator!=(int x) {
    return !operator==(x);};

  //find the next occurance of s, set position to point to it
  bool iwebstream::find(const string & s);
  //find the next occurance of s, set position to point past it
  bool iwebstream::findAndPass(const string & s);

  //return whatever is between the next begin & end. If begin is not found,
  // return ""
  string findTag(const string & begin, const string & end, const string & after = "",
		 const string & before = "");
};

#endif // IWEBSTREAM_H
