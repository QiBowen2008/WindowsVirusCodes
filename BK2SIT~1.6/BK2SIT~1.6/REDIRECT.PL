#!/usr/bin/perl 
$|=1;

use CGI param, header;

## Is this properly posted from a web page?
$ENV{"REQUEST_METHOD"} || exit(1);

$debug=0;
open(DEBUG, ">>/tmp/redirect.log") if $debug;

# should point to a directory where all hit logs go to
#$basedir = "/var/log/bk2site";
$basedir = ".";

$logfile = param("logfile") || "urllog";

$url = param("url");

#the CGI module seems to change +'s into spaces so
#replace all spaces with a +, 
$url =~ tr/ /+/;

foreach $par (param()) {
  print DEBUG "param($par) = " . param($par) . "\n" if $debug;

  if (not (($par eq "url") or ($par eq "logfile")) ) {
    $url .= "&" .  $par . "=" . param($par);
  }
}

print DEBUG "\$logfile = $logfile\n" if $debug;
print DEBUG "\$url = $url\n" if $debug;

# References to parent directories are not allowed!
if ( $logfile =~ /\.\./ ) {
  die("Illegal value for parameter 'logfile' ($logfile)!");
}

# prepend base directory of hit logs to given filename
$logfile = "$basedir/$logfile";

print DEBUG "The real logfile is >$logfile<\n" if $debug;

open (F, ">>$logfile") ||
  die ("Failed to open >$logfile< ($!)!");
$t = time;
print F "$t\t$url\n";
close F;

print "Location: $url\n\n";
print "Please see <A HREF=\"http://$url\">http://$url</A>\n";
#close(STDOUT);

close(DEBUG) if $debug;
exit;
