#define LOG_TAG "sm"

#include "security.h"
#include <stdlib.h>
#include <utils/log.h>

#ifdef SM_LOG
# define LOGD DLOG
#else
# define LOGD(...)
#endif
#define KEY_DISTRIBUTION_RANDOM (KEY_DIST_RESPONDER_ENC_CENTRAL | KEY_DIST_RESPONDER_ID | KEY_DIST_INITIATOR_ENC_CENTRAL | KEY_DIST_INITIATOR_ID)
#define KEY_DISTRIBUTION_PUBLIC  (KEY_DIST_RESPONDER_ENC_CENTRAL | KEY_DIST_INITIATOR_ENC_CENTRAL)

extern void HandleSmRemoteIoCapabilityInd(CL_SM_REMOTE_IO_CAPABILITY_IND_T *msg)
{
    LOGD("auth requirements : %x\n",msg->authentication_requirements);
    LOGD("io capability : %x\n",msg->io_capability);
    LOGD("oob data present : %s\n",BOOL_STR(msg->oob_data_present));
}

extern void HandleSmIoCapabilityReq(CL_SM_IO_CAPABILITY_REQ_IND_T *ind)
{
    u16 key_dist = KEY_DIST_NONE;
    bool can_pair = FALSE;
    const tp_bdaddr* remote_bdaddr = &ind->tpaddr;

    /* Check if this is for a BR/EDR device */
    /*
    if((remote_bdaddr->transport == TRANSPORT_BREDR_ACL) && (AuthCanSinkPair()))
    {
        can_pair = TRUE;
    }
    */

    /* Check if this is for a LE device */
    if ((remote_bdaddr->transport == TRANSPORT_BLE_ACL) && 1 )/*(sinkBleGapIsBondable()))*/
    {
        can_pair = TRUE;
        key_dist = (remote_bdaddr->taddr.type == TYPED_BDADDR_RANDOM) ?
            KEY_DISTRIBUTION_RANDOM : KEY_DISTRIBUTION_PUBLIC;
    }

    /* If not pairable should reject */
    if(can_pair)
    {
        cl_sm_io_capability local_io_capability = cl_sm_io_cap_no_input_no_output;
        mitm_setting sink_mitm_setting = mitm_not_required;

        LOGD("Sending IO Capability : %04x(key_dist)\n",key_dist);

        /* Send
         * Response
         * and
         * request
         * to bond
         * with
         * device
         * */
        ConnectionSmIoCapabilityResponse(&ind->tpaddr, local_io_capability, sink_mitm_setting, TRUE, key_dist, oob_data_none, NULL, NULL);
    }
    else
    {
        LOGD("Rejecting IO Capability Req \n");
        ConnectionSmIoCapabilityResponse(&ind->tpaddr, cl_sm_reject_request, mitm_not_required, FALSE, key_dist, oob_data_none, NULL, NULL);
    }

}
