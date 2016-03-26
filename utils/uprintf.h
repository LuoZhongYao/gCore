#ifndef __Z_IO_H__
#define __Z_IO_H__
#include <stream.h>
#include <sink.h>
#include "utils.h"

struct iovec {
    u8      *iov_base;
    size_t  iov_len;
    bool    iov_free;
};

int swrite(Sink sink,const void *buffer,size_t length);
int swritev(Sink sink,const struct iovec *iov,int cnt);

int uwrite(const void *buffer, size_t length);
int uwritev(const struct iovec *iov,int cnt);
int uprintf(const char *fmt,...) __attribute__((format(printf,1,2)));
#endif
