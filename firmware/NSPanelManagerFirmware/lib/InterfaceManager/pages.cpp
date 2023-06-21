#include <InterfaceConfig.hpp>
#include <Light.hpp>
#include <MqttLog.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <Scene.hpp>
#include <TftDefines.h>
#include <pages.hpp>

void RoomPage::setLightVisibility(uint8_t position, bool visibility) {
  switch (position) {
  case 1:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT1_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT1_SW_NAME, visibility);
    break;
  case 2:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT2_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT2_SW_NAME, visibility);
    break;
  case 3:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT3_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT3_SW_NAME, visibility);
    break;
  case 4:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT4_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT4_SW_NAME, visibility);
    break;
  case 5:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT5_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT5_SW_NAME, visibility);
    break;
  case 6:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT6_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT6_SW_NAME, visibility);
    break;
  case 7:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT7_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT7_SW_NAME, visibility);
    break;
  case 8:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT8_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT8_SW_NAME, visibility);
    break;
  case 9:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT9_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT9_SW_NAME, visibility);
    break;
  case 10:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT10_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT10_SW_NAME, visibility);
    break;
  case 11:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT11_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT11_SW_NAME, visibility);
    break;
  case 12:
    NSPanel::instance->setComponentVisible(ROOM_LIGHT12_LABEL_NAME, visibility);
    NSPanel::instance->setComponentVisible(ROOM_LIGHT12_SW_NAME, visibility);
    break;

  default:
    LOG_ERROR("Trying to set visibility of room light that doesn't exist");
    break;
  }
}

void RoomPage::setLightName(uint8_t position, const char *name) {
  switch (position) {
  case 1:
    NSPanel::instance->setComponentText(ROOM_LIGHT1_LABEL_NAME, name);
    break;
  case 2:
    NSPanel::instance->setComponentText(ROOM_LIGHT2_LABEL_NAME, name);
    break;
  case 3:
    NSPanel::instance->setComponentText(ROOM_LIGHT3_LABEL_NAME, name);
    break;
  case 4:
    NSPanel::instance->setComponentText(ROOM_LIGHT4_LABEL_NAME, name);
    break;
  case 5:
    NSPanel::instance->setComponentText(ROOM_LIGHT5_LABEL_NAME, name);
    break;
  case 6:
    NSPanel::instance->setComponentText(ROOM_LIGHT6_LABEL_NAME, name);
    break;
  case 7:
    NSPanel::instance->setComponentText(ROOM_LIGHT7_LABEL_NAME, name);
    break;
  case 8:
    NSPanel::instance->setComponentText(ROOM_LIGHT8_LABEL_NAME, name);
    break;
  case 9:
    NSPanel::instance->setComponentText(ROOM_LIGHT9_LABEL_NAME, name);
    break;
  case 10:
    NSPanel::instance->setComponentText(ROOM_LIGHT10_LABEL_NAME, name);
    break;
  case 11:
    NSPanel::instance->setComponentText(ROOM_LIGHT11_LABEL_NAME, name);
    break;
  case 12:
    NSPanel::instance->setComponentText(ROOM_LIGHT12_LABEL_NAME, name);
    break;

  default:
    LOG_ERROR("Trying to set name of room light that doesn't exist");
    break;
  }
}

void RoomPage::setLightState(uint8_t position, bool state) {
  switch (position) {
  case 1:
    NSPanel::instance->setComponentVal(ROOM_LIGHT1_SW_NAME, state ? 1 : 0);
    break;
  case 2:
    NSPanel::instance->setComponentVal(ROOM_LIGHT2_SW_NAME, state ? 1 : 0);
    break;
  case 3:
    NSPanel::instance->setComponentVal(ROOM_LIGHT3_SW_NAME, state ? 1 : 0);
    break;
  case 4:
    NSPanel::instance->setComponentVal(ROOM_LIGHT4_SW_NAME, state ? 1 : 0);
    break;
  case 5:
    NSPanel::instance->setComponentVal(ROOM_LIGHT5_SW_NAME, state ? 1 : 0);
    break;
  case 6:
    NSPanel::instance->setComponentVal(ROOM_LIGHT6_SW_NAME, state ? 1 : 0);
    break;
  case 7:
    NSPanel::instance->setComponentVal(ROOM_LIGHT7_SW_NAME, state ? 1 : 0);
    break;
  case 8:
    NSPanel::instance->setComponentVal(ROOM_LIGHT8_SW_NAME, state ? 1 : 0);
    break;
  case 9:
    NSPanel::instance->setComponentVal(ROOM_LIGHT9_SW_NAME, state ? 1 : 0);
    break;
  case 10:
    NSPanel::instance->setComponentVal(ROOM_LIGHT10_SW_NAME, state ? 1 : 0);
    break;
  case 11:
    NSPanel::instance->setComponentVal(ROOM_LIGHT11_SW_NAME, state ? 1 : 0);
    break;
  case 12:
    NSPanel::instance->setComponentVal(ROOM_LIGHT12_SW_NAME, state ? 1 : 0);
    break;

  default:
    LOG_ERROR("Trying to set state of room light that doesn't exist");
    break;
  }
}

void RoomPage::setCurrentRoomLabel(const char *label) {
  NSPanel::instance->setComponentText(ROOM_PAGE_CURRENT_ROOM_LABEL_NAME, label);
}
