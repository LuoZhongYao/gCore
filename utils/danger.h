/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 19
 ************************************************/
#ifndef __DANGER_H__
#define __DANGER_H__

#define offsetof(type, member) (u16)&(((type*)0)->member)

#define container_of(ptr,type,member)   ({\
        const __typeof__(ptr) _mptr = (ptr);\
        (type *)((char *)_mptr - offsetof(type,member));})


#endif

