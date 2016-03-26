#include <gatt.h>
#include <utils/uprintf.h>
#include "../gatt/gatt.h"
#include "../gatt/app_gatt_db.h"
#include "cmdline_handler.h"

void cmd_ble_version(void)
{
    uprintf("aCTRL-0.0.0\r\n");
}

void cmd_ble_touch_press(u16 x,u16 y)
{
    gatt_ctx_t *gatt = GattCtx();
    u8 report[13] __unused = {
        0x03, 0x01,
        x & 0xff, x >> 8 & 0xff,
        y & 0xff, y >> 8 & 0xff,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x01
    };
    GattNotificationRequest(&gatt->task,gatt->cid,HANDLE_HID_INPUT_REPORT,sizeof(report),report);
}

void cmd_ble_touch_release(u16 x,u16 y)
{
    gatt_ctx_t *gatt = GattCtx();
    u8 report[13] __unused = {
        0x2, 0x1,
        x & 0xff, (x >> 8) & 0xff,
        y & 0xff, (y >> 8) & 0xff,

        0, 0,
        0, 0,
        0, 0,
        1
    };
    GattNotificationRequest(&gatt->task,gatt->cid,HANDLE_HID_INPUT_REPORT,sizeof(report),report);
}

void cmd_ble_touch_move(u8 key,u16 x,u16 y)
{
    gatt_ctx_t *gatt = GattCtx();
    y = 480 - y;
    x = (u32)x * 960 / 800;
    y = (u32)y * 960 / 480;
    u8 report[13] __unused = {
        0x2 | (key & 1), 0x1,
        y & 0xff, (y >> 8) & 0xff,
        x & 0xff, (x >> 8) & 0xff,

        0, 0,
        0, 0,
        0, 0,
        1
    };
    GattNotificationRequest(&gatt->task,gatt->cid,HANDLE_HID_INPUT_REPORT,sizeof(report),report);
}

/*****************************************************************************/

void cmd_ble_hid_disconnect(void)
{
    uprintf("HID=0\r\n");
}

void cmd_ble_disconnect(void)
{
    uprintf("IA\r\n");
}

void cmd_ble_hfp_status(void)
{
    uprintf("MG3\r\n");
}

void cmd_ble_hid_status(void)
{
    uprintf("HID=3\r\n");
}

void cmd_ble_port(void)
{
    uprintf("HSP011300640\r\n");
}

void cmd_ble_pair_list(void)
{
    uprintf("MX0ac61eae9db59刘军叁的iPhone\r\n");
}

void cmd_ble_query_bdaddr(void)
{
    uprintf("JHac61eae9db59\r\n");
}
