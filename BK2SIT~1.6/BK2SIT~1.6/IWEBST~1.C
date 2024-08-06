// Author: Jose M. Vidal
// $Id: iwebstream.C,v 1.11 2000/05/24 14:31:08 jmvidal Exp $
// This code is copyright of Jose M. Vidal and released under
// the GNU General Public License
//
// A class thats is meant to look like an istream, regardless of
//  whether it opens a local file or an http object.
//  Sometime later I might also add ftp.
//
#include <cstdio>
#include <cctype>
//#include <ctime>
#include <sys/time.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include <iostream>
#include <sys/uio.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <exception>
#include "iwebstream.H"

//#define DEBUG

//#define DEBUG_TIMEOUT

// The constructor.
iwebstream::iwebstream(string url, int timeout = 30) : defaultTimeout(timeout), data(""), position(0) {
  string origurl = url;
  string method;
  string host;
  string filename; //directory
  string portString;
  int port;

  if (url.substr(0,5) == "http:"){
    method = "http";
    url = url.substr(5, url.size() - 5);
  }
  else if (url.substr(0,5) == "file:") {
    method = "file";
    url = url.substr(5, url.size() - 5);
  }
  else
    method = "file";

  //  cout << "url=" << url << endl;
  if (url.substr(0,2) == "//") {
    url = url.substr(2, url.size() - 2);
    string::size_type end = url.size();
    if (url.find(":") != string::npos)
      end = url.find(":");
    else if (url.find("/") != string::npos)
      end = url.find("/");
    host = url.substr(0,end);
    if (end == url.size()){
      url = ""; //substr with len 0 core dumps
    }
    else {
      url = url.substr(end, url.size() - end);
    }
  }
  else{
    host = "";
    if (method == "http")
      cerr << "Malformed URL = " << origurl << endl;
  }
  //  cout << "url=" << url << endl;

  if ((url != "") && (url[0] == ':')) {
    string::size_type end = url.size();
    if (url.find("/") != string::npos)
      end = url.find("/");
    portString = url.substr(1,end -1);
    port = atoi(portString.c_str());
    if (url.size() >= end)
      url = "";
    else
      url = url.substr(end, url.size() - end);
  }
  else {
    port = 80;
    portString = "80";
  }
  //  cout << "url=" << url << endl;

  filename = url;
  if (filename == "")
    filename = "/";

  if (method == "file") { 
      readFile(filename);
  }
  else if (method == "http") { //an http request
    if (connect(host, port, timeout) < 0){
      cerr << "Could not connect to " << host << endl;
      return;
    }
    int count = 0;
    while ((get(host, portString, filename, data) == -1) && (count++ < 5)) {
      cerr << "Error downloading, trying again (" << count << "/5)..." << endl;
      data = "";
      if (connect(host, port, timeout) < 0){
	cerr << "Could not connect to " << host << endl;
	return;
      }
    }
    //    cout << "Done with get" << endl;
  }
  else {
    cerr << "Unknown method " << method << " in " << url << endl;
  }
}

  

void
iwebstream::readFile(string & filename) {
  ifstream ifs (filename.c_str());
  if (ifs == 0) {
    cerr << "Could not open " << filename << endl;
    return;
  }
  for (char c = ifs.get(); !ifs.eof(); c=ifs.get()){
    data += c;
  }
  ifs.close();
}

