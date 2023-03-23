/*
 * pages.cpp
 *
 *  Created on: Mar 6, 2023
 *      Author: Tim Panajott
 */

#include <pages.h>
#include <NSPanel.h>
#include <TftDefines.h>

int HomePage::getDimmingValue() {
	int newValue = NSPanel::instance->getComponentIntVal(HOME_DIMMER_SLIDER_NAME);
	// TODO: Make configurable
	if(newValue > 95) {
		return 100;
	} else {
		return newValue;
	}
}

void HomePage::setDimmingValue(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_DIMMER_SLIDER_NAME, value);
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

void HomePage::setCeilingLightsState(bool state) {
	NSPanel::instance->setComponentVal(HOME_BUTTON_CEILING_NAME, state ? 1 : 0);
}

void HomePage::setTableLightsState(bool state) {
	NSPanel::instance->setComponentVal(HOME_BUTTON_TABLE_NAME, state ? 1 : 0);
}

void HomePage::setSliderLightLevelColor(uint color) {
	NSPanel::instance->setComponentForegroundColor(HOME_DIMMER_SLIDER_NAME, color);
}

void HomePage::setSliderColorTempColor(uint color) {
	NSPanel::instance->setComponentForegroundColor(HOME_COLOR_TEMP_SLIDER_NAME, color);
}

void HomePage::setHighlightCeilingVisibility(bool visibility) {
	NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_CEILING_NAME, visibility);
}

void HomePage::setHighlightTableVisibility(bool visibility) {
	NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_TABLE_NAME, visibility);
}