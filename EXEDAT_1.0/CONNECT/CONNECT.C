/*
   This code connects a file to an other file.
   In our case, we use it to add an archive at the end of an .exe file
   Syntax: connect .exe .res
*/

#include <stdio.h>
#include <string.h>
#include <io.h>

struct exedat_header
{
  char header[8];       /* identification = "EXEDAT"  */
  short version;        /* 1                          */
  long number_files;    /* number of files in archive */
} header;


int main(int argc, char *argv[])
{
 FILE* exefptr;
 FILE* resfptr;
 long tmpl;
 int tmp;

 if (argc < 3)
      {
      printf("Syntax: connect exe_name archive_name\n");
      return(0);
      }

 if ((exefptr = fopen(argv[1], "ab")) == NULL) /* append in binairy mode */
      {
      printf("Error opening %s.\n", argv[1]);
      return(0);
      }

 if ((resfptr = fopen(argv[2], "rb")) == NULL) /* read in binairy mode */
      {
      printf("Error opening %s.\n", argv[2]);
      return(0);
      }
 tmpl = sizeof(header);

 fseek(resfptr, -tmpl, SEEK_END);
 fread(&header, tmpl, 1, resfptr);

 if (strcmp(header.header, "EXEDAT") != 0) /* Check if header = "EXEDAT" */
      {
      printf("This is not an EXEDAT archive file!\n");
      fclose(resfptr);
      fclose(exefptr);
      return(0); /* exit */
      }

 fseek(resfptr, 0L, SEEK_SET);

 tmp = getc(resfptr);

 while (!feof(resfptr))
   {
   putc(tmp, exefptr);
   tmp = getc(resfptr);
   }

 fclose(resfptr);
 fclose(exefptr);

 printf("All done.");

 return(0);
}
