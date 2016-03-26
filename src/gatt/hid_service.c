#define LOG_TAG "hid"

#include <gatt.h>
#include <stdlib.h>
#include <memory.h>
#include <utils/log.h>

#include "hid_service.h"
#include "app_gatt_db.h"

#ifdef  BLE_LOG
#   define  LOGD    DLOG
#   define  LOGE    ELOG
#   define  LOGW    WLOG
#else
#   define LOGD(...)
#   define LOGW LOGD
#   define LOGE LOGD
#endif

static struct
{
    /* INPUT report (BOOT or Interrupt) notification handle as configured 
     * by peer host device.
     */
    u16                  input_report_handle;

    /* Input Report Client Configuration */
    gatt_client_config      input_client_config;

    /* Input Boot Report Client Configuration */
    gatt_client_config      input_boot_client_config;

    /* HID protocol mode is hid_report_mode mode by default */
    hid_protocol_mode       report_mode;

    /* Set to TRUE if the HID device is suspended. By default set to FALSE (ie., 
     * Not Suspended)
     */
    bool                    suspended;

    /* This flag is set if the notifications for Boot or Report mode input 
     * reports are enabled and the corresponding input reporting mode is set
     */
    bool                    notify_enable;

} self;


static void updateInputReportConfiguration(gatt_client_config config,u16 handle);
static void handleControlPointUpdate(hid_control_point_op control_op);

static const u8 hid_report_map[] = {
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
    0x09, 0x04,                         // USAGE (Touch Screen)
    0xa1, 0x01,                         // COLLECTION (Application)
    0x85, 0x01,                         //   REPORT_ID (Touch)
    0x09, 0x22,                         //   USAGE (Finger)
    0xa1, 0x02,                         //   COLLECTION (Logical)
    0x09, 0x42,                         //     USAGE (Tip Switch)
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                         //     REPORT_SIZE (1)
    0x95, 0x01,                         //     REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x32,                         //     USAGE (In Range)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x95, 0x06,                         //     REPORT_COUNT (6)
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x75, 0x08,                         //     REPORT_SIZE (8)
    0x09, 0x51,                         //     USAGE ( Temp Identifier)
    0x95, 0x01,                         //     REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x05, 0x01,                         //     USAGE_PAGE (Generic Desk..
    0x26, 0xc0, 0x03,                   //     LOGICAL_MAXIMUM (4095)
    0x75, 0x10,                         //     REPORT_SIZE (16)
    0x55, 0x0e,                         //     UNIT_EXPONENT (-2)
    0x65, 0x13,                         //     UNIT (Inch,EngLinear)
    0x09, 0x30,                         //     USAGE (X)
    0x35, 0x00,                         //     PHYSICAL_MINIMUM (0)
    0x46, 0x80, 0x02,                   //     PHYSICAL_MAXIMUM (1080)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x46, 0xc0, 0x03,                   //     PHYSICAL_MAXIMUM (1920)
    0x09, 0x31,                         //     USAGE (Y)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0xc0,                               //   END_COLLECTION
    0xa1, 0x02,                         //   COLLECTION (Logical)
    0x05, 0x0d,                         //     USAGE_PAGE (Digitizers)
    0x09, 0x42,                         //     USAGE (Tip Switch)
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                         //     REPORT_SIZE (1)
    0x95, 0x01,                         //     REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x32,                         //     USAGE (In Range)
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)
    0x95, 0x06,                         //     REPORT_COUNT (6)
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x75, 0x08,                         //     REPORT_SIZE (8)
    0x09, 0x51,                         //     USAGE ( Temp Identifier)
    0x95, 0x01,                         //     REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x05, 0x01,                         //     USAGE_PAGE (Generic Desk..
    0x26, 0xc0, 0x03,                   //     LOGICAL_MAXIMUM (4095)
    0x75, 0x10,                         //     REPORT_SIZE (16)
    0x55, 0x0e,                         //     UNIT_EXPONENT (-2)
    0x65, 0x13,                         //     UNIT (Inch,EngLinear)
    0x09, 0x30,                         //     USAGE (X)
    0x35, 0x00,                         //     PHYSICAL_MINIMUM (0)
    0x46, 0x80, 0x02,                   //     PHYSICAL_MAXIMUM (1080)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x46, 0xc0, 0x03,                   //     PHYSICAL_MAXIMUM (1920)
    0x09, 0x31,                         //     USAGE (Y)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0xc0,                               //   END_COLLECTION
    0x05, 0x0d,                         //   USAGE_PAGE (Digitizers)
    0x09, 0x54,                         //   USAGE (Actual count)
    0x95, 0x01,                         //   REPORT_COUNT (1)
    0x75, 0x08,                         //   REPORT_SIZE (8)
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)
    0x85, 0x02,                         //   REPORT_ID (Feature)
    0x09, 0x55,                         //   USAGE(Maximum Count)
    0x25, 0x02,                         //   LOGICAL_MAXIMUM (2)
    0xb1, 0x02,                         //   FEATURE (Data,Var,Abs)
    0xc0,                               // END_COLLECTION
    0x09, 0x0E,                         // USAGE (Configuration)
    0xa1, 0x01,                         // COLLECTION (Application)
    0x85, 0x03,                         //   REPORT_ID (Feature)
    0x09, 0x22,                         //   USAGE (Finger)
    0xa1, 0x00,                         //   COLLECTION (physical)
    0x09, 0x52,                         //     USAGE (Input Mode)
    0x09, 0x53,                         //     USAGE (Device Index
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)
    0x25, 0x0a,                         //     LOGICAL_MAXIMUM (10)
    0x75, 0x08,                         //     REPORT_SIZE (8)
    0x95, 0x02,                         //     REPORT_COUNT (2)
    0xb1, 0x02,                         //     FEATURE (Data,Var,Abs)
    0xc0,                               //   END_COLLECTION
    0xc0                               // END_COLLECTION
};
static const u8 hid_information[] = {HID_FLAG_CLASS_SPEC_RELEASE_L,HID_FLAG_CLASS_SPEC_RELEASE_H,HID_FLAG_COUNTRY_CODE,HID_INFO_FLAGS};

