#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <io.h>

#define compr_none 0
#define compr_lzss 1
#define compr_lzari 2
#define headerdef "EXEDAT"

#include "lzss.h"
#include "lzari.h"

struct exedat_header
{
  char header[8];       /* identification = "EXEDAT"  */
  short version;        /* 1                          */
  long number_files;    /* number of files in archive */
};

struct exedat_indexblock
{
  char name[13];        /* name of file in archive                  */
  long lokation;        /* offset of filedata from end of .exe file */
  long length;          /* filesize (could be compressed size)      */
  long original_length; /* filesize without compression             */
  short compression;    /* compression type (see compr_lzss at top) */
};

long filelen(FILE *fptr)
{
  long oldplace, calclen;
  oldplace = ftell(fptr);
  fseek(fptr, 0L, SEEK_END);
  calclen = ftell(fptr);
  fseek(fptr, oldplace, SEEK_SET);
  return(calclen);
}


int main(int argc, char *argv[])
{
  exedat_header header = { "", 1, 0 };
  exedat_indexblock* index_array;
  unsigned char** data_array;
  short* delete_array;
  short command_pos;
  char command;
  long i;

  FILE* fptr;

  /* find command */
  command_pos = -1;
  for (i = argc-1; i >=0 ; --i)
	if ((strlen(argv[i]) == 1) && (strpbrk(argv[i], "nladx") != NULL))
	  {
	  command_pos = i;
	  command = argv[command_pos][0];
	  break;
	  }

  /* no command found */
  if (command_pos == -1)
	{
	cout << "No command found!" << endl << endl;

	cout << "Examples:" << endl;
	cout << "exedat n archive.res 	          //create New archive" << endl;
	cout << "exedat l archive.res 	          //List archive" << endl;
	cout << "exedat a archive.res file1.bin 0 //Add file1.bin with no compr." << endl;
	cout << "exedat a archive.res file1.bin 1 //Add file1.bin with lzss" << endl;
	cout << "exedat a archive.res file1.bin 2 //Add file1.bin with lzari" << endl;
	cout << "exedat d archive.res file1.bin   //Delete file1.bin" << endl;
	cout << "exedat x archive.res file1.bin   //eXtract file1.bin" << endl;
	return(0);
	}

  /* new archive */
  if (command == 'n')
	{
	fptr = fopen(argv[command_pos+1], "wb");
	strcpy(header.header, headerdef);
	fwrite(&header, sizeof(header), 1, fptr);
	fclose(fptr);
	cout << "New archive " << argv[command_pos+1] << " created.";
	return(0); /* exit */
	}

  /* check archive */
  fptr = fopen(argv[command_pos+1], "rb");
  if (fptr == NULL)
	{
	cout << "Archive doesn't exist!" << endl;
	return(0); /* exit */
	}

  fseek(fptr, -sizeof(header), SEEK_END);
  fread(&header, (long) sizeof(header), 1, fptr);

  if (strcmp(header.header, headerdef) != 0) /* Check if header = "EXEDAT" */
	{
	cout << "This is not an EXEDAT archive file!" << endl;
	return(0); /* exit */
	}

  cout << "Number of files in " << argv[command_pos+1] << " = " << header.number_files << endl;

  long disk_blocks = header.number_files;
  long new_blocks = header.number_files;
  if (command == 'a') new_blocks++;

  index_array = new exedat_indexblock[new_blocks];
  data_array = new unsigned char*[new_blocks];

  if (command == 'l') /* list */
	cout << "Index: " << endl;

  /* load all headers */
  for (i = 0; i < disk_blocks; i++)
	{
	fseek(fptr, -(sizeof(header)+(sizeof(exedat_indexblock)*(i+1))) , SEEK_END);
	fread(&index_array[i], sizeof(exedat_indexblock), 1, fptr);

	if (command == 'l')
	  {
	  printf("Name: %-12s", index_array[i].name);
	  printf(" Lokation: %-8ld", index_array[i].lokation);
	  printf(" Len: %-8ld", index_array[i].length);
	  printf(" OLen: %-8ld", index_array[i].original_length);
	  printf(" Compr: %-2d\n", index_array[i].compression);
	  }
	}

  /* exit if command = list */
  if (command == 'l')
	{
	fclose(fptr);
	delete [] index_array;
	delete [] data_array;
	return(0); /* exit */
	}

  /* if adding, then check for compression type */
  if ((command == 'a') && ((strlen(argv[argc-1]) != 1) || (strpbrk(argv[argc-1], "012") == NULL)))
	{
	cout << "compression type is wrong or forgotten!" << endl;
	fclose(fptr);
	delete [] index_array;
	delete [] data_array;
	return(0); /* exit */
	}

  long seeklong = sizeof(header)+(sizeof(exedat_indexblock) * disk_blocks);

  /* load data */
  for (i = 0; i < disk_blocks; i++)
	{
	data_array[i] = new unsigned char[index_array[i].length];
	seeklong += index_array[i].length;
	fseek(fptr, -seeklong, SEEK_END);
	fread(data_array[i], index_array[i].length, 1, fptr);
	}

  fclose(fptr); /* close archive after loading */


  if (command == 'a')
	{
	fptr = fopen(argv[command_pos+2], "rb");

	/* fill new index */
	strcpy(index_array[disk_blocks].name, argv[command_pos+2]);
	index_array[disk_blocks].lokation = 0;
	index_array[disk_blocks].original_length = filelen(fptr);

	long real_size = filelen(fptr);
	unsigned char* tmpbuffer;
	long compr_length;

	switch(argv[command_pos+3][0]) /* compression type */
	  {
	  case compr_lzari+'0':
		tmpbuffer = new unsigned char[real_size];
		fread(tmpbuffer, real_size, 1, fptr);

		cout << "Crunching with lzari..." << endl;
		data_array[disk_blocks] = new unsigned char[real_size];
		compr_length = lzari_compress(data_array[disk_blocks], tmpbuffer, real_size);

		delete [] tmpbuffer;

		index_array[disk_blocks].length = compr_length;
		index_array[disk_blocks].compression = compr_lzari;
		break;
	  case compr_lzss+'0':
		tmpbuffer = new unsigned char[real_size];
		fread(tmpbuffer, real_size, 1, fptr);

		cout << "Crunching with lzss..." << endl;
		data_array[disk_blocks] = new unsigned char[real_size];
		compr_length = lzss_compress(data_array[disk_blocks], tmpbuffer, real_size);

		delete [] tmpbuffer;

		index_array[disk_blocks].length = compr_length;
		index_array[disk_blocks].compression = compr_lzss;
		break;
	  case compr_none+'0':
		cout << "Adding " << real_size << " bytes " << endl;
		index_array[disk_blocks].length = filelen(fptr);
		index_array[disk_blocks].compression = compr_none;

		data_array[disk_blocks] = new unsigned char[index_array[disk_blocks].length];
		fread(data_array[disk_blocks], index_array[disk_blocks].length, 1, fptr);
		break;
	  }

	fclose(fptr);
	}

  if (command == 'x') /* extract */
	{
	for (i = 0; i < disk_blocks; i++)
	  if (strcmp(index_array[i].name, argv[command_pos+2]) == 0)
		{
		unsigned char* tmpbuffer;

		switch(index_array[i].compression)
		  {
		  case compr_lzari:
			cout << "Extracting with lzari..." << endl;
			tmpbuffer = new unsigned char[index_array[i].original_length];
			lzari_decompress(data_array[i], tmpbuffer, index_array[i].length);
			fptr = fopen(index_array[i].name, "wb");
			fwrite(tmpbuffer, index_array[i].original_length, 1, fptr);
			fclose(fptr);
			delete [] tmpbuffer;
			break;
		  case compr_lzss:
			cout << "Extracting with lzss..." << endl;
			tmpbuffer = new unsigned char[index_array[i].original_length];
			lzss_decompress(data_array[i], tmpbuffer, index_array[i].length);
			fptr = fopen(index_array[i].name, "wb");
			fwrite(tmpbuffer, index_array[i].original_length, 1, fptr);
			fclose(fptr);
			delete [] tmpbuffer;
			break;
		  case compr_none:
			cout << "Extracting..." << endl;
			fptr = fopen(index_array[i].name, "wb");
			fwrite(data_array[i], index_array[i].length, 1, fptr);
			fclose(fptr);
			break;
		  }
		}
	}

  delete_array = new short[new_blocks];

  /* prepare delete_array */
  for (i = 0 ; i < new_blocks; i++)
	delete_array[i] = 0;

  /* mark the file to delete */
  if (command == 'd')
	for (i = 0; i < disk_blocks; i++)
	  if (strcmp(index_array[i].name, argv[command_pos+2]) == 0)
		delete_array[i] = 1;

  /* write everything back to disk */

  header.number_files = new_blocks;
  if (command == 'd')
	header.number_files--;

  fptr = fopen(argv[command_pos+1], "wb");

  /* write data backwards from array */
  for (i = (new_blocks-1); i >= 0; i--)
	if (delete_array[i] != 1)
	  fwrite(data_array[i], index_array[i].length, 1, fptr);

  long offset_calc = sizeof(header); /* first add the header */
  offset_calc += header.number_files*sizeof(exedat_indexblock);

  for (i = 0; i < new_blocks; i++)
    offset_calc += index_array[i].length;

  /* write index_blocks */
  for (i = (new_blocks-1); i >= 0; i--)
	if (delete_array[i] != 1)
	  {
	  index_array[i].lokation = offset_calc;
	  fwrite(&index_array[i], sizeof(exedat_indexblock), 1, fptr);
	  offset_calc -= index_array[i].length;
	  }

  fwrite(&header, sizeof(header), 1, fptr); /* write header */

  delete [] index_array;
  for (i = 0; i < new_blocks; i++)
	delete [] data_array[i];
  delete [] data_array;
  delete [] delete_array;

  fclose(fptr);

  cout << "All done." << endl;

  return(0);
}

