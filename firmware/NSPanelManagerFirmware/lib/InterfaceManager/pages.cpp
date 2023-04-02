/*
 * pages.cpp
 *
 *  Created on: Mar 6, 2023
 *      Author: Tim Panajott
 */

#include <pages.h>
#include <NSPanel.h>
#include <TftDefines.h>
#include <InterfaceManager.h>
#include <MqttLog.h>

int HomePage::getDimmingValue() {
	int newValue = NSPanel::instance->getComponentIntVal(HOME_DIMMER_SLIDER_NAME);
	if(newValue > InterfaceManager::instance->config.raiseToMaxLightLevelAbove) {
		return 100;
	} else {
		return newValue;
	}
}

void HomePage::setDimmingValue(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_DIMMER_SLIDER_NAME, value);
}

int HomePage::getColorTempValue() {
	return NSPanel::instance->getComponentIntVal(HOME_LIGHT_COLOR_SLIDER_NAME);
}

void HomePage::setColorTempValue(uint8_t value) {
	NSPanel::instance->setComponentVal(HOME_LIGHT_COLOR_SLIDER_NAME, value);
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
	NSPanel::instance->setComponentForegroundColor(HOME_LIGHT_COLOR_SLIDER_NAME, color);
}

void HomePage::setHighlightCeilingVisibility(bool visibility) {
	NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_CEILING_NAME, visibility);
}

void HomePage::setHighlightTableVisibility(bool visibility) {
	NSPanel::instance->setComponentVisible(HOME_PIC_HIGHLIGHT_TABLE_NAME, visibility);
}


void RoomPage::setLightVisibility(uint8_t position, bool visibility) {
	LOG_DEBUG("Setting pos ", position, " visibility to: ", visibility);
	switch (position)
	{
	case 1:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT1_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT1_SW_NAME, visibility);
		break;
	case 2:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT2_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT2_SW_NAME, visibility);
	case 3:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT3_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT3_SW_NAME, visibility);
	case 4:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT4_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT4_SW_NAME, visibility);
	case 5:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT5_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT5_SW_NAME, visibility);
	case 6:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT6_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT6_SW_NAME, visibility);
	case 7:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT7_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT7_SW_NAME, visibility);
	case 8:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT8_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT8_SW_NAME, visibility);
	case 9:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT9_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT9_SW_NAME, visibility);
	case 10:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT10_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT10_SW_NAME, visibility);
	case 11:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT11_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT11_SW_NAME, visibility);
	case 12:
		NSPanel::instance->setComponentVisible(ROOM_LIGHT12_LABEL_NAME, visibility);
		NSPanel::instance->setComponentVisible(ROOM_LIGHT12_SW_NAME, visibility);
	
	default:
		LOG_ERROR("Trying to set visibility of room light that doesn't exist");
		break;
	}
}

void RoomPage::setLightName(uint8_t position, const char* name) {
	switch (position)
	{
	case 1:
		NSPanel::instance->setComponentText(ROOM_LIGHT1_LABEL_NAME, name);
		break;
	case 2:
		NSPanel::instance->setComponentText(ROOM_LIGHT2_LABEL_NAME, name);
	case 3:
		NSPanel::instance->setComponentText(ROOM_LIGHT3_LABEL_NAME, name);
	case 4:
		NSPanel::instance->setComponentText(ROOM_LIGHT4_LABEL_NAME, name);
	case 5:
		NSPanel::instance->setComponentText(ROOM_LIGHT5_LABEL_NAME, name);
	case 6:
		NSPanel::instance->setComponentText(ROOM_LIGHT6_LABEL_NAME, name);
	case 7:
		NSPanel::instance->setComponentText(ROOM_LIGHT7_LABEL_NAME, name);
	case 8:
		NSPanel::instance->setComponentText(ROOM_LIGHT8_LABEL_NAME, name);
	case 9:
		NSPanel::instance->setComponentText(ROOM_LIGHT9_LABEL_NAME, name);
	case 10:
		NSPanel::instance->setComponentText(ROOM_LIGHT10_LABEL_NAME, name);
	case 11:
		NSPanel::instance->setComponentText(ROOM_LIGHT11_LABEL_NAME, name);
	case 12:
		NSPanel::instance->setComponentText(ROOM_LIGHT12_LABEL_NAME, name);
	
	default:
		LOG_ERROR("Trying to set name of room light that doesn't exist");
		break;
	}
}

void RoomPage::setLightState(uint8_t position, bool state) {
	switch (position)
	{
	case 1:
		NSPanel::instance->setComponentVal(ROOM_LIGHT1_SW_NAME, state ? 1 : 0);
		break;
	case 2:
		NSPanel::instance->setComponentVal(ROOM_LIGHT2_SW_NAME, state ? 1 : 0);
	case 3:
		NSPanel::instance->setComponentVal(ROOM_LIGHT3_SW_NAME, state ? 1 : 0);
	case 4:
		NSPanel::instance->setComponentVal(ROOM_LIGHT4_SW_NAME, state ? 1 : 0);
	case 5:
		NSPanel::instance->setComponentVal(ROOM_LIGHT5_SW_NAME, state ? 1 : 0);
	case 6:
		NSPanel::instance->setComponentVal(ROOM_LIGHT6_SW_NAME, state ? 1 : 0);
	case 7:
		NSPanel::instance->setComponentVal(ROOM_LIGHT7_SW_NAME, state ? 1 : 0);
	case 8:
		NSPanel::instance->setComponentVal(ROOM_LIGHT8_SW_NAME, state ? 1 : 0);
	case 9:
		NSPanel::instance->setComponentVal(ROOM_LIGHT9_SW_NAME, state ? 1 : 0);
	case 10:
		NSPanel::instance->setComponentVal(ROOM_LIGHT10_SW_NAME, state ? 1 : 0);
	case 11:
		NSPanel::instance->setComponentVal(ROOM_LIGHT11_SW_NAME, state ? 1 : 0);
	case 12:
		NSPanel::instance->setComponentVal(ROOM_LIGHT12_SW_NAME, state ? 1 : 0);
	
	default:
		LOG_ERROR("Trying to set state of room light that doesn't exist");
		break;
	}
}