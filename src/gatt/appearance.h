/*******************************************************************************
 *  Copyright 2012-2015 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.5.1
 *  Application version 2.5.1.0
 *
 *  FILE
 *      appearance.h
 *
 *  DESCRIPTION
 *      This file defines macros for commonly used appearance values, which are 
 *      defined by BT SIG.
 *
 ******************************************************************************/

#ifndef _APPEARANCE_H_
#define _APPEARANCE_H_

/*=============================================================================*
 *         Public Definitions
 *============================================================================*/

/* Brackets should not be used around the value of a macro. The parser 
 * which creates .c and .h files from .db file doesn't understand  brackets 
 * and will raise syntax errors.
 */

/* For UUID values, refer http://developer.bluetooth.org/gatt/characteristics/
 * Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.
 * appearance.xml
 */

/* Keyboard appearance value */
#define APPEARANCE_KEYBOARD_VALUE           0x03C1

/* Digitizer Tablet appearance value */
#define APPEARANCE_TOUCH_VALUE              0x03C5

/* Tag appearance value */
#define APPEARANCE_TAG_VALUE                0x0200

#endif /* _APPEARANCE_H_ */
