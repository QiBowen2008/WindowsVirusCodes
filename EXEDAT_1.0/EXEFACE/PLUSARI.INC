/****************************************************************************
*************************  BEGIN EXEDAT HEADER  *****************************
****************************************************************************/

//LZSS_DECODE(PD) by Haruhiko Okumura
//LZARI_DECODE(PD) by Haruhiko Okumura
//EXEDAT by Ren‚ Olsthoorn in 1996

#define compr_none 0
#define compr_lzss 1
#define compr_lzari 2
#define N	 4096
#define F	   18
#define THRESHOLD   2
#define F2         60
#define M          15
#define Q1  (1UL << M)
#define Q2  (2 * Q1)
#define Q3  (3 * Q1)
#define Q4  (4 * Q1)
#define MAX_CUM (Q1 - 1)
#define N_CHAR  (256 - THRESHOLD + F2)

unsigned long low, high, value;
int char_to_sym[N_CHAR], sym_to_char[N_CHAR + 1], shifts;
unsigned int sym_freq[N_CHAR + 1], sym_cum[N_CHAR + 1],	position_cum[N + 1];
unsigned char text_buf [N + F2 - 1];
unsigned char* from_buffer, *to_buffer;
unsigned long from_pointer, from_max, to_pointer, textsize, codesize;
unsigned int PB_buffer, PB_mask, GB_buffer, GB_mask;
FILE* lz_fptr;

char* archive_name;

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

int getc_lz(void)
  {
  if (from_pointer++ == from_max)
	return(EOF);
  return(getc(lz_fptr));
  }

void lzss_decode(void)
{
	int i, j, k, r, c;
	unsigned int flags;

	for (i = 0; i < N - F; i++) text_buf[i] = ' ';
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = getc_lz()) == EOF) break;
			flags = c | 0xff00;
		}
		if (flags & 1) {
			if ((c = getc_lz()) == EOF) break;
			putc_buffer(c);  text_buf[r++] = c;  r &= (N - 1);
		} else {
			if ((i = getc_lz()) == EOF) break;
			if ((j = getc_lz()) == EOF) break;
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
  lz_fptr = fptr1;
  from_pointer = 0;
  from_max = comp_size;
  to_pointer = 0;
  to_buffer = tbuffer;
  lzss_decode();
}

void PutBit(int bit)
{
	if (bit) PB_buffer |= PB_mask;
	if ((PB_mask >>= 1) == 0) {
		PB_buffer = 0;  PB_mask = 128;  codesize++;
	}
}

void FlushBitBuffer(void)
{
	int  i;
	for (i = 0; i < 7; i++) PutBit(0);
}

int GetBit(void)
{
	if ((GB_mask >>= 1) == 0) {
		GB_buffer = getc_lz(); GB_mask = 128;
	}
	return ((GB_buffer & GB_mask) != 0);
}

void StartModel(void)
{
	int ch, sym, i;

	sym_cum[N_CHAR] = 0;
	for (sym = N_CHAR; sym >= 1; sym--) {
		ch = sym - 1;
		char_to_sym[ch] = sym;  sym_to_char[sym] = ch;
		sym_freq[sym] = 1;
		sym_cum[sym - 1] = sym_cum[sym] + sym_freq[sym];
	}
	sym_freq[0] = 0;
	position_cum[N] = 0;
	for (i = N; i >= 1; i--)
		position_cum[i - 1] = position_cum[i] + 10000 / (i + 200);
}

void UpdateModel(int sym)
{
	int i, c, ch_i, ch_sym;

	if (sym_cum[0] >= MAX_CUM) {
		c = 0;
		for (i = N_CHAR; i > 0; i--) {
			sym_cum[i] = c;
			c += (sym_freq[i] = (sym_freq[i] + 1) >> 1);
		}
		sym_cum[0] = c;
	}
	for (i = sym; sym_freq[i] == sym_freq[i - 1]; i--) ;
	if (i < sym) {
		ch_i = sym_to_char[i];    ch_sym = sym_to_char[sym];
		sym_to_char[i] = ch_sym;  sym_to_char[sym] = ch_i;
		char_to_sym[ch_i] = sym;  char_to_sym[ch_sym] = i;
	}
	sym_freq[i]++;
	while (--i >= 0) sym_cum[i]++;
}

