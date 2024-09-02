#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <RoomPage.hpp>
#include <TftDefines.h>

void RoomPage::show() {
  this->_selectedLight = nullptr;
  PageManager::SetCurrentPage(this);
  NSPanel::instance->goToPage(ROOM_PAGE_NAME);
  this->update();

  if (RoomManager::hasValidCurrentRoom()) {
    // TODO: Listen to room state change instead
    // for (int i = 0; i < 12; i++) {
    //   Light *displayLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(i + 1);
    //   if (displayLight != nullptr) {
    //     displayLight->attachUpdateCallback(this);
    //     displayLight->attachDeconstructCallback(this);
    //   }
    // }
  }
}

void RoomPage::update() {
  if (RoomManager::hasValidCurrentRoom()) {
    this->setCurrentRoomLabel((*RoomManager::currentRoom)->name.c_str());
    for (int i = 0; i < 12; i++) {
      NSPanelLightStatus *displayLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(i + 1);

      if (displayLight != nullptr) {
        // Add two spaces to the left of the name before sending name to panel
        // See issue #22
        std::string display_name = "  ";
        display_name.append(displayLight->name);

        this->setLightName(displayLight->room_view_position, display_name.c_str());
        this->setLightState(displayLight->room_view_position, displayLight->light_level > 0);
        this->setLightVisibility(displayLight->room_view_position, true);
      } else {
        this->setLightVisibility(i + 1, false); // If no light was found, hide the position
      }
    }
  }
}

void RoomPage::unshow() {
  if (RoomManager::hasValidCurrentRoom()) {
    // TODO: Detach room callback
    // for (int i = 0; i < 12; i++) {
    //   Light *displayLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(i + 1);
    //   if (displayLight != nullptr) {
    //     displayLight->detachUpdateCallback(this);
    //     displayLight->detachDeconstructCallback(this);
    //   }
    // }
  }
}

void RoomPage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  switch (component) {
  case ROOM_PAGE_BACK_BUTTON_ID:
    // NSPanel::instance->goToPage(HOME_PAGE_NAME);
    PageManager::GetHomePage()->show();
    break;
  case ROOM_PAGE_PREVIOUS_ROOM_BUTTON_ID:
    RoomManager::goToPreviousRoom();
    this->update();
    break;
  case ROOM_PAGE_NEXT_ROOM_BUTTON_ID:
    RoomManager::goToNextRoom();
    this->update();
    break;
  case ROOM_LIGHT1_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(1);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(1, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT2_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(2);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(2, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT3_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(3);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(3, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT4_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(4);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(4, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT5_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(5);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(4, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT6_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(6);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(6, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT7_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(7);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(7, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT8_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(8);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(8, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT9_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(9);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(9, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT10_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(10);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(10, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT11_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(11);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(11, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT12_SW_CAP_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(12);
      if (this->_selectedLight != nullptr) {
        this->_toggleSelectedLight();
        RoomPage::setLightState(12, this->_selectedLight->light_level > 0);
      }
    }
    break;
  }
  case ROOM_LIGHT1_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(1);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT2_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(2);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT3_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(3);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT4_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(4);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT5_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(5);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT6_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(6);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT7_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(7);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT8_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(8);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT9_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(9);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT10_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(10);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT11_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(11);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }
  case ROOM_LIGHT12_LABEL_ID: {
    if (RoomManager::hasValidCurrentRoom()) {
      this->_selectedLight = (*RoomManager::currentRoom)->getLightAtRoomViewPosition(12);
      if (this->_selectedLight != nullptr) {
        PageManager::GetLightPage()->selectedLight = this->_selectedLight;
        PageManager::GetLightPage()->show();
        PageManager::GetLightPage()->updateValues();
      }
    }
    break;
  }

  default:
    LOG_ERROR("Unknown component touched on room view: ", component);
    break;
  }
}

void RoomPage::_toggleSelectedLight() {
  if (this->_selectedLight != nullptr) {
    // TODO: Implement with protobuf
    // std::list<Light *> lightsToChange;
    // lightsToChange.push_back(this->_selectedLight);
    // if (this->_selectedLight->getLightLevel() == 0) {
    //   int dim_to_level = PageManager::GetHomePage()->getDimmingValue();
    //   if (dim_to_level == 0) {
    //     LOG_INFO("Trying to turn on a light but the current average room level is 0. Defaulting to 50%");
    //     dim_to_level = 50;
    //   }
    //   LightManager::ChangeLightsToLevel(&lightsToChange, dim_to_level);
    // } else {
    //   LightManager::ChangeLightsToLevel(&lightsToChange, 0);
    // }
    this->update();
  }
}

void RoomPage::entityUpdateCallback(DeviceEntity *entity) {
  this->update();
}

void RoomPage::entityDeconstructCallback(DeviceEntity *entity) {
  this->update();
}

void RoomPage::roomChangedCallback() {
  this->update();
}

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
