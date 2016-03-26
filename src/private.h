/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 23
 ************************************************/
#ifndef __PRIVATE_H__
#define __PRIVATE_H__
#include <message.h>
#include <codec.h>

#include <utils/danger.h>
#include <pt/pt.h>

typedef struct {
    TaskData    task;
    Task        codec_task;
    CsrInternalCodecTaskData codec;
    struct  pt  pt;
}main_ctx_t;

#define TASK_HANDLER(name)  void name(Task task,MessageId id,Message message)
#define TASK_MAIN_CTX(ptr) container_of(ptr,main_ctx_t,task)

#endif

