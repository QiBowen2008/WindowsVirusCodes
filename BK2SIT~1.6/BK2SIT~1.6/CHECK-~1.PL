#! /usr/bin/perl -w
#
# Copyright 1999/2000  August Hoerandl (august.hoerandl@gmx.at)
#                 http://elina.htlw16.ac.at/~hoerandl
#
# parts from search.pl which is
# Copyright 1999 Jose M. Vidal
# Jose M. Vidal, vidal@multiagent.com, http://jmvidal.ece.sc.edu
#
# This program is free software.  You can redistribute it and/or modify
# it under the terms of the GNU General Public License
#
# create database with meta info 
#
# actions
#  read database
#    read to @allRecords
#  read metadata
#    read to $meta{$URL}, $date{$URL}, $error{$URL} 
#  check all (expired) urls
#    based on $date{$URL}
#  dump metadata 
#  dump database with meta info
#  dump errordata
#
# History
#  0.1 
#  4. Sept. 1999 - first version
#  0.2
#  12. Sept. 1999 - little bug with multiple line meta corrected
#                   added option & getoptions
#  0.3
#  21. Sept. 1999 - merged back file detection from Jose
#
#  0.4
#  26. Oct 1999 - added code for: meta Classification
#  31. Oct 1999 - added noproxy, showrecheck, $time, error.html
#
#  0.5 
#  12. Nov 1999 - added  checkheadline
#                 strip metainfo (multiple blanks, html)
#                 convert iso
#   
#  26. Nov. 1999 - added $urlto, $urlmaxsize
#  29. Nov. 1999 - added $agent
#  23. Jan. 2000 - fixed bug which used up all memory 
#                  added <meta value="..."> and <meta contents="...">
#  10. Feb. 2000 - use LWP::Parallel::UserAgent
# http://theoryx5.uwinnipeg.ca/CPAN/data/ParallelUserAgent/Bundle/ParallelUA.html
#                  install: perl -MCPAN -e 'install Bundle::ParallelUA' 
#                  --maxpar number of host to query in parallel 
#
#  11. Feb. 2000 - fixed dump_database (replaces $error)
#
#  20. Feb. 2000 - fixed make_url  
#                  (thanks to Andreas Hille for beta testing)
#                  urlmaxsize to handle very big pages
#                  urlmaxresponse to handle endless small answers     
#  26. Feb. 2000 - added sorting (errorcode) to dump_errordata_html
#
#   4. Mar. 2000 - added  $req->previous / $urldata to allow redirection
#
#  22. Mar. 2000 - added sorting to dump_errordata_html (errorcount)

$version = "0.65";

$database   = "/usr/local/httpd/cgi-bin/urls.db";      # Name of db file.
$newdb = "";             # result: database
$metadata = "meta.db";   # result: metainfo
$errordata = "error.db"; # result: urls with error

$error_recheck = 10; # number of days for recheck after error
$age_recheck = 40;  # number of days for recheck 
# add some randomness - so we won't fetch all urls on the same day 
$random = .5;   # random number of days added (factor)

$dumpfreq = 10;  # when to dump metadata (number of urls read) 
$maxurls = 2;   # max url to fetch recursive (frames)
$maxcheck = 1000; # max number of urls to check in one run

$sleep = 2; # time to sleep between url requests - not supported !!!
$debug = 0; # set to 1 for debug output
$showrecheck = 0; #  set to 1 to see next recheck

$maxpar = 10; # maximum requests in parallel

$showhash = 1; # show hashes on progress

$checkerror = 0; # check all urls with errorcount > 0
$reseterror = 0; # reset all error counters - only with $checkerror
$checknometa = 0; # check all urls with meta data = ""

$checkheadline = 0; # add <h1>, <h2>, ... to meta info - see $whichheadline
$whichheadline = "1-2"; # which headlines to check

$urlto = 0; # timeout for getting url (seconds)
$urlmaxsize = 0; # max size of page to get
$urlmaxresponse = 500; # max number of responses (parts)

