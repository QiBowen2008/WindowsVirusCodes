
extern "C"
{
  long lzari_compress(unsigned char* tbuffer, unsigned char* fbuffer, long real_size);
  void lzari_decompress(unsigned char* fbuffer, unsigned char* tbuffer, long comp_size);
}