/*-----------------------------------------------------------------------------*
 *  NAME
 *      updateInputReportConfiguration
 *
 *  DESCRIPTION
 *      This function is used to update client configurations.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void updateInputReportConfiguration(gatt_client_config config, u16 handle)
{
    /* Whenever CCCD of input report or boot input report are written, the
     * input report handle and notification status of input report need to be
     * updated as single variable is being maintained for both boot and report
     * protocol modes.
     */
    if(config & gatt_client_config_notification)
    {
        self.input_report_handle = handle;
        self.notify_enable = TRUE;
    }
    else
    {
        self.notify_enable = FALSE;
    }
}    

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleControlPointUpdate
 *
 *  DESCRIPTION
 *      This function is used to handle HID Control Point updates
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void handleControlPointUpdate(hid_control_point_op control_op)
{

    switch(control_op)
    {
        case hid_suspend:
        {
             self.suspended = TRUE;

             /* Host has suspended its operations, application may like 
              * to do a low frequency key scan. The sample application is 
              * not doing any thing special in this case other than not sending
              * a connection parameter update request if the remote host is
              * currently suspended.
              */
        }
        break;

        case hid_exit_suspend:
        {
             self.suspended = FALSE;

             /* Host has exited suspended mode, application may like 
              * to do a normal frequency key scan. The sample application is 
              * not doing any thing special in this case.
              */
        }
        break;

        default:
        {
            /* Ignore invalid value */
        }
        break;
    }

}

/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HidDataInit
 *
 *  DESCRIPTION
 *      This function is used to initialise HID service data 
 *      structure.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/

