// Author: Jose M. Vidal
// $Id: channel.C,v 1.6 2000/01/04 16:03:31 jmvidal Exp $
// This code is copyright of Jose M. Vidal and released under
// the GNU General Public License

#include "channel.H"

//defined in reference.C
extern string trim(const string & s);

bool channel::readChannel() {
  if (read && title == "")
    return false; //we failed before, dont try again
  else if (read)
    return true; //already read it OK.
  read = true; //we will try to read it.
  cout << "Reading channel " << name << endl;
  iwebstream ifs(url);
  if (ifs == 0) { //could not read url, try backup file
    cerr << "Could not read channel=" << name << ", trying backup file" << endl;
    iwebstream ifs2(backupFile); //backup file could also be a url!
    if (ifs2 == 0) {
      cerr << "Could not read backup file=" << backupFile << endl;
      return false;
    }
    reference r;
    readStream(ifs2);
    while (r.readXml(ifs2)){
      items.push_back(r);
      r.comment = "";
      r.title = "";
      r.url = "";
    }
  }
  else { //read url OK.
    reference r;
    if (readStream(ifs)){ //parsed <channel> section OK
      while (r.readXml(ifs)) 
	items.push_back(r);
	r.comment = "";
	r.title = "";
	r.url = "";
    }
    else { //could not parse <channel> section
      cerr << "Could not parse channel=" << name << ", trying backup file" << endl;
      iwebstream ifs2(backupFile); //backup file could also be a url!
      if (ifs2 == 0) {
	cerr << "Could not read backup file=" << backupFile << endl;
	return false;
      }
      reference r;
      readStream(ifs2);
      while (r.readXml(ifs2)){
	items.push_back(r);
	r.comment = "";
	r.title = "";
	r.url = "";
      }
    }
  }
  return true;
}

//not used, dont know if it works.
void channel::readImage() {
  if (imageLink != "") {
    iwebstream image(imageLink);
    if (image != 0){
      string filename = backupFile + "-image";
      ofstream of(filename.c_str());
      for (char c = image.get(); !image.eof(); c =image.get()) {
	of << c;
      }
      of.close();
    }
  }
}

bool channel::readStream(iwebstream & is) {
  const string channelT = "<channel>";
  const string titleT = "<title>";
  const string titleET = "</title>";
  const string linkT = "<link>";
  const string linkET = "</link>";
  const string descriptionT = "<description>";
  const string descriptionET = "</description>";
  const string languageT = "<language>";
  const string languageET = "</language>";
  const string imageT = "<image>";
  const string imageET = "</image>";
  const string urlT = "<url>";
  const string urlET = "</url>";
  if (!is.find(channelT)) {
    cerr << "Channel file does not have <channel> section" << endl;
    cerr << "The channel we downloaded is:" << endl;
    cerr << is.getData() << endl;
    return false;
  }
  
  title = is.findTag(titleT, titleET, channelT);
  title = trim(title);
  siteUrl = is.findTag(linkT, linkET, channelT);
  description = is.findTag(descriptionT, descriptionET, channelT);
  description = trim(description);
  language = is.findTag(languageT, languageET, channelT);

  //if there is an image, read it
  imageTitle = is.findTag(titleT, titleET, imageT);
  imageUrl = is.findTag(urlT, urlET, imageT);
  imageLink = is.findTag(linkT, linkET, imageT);

  ofstream of(backupFile.c_str());
  if (of == 0) {
    cerr << "Could not write channel to " << backupFile << endl;
    return true;
  }
  of << is.getData();
  of.close();
  return true;
}

void channel::increaseHits(const string & url, int x){
  for (vector<reference>::iterator i = items.begin(); i!= items.end(); ++i){
    if ((*i).url == url)
      (*i).hits+= x;
  };
}
