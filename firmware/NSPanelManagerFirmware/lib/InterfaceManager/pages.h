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
	static void setSaturationValue(uint8_t value);
	static int getSaturationValue();
	static void setColorTempValue(uint8_t value);
	static int getColorTempValue();
};


#endif /* LIB_INTERFACEMANAGER_PAGES_H_ */
