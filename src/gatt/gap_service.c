#define LOG_TAG "gap"

#include <gatt.h>
#include <stdlib.h>
#include <connection.h>
#include <utils/utils.h>

#include "gap_service.h"
#include "app_gatt_db.h"

#ifdef  BLE_LOG
# define LOGD   DLOG
# define LOGW   WLOG
#else
# define LOGD(...)
# define LOGW LOGD
#endif

static const u8 g_device_name[] = {
    ble_ad_type_complete_local_name, 
    'a', 'C', 'T', 'R','L','\0'}; 

static void GapHandleAccessRead(GATT_ACCESS_IND_T *p_ind)
{
    u16 length = 0;
    const u8  *p_value = NULL;
    gatt_status_t rc = gatt_status_success;

    switch(p_ind->handle) {
    case HANDLE_DEVICE_NAME: {
        /* Validate offset against length, it should be less than 
         * device name length
         */
        if(p_ind -> offset < sizeof(g_device_name)) {
            length = sizeof(g_device_name) - p_ind->offset;
            p_value = (g_device_name + p_ind->offset);
        } else {
            rc = gatt_status_invalid_offset;
        }
    } break;

    default:
        /* Let firmware handle the request */
        rc = gatt_status_invalid_permissions;
    break;

    }

    GattAccessResponse(p_ind->cid, p_ind->handle, rc,
                  length, p_value);
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      GapHandleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles write operation on GAP service attributes
 *      maintained by the application and responds with the GATT_ACCESS_RSP 
 *      message.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/

static void GapHandleAccessWrite(GATT_ACCESS_IND_T *p_ind)
{
    gatt_status_t rc = gatt_status_success;

    switch(p_ind->handle) {
    case HANDLE_DEVICE_NAME:
        /* Update device name */
        LOGW("update device name not support\n");
    break;

    default: {
        /* No more IRQ characteristics */
        rc = gatt_status_write_not_permitted;
    } break;

    }

    GattAccessResponse(p_ind->cid, p_ind->handle, rc, 0, NULL);
}

static bool GapCheckHandleRange(uint16 handle)
{
    return ((handle >= HANDLE_GAP_SERVICE) &&
            (handle <= HANDLE_GAP_SERVICE_END))
        ? TRUE : FALSE;
}


const service_ctx_t gap_service = {
    .Init   = NULL,
    .DeInit = NULL,
    .CheckHandleRange = GapCheckHandleRange,
    .Read   = GapHandleAccessRead,
    .Write  = GapHandleAccessWrite,
};

