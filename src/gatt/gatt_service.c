#define LOG_TAG "gatt"

#include <stdlib.h>
#include <memory.h>
#include <utils/log.h>
#include <utils/utils.h>

#include "gatt.h"
#include "app_gatt_db.h"
#include "service.h"

#ifdef BLE_LOG
# define LOGD DLOG
#else
# define LOGD(...)
#endif


static struct {
    gatt_client_config serv_changed_config;
} self;

static bool gatt_service_check_handle_range(u16 handle)
{
    return ((handle >= HANDLE_GATT_SERVICE) &&
            (handle <= HANDLE_GATT_SERVICE_END))
        ? TRUE : FALSE;
}

static  void gatt_service_read(GATT_ACCESS_IND_T *ind)
{
    u16  data_length = 0;
    u8   value[2] = {0x00,0x00};
    gatt_status_t rc = gatt_status_read_not_permitted;

    if(ind->handle == HANDLE_SERVICE_CHANGED_CLIENT_CONFIG) {
        /* Service changed client
         * characteristic configuration
         * descriptor read
         *          * has been requested
         *                   */
        data_length = 2;
        value[0] = self.serv_changed_config & 0xff;
        value[1] = self.serv_changed_config >> 8;
        rc = gatt_status_success;
    }

    /* Send Access Response */
    GattAccessResponse(ind->cid, ind->handle, rc, data_length, value);
}

static void gatt_service_write(GATT_ACCESS_IND_T *ind)
{
    u16  client_config;
    u8  *value = ind->value;
    gatt_status_t rc = gatt_status_write_not_permitted;

    if(ind->handle == HANDLE_SERVICE_CHANGED_CLIENT_CONFIG) {
        client_config = value[0] | value[1] << 8;
        LOGD("client config : %04x\n",client_config);
        if((client_config == gatt_client_config_indication) ||
           (client_config == gatt_client_config_none))
        {
            self.serv_changed_config = client_config;
            rc = gatt_status_success;
        } else {
            rc = gatt_status_application_error;
        }

    }
    /* Send Access Response */
    GattAccessResponse(ind->cid, ind->handle, rc, 0, NULL);
}

static void gatt_service_init(void)
{
    self.serv_changed_config = gatt_client_config_none;
}

static void gatt_service_reset(void)
{
    gatt_service_init();
}

static void gatt_service_deinit(void)
{
    bzero((char *)&self,sizeof(self));
}

const service_ctx_t gatt_service = {
    .Init               = gatt_service_init,
    .Reset              = gatt_service_reset,
    .DeInit             = gatt_service_deinit,
    .CheckHandleRange   = gatt_service_check_handle_range,
    .Read               = gatt_service_read,
    .Write              = gatt_service_write,
};
