struct bitmap8 *deflate_decompress_mask(unsigned char *compdata,int compsize,int x,int y);
U8 *deflate_decompress(unsigned char *compdata,int compsize,int uncompsize); // free U8* with delete[]
void deflate_stats();
