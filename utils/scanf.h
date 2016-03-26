/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2015 八月 21
 ************************************************/
#ifndef __SCANF_H__
#define __SCANF_H__

#include <stdarg.h>
#include <utils/file.h>

#define EOF -1

int __scanf(FILE *stx,const char *format, ...);
int _doscan(FILE *stream, const char *format, va_list ap);

#endif

