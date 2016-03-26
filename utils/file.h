/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 二月 18
 ************************************************/
#ifndef __FILE_H__
#define __FILE_H__
#include <utils/utils.h>

#define EOF -1

struct file {
    char *buffer;
    u16  read_pos;
    u16  write_pos;
    u16  length;
    void (*fn)(const void *,u16);
};

typedef struct file FILE;

void clear_file(FILE *stx);
void create_file(FILE *stx,void *buffer,u16 n,void (*fn)(const void *,u16));
int putc(int c,FILE *stx);
int getc(FILE *stx);
int ungetc(int c,FILE *stx);

#endif

