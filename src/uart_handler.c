#include <message.h>
#include <source.h>
#include <stream.h>
#include <stdlib.h>
#include <utils/utils.h>
#include <utils/file.h>
#include <utils/uprintf.h>

#include "command/cmdline.h"
#include "uart_handler.h"

static void uart_handler(Task task,MessageId id,Message message);
static const TaskData task = {uart_handler};

#if !defined(FC_CLI)

static u16 parse_data(Source src,const void *buff,u16 len)
{
    FILE stx;
    void *cmd_start = memmem((void*)buff,len,"AT#",3);
    void *cmd_end = NULL;
    if(!cmd_start) return 0;
    /** ignore AT# **/
    cmd_start += 3;
    len = (buff + len - cmd_start);

    /** cmd length **/
    cmd_end = memmem(cmd_start,len,"\r\n",2);
    if(!cmd_end) return 0;
    create_file(&stx,cmd_start,cmd_end - cmd_start,NULL);
    cli_parse(&stx);
    return cmd_end + 2 - buff;
}

//#define UART_DUMP
#ifdef UART_DUMP
static void __DUMP(const char *s,u16 len)
{
    u16 i = 0;
    uprintf("%d: ",len);
    for(i = 0;i < len;i++) {
        if(s[i] == '\r')
            uprintf("^R");
        else if(s[i] == '\n')
            uprintf("^N");
        else
            uprintf("%c", s[i]);
    }
    uprintf("\n");
}
#endif

#endif  /* FC_CLI */

static void  cli_handler(Source src)
{
#if !defined(FC_CLI)
    u16 len = SourceSize(src);
    /* AT#XX\r\n */
    while(len > 6) {
        char *p = (char *)SourceMap(src);
        u16 clen = parse_data(src,p,len);
#ifdef UART_DUMP
        __DUMP(p,len);
        __DUMP(p,clen);
#endif
        if(!clen){
            char *s = memmem(p,len,"\r\n",2);
            if(s) 
                SourceDrop(src,s + 2 - p);
            break;
        }
        SourceDrop(src,clen);
        len = SourceSize(src);
    }

#else
    void *ptr;
    u16  clen,len;
__report:
    ptr = (void*)SourceMap(src);
    len = SourceSize(src);
    clen = fc_cli_api(ptr,len);
    if(clen) {
        SourceDrop(src,clen);
        goto __report;
    } 
#endif
}

static void uart_handler(Task task,MessageId id,Message message)
{
    switch(id) {
    case MESSAGE_MORE_DATA: {
        Source src = ((MessageMoreData *)message)->source;
        cli_handler(src);
    } break;
    default:
        break;
    }
}


void UartInit(void)
{
    MessageSinkTask(StreamUartSink(),(Task)&task);
}