# useragent to send in http request ("spider name")
$agent = "check-meta - spider"; 

$noproxy = 0; # dont use proxy from environment

# lines shouldn´t get too long 
$maxlinelength = 4*1024; # max line length during get meta

$test1 = 0; # simple test
$test2 = 0; # more tests
$test3 = 0; # even more tests
$test4 = 0; # even more tests

# ------------------------------------------------------------------
# no user serviceable parts below
# ------------------------------------------------------------------

use LWP;
use HTTP::Request::Common;
use Getopt::Long;

require LWP::Parallel::UserAgent;
use HTTP::Request; 
use LWP::Parallel::UserAgent qw(:CALLBACK);
#use LWP::Debug qw(+);
#require LWP::Debug;

# ------------------------------------------------------------------

$numlines_bk2site = 9;   #  number of lines - standard
$numlines_metadata = 10; #  number of lines - with metadata

# ------------------------------------------------------------------

$countmeta = 0;
$counthead = 0;

# ------------------------------------------------------------------

sub norm_url {
  local($url) = @_;
  my $req = HTTP::Request->new('GET', $url);
  return $req->url;
}

# ------------------------------------------------------------------


sub read_database {
  open(DB,"<$database") ||
    die ("Can't open $database; $!");
  $db_line1 = <DB>;
  chop($db_line1);
  if ($db_line1 eq "#bk2site urls.db generated file"){
    printf "bk2site urls.db generated file\n" if $debug;
    $numlines = $numlines_bk2site;
  }
  else {
    printf "bk2site urls.db has already metadata\n" if $debug;
    $numlines = $numlines_metadata;
  };
  $db_line2 = <DB>;
  $db_line3 = <DB>;
  $db_line4 = <DB>;
  @allRecords = <DB>; # there are numlines lines per record,
                      # so mutiply index by 8

  close(DB);
  ## 0 is the type
  ## 1 is the ParentTitle
  ## 2 is the url or relative dir
  ## 3 is the Title
  ## 4 is the comment
  ## 5 is the creation time
  ## 6 is the last modified time
  ## 7 is the last visit time
  ## 8 is the number of user hits
  ## 9 is the meta info

  $typeN = 0;
  #  $parentTitleN = 1;
  $urlN = 2;
  #  $titleN = 3;
  #  $commentN = 4;
  #  $creationtimeN = 5;
  #  $modifiedtimeN = 6;
  #  $visittimeN = 7;
  #  $urlhitsN = 8;
  
#  $numlines = 10; ##0-9, as above

  #  my $numRecords = 15;  # test only
  my $numRecords = ($#allRecords + 1)/$numlines;

  my $i;
  for ($i=0; $i < $numRecords; ++$i){ #initialize meta
    my $cr = ($i * $numlines);
    next if (($allRecords[$cr + $typeN]) eq "FOLDER\n");
    my $url =    $allRecords[$cr + $urlN];
    chop($url);
    $url = &norm_url($url); 
    print "url $url added\n" if $debug;
    $error{$url} = 0;
    $date{$url} = 0;
    $meta{$url} = "";
  }
}

sub dump_database {
  print "\ndumping database $newdb\n";
  open(DB,">$newdb") ||
    die ("Can't open $newdb; $!");
#  print DB "#bk2site urls.db generated file metadata";
  if ($db_line1 eq "#bk2site urls.db generated file") {
    print DB "#bk2site urls.db generated file metadata\n";
  }
  else {
    print DB "$db_line1\n";
  }
  print DB "$db_line2";
  print DB "$db_line3";
  print DB "$db_line4";

  my $numRecords = ($#allRecords + 1)/$numlines;
  for ($i=0; $i < $numRecords; $i++) {
    my $cr = ($i * $numlines);
    my $url = $allRecords[$cr + $urlN];
    chop($url);
    for($j = 0; $j <  $numlines_bk2site; $j++) {
      print DB "$allRecords[$cr + $j]";
    }
    if (defined $error{$url} && $error{$url} > 0) {
      print DB "\n";
    } elsif (defined $meta{$url}) {
      print DB "$meta{$url}\n";
    } else {
      print DB "\n";
    }
  }
  close DB;
}

# ------------------------------------------------------------------
# add url + filename
sub make_url {
  local($url, $file) = @_;

  print "\nmake url [$url][$file]\n" if $debug; 
  if ( $file =~ /^http/) { # full url
    return $file;
  } elsif ( $file =~ /^\//) {  # filename starting with /
    $url =~ /(.*\/\/[^\/]*)\//; # get only host part
    return $1 . $file;
  }
  if ( $url =~ /\/$/ ) { # url ends with /
    return $url . $file;
  } elsif( $url =~ /(.*\/)[^\/]+/ ) { # url with ..../file.ext
    # print "+\n";
    return $1 . $file;
  } else {
    return $url . "/" . $file;
  }
}

sub conv_iso {
  local($line) = @_;
  $line =~ s/&AElig;/Æ/ig;
  $line =~ s/&Aacute;/Á/ig;
  $line =~ s/&Acirc;/Â/ig;
  $line =~ s/&Agrave;/À/ig;
  $line =~ s/&Atilde;/Ã/ig;
  $line =~ s/&Ccedil;/Ç/ig;
  $line =~ s/&Eacute;/É/ig;
  $line =~ s/&Egrave;/È/ig;
  $line =~ s/&Euml;/Ë/ig;
  $line =~ s/&Iacute;/Í/ig;
  $line =~ s/&Icirc;/Î/ig;
  $line =~ s/&Igrave;/Ì/ig;
  $line =~ s/&Iuml;/Ï/ig;
  $line =~ s/&Ntilde;/Ñ/ig;
  $line =~ s/&Oacute;/Ó/ig;
  $line =~ s/&Ocirc;/Ô/ig;
  $line =~ s/&Ograve;/Ò/ig;
  $line =~ s/&Oslash;/Ø/ig;
  $line =~ s/&Uacute;/Ú/ig;
  $line =~ s/&Ugrave;/Ù/ig;
  $line =~ s/&Yacute;/Ý/ig;
  $line =~ s/&aacute;/á/ig;
  $line =~ s/&acirc;/â/ig;
  $line =~ s/&acute;/´/ig;
  $line =~ s/&aelig;/æ/ig;
  $line =~ s/&agrave;/à/ig;
  $line =~ s/&aring;/å/ig;
  $line =~ s/&atilde;/ã/ig;
  $line =~ s/&brvbar;/¦/ig;
  $line =~ s/&ccedil;/ç/ig;
  $line =~ s/&cedil;/¸/ig;
  $line =~ s/&cent;/¢/ig;
  $line =~ s/&copy;/©/ig;
  $line =~ s/&curren;/¤/ig;
  $line =~ s/&deg;/°/ig;
  $line =~ s/&eacute;/é/ig;
  $line =~ s/&ecirc;/ê/ig;
  $line =~ s/&egrave;/è/ig;
  $line =~ s/&euml;/ë/ig;
  $line =~ s/&frac12;/½/ig;
  $line =~ s/&frac14;/¼/ig;
  $line =~ s/&frac34;/¾/ig;
  $line =~ s/&iacute;/í/ig;
  $line =~ s/&icirc;/î/ig;
  $line =~ s/&igrave;/ì/ig;
  $line =~ s/&iexcl;/¡/ig;
  $line =~ s/&iquest;/¿/ig;
  $line =~ s/&iuml;/ï/ig;
  $line =~ s/&laquo;/\«/ig;
  $line =~ s/&macr;/¯/ig;
  $line =~ s/&micro;/µ/ig;
  $line =~ s/&middot;/·/ig;
  $line =~ s/&nbsp;/ /ig;
  $line =~ s/&not;/¬/ig;
  $line =~ s/&ntilde;/ñ/ig;
  $line =~ s/&oacute;/ó/ig;
  $line =~ s/&ocirc;/ô/ig;
  $line =~ s/&ograve;/ò/ig;
  $line =~ s/&ordf;/ª/ig;
  $line =~ s/&ordm;/º/ig;
  $line =~ s/&oslash;/ø/ig;
  $line =~ s/&otilde;/õ/ig;
  $line =~ s/&para;/¶/ig;
  $line =~ s/&pound;/£/ig;
  $line =~ s/&plusmn;/±/ig;
  $line =~ s/&laquo;/\»/ig;
  $line =~ s/&reg;/®/ig;
  $line =~ s/&sect;/§/ig;
  $line =~ s/&shy;/­/ig;
  $line =~ s/&sup1;/¹/ig;
  $line =~ s/&sup2;/²/ig;
  $line =~ s/&sup2;/³/ig;
  $line =~ s/&uacute;/ú/ig;
  $line =~ s/&ucirc;/û/ig;
  $line =~ s/&ugrave;/ù/ig;
  $line =~ s/&uml;/¨/ig;
  $line =~ s/&yacute;/ý/ig;
  $line =~ s/&yen;/¥/ig;
  $line =~ s/&Auml;/Ä/ig;
  $line =~ s/&auml;/ä/ig;
  $line =~ s/&Ouml;/Ö/ig;
  $line =~ s/&ouml;/ö/ig;
  $line =~ s/&Uuml;/Ü/ig;
  $line =~ s/&uuml;/ü/ig;
  $line =~ s/&szlig;/ß/ig;
  $line =~ s/&sect;/§/ig;
  $line =~ s/&para;/¶/ig;
  $line =~ s/&copy;/©/ig;
  $line =~ s/&iexcl;/¡/ig;
  $line =~ s/&iquest;/¿/ig;
  $line =~ s/&cent;/¢/ig;
  $line =~ s/&pound;/£/ig;
  $line =~ s/&times;/×/ig;
  $line =~ s/&plusmn;/±/ig;
  $line =~ s/&divide;/÷/ig;
  $line =~ s/&not;/¬/ig;
  $line =~ s/&mu;/µ/ig;
  $line =~ s/&Ae;/Ä/ig;
  $line =~ s/&ae;/ä/ig;
  $line =~ s/&Oe;/Ö/ig;
  $line =~ s/&oe;/ö/ig;
  $line =~ s/&Ue;/Ü/ig;
  $line =~ s/&ue;/ü/ig;
  $line =~ s/&sz;/ß/ig;

  $line =~ s/&quot;/\'/ig;
  $line =~ s/&amp;/&/ig;

  $line =~ s/&lt;//ig;
  $line =~ s/&gt;//ig;

#  while ($line =~ /&\#([0-9a-f]*);/i) {
#    my $c = chr($1);
#    $line =~ s/&\#[0-9a-f]*;/$c/i;
#  }
  
  return $line;
}

# ------------------------------------------------------------------

sub read_metadata {
  print "reading metadata $metadata\n";
  open(DB,"<$metadata") ||
    return;
  while ($url = <DB>) {
    chop $url;
    $url = &norm_url($url); 
    $error= <DB>;
    $date = <DB>;
    $meta = <DB>;
    if (defined $meta{$url}) {
      chop $error;
      chop $date;
      chop $meta;
      $error{$url}= $error;
      $date{$url} = $date;
      $meta{$url} = $meta;
      print "added [$url]\n" if $debug;
      if ($checkerror && $error > 0) {
	$date{$url} = $time - 1;
	$error{$url} = 1 if $reseterror;
      }
      if ($checknometa && $meta eq "") {
	$date{$url} = $time - 1;
      }
    } else {
      print "skipped $url\n";
    } 
  }
  close(DB);
}


sub dump_metadata {
  print "dumping metadata to $metadata\n";
  open(DB,">$metadata") ||
    die ("Can't open $metadata; $!");
  foreach $url (sort keys (%meta)) {
    if (defined($error{$url}) && defined($date{$url})) {
      print DB "$url\n";
      print DB "$error{$url}\n";
      print DB "$date{$url}\n";
      print DB "$meta{$url}\n";
      print "dumped [$url]\n" if $debug;
    } else {
      print "'$url': no error+date set - redirection\n" if $debug;
    }
  }
  close(DB);
}

sub dump_errordata {
  print "dumping errordata to $errordata\n";
  open(DB,">$errordata") ||
    die ("Can't open $errordata; $!");
  foreach $url (sort keys (%meta)) {
    next if (! defined $error{$url});

    if ($error{$url} > 0) {
      print DB "$url\n";
      print DB "$error{$url}\n";
      print DB "$meta{$url}\n\n";
      print "dumped [$url]\n" if $debug;
    }
  }
  close(DB);
}

sub dump_errordata_html {
  print "dumping errordata to $errordata.html\n";
  open(DB,">$errordata.html") ||
    die ("Can't open $errordata.html; $!");
  print DB <<EOF;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"> 
<html>
  <head>
    <TITLE>Errors</TITLE>
  </head>
  <BODY>
    <table border>
    <tr>
      <th>Error Count</th>
      <th>URL</th>
      <th>Error Message</th>
EOF

  foreach $url (sort keys (%meta)) {
    next if (! defined $error{$url});
    if ($error{$url} > 0) {
      my $line = "$error{$url}     </td>\n" .
	" <td><a href=\"$url\">$url</a></td>\n" .
	  " <td>$meta{$url}</td>\n</tr>\n";
      push(@errors, $line);
    }
  }

  sub numerically{ substr($b, 0, 5) <=>  substr($a, 0, 5); }
  foreach $entry (sort numerically @errors) {
    print DB  "<tr>\n<td align=\"center\">$entry";
  }

  print DB "\n</table>\n";
#  print DB "$count URLs";
  print DB "\n</body>\n";
  close(DB);
}

# ------------------------------------------------------------------

sub save_meta {
  my($url, $metainfo) = @_;
  $meta{$url} .= $metainfo;
  print "new meta{$url}: [$meta{$url}]\n" if $debug;
  $done{$url} = 1;
}

sub check_response {

  my($url, $content) = @_;
  my $base = $url;
  if (defined $baseurl{$url} ) {
    $base = $baseurl{$url};
  }
  local(@lines) = split(/^/,$content);

  $bytes = length($content);
  print "got $bytes for $url ($base)\n" if $debug;

  local($old) = "";
  for (@lines) {
    last if (!$checkheadline && /body/i);
    $_ = $old . " ". $_ if ($old ne ""); # add old unfinished line
    print "checking [$_]\n" if $debug; 
    if ( /meta/i && 
	 (/keywords/i || /description/i ||
	  /classification/i || /page-topic/i )) {
      if (/content *= *[\"\']([^\"\']*)[\"\']/i ||
	  /contents *= *[\"\']([^\"\']*)[\"\']/i ||
	  /value *= *[\"\']([^\"\']*)[\"\']/i) {
#	print "meta = $1\n" if $debug;
	&save_meta($base, "$1 ");
	$old = "";
	$countmeta++;
      } else {
	# line with meta but no content - keep & merge with next line
	if (length($_) < $maxlinelength) {
	  $old = $_;
	  chop($old);
	  print "no content: $old\n" if $debug;
	} else {
	  print "line too long: $_\n";
	  $old = "";
	}
      }
    } elsif ($checkheadline && /<h[$whichheadline]/i) {
      print "head?: $_" if $debug;
      if (/<h([$whichheadline])[^>]*>(.*)<\/h[$whichheadline]>/i) { 
	print "headline $1 [$2]\n" if $debug;
	&save_meta($base, "$2 ");
	$old = "";
	$counthead++;
      } else {
	$old = $_;
	chop($old);
	print "no headline: $old\n" if $debug;
      }
    } else {
      $old = "";
    }
    # look for: frame src="..."
    if (/frame/i && /src *= *\"([^\"]*)\"/i) {
      my $frm =  &make_url($url,$1);
      print "get frame $frm in $url\n" if $debug;
      $baseurl{$frm}=$url;
      check($frm);
    }
  }
}


# ------------------------------------------------------------------


# our callback function gets called whenever some data comes in
# (same parameter format as standard LWP::UserAgent callbacks!)
sub handle_answer {
  my ($content, $response, $protocol, $entry) = @_;

  print "Handling answer from '",$response->request->url,": ",
  length($content), " bytes, Code ",
  $response->code, ", ", $response->message,"\n" if $debug;

  print STDERR "#" if $showhash;

  if (length ($content) ) {
    # just store content if it comes in
    # print "check content\n";
    $response->add_content($content);
    if ($count{$response->request->url}++ < $urlmaxresponse) {
      &check_response($response->request->url, $content);
    } else {
      local($title) = $response->request->url;
      print "oops - $title reached urlmaxresponse\n";
      return C_ENDCON;
    }
  }
  return undef;
}

# ------------------------------------------------------------------


sub check_init {
  $pua = LWP::Parallel::UserAgent->new();
  $pua->in_order  (1);  # handle requests in order of registration
  $pua->duplicates(1);  # ignore duplicates
  #  $pua->timeout   (2);  # in seconds
  $pua->redirect  (1);  # follow redirects

  if($urlto > 0) {
    $pua->timeout($urlto);
  }
  if (! $noproxy) {
    $pua->env_proxy();
  }
  if ($urlmaxsize > 0) {
    $pua->max_size($urlmaxsize);
  }

  $pua->agent($agent);

  $pua->max_hosts($maxpar); # sets maximum number of locations accessed in parallel
  $pua->max_req  ($maxpar); # sets maximum number of parallel requests per host

  print "LWP::Parallel::UserAgent created\n";
}

# ------------------------------------------------------------------

sub check {
  local($url) = @_;
  print "Registering '".$url."'\n";
  $meta{$url} = "";  
  $error{$url} = 0 unless defined($error{$url});
  $date{$url} = 0 unless defined($date{$url});
  $count{$url} = 0 unless defined($count{$url});

  my $req = HTTP::Request->new('GET', $url);

  if ($url ne $req->url) {
    print "skipped: strange '$url' nedds escape chars ".$req->url."\n";
#    exit 1;
  }
  
  if ( my $res = $pua->register ( $req, \&handle_answer)) { 
#        print STDERR $res->error_as_HTML;
      print "oops -- '$url' already seen ? \n";
    }    
}

# ------------------------------------------------------------------

sub check_all {
  print "\n\n";
  my $all = 0;
  foreach $url (keys (%meta)) {
    $all++;
  }
  my $i = 0;
  my $checked = 0;
  foreach $url (keys (%meta)) {
    $i ++;

    if ($time > $date{$url}) {

      print "check $time > $date{$url}\n" if $debug;
#      sleep($sleep) if ($sleep > 0);
#      print "checking [$url]  ", $checked, "/", $i, "/", $all, "\n";
      &check($url);
      $checked ++;
#      if ($checked % $dumpfreq == 0) {
#	&dump_metadata;
#	&dump_errordata;
#      }
    } else {
      if ($showrecheck) {
	printf("recheck [$url] in %.0f days\n", ($date{$url}-$time)/24/60/60);
      }
    }
    last if ($checked >= $maxcheck);
  }

  print STDERR "to check: ", $checked, " of ",  $all, " urls\n";
#  print STDERR "meta: $countmeta";
#  print STDERR " headlines: $counthead" if $checkheadline;
#  print STDERR "\n";
}


sub do_check {
  my $entries = $pua->wait();

  print "\n\nwait() done -- checking results:\n";
  foreach (keys %$entries) {
    print "\n";
    my $res = $entries->{$_}->response;
    local($url) = $res->request->url;
    local($urldata) = $url; # holds real url (without redirects)

    my $r = $res; 
    my @redirects;
    while ($r) { 
        $r = $r->previous; 
	if ($r) {
	  push (@redirects, $r);
	  $urldata = $r->request->url;
	}
	# print "from '$url' to '$urltosave'\n" if $r;
    }

    if ($url ne $urldata) {
      print "redirect: '$urldata' was redirected to '$url'\n" 
    }

    if (! defined($baseurl{$url})) {
      if ($done{$url}) {
	$metainfo = &conv_iso($meta{$url});
	$metainfo =~ s/<[^>]*>/ /g;
	#  $metainfo =~ s/,/ /g;
	$metainfo =~ s/[\0- ]/ /g;
	$metainfo =~ s/  +/ /g;
	$meta{$urldata} = $metainfo;
	$date{$urldata} = $time + 24*60*60*($age_recheck + 
					 rand($random*$age_recheck));
	$error{$urldata} = 0;
	print "meta was found: $urldata\n  [$metainfo]\ndate=[$date{$urldata}]\n";
      } elsif ($res->code >= 300) {
	print "error: $urldata = [", $res->code . ": " . $res->message,"]\n";
	$date{$urldata} = $time + 
	  24*60*60*($error_recheck + rand($random*$error_recheck));
	$error{$urldata}++;
	$metainfo = $res->code . ": " . $res->message;
	$metainfo =~ s/<[^>]*>/ /g;
	$metainfo =~ s/[\0- ]/ /g;
	$metainfo =~ s/  +/ /g;
	$meta{$urldata} = $metainfo ;
      } else {
	print "no meta for '$urldata' was \t", 
	   $res->code,": ", $res->message,"\n";
	$meta{$urldata} = "";
	$date{$urldata} = $time + 24*60*60*($age_recheck + 
					 rand($random*$age_recheck));
	$error{$urldata} = 0;
      }
    }
  }   
}

# ------------------------------------------------------------------

sub test1 {
  # test only
  $urlto = 10;

  &check("http://localhost/~hoerandl/xxx.html");
  &check("http://localhost/~hoerandl/xxxx.html");
  &check("http://localhost/~hoerandl");
  &check("http://localhost/~hoerandl/tk");
  &check("http://localhost/~hoerandl/tk/tk-linux-howto.html");
  &check("http://elina/");
  &check("http://elina/start");
  &check("http://localhost/start/");
  &check("http://elina.htlw16.ac.at/");
  &check("http://www.linux.org/");
  &check("http://www.linux.de");
  &check("http://www.tu-harburg.de/~semb2204/dlhp/index.html");
  &check("http://ahop.de/ahoppage/bookmark/internet.htm");
  &do_check;

  die "test 1 done";
}

sub test2 {
  $urlto = 10;

  &check("ftp://ftp.rubyriver.com/pub/jhardin/antispam/procmail-security.html");
  &check("http://1stlinuxsearch.com/");
  &check("http://www.linux.org/");
#  &check("http://www.linux.de");
  &check("http://www.tu-harburg.de/~semb2204/dlhp/index.html");
  &check("http://www.isc.org/dhcp.html");
  &check("http://www.schulchemie-online.de/");
  &check("http://linux.s-one.net.sg/resource/");
  &check("http://www.slug.org.au/etherboot/doc/html/diskless.html");	

  &do_check;

  die "test 2 done";
}

sub test3 {
  $urlto = 10;

  &check("http://www.cityusa.net/cdcentral/kungfuflix/main.html");
  &check("http://www.gratis.de");
  &do_check;

  die "test 3 done";
}

sub test4 {
  $urlto = 10;

#  &check("http://dmoz.org/Computers/Operating_Systems/Linux/");
  &check("http://www.hsp.de/lfs/");
#  &check("http://www.kt.opensrc.org/KC/gimp-devel/");
  &check("http://www.searchlinux.com/");

  &do_check;

  die "test 4 done";
}


# ------------------------------------------------------------------

sub usage {
  die <<EOF;
check-meta.pl [options]
  create database with meta info / version $version
  --age_recheck=NUM      number of days for recheck ($age_recheck)
  --agent=STRING         useragent to send in http request ($agent)
  --checkerror           check all urls with errorcount > 0 ($checkerror)
  --checkheadline        use headlines as meta info ($checkheadline)
  --checknometa          check all urls with meta data "" ($checknometa)
  --database=name        Database file ($database)
  --debug                show debug output ($debug)
  --dumpfreq=NUM         when to dump metadata - number of urls read ($dumpfreq) 
  --error_recheck=NUM    number of days for recheck after error ($error_recheck)
  --errordata=name       Database file with errors ($errordata)
  --help                 Show this help
  --maxcheck=NUM         max number of urls to check in one run ($maxcheck)
  --maxpar=NUM           max number of parallel requests ($maxpar)
  --maxurls=NUM          max number of urls to fetch recursive ($maxurls)
  --metadata=name        Database file for meta data ($metadata)
  --newdb=name           output file (same as database)
  --noproxy              don´t use a proxy ($noproxy)
  --random=NUM           random number of days added - factor ($random)
  --reseterror           reset all error counters - only with checkerror
  --sleep=NUM            unused - suppotr for backward compatibility
  --urlmaxresponse=NUM   max. number of responses ($urlmaxresponse)
  --urlmaxsize=NUM       max size of page in bytes; 0 = unlimited ($urlmaxsize)
  --urlto=NUM            timeout for getting url - seconds ($urlto)
  --whichheadline=FRM-TO which headlines to include ($whichheadline)
EOF
} 

# ------------------------------------------------------------------
#
# program starts here
#

$res = GetOptions (
		   "age_recheck=i" => \$age_recheck,
		   "agent=s" => \$agent, 
		   "checkerror!" => \$checkerror,
		   "checkheadline!" => \$checkheadline,
		   "checknometa!" => \$checknometa,
		   "database=s" => \$database,
		   "debug!" => \$debug,
		   "dumpfreq=i" => \$dumpfreq,
		   "error_recheck=i" => \$error_recheck,
		   "errordata=s" => \$errordata,
		   "help!" => \$showhelp,
		   "maxcheck=i" => \$maxcheck,
                   "maxpar=i" => \$maxpar,
		   "maxurls=i" => \$maxurls,
		   "metadata=s" => \$metadata,
		   "newdb=s" => \$newdb,
		   "noproxy!" => \$noproxy,
		   "random=f" => \$random,
		   "reseterror!" => \$reseterror,
		   "showrecheck!" => \$showrecheck,
		   "urlmaxsize=i" => \$urlmaxsize,
		   "$urlmaxresponse=i" =>  \$urlmaxresponse,
		   "urlto=i" => \$urlto,
		   "whichheadline=s" => \$whichheadline,
		   # not used any more
		   "sleep=i" => \$sleep,
		   # internal use only
		   "test1!" => \$test1,
		   "test2!" => \$test2,
		   "test3!" => \$test3,
		   "test4!" => \$test4,
		  );
&usage if (!$res || $showhelp || $#ARGV >= 0);
$time = time;

print $time;

if ($test1) { 
  &check_init;
  &test1;
  exit;
}
if ($test2) {
  &check_init;
  &test2;
  exit;
}
if ($test3) {
  &check_init;
  &test3;
  exit;
}
if ($test4) {
  &check_init;
  &test4;
  exit;
}

if ($newdb eq "") {
  $newdb = $database;
}

&read_database;
&read_metadata;
&dump_errordata;

&check_init;
&check_all;
&do_check;

&dump_metadata;
&dump_database;
&dump_errordata;
&dump_errordata_html;