// The connect() method accepts a hostname and port of a WWW server to connect
// to, as well as a timeout to be used during gets of pages.  It returns 0
// if successful, -1 if not.
int
iwebstream::connect(string hostName, unsigned short int port, int timeOut)
{
  struct sockaddr_in name;
  struct hostent *hp;
  //  cout << "connect : " << hostName << ":" << port << " " << timeOut << endl;
  // Create TCP socket
  if ((socketDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("INET Domain Socket");
    exit(1);
  }

  // Create structure that points to the WWW server
  name.sin_family = (short int)AF_INET;

  // A 1 here appears as port 256.
  name.sin_port = htons(port); 

  // Resolve hostname to an IP address
  hp = gethostbyname(hostName.c_str());
  if (hp == NULL) {
    socketDesc = -1;
    cerr << "Could not resolve host name= " << hostName << endl;
    return (-1);
  }
  memcpy(&name.sin_addr.s_addr, hp->h_addr, hp->h_length);
  //  cerr << "h_addr= " << (int)(char)hp->h_addr[0] << "." << (char)hp->h_addr[1] << endl;
  //  cerr << "h_length= " << hp->h_length << endl;

  // Connect to the host at the specified port
  if (::connect(socketDesc, (struct sockaddr *) &name, sizeof(name)) < 0) {
    perror("connect");
    socketDesc = -1;
    cerr << "Could not connect to the host at the given port." << endl;
    return (-1);
  }
  //  cerr << "Connected." << endl;
  // Initialize structures used during the select() system call in the
  // get() method
  FD_ZERO(&read_map);
  FD_SET(socketDesc, &read_map);
  tval.tv_sec = timeOut;
  tval.tv_usec = 0;
  return (0);
}

const string version = VERSION;

// The get() method accepts the path to a WWW page and gets that page from
// the server to which the object is currently connected.  The page is 
// returned by reference in the pageContents string.
// Return -1 if there was an error
int
iwebstream::get(const string hostname, const string port, const string path, string& pageContents)
{
  string request;
  char buf[10000];
  int socketsReady;
  int numRead = 0;
  bool timedOut = false;

  // Build the request and send it to the WWW server
  request = "GET " +  path + " HTTP/1.0\r\n" +
    "User-Agent: bk2site/" + version + " (http://bk2site.sourceforge.net/)\r\n" +
    "Host: " + hostname + ":" + port +"\r\n" +
    "Accept: */*\r\n\r\n";
  write(socketDesc, request.c_str(), request.length());
  //  cout << "----------" << endl << request << "-------------" << endl;
 
#ifdef DEBUG
  cout << "HTTP commands sent, waiting for reply..." << endl;
  cout << "Timeout = " << tval.tv_sec << endl;
#endif

  // Keep reading data until error, timeout, or we read all the needed bytes.
  int totalRead = 0;
  int totalSize = 100000000;
  int maxReadZero = 100; //max number of times we read 0 bytes before closing connection
  int numReadZero = 0;  //number of times we have have read 0 bytes
  string lengthKeyword = "Content-Length:";
  bool foundHeader = false;
  string::size_type endHeader;
  do {
    do {
      socketsReady = select(socketDesc + 1, &read_map, NULL, NULL, &tval);
    }
    while(socketDesc == -1 && errno == EINTR);
    //    cout << "socketsReady= " << socketsReady << endl;
    if (socketsReady > 0) {
      // Data is available on the socket

      do{
	numRead = read(socketDesc, buf, sizeof(buf));
      }
      while (numRead == -1 && errno == EINTR); //I got this from wget source code.
      if (numRead == 0)
	numReadZero++;
      //      cout << numRead << "." << numReadZero << endl;
      if ((numRead != 0) && (numRead != EOF)) {

	totalRead += numRead;
	// Append data to the string holding the page contents
	string newstuff(buf,numRead);
	//	cout << "---" << newstuff;
	pageContents += newstuff;

	if (((endHeader = pageContents.find("\r\n\r\n")) != string::npos) && !foundHeader){
	
	  foundHeader = true;
	  totalRead -= endHeader;
	  totalRead -= 4;
	  string::size_type lstart = pageContents.find(" ");
	  string resultCode = pageContents.substr(lstart+1,3);
	  if (resultCode != "200"){ //we are only happy with 200, for now
	    lstart = pageContents.find("\r");
	    cerr << "ERROR: " << pageContents.substr(0,lstart) << endl;
	    timedOut = true;
	    break;
	  }
	  lstart = pageContents.find(lengthKeyword);
	  if (lstart != string::npos){
	    lstart += lengthKeyword.size();
	    string::size_type lend = pageContents.find("\r",lstart);
	    string length = pageContents.substr(lstart, lend - lstart);
	    totalSize = atoi(length.c_str());
	  }
	}
      }
    } else {
      cerr << "Connection timed out." << endl;
//       cout << pageContents << endl;
      timedOut = true;
    }
//     cout << "numRead=" << numRead << " totalRead=" << totalRead << " totalSize=" << totalSize << endl <<
//       " numReadZero=" << numReadZero << " maxReadZero=" << maxReadZero << endl;
  } while ((numRead != -1) && (totalRead < totalSize) && (numReadZero < maxReadZero) && !timedOut);

  //  cout << "Done reading timedOut=" << timedOut << endl;
  if (numRead == -1){
    perror("ERROR");
    pageContents = "";
    return -1;
  }

  if (timedOut)   //we return nothing instead of returning part of a page
    pageContents = "";

  ::close(socketDesc);

  string::size_type pos = pageContents.find("\r\n\r\n"); //get rid of initial cruft
  if (pos != string::npos){
    pos += 4;
    pageContents = pageContents.substr(pos, pageContents.size() - pos);
  }
  
  return 0;
}

iwebstream & iwebstream::operator>>(int & t){
  string s;
  operator>>(s); //read it as string
  t = atoi(s.c_str());
  return *this;
}

iwebstream & iwebstream::operator>>(long int & t){
  string s;
  operator>>(s); //read it as string
  t = atoi(s.c_str());
  return *this;
}

iwebstream & iwebstream::operator>>(string & s){
  s = "";
  if (position >= data.size())  //return 0?
    return *this;

  while (data[position] != ' ' && data[position] != '\n' && data[position] != '\t') {
    s += data[position++];
    if (position >= data.size()) 
      return *this;
  }
  position++; //move beyond whitespace
  return *this;
}

void iwebstream::getline(string & s){
  s = "";
  if (eof()) return;
  while ((data[position] != '\n') && (position < data.size())) 
    s += data[position++];
  position++; //move past /n
  return;
}

void iwebstream::putback(char & c){
  if (position <= 0) return;
  data[--position] = c;
}

char iwebstream::get(){
  if (position >= data.size())
    return 0;
  return data[position++];
}

bool iwebstream::eof(){
  if (position >= data.size())
    return true;
  return false;
}

//ignore x, assume its a comparison with 0.
bool iwebstream::operator==(int x){
  if (data == "")
    return true;
  else
    return false;
}

//from Stroustrup book
// int iwebstream::cmpNC(const string& s, const string &s2)
// {
//   string::const_iterator p = s.begin();
//   string::const_iterator p2 = s2.begin();
  
//   while (p != s.end() && p2 != s2.end()) {
//     if (toupper(*p) != toupper(*p2))
//       return (toupper(*p) < toupper(*p2)) ? -1 : 1;
//     ++p;
//     ++p2;
//   }
//   return (s2.size() == s.size()) ? 0 : (s.size() < s2.size()) ? -1 : 1;
// }

//find s, starting at position p, ignoring case.
// return data.size() if not found
unsigned int iwebstream::findNC(const string & s, unsigned int p){
  unsigned int matchpos = 0;
  for (; p < data.size() && matchpos < s.size() ; p++) {
    if (toupper(data[p]) == toupper(s[matchpos]))
      matchpos++;
    else
      matchpos = 0;
  }
  if (matchpos == 0)
    return p;
  else
    return p - s.size();
}

bool iwebstream::find(const string & s){
  position = findNC(s, position);
  if (position == data.size())
    return false;
  return true;
}

bool iwebstream::findAndPass(const string & s){
  position = findNC(s, position) + s.size();
  if (position == data.size())
    return false;
  return true;
}

//return whatever is between the next begin & end, which must be between the next
//  "after" and "before".
// Case is ignored in comparisions.
// If begin is not found, return "". Move position to point to char after end;
// If end == "" then just find begin and place position just after it
string iwebstream::findTag(const string & begin, const string & end, const string & after = "",
			   const string & before = "") {
//   cout << "findTag begin=" << begin << "=" << endl
//        << "end=" << end << "=" << endl 
//        << "after=" << after << "=" << endl
//        << "before=" << before << "=" << endl;
  unsigned int pos = position;
  unsigned int afterPos = position;
  if (after != "")
    pos = findNC(after, pos);
  if (pos == data.size())
    return "";
  afterPos = pos;
  pos = findNC(begin, pos);
  if (pos == data.size()) 
    return "";
  unsigned int temp = findNC(before, afterPos);
  if ((before != "") && (pos > findNC(before, afterPos))) //if the begin..end block is after "before".
    return "";
  if (temp == data.size())
    return ""; //afterPos not found
  pos += begin.size();
  if (end == "")
    return "";
  unsigned int posend = findNC(end, pos);
  if (posend == data.size())
    return ""; //end not found

  string result = data.substr(pos, posend-pos);
  return result;
}

