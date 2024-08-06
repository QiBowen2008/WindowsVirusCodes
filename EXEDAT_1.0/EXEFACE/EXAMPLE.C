#include <stdio.h>
#include <string.h>
#include <malloc.h>

/* Choose between including "lzssnone.inc" and "plusari.inc"            */
/* "lzssnone.inc" offers decompression of 0(none) and 1(lzss)           */
/* "plusari.inc"  offers decompression of 0(none), 1(lzss) and 2(lzari) */

/* #include "lzssnone.inc" */
#include "plusari.inc"

void main(int argc, char *argv[])
{
  unsigned char* databuffer;
  long i, datalen;

  exedat_archive(argv[0]);              //STEP 1: which archive are we using?
  datalen = exedat_search("file2.txt"); //STEP 2: search file2.txt in archive
  databuffer = malloc(datalen);
  exedat_load(databuffer);              //STEP 3: load the file in buffer

  for (i = 0; i < datalen; i++)
	putc((char)databuffer[i], stdout);  //output the characters we loaded

  free(databuffer);
  return;
}
