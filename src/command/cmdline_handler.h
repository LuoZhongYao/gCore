/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 01
 ************************************************/
#ifndef __CMDLINE_HANDLER_H__
#define __CMDLINE_HANDLER_H__

#include <utils/utils.h>

void cmd_ble_version(void);
void cmd_ble_touch_press(u16 x,u16 y);
void cmd_ble_touch_release(u16 x,u16 y);
void cmd_ble_touch_move(u8 key,u16 x,u16 y);

void cmd_ble_hid_disconnect(void);
void cmd_ble_disconnect(void);
void cmd_ble_hfp_status(void);
void cmd_ble_hid_status(void);
void cmd_ble_pair_list(void);
void cmd_ble_port(void);
void cmd_ble_query_bdaddr(void);



#endif

