#define LOG_TAG "gatt"

#include <gatt.h>
#include <connection.h>
#include <memory.h>

#include <utils/utils.h>
#include <utils/delay.h>
#include <utils/log.h>

#include "app_gatt_db.h"
#include "gatt.h"
#include "service.h"
#include "appearance.h"

#ifdef  BLE_LOG
# define LOGD   DLOG
# define LOGW   WLOG
# define LOGE   ELOG
#else
# define LOGD(...)
# define LOGW LOGD
# define LOGE LOGD
#endif

static gatt_ctx_t self;

static const u8 ad[] = {
    0x02,
    ble_ad_type_flags,
    BLE_FLAGS_GENERAL_DISCOVERABLE_MODE | BLE_FLAGS_DUAL_HOST,
    
    0x03,
    ble_ad_type_complete_uuid16,
    GATT_SERVICE_UUID_HUMAN_INTERFACE_DEVICE & 0xff,
    GATT_SERVICE_UUID_HUMAN_INTERFACE_DEVICE >> 8,

    0x07,
    ble_ad_type_complete_local_name,
    'a','C','T','R','L','\0',

    0x03,
    ble_ad_type_appearance,
    APPEARANCE_TOUCH_VALUE  & 0xff,
    APPEARANCE_TOUCH_VALUE >> 0x08,
};

extern gatt_ctx_t *GattCtx(void)
{
    return &self;
}

static void gatt_start_fast_advertising(void)
{
    ble_adv_params_t params = {
        .undirect_adv.adv_interval_min = 60,
        .undirect_adv.adv_interval_max = 60,
        .undirect_adv.filter_policy = ble_filter_none,
    };

    ConnectionDmBleSetAdvertisingParamsReq(ble_adv_ind,FALSE,BLE_ADV_CHANNEL_ALL,&params);
    GattConnectRequest(&self.task, NULL, gatt_connection_ble_slave_undirected, FALSE);
}

static void gatt_handle_init_cfm(GATT_INIT_CFM_T *msg)
{
    if(msg->status == gatt_status_success) {
        ServiceInit();
        ConnectionDmBleAddTdlDevicesToWhiteListReq(TRUE);
        ConnectionDmBleSetAdvertisingDataReq(sizeof(ad),ad);
        DelayApply(100,(delay_fn)gatt_start_fast_advertising,NULL);
    }
}

static void gatt_handle_disconnect_ind(GATT_DISCONNECT_IND_T *msg)
{
    ServiceReset();
    gatt_start_fast_advertising();
}

static void gatt_handle_connect_cfm(GATT_CONNECT_CFM_T *msg)
{
    if(msg->status == gatt_status_initialising) {
        ConnectionDmBleSetAdvertiseEnable(TRUE);
    } else if(msg->status == gatt_status_success) {
        self.cid = msg->cid;
        ConnectionDmBleSetAdvertiseEnable(FALSE);
        //ConnectionDmBleSecurityReq(&self.task,&msg->taddr,ble_security_encrypted_bonded,ble_connection_slave_directed);
    }
}

extern void GattHandler(Task task,MessageId id,Message message)
{
    switch(id) {

    case GATT_INIT_CFM: {
        INST_MSG(GATT_INIT_CFM_T);
        self.task = *task;
        gatt_handle_init_cfm(msg);
    } break;

    case GATT_CONNECT_IND: {
        INST_MSG(GATT_CONNECT_IND_T);
        LOGD("%04x GATT_CONNECT_IND\n", msg->cid);
        self.cid = msg->cid;
        GattConnectResponse(&self.task,msg->cid,msg->flags,TRUE);
    } break;

    case GATT_CONNECT_CFM: {
        INST_MSG(GATT_CONNECT_CFM_T);
        LOGD("[%s](%04x) %04x GATT_CONNECT_CFM\n",
             DONE_STR((msg->status == gatt_status_success) ||
                      (msg->status == gatt_status_initialising)),
             msg->status,
             msg->cid
             );
        gatt_handle_connect_cfm(msg);
    } break;

    case GATT_DISCONNECT_IND: {
        INST_MSG(GATT_DISCONNECT_IND_T);
        gatt_handle_disconnect_ind(msg);
    } break;

    case GATT_EXCHANGE_MTU_IND :{
        INST_MSG(GATT_EXCHANGE_MTU_IND_T);
        GattExchangeMtuResponse(msg->cid, 0);

    } break;
    case GATT_ACCESS_IND: {
        INST_MSG(GATT_ACCESS_IND_T);
        ServiceAccessInd(msg);
    } break;

    case GATT_NOTIFICATION_CFM: {
        INST_MSG(GATT_NOTIFICATION_CFM_T);
        if(msg->status != gatt_status_success) {
            LOGE("[%s](%04x) %04x,%04x GATT_NOTIFICATION_CFM\n",
                 DONE_STR(msg->status == gatt_status_success),
                 msg->status,
                 msg->cid,
                 msg->handle
                 );
        }
    } break;

    default :
        LOGW("GATT %04x\n",id);
    break;
    }
}

