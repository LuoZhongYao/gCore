#define LOG_TAG "main"

#include "private.h"
#include "security.h"
#include "uart_handler.h"
#include "gatt/gatt.h"
#include "gatt/app_gatt_db.h"

#include <connection.h>
#include <gatt.h>
#include <memory.h>

#include <utils/log.h>
#ifdef MAIN_LOG
# define LOGD   DLOG
# define LOGW   WLOG
# define LOGE   ELOG
#else
# define LOGD(...)
# define LOGW LOGD
# define LOGE LOGD
#endif

extern void _init(void);

static PT_THREAD(init_main_thread(Task task,MessageId id,Message message));

static TASK_HANDLER(codec_handler)
{
    switch(id) {
    case CODEC_INIT_CFM:
    break;

    default:
        LOGW("CODEC ? %04x\n",id);
    break;
    }
}

static TASK_HANDLER(cl_handler)
{
    switch(id) {
        /* BLE message */
    case CL_DM_BLE_SET_ADVERTISING_PARAMS_CFM: {
        INST_MSG(CL_DM_BLE_SET_ADVERTISING_PARAMS_CFM_T);
        LOGD("[%s](%04x) CL_DM_BLE_SET_ADVERTISING_PARAMS_CFM\n",
             DONE_STR(!msg->status),
             msg->status);
    } break;
    case CL_DM_BLE_ADVERTISING_PARAM_UPDATE_IND:
    case CL_DM_BLE_SET_ADVERTISING_DATA_CFM:
    case CL_DM_BLE_ADD_DEVICE_TO_WHITE_LIST_CFM:
        break;


        /* ignore */
    case CL_INIT_CFM:
    case CL_DM_ACL_CLOSED_IND:
    case CL_DM_ACL_OPENED_IND:
        break;

    case CL_SM_REMOTE_IO_CAPABILITY_IND: {
        INST_MSG(CL_SM_REMOTE_IO_CAPABILITY_IND_T);
        HandleSmRemoteIoCapabilityInd(msg);
    } break;

    case CL_SM_IO_CAPABILITY_REQ_IND: {
        INST_MSG(CL_SM_IO_CAPABILITY_REQ_IND_T);
        HandleSmIoCapabilityReq(msg);
    } break;

    case CL_SM_ENCRYPTION_CHANGE_IND: {
        INST_MSG(CL_SM_ENCRYPTION_CHANGE_IND_T);
        LOGD("encrypted changed : %s\n",BOOL_STR(msg->encrypted));
    } break;

    case CL_SM_BLE_SIMPLE_PAIRING_COMPLETE_IND: {
        INST_MSG(CL_SM_BLE_SIMPLE_PAIRING_COMPLETE_IND_T);
        LOGD("simple pairing complete : %x\n",msg->status);
    } break;

    case CL_DM_BLE_SECURITY_CFM: {
        INST_MSG(CL_DM_BLE_SECURITY_CFM_T);
        LOGD("security : %x\n",msg->status);
    } break;

    default:
        LOGW("CL ? %04x\n",id);
        break;
    }
}

static TASK_HANDLER(super_handler)
{
    /******** Synchronization initialization *************/
    switch(id) {
    case CL_INIT_CFM:
    case GATT_INIT_CFM:
    case CODEC_INIT_CFM: {
        char ret = init_main_thread(task,id,message);
        if(ret == PT_EXITED) {
            LOGE("Something bad, I'm go die\n");
            Panic();
        } else if(ret == PT_ENDED) {
            LOGD("Initialization is complete\n");
        }
    } break;

    default : 
    break;
    }

    switch(id) {
    case CL_MESSAGE_BASE ... CL_MESSAGE_TOP         : cl_handler(task,id,message);      break;
    case CODEC_MESSAGE_BASE ... CODEC_MESSAGE_TOP   : codec_handler(task,id,message);   break;
    case GATT_MESSAGE_BASE ... GATT_MESSAGE_TOP     : GattHandler(task,id,message);      break;

    default:
    break;
    }

}

extern const u16 gattDatabase[];
static PT_THREAD(init_main_thread(Task task,MessageId id,Message message))
{
    main_ctx_t *ctx = TASK_MAIN_CTX(task);
    PT_BEGIN(&(ctx->pt));

    ctx->task.handler = super_handler;

    /* init connection lib */
    ConnectionInit(&ctx->task);
    PT_YIELD(&ctx->pt);
    {
        INST_MSG(CL_INIT_CFM_T);
        LOGD("[%s](%04x) ConnectionInit\n",
             DONE_STR(msg->status == success),
             msg->status);
        if(msg->status != success) {
            PT_EXIT(&(ctx->pt));
        }
    }

    /* init codec lib */
    CodecInitCsrInternal(&ctx->codec,&ctx->task);
    PT_YIELD(&ctx->pt);
    {
        INST_MSG(CODEC_INIT_CFM_T);
        LOGD("[%s](%04x) CodecInitCsrIniternal\n",
             DONE_STR(msg->status == success),
             msg->status);
        if(msg->status != success) {
            PT_EXIT(&(ctx->pt));
        }
    }

    /* init gatt */
    GattInit(&ctx->task,GattGetDatabaseSize(),(u16 *)gattDatabase);
    PT_YIELD(&ctx->pt);
    {
        INST_MSG(GATT_INIT_CFM_T);
        LOGD("[%s](%d) GattInit\n",
             DONE_STR(msg->status == gatt_status_success),
             msg->status);
        if(msg->status != success) {
            PT_EXIT(&(ctx->pt));
        }
    }

    /* init uart */
    UartInit();

    PT_END(&(ctx->pt));
}

void _init(void)
{
    /* */
}

int main(void)
{
    main_ctx_t *ctx;

    LOGD("\n");
    LOGD("------------- startup -----------\n");

    ctx = malloc(sizeof(*ctx));
    memset(ctx,0,sizeof(*ctx));

    PT_INIT(&ctx->pt);
    if(!PT_SCHEDULE(init_main_thread(&ctx->task,0,NULL))) {
        LOGE("Something bad, I'm go die\n");
        Panic();
    }

    /* Start the message scheduler loop */
    MessageLoop();

    /* Never get here...(csr say)*/
    return 0;
}
