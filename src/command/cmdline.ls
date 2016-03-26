%keep %{
/* #if defined(AT_GM) */
#include <utils/utils.h>
#include <utils/file.h>
#include <utils/scanf.h>

#include "cmdline_handler.h"
#include "cmdline.h"

#define __WRT(v,b,l)   \
    do{\
        struct iovec * _v = (v);\
        _v->iov_base = (u8 *)(b);\
        _v->iov_len = (l);\
        _v->iov_free = false;\
    }while(0)

#define __SND(v,...)   \
    do{\
        struct iovec *_v = (v);\
        _v->iov_base = (u8*)malloc(100);\
        sprintf(_v->iov_base,##__VA_ARGS__);\
        _v->iov_len = strlen(_v->iov_base);\
        _v->iov_free = true;\
    }while(0)

#define __IND(v,ind)   __WRT(v,ind,sizeof(ind) - 1)
#define __FLUSH  lwritev
#define __CMD(...)
%}

%bdinit "{0,{0,0,0}}"

CMD {
    ["U0",,cmd_ble_version(void)]
    ["CY",,cmd_ble_hfp_status(void)]
    ["HY",,cmd_ble_hid_status(void)]
    ["QA",,cmd_ble_query_bdaddr(void)]
    ["SP5",,cmd_ble_port(void)]
    ["HC",,cmd_ble_hid_status(void)]
    ["HD",,cmd_ble_hid_disconnect(void)]
    ["CD",,cmd_ble_disconnect(void)]
    ["HQ",("%05x%05x",&x,&y),cmd_ble_touch_press(u16 x,u16 y)]
    ["HR",("%05x%05x",&x,&y),cmd_ble_touch_release(u16 x,u16 y)]
    ["HM",("%2x%4x%4x",&key,&x,&y),cmd_ble_touch_move(u8 key,u16 x,u16 y)]
    ["MX",,cmd_ble_pair_list(void)]
}

%keep %{
/* #endif */
%}