static void Output(int bit)
{
	PutBit(bit);
	for ( ; shifts > 0; shifts--) PutBit(! bit);
}

void EncodePosition(int position)
{
	unsigned long int  range;

	range = high - low;
	high = low + (range * position_cum[position    ]) / position_cum[0];
	low +=       (range * position_cum[position + 1]) / position_cum[0];
	for ( ; ; ) {
		if (high <= Q2) Output(0);
		else if (low >= Q2) {
			Output(1);  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			shifts++;  low -= Q1;  high -= Q1;
		} else break;
		low += low;  high += high;
	}
}

void EncodeEnd(void)
{
	shifts++;
	if (low < Q1) Output(0);  else Output(1);
	FlushBitBuffer();
}

int BinarySearchSym(unsigned int x)
{
	int i, j, k;

	i = 1;  j = N_CHAR;
	while (i < j) {
		k = (i + j) / 2;
		if (sym_cum[k] > x) i = k + 1;  else j = k;
	}
	return i;
}

int BinarySearchPos(unsigned int x)
{
	int i, j, k;

	i = 1;  j = N;
	while (i < j) {
		k = (i + j) / 2;
		if (position_cum[k] > x) i = k + 1;  else j = k;
	}
	return i - 1;
}

void StartDecode(void)
{
	int i;
	for (i = 0; i < M + 2; i++)
		value = 2 * value + GetBit();
}

int DecodeChar(void)
{
	int	 sym, ch;
	unsigned long int  range;

	range = high - low;
	sym = BinarySearchSym((unsigned int)
		(((value - low + 1) * sym_cum[0] - 1) / range));
	high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
	low +=       (range * sym_cum[sym    ]) / sym_cum[0];
	for ( ; ; ) {
		if (low >= Q2) {
			value -= Q2;  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			value -= Q1;  low -= Q1;  high -= Q1;
		} else if (high > Q2) break;
		low += low;  high += high;
		value = 2 * value + GetBit();
	}
	ch = sym_to_char[sym];
	UpdateModel(sym);
	return ch;
}

int DecodePosition(void)
{
	int position;
	unsigned long int  range;

	range = high - low;
	position = BinarySearchPos((unsigned int)
		(((value - low + 1) * position_cum[0] - 1) / range));
	high = low + (range * position_cum[position    ]) / position_cum[0];
	low +=       (range * position_cum[position + 1]) / position_cum[0];
	for ( ; ; ) {
		if (low >= Q2) {
			value -= Q2;  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			value -= Q1;  low -= Q1;  high -= Q1;
		} else if (high > Q2) break;
		low += low;  high += high;
		value = 2 * value + GetBit();
	}
	return position;
}

void lzari_Decode(void)
{
	int  i, j, k, r, c;
	unsigned long int  count, temp;

	textsize = 0;
	temp = getc_lz() << 24;
	textsize |= temp;
	temp = getc_lz() << 16;
	textsize |= temp;
	temp = getc_lz() << 8;
	textsize |= temp;
	temp = getc_lz();
	textsize |= temp;

	if (textsize == 0) return;
	StartDecode();  StartModel();
	for (i = 0; i < N - F2; i++) text_buf[i] = ' ';
	r = N - F2;
	for (count = 0; count < textsize; ) {
		c = DecodeChar();
		if (c < 256) {
			putc_buffer(c);  text_buf[r++] = c;
			r &= (N - 1);  count++;
		} else {
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			for (k = 0; k < j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				putc_buffer(c);  text_buf[r++] = c;
				r &= (N - 1);  count++;
			}
		}
	}
}

void lzari_decompress(FILE* fptr1, unsigned char* tbuffer, long comp_size)
{
  lz_fptr = fptr1;
  textsize = 0;
  codesize = 0;
  from_pointer = 0;
  to_pointer = 0;
  PB_buffer = 0;
  PB_mask = 128;
  GB_buffer = 0;
  GB_mask = 0;
  low = 0;
  high = Q4;
  value = 0;
  shifts = 0;
  from_max = comp_size;
  to_buffer = tbuffer;
  lzari_Decode();
  return;
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
	case compr_lzari:
	  lzari_decompress(fptr, tbuffer, indexblock.length);
	  break;
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
