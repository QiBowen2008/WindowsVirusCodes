/****************************************************************************
*************************  BEGIN EXEDAT HEADER  *****************************
****************************************************************************/

/* LZSS_DECODE(PD) by Haruhiko Okumura */
/* EXEDAT by Ren‚ Olsthoorn in 1996 */

#define compr_none 0
#define compr_lzss 1
#define N	4096
#define F	  18
#define THRESHOLD  2

unsigned char text_buf [N + F - 1];
unsigned char* from_buffer, *to_buffer;
unsigned long from_pointer, from_max, to_pointer;
char* archive_name = NULL;
FILE* lzss_fptr;

struct exedat_header
{
  char header[8];       // identification = "EXEDAT"
  short version;        // 1
  long number_files;    // number of files in archive
} header;

struct exedat_indexblock
{
  char name[13];        // name of file in archive
  long lokation;        // offset of filedata from end of .exe file
  long length;          // filesize (could be compressed size)
  long original_length; // filesize without compression
  short compression;    // compression type (see compr_lzss at top)
} indexblock;


int putc_buffer(int outc)
  {
  to_buffer[to_pointer++] = (unsigned char) outc;
  return(1);
  }

int getc_lzss(void)
  {
  if (from_pointer++ == from_max)
	return(EOF);
  return(getc(lzss_fptr));
  }

void lzss_decode(void)
{
	int i, j, k, r, c;
	unsigned int flags;

	for (i = 0; i < N - F; i++) text_buf[i] = ' ';
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = getc_lzss()) == EOF) break;
			flags = c | 0xff00;
		}
		if (flags & 1) {
			if ((c = getc_lzss()) == EOF) break;
			putc_buffer(c);  text_buf[r++] = c;  r &= (N - 1);
		} else {
			if ((i = getc_lzss()) == EOF) break;
			if ((j = getc_lzss()) == EOF) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				putc_buffer(c);  text_buf[r++] = c;  r &= (N - 1);
			}
		}
	}
}

void lzss_decompress(FILE* fptr1, unsigned char* tbuffer, long comp_size)
{
  lzss_fptr = fptr1;
  from_pointer = 0;
  from_max = comp_size;
  to_pointer = 0;
  to_buffer = tbuffer;
  lzss_decode();
}

void exedat_archive(char* name)
{
  if (archive_name != NULL)
    free(archive_name);
  /* the last allocation of archive_name is going to be freed on exit */
  archive_name = (char *) malloc(strlen(name)+1);
  strcpy(archive_name, name);
}

long exedat_search(char* name)
{
  FILE* fptr;
  long i, seeklong;

  fptr = fopen(archive_name, "rb"); /* no check, so don't make mistakes! */
  seeklong = sizeof(header);
  fseek(fptr, -seeklong, SEEK_END);
  fread(&header, sizeof(header), 1, fptr);
  for (i = 0; i < header.number_files; i++)
	{
	seeklong += sizeof(indexblock);
	fseek(fptr, -seeklong, SEEK_END);
	fread(&indexblock, sizeof(indexblock), 1, fptr);
	if (strcmp(indexblock.name, name) == 0)
	  break;
	}
  fclose(fptr);
  return(indexblock.original_length);
}

void exedat_load(unsigned char* tbuffer)
{
  FILE* fptr;
  fptr = fopen(archive_name, "rb");
  fseek(fptr, -indexblock.lokation, SEEK_END);
  switch (indexblock.compression)
	{
	case compr_lzss:
	  lzss_decompress(fptr, tbuffer, indexblock.length);
	  break;
	case compr_none:
	  fread(tbuffer, indexblock.length, 1, fptr);
	  break;
	}
  fclose(fptr);
  return;
}

/****************************************************************************
*************************  END EXEDAT HEADER  *******************************
****************************************************************************/
