/*******************************************************************************
 *  Copyright 2012-2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.5.1
 *  Application version 2.5.1.0
 *
 *  FILE
 *      hid_service.h
 *
 *  DESCRIPTION
 *      Header definitions for HID service
 *
 ******************************************************************************/

#ifndef _HID_SERVICE_H_
#define _HID_SERVICE_H_

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "user_config.h"
#include "service.h"
#include <message.h>

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Control operation values can be referred from http://developer.bluetooth.org/
 * gatt/characteristics/Pages/
 * CharacteristicViewer.aspx?u=org.bluetooth.characteristic.hid_control_point.xml
 */
/* Suspend command is sent from the host when it wants to enter power saving
 * mode. Exit suspend is used by host to resume normal operations.
 */
typedef enum
{
    hid_suspend      = 0,
    hid_exit_suspend = 1,
    hid_rfu

} hid_control_point_op;

/* Protocol mode values can be referred from http://developer.bluetooth.org/
 * gatt/characteristics/Pages/
 * CharacteristicViewer.aspx?u=org.bluetooth.characteristic.protocol_mode.xml
 */
/* When the HID host switches between boot mode and report mode, the protocol
 * mode characteristic is written with these values.
 */
typedef enum
{
    hid_boot_mode   = 0,
    hid_report_mode = 1

} hid_protocol_mode;

/*=============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function checks whether notification has been enabled on a
 * report characteristic referred by the 'report_id'
 */
extern bool HidIsNotifyEnabledOnReportId(u8 report_id);

/* This function returns the present protocol mode the mouse is operating in */
extern hid_protocol_mode GetReportMode(void);

/* This function sends reports as notifications for the report characteristics
 * of HID service
 */
extern void HidSendInputReport(Task task,u16 ucid, u8 report_id, u8 *report);

/* This function checks if the HID host has entered suspend state */
extern bool HidIsStateSuspended(void);

extern const service_ctx_t hid_service;

#endif /* _HID_SERVICE_H_ */
