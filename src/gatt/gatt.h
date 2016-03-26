/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 23
 ************************************************/
#ifndef __BLE_H__
#define __BLE_H__

#include <message.h>
#include <utils/utils.h>

typedef struct {
    u16      cid;
    TaskData task;
}gatt_ctx_t;

extern gatt_ctx_t *GattCtx(void);
extern void GattHandler(Task task,MessageId id,Message message);

#endif

