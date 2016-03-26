/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 17
 ************************************************/
#ifndef __LOG_H__
#define __LOG_H__

#define LOG_ENABLED
#ifdef  LOG_ENABLED
#include "uprintf.h"

extern void logd_bytes(const void *data,const int length);
#   define DLOG(fmt,...) uprintf("D/%-7s: " fmt,LOG_TAG,##__VA_ARGS__)
#   define WLOG(fmt,...) uprintf("W/%-7s: " fmt,LOG_TAG,##__VA_ARGS__)
#   define ELOG(fmt,...) uprintf("E/%-7s: " fmt,LOG_TAG,##__VA_ARGS__)
#   define BLOG(data,length,fmt,...)   ({uprintf("B/%-7s: " fmt,LOG_TAG,##__VA_ARGS__);logd_bytes(data,length);})

    /* switch debug */
#   define MAIN_LOG
#   define BLE_LOG
#   define SM_LOG

#else

#   define DLOG(...)
#   define WLOG(...)
#   define ELOG(...)

#endif

#define NOT_IMPL()      WLOG("Not Impl %s:%d\n",__FILE__,__LINE__)
#define BOOL_STR(cond)  (cond) ? "True" : "False"
#define DONE_STR(cond)  (cond) ? "DONE" : "FAIL"

#endif

