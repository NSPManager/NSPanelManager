#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <Room.hpp>
#include <RoomManager.hpp>
#include <Scene.hpp>
#include <ScenePage.hpp>
#include <TftDefines.h>

void ScenePage::show() {
  PageManager::SetCurrentPage(this);
  RoomManager::attachRoomChangeCallback(this);
  NSPanel::instance->goToPage(SCENES_PAGE_NAME);
  this->update();
}

void ScenePage::update() {
  this->_updateDisplay();
}

void ScenePage::unshow() {
  RoomManager::detachRoomChangeCallback(this);
}

void ScenePage::roomChangedCallback() {
  this->_updateDisplay();
}

void ScenePage::entityUpdateCallback(DeviceEntity *entity) {
  this->_updateDisplay();
}

void ScenePage::entityDeconstructCallback(DeviceEntity *entity) {
  this->_updateDisplay();
}

void ScenePage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  if (pressed) {
    return; // We only care about when the user releases the finger from the panel
  }

  switch (component) {
  case SCENES_PAGE_BACK_BUTTON_ID: {
    PageManager::GoBack();
    break;
  }
  case SCENES_PAGE_SCENE1_LABEL_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 1) {
      (*RoomManager::currentRoom)->scenes[0]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE2_LABEL_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 2) {
      (*RoomManager::currentRoom)->scenes[1]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE3_LABEL_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 3) {
      (*RoomManager::currentRoom)->scenes[2]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE4_LABEL_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 4) {
      (*RoomManager::currentRoom)->scenes[3]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE1_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 1) {
      (*RoomManager::currentRoom)->scenes[0]->save();
      ScenePage::_setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_SCENE2_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 2) {
      (*RoomManager::currentRoom)->scenes[1]->save();
      ScenePage::_setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_SCENE3_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 3) {
      (*RoomManager::currentRoom)->scenes[2]->save();
      ScenePage::_setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_SCENE4_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 4) {
      (*RoomManager::currentRoom)->scenes[3]->save();
      ScenePage::_setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_PREVIOUS_SCENES_BUTTON_ID: {
    RoomManager::goToPreviousRoom();
    // ScenePage::showScenes((*RoomManager::currentRoom)->scenes);
    ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    break;
  }
  case SCENES_PAGE_NEXT_SCENES_BUTTON_ID: {
    RoomManager::goToNextRoom();
    // ScenePage::showScenes((*RoomManager::currentRoom)->scenes);
    ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    break;
  }
  default:
    break;
  }
}

void ScenePage::_updateDisplay() {
  this->_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());

  for (int i = 0; i < 4; i++) {
    switch (i) {
    case 0: {
      if ((*RoomManager::currentRoom)->scenes.size() >= 1) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE1_LABEL_NAME, (*RoomManager::currentRoom)->scenes[0]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", (*RoomManager::currentRoom)->scenes[0]->name.c_str(), " in slot 1");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 1: {
      if ((*RoomManager::currentRoom)->scenes.size() >= 2) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE2_LABEL_NAME, (*RoomManager::currentRoom)->scenes[1]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", (*RoomManager::currentRoom)->scenes[1]->name.c_str(), " in slot 2");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 2: {
      if ((*RoomManager::currentRoom)->scenes.size() >= 3) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE3_LABEL_NAME, (*RoomManager::currentRoom)->scenes[2]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", (*RoomManager::currentRoom)->scenes[2]->name.c_str(), " in slot 3");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 3: {
      if ((*RoomManager::currentRoom)->scenes.size() >= 4) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE4_LABEL_NAME, (*RoomManager::currentRoom)->scenes[3]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", (*RoomManager::currentRoom)->scenes[3]->name.c_str(), " in slot 1");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    default:
      break;
    }
  }
}

void ScenePage::_setRoomLabelText(const char *text) {
  NSPanel::instance->setComponentText(SCENES_PAGE_CURRENT_SCENES_LABEL_NAME, text);
}
