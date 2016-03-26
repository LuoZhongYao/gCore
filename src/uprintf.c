#include <stdio.h>
#include <memory.h>
#include <message.h>
#include <panic.h>
#include <utils/uprintf.h>
#define MAX_BUFFER_LENGTH 110

int swrite(Sink sink,const void *buffer,size_t length)
{
    if(SinkClaim(sink,length) != 0xffff) {
        memcpy(SinkMap(sink),buffer,length);
        SinkFlush(sink,length);
    }
    return length;
}

int swritev(Sink sink,const struct iovec *iov,int cnt)
{
    u8 *u;
    int i,w = -1,c;

    FALSE_RETURN(cnt,-1);
    for(i = 0,c = 0;i < cnt;i++)
        c += iov[i].iov_len;

    if(SinkClaim(sink,c) == 0xffff)
        goto out;
    u = SinkMap(sink);

    if(u == NULL)
        goto out;

    for(i = 0,w = 0;i < cnt;i++) {
        memcpy(u + w,iov[i].iov_base,iov[i].iov_len);
        w += iov[i].iov_len;
    }
    SinkFlush(sink,w);

out:
    for(i = 0;i < cnt;i++) {
        if(iov[i].iov_free)
            free(iov[i].iov_base);
    }
    return w;
}

int uwrite(const void *buffer,size_t length)
{
    Sink sink = StreamSinkFromSource(StreamUartSource());
    return swrite(sink,buffer,length);
}

int uwritev(const struct iovec *iov,int cnt)
{
    Sink sink = StreamSinkFromSource(StreamUartSource());
    return  swritev(sink,iov,cnt);
}

int uprintf(const char *fmt,...)
{
    char *buffer = malloc(MAX_BUFFER_LENGTH);
    if(buffer) {
        int plen;
        va_list args;
        va_start(args,fmt);
        plen = vsprintf(buffer,fmt,args);
        va_end(args);
        uwrite(buffer,plen);
        free(buffer);
        return plen;
    } else {
#define MSG "D/system: memory overflow\n"
        uwrite(MSG,sizeof(MSG) - 1);
    }
    return 0;
}
