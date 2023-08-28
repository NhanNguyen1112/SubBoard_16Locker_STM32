
#ifndef __String__
#define __String__

int str_len(const char *src);
void str_cpy(char *dest, const char *src, const char len);
void str_cat(char *dest, const char *src);
void str_lwr(char *s); /* Hoa ve thuong */
void str_upr(char *s); /* Thuong ve hoa */
void BufferClear(unsigned char *Buffer, unsigned int Length);

#endif /* __String__ */





