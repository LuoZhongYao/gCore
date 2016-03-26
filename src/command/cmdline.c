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

static void cli_case_U(FILE *stx)
{
    switch(getc(stx)) {
    case '0': {
        cmd_ble_version();
    } break;

    }
}

static void cli_case_Q(FILE *stx)
{
    switch(getc(stx)) {
    case 'A': {
        cmd_ble_query_bdaddr();
    } break;

    }
}

static void cli_case_SP(FILE *stx)
{
    switch(getc(stx)) {
    case '5': {
        cmd_ble_port();
    } break;

    }
}

static void cli_case_S(FILE *stx)
{
    switch(getc(stx)) {
    case 'P': {
        cli_case_SP(stx);
    } break;

    }
}

static void cli_case_C(FILE *stx)
{
    switch(getc(stx)) {
    case 'D': {
        cmd_ble_disconnect();
    } break;

    case 'Y': {
        cmd_ble_hfp_status();
    } break;

    }
}

static void cli_case_H(FILE *stx)
{
    switch(getc(stx)) {
    case 'M': {
        u8 key = 0;
        u16 x = 0;
        u16 y = 0;
        __scanf(stx,"%2x%4x%4x",&key,&x,&y);
        cmd_ble_touch_move(key,x,y);
    } break;

    case 'Y': {
        cmd_ble_hid_status();
    } break;

    case 'C': {
        cmd_ble_hid_status();
    } break;

    case 'D': {
        cmd_ble_hid_disconnect();
    } break;

    case 'Q': {
        u16 x = 0;
        u16 y = 0;
        __scanf(stx,"%05x%05x",&x,&y);
        cmd_ble_touch_press(x,y);
    } break;

    case 'R': {
        u16 x = 0;
        u16 y = 0;
        __scanf(stx,"%05x%05x",&x,&y);
        cmd_ble_touch_release(x,y);
    } break;

    }
}

static void cli_case_M(FILE *stx)
{
    switch(getc(stx)) {
    case 'X': {
        cmd_ble_pair_list();
    } break;

    }
}

extern void cli_parse(FILE *stx)
{
    switch(getc(stx)) {
    case 'U':
        cli_case_U(stx);
        break;
    case 'Q':
        cli_case_Q(stx);
        break;
    case 'S':
        cli_case_S(stx);
        break;
    case 'C':
        cli_case_C(stx);
        break;
    case 'H':
        cli_case_H(stx);
        break;
    case 'M':
        cli_case_M(stx);
        break;
    }
}
/* #endif */

