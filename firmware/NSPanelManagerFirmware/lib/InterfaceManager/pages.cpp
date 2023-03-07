/*
 * pages.cpp
 *
 *  Created on: Mar 6, 2023
 *      Author: Tim Panajott
 */

#include <pages.h>
#include <NSPanel.h>
#include <TftDefines.h>

int HomePage::getSaturationValue() {
	return NSPanel::instance->getComponentIntVal(HOME_SATURATION_SLIDER_NAME);
}

void HomePage::setSaturationValue(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_SATURATION_SLIDER_NAME, value);
}

int HomePage::getColorTempValue() {
	return NSPanel::instance->getComponentIntVal(HOME_COLOR_TEMP_SLIDER_NAME);
}

void HomePage::setColorTempValue(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_COLOR_TEMP_SLIDER_NAME, value);
}

void HomePage::setCeilingBrightnessLabelText(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_LABEL_CEILING_BRIGHTNESS, value);
}

void HomePage::setTableBrightnessLabelText(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_LABEL_TABLE_BRIGHTNESS, value);
}
