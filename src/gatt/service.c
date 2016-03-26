#define LOG_TAG "service"

#include <stdlib.h>
#include <utils/log.h>

#include "service.h"
#include "gatt_service.h"
#include "gap_service.h"
#include "hid_service.h"

#ifdef  BLE_LOG
# define LOGD   DLOG
# define LOGE   ELOG
# define LOGW   WLOG
#else
# define LOGD(...)
# define LOGE LOGD
# define LOGW LOGD
#endif

#define MASK_FALSE_NIL(b,m,s)  (((b) & (m)) ? s : "")
#define FLAGS_FMT   "%s%s%s%s%s"
#define FLAGS_STR(x)   \
    MASK_FALSE_NIL(x , ATT_ACCESS_READ               , "R" ) , \
    MASK_FALSE_NIL(x , ATT_ACCESS_WRITE              , "W" ) , \
    MASK_FALSE_NIL(x , ATT_ACCESS_PERMISSION         , "P" ) , \
    MASK_FALSE_NIL(x , ATT_ACCESS_WRITE_COMPLETE     , "C" ) , \
    MASK_FALSE_NIL(x , ATT_ACCESS_ENCRYPTION_KEY_LEN , "E" )


#define  for_each_ser(pos)  for(u16 i = 0;pos = __server[i],i < ARRAY_SIZE(__server);++i)

#define can_apply(ser,fn)   if(ser && ser->fn)
static const service_ctx_t *__server [] = {
    &gatt_service,
    &gap_service,
    &hid_service,
};

static const service_ctx_t * ble_get_service_by_handle(u16 handle)
{
    const service_ctx_t *pos = NULL;
    for_each_ser(pos) {
        can_apply(pos,CheckHandleRange) {
            if(pos->CheckHandleRange(handle)) {
                return pos;
            }
        }
    }
    return NULL;
}

extern void ServiceAccessInd(GATT_ACCESS_IND_T *ind)
{
    const service_ctx_t *ser = ble_get_service_by_handle(ind->handle);
    LOGE("HANDLE %04x offset %04x [" FLAGS_FMT "]\n",ind->handle,ind->offset,FLAGS_STR(ind->flags));
    if(!ser)
        goto err;
    if(ind->flags ==
       (ATT_ACCESS_WRITE |
        ATT_ACCESS_PERMISSION |
        ATT_ACCESS_WRITE_COMPLETE))
    {
        can_apply(ser,Write) {
            ser->Write(ind);
            return;
        }
    } else if(ind->flags ==
              (ATT_ACCESS_READ |
               ATT_ACCESS_PERMISSION))
    {
        can_apply(ser,Read) {
            ser->Read(ind);
            return;
        }
    }
err:
    LOGE("? HANDLE %04x offset %04x [" FLAGS_FMT "]\n",ind->handle,ind->offset,FLAGS_STR(ind->flags));
    GattAccessResponse(ind->cid,ind->handle,gatt_status_request_not_supported,0,NULL);

}

extern void ServiceInit(void)
{
    const service_ctx_t *ser;
    for_each_ser(ser) {
        can_apply(ser,Init) {
            ser->Init();
        }
    }
}

extern void ServiceReset(void)
{
    const service_ctx_t *ser;
    for_each_ser(ser) {
        can_apply(ser,Reset) {
            ser->Reset();
        }
    }
}
