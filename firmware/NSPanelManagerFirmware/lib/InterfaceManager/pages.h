/*
 * pages.h
 *
 *  Created on: Mar 6, 2023
 *      Author: Tim Panajott
 */

#include <Arduino.h>

#ifndef LIB_INTERFACEMANAGER_PAGES_H_
#define LIB_INTERFACEMANAGER_PAGES_H_


class HomePage {
public:
	static void setDimmingValue(uint8_t value);
	static int getDimmingValue();
	static void setColorTempValue(uint8_t value);
	static int getColorTempValue();
	static void setCeilingBrightnessLabelText(uint8_t value);
	static void setTableBrightnessLabelText(uint8_t value);
	static void setCeilingLightsState(bool state);
	static void setTableLightsState(bool state);
};


#endif /* LIB_INTERFACEMANAGER_PAGES_H_ */
