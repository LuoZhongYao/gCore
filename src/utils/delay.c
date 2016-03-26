#include <utils/delay.h>
#include <stdlib.h>
#include <panic.h>

static void delay_handler(Task task,MessageId id,Message message);
static TaskData __task = {delay_handler};

typedef struct {
    delay_fn fn;
    void *args;
}DELAY_ARGS;

static void delay_handler(Task task,MessageId id,Message message)
{
    DELAY_ARGS *msg = (DELAY_ARGS*)message;
    msg->fn(msg->args);
}


extern void DelayApply(Delay delay,delay_fn fn,void *args)
{
    MESSAGE_MAKE(message,DELAY_ARGS);
    message->args = args;
    message->fn   = fn;
    MessageSendLater(&__task,0xFFFF,message,delay);
}
