/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 三月 23
 ************************************************/
#ifndef __BLE_SERVICE_H__
#define __BLE_SERVICE_H__

#include <utils/utils.h>
#include <gatt.h>

typedef enum {
    gatt_client_config_none         = 0x0000,
    gatt_client_config_notification = 0x0001,
    gatt_client_config_indication   = 0x0002,
    gatt_client_config_reserved     = 0xFFF4
} gatt_client_config;

typedef struct {
    void (*Init)(void);
    void (*DeInit)(void);
    void (*Reset)(void);
    bool (*CheckHandleRange)(u16 handle);
    void (*Read)(GATT_ACCESS_IND_T *ind);
    void (*Write)(GATT_ACCESS_IND_T *ind);
}service_ctx_t;

extern void ServiceInit(void);
extern void ServiceReset(void);
extern void ServiceAccessInd(GATT_ACCESS_IND_T *ind);


#endif

