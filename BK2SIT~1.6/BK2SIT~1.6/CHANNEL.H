// Author: Jose M. Vidal
// $Id: channel.H,v 1.2 1999/11/13 00:10:12 jmvidal Exp $
// This code is copyright of Jose M. Vidal and released under
// the GNU General Public License
//
#ifndef CHANNEL_H
#define CHANNEL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <vector>

#include "reference.H"
#include "iwebstream.H"

class channel {
  vector<reference> items;

  string name;
  string url;
  string backupFile; //to write the channel contents, to read the channel from if url fails.
  bool read; //true if we have readChannel() already
  
  //the data below we read from the channel file after fetching it
  string title; 
  string siteUrl;
  string description;
  string image; //also a url, to an image
  string language;
  
  string imageTitle;
  string imageUrl;
  string imageLink;
  bool readStream(iwebstream & is);
  void readImage();

public:
  channel (): name(""), url(""), backupFile(""), read(false), title(""), siteUrl(""), 
    description(""), language(""), imageTitle(""), imageUrl(""), imageLink("") {};
  channel (const string & n, const string & u, const string & f):
    name(n), url(u), backupFile(f), read(false), title(""), siteUrl(""), description(""),
    language(""), imageTitle(""), imageUrl(""), imageLink("") {};

  //this is the same as default:
  //  channel (const channel & c) : name(c.name), url(c.url), backupFile(c.backupFile), 
  //    read(c.read), title(c.title), siteUrl(c.siteUrl), description(c.description), 
  //    language(c.language), imageTitle(c.imageTitle), imageUrl(c.imageUrl), 
  //    imageLink(c.imageLink) {};

  vector<reference> getItems(){
    return items;
  };
  string getName(){
    return name;
  }
  //returns true if successful, false if we had to read it from backupFile;
  bool readChannel();
  void increaseHits(const string & url, int x);

};

class channelContainer {
  vector<channel> channels;
public:
  channelContainer() {};
  void addNewChannel(const string & name, const string & url, const string & backupFile) {
    if (nameExists(name)) //exit silently
      return;
    channel c(name, url, backupFile);
    c.readChannel();
    channels.push_back(c);
  }
  bool nameExists(const string &name) {
    channel c;
    return getChannel (name, c);
  }
  bool getChannel (const string & name, channel & c) {
    for (vector<channel>::iterator i = channels.begin(); i != channels.end(); ++i) {
      channel &r = *i;
      if (r.getName() == name) {
	c = r;
	return true;
      }
    }
    return false;
  }
  void increaseHits(const string & url, int x){
    for (vector<channel>::iterator i = channels.begin(); i != channels.end(); ++i) {
      channel &r = *i;
      r.increaseHits(url, x);
    }
  };
};
  
#endif
