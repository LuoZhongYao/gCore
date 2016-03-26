/*******************************************************************************
 *  Copyright 2012-2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.5.1
 *  Application version 2.5.1.0
 *
 *  FILE
 *      gap_service.h
 *
 *  DESCRIPTION
 *      Header definitions for GAP service
 *
 ******************************************************************************/

#ifndef _GAP_SERVICE_H_
#define _GAP_SERVICE_H_

#include "service.h"
#include "user_config.h"

/* This function is used to get the reference to the 'g_device_name' 
 * array, which contains AD Type and device name
 */
extern uint8 *GapGetNameAndLength(uint16 *p_name_length);
extern void GapReadDataFromNVM(uint16 *p_offset);

extern const service_ctx_t gap_service;

#endif /* _GAP_SERVICE_H_ */