static void hid_service_init(void)
{
    self.input_report_handle      = 0xFFFF;
    self.notify_enable            = FALSE;
    self.input_client_config      = gatt_client_config_none;
    self.input_boot_client_config = gatt_client_config_none;
    self.report_mode              = hid_report_mode;
    self.suspended                = FALSE;
}

static void hid_service_reset(void)
{
    hid_service_init();
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HidInitChipReset
 *
 *  DESCRIPTION
 *      This function is used to initialise HID service data 
 *      structure at chip reset
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/

static void hid_service_deinit(void)
{
    /* HID service specific things that need to be done upon a power reset. */
    bzero((char*)&self,sizeof(self));
}


static void hid_service_handle_read(GATT_ACCESS_IND_T *ind)
{
    switch(ind->handle) {

    case HANDLE_HID_INFORMATION : {
        GattAccessResponse(ind->cid,ind->handle,gatt_status_success,sizeof(hid_information),hid_information);
    } break;

    case HANDLE_HID_REPORT_MAP: {
        u16 size = sizeof(hid_report_map) - ind->offset;
        if(size > 0x16) {
            size = 0x16;
        }
        GattAccessResponse(ind->cid,ind->handle,gatt_status_success,size,hid_report_map + ind->offset);
    } break;

    case HANDLE_HID_INPUT_RPT_CLIENT_CONFIG: {
        u8 val[2];
        val[0] = self.input_client_config & 0xff;
        val[1] = self.input_client_config >> 8;
        GattAccessResponse(ind->cid,
                           ind->handle,
                           gatt_status_success,
                           2,
                           val);
    } break;

    case HANDLE_HID_INPUT_REPORT: {
        NOT_IMPL();
        GattAccessResponse(ind->cid,
                           ind->handle,
                           gatt_status_read_not_permitted,
                           0,
                           NULL);
    } break;

    case HANDLE_HID_PROTOCOL_MODE: {
        GattAccessResponse(ind->cid,
                           ind->handle,
                           gatt_status_success,
                           1,
                           (const u8*)&self.report_mode);
    } break;

    case HANDLE_HID_FEATURE_REPORT: {
        NOT_IMPL();
        GattAccessResponse(ind->cid,
                           ind->handle,
                           gatt_status_read_not_permitted,
                           0,
                           NULL);

    } break;

    default: {
        GattAccessResponse(ind->cid,
                           ind->handle,
                           gatt_status_read_not_permitted,
                           0,
                           NULL);
    } break;
    }


}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HidHandleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles Write operation on HID service attributes 
 *      maintained by the application.and responds with the GATT_ACCESS_RSP 
 *      message.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/

static void hid_service_handle_write(GATT_ACCESS_IND_T *p_ind)
{
    u16 client_config, input_rpt_hndl = HANDLE_HID_INPUT_REPORT;
    gatt_status_t rc = gatt_status_success;
    bool update_input_report_config = FALSE;

    switch(p_ind->handle) {

    case HANDLE_HID_INPUT_RPT_CLIENT_CONFIG: {
        /* Copy the value being written by the remote device to a local
         * variable
         */
        client_config = p_ind->value[0]  | p_ind->value[1] << 8;


        /* Client Configuration is bit field value so ideally bitwise 
         * comparison should be used but since the application supports only 
         * notifications, direct comparison is being used.
         */
        if((client_config == gatt_client_config_notification) ||
           (client_config == gatt_client_config_none))
        {
            /* CCCD of input report and boot input report have different
             * offsets at which their values are written into NVM
             */
            self.input_client_config = client_config;

            /* If Report mode set, update Input Report Configuration 
             * for Report mode
             */
            if(self.report_mode)
            {
                update_input_report_config = TRUE;
                input_rpt_hndl = HANDLE_HID_INPUT_REPORT;
            }

            /* Device shall only trigger Input reports (boot / main) if 
             * notifications are enabled for characteristic corresponding
             * to the current protocol mode.
             */
            if(update_input_report_config)
            {
                updateInputReportConfiguration(client_config, 
                                               input_rpt_hndl);
            }
        }
        else
        {
            /* INDICATION or RESERVED */

            /* Return Error as only Notifications are supported for 
             * mouse application
             */

            rc = gatt_status_application_error;
        }
    } break;

    case HANDLE_HID_CONTROL_POINT: {
        /* Copy the value being written by the remote device to a local
         * variable.
         */

        handleControlPointUpdate(p_ind->value[0]);
    } break;

    case HANDLE_HID_PROTOCOL_MODE: {
        /* Copy the value being written by the remote device to a local
         * variable.
         */
        u8 mode = p_ind->value[0];
        /* Update the protocol mode only if the value being written is not
         * the same as the already existing protocol mode.
         */
        hid_protocol_mode old_mode = self.report_mode;

        if(((mode == hid_boot_mode) ||
            (mode == hid_report_mode)) &&
           (mode != old_mode)) /* Change of Protocol Mode */
        {
            {
                client_config = self.input_client_config;
                input_rpt_hndl = HANDLE_HID_INPUT_REPORT;
            }

            self.report_mode = mode;

            /* After the protocol mode is updated, update the service
             * notification status from the notifications
             * on CCCD of the respective mode handles.
             */
            updateInputReportConfiguration(client_config,
                                           input_rpt_hndl);

        } /* Else Ignore the value */
    } break;

    case HANDLE_HID_FEATURE_REPORT: {
        /* Copy the value being written by the remote device to a local
         * variable.
         */
# if 0
        sensor_resolution resolution = BufReadu8(&p_value);

        /* Update the mouse resolution. This is an example feature report.
         * Customers can define vendor specific feature reports and
         * implement their handling as per the requirements
         */
        if(!SetSensorResolution(resolution))
        {
            rc = gatt_status_att_val_oor;
        }
#else
        NOT_IMPL();
#endif
    } break;

    default: {
        /* No more IRQ characteristics in HID service */
        rc = gatt_status_write_not_permitted;
    } break;
    }

    /* Send ACCESS RESPONSE */
    GattAccessResponse(p_ind->cid, p_ind->handle, rc, 0, NULL);
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HidIsNotifyEnabledOnReportId
 *
 *  DESCRIPTION
 *      This function returns whether notifications are enabled on CCCD of
 *      reports specified by 'report_id'.
 *
 *  RETURNS
 *      TRUE/FALSE: Notification configured or not
 *
 *----------------------------------------------------------------------------*/

extern bool HidIsNotifyEnabledOnReportId(u8 report_id)
{
    switch(report_id)
    {
        case HID_INPUT_REPORT_ID: /* whether notifications are enabled on boot
                                   *  input or input report handle.
                                   */
            return self.notify_enable;

        default:
            return FALSE;
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      GetReportMode
 *
 *  DESCRIPTION
 *      This function returns the present protocol mode the mouse is operating
 *      in.
 *
 *  RETURNS
 *      Present protocol mode.
 *
 *---------------------------------------------------------------------------*/

extern hid_protocol_mode GetReportMode(void)
{
    return self.report_mode;
}

static bool hid_service_check_handle_range(u16 handle)
{
    return ((handle >= HANDLE_HID_SERVICE) &&
            (handle <= HANDLE_HID_SERVICE_END))
            ? TRUE : FALSE;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      HidIsStateSuspended
 *
 *  DESCRIPTION
 *      This function is used to check if the HID host has entered suspend state
 *
 *  RETURNS
 *      Boolean - TRUE if the remote device is in suspended state.
 *
 *----------------------------------------------------------------------------*/
extern bool HidIsStateSuspended(void)
{
    return self.suspended;
}


const service_ctx_t hid_service = {
    .Init               = hid_service_init,  
    .Reset              = hid_service_reset,
    .DeInit             = hid_service_deinit,
    .CheckHandleRange   = hid_service_check_handle_range,
    .Read               = hid_service_handle_read,
    .Write              = hid_service_handle_write,
};

