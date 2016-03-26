/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 25
 ************************************************/
#ifndef __DELAY_H__
#define __DELAY_H__
#include <message.h>

typedef void (*delay_fn)(void *);

extern void DelayApply(Delay delay,delay_fn fn,void *);

#endif

