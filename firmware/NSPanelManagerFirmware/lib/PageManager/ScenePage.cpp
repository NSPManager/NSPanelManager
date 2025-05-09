#include <InterfaceConfig.hpp>
#include <NSPanel.hpp>
#include <PageManager.hpp>
#include <RoomManager.hpp>
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

void ScenePage::doSceneSaveProgress(void *param) {
  unsigned long countStarted = millis();
  NSPanel::instance->setComponentVisible(SCENES_PAGE_SAVE_SLIDER_NAME, true);
  uint8_t lastSaveProgress = 255;
  while (millis() - countStarted < 3000 && ScenePage::_doSceneSaveProgress) {
    uint8_t saveProgress = (millis() - countStarted) / 30;
    if (saveProgress > 100) {
      saveProgress = 100;
    }
    if (saveProgress != lastSaveProgress) {
      NSPanel::instance->setComponentVal(SCENES_PAGE_SAVE_SLIDER_NAME, saveProgress);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  if (ScenePage::_doSceneSaveProgress) {
    // Finger is still held at save button after timeout. Save the page.
    if (ScenePage::_sceneToSave != nullptr) {
      // ScenePage::_sceneToSave->save(); // TODO: Save scene command
      PageManager::GetScenePage()->_setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }

  if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
    PageManager::GetScenePage()->_setRoomLabelText(RoomManager::currentRoom->name());
  } else {
    PageManager::GetScenePage()->_setRoomLabelText("Global Scenes");
  }
  NSPanel::instance->setComponentVisible(SCENES_PAGE_SAVE_SLIDER_NAME, false);
  vTaskDelete(NULL);
}

void ScenePage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  std::vector<PROTOBUF_NSPANEL_SCENE *> scenes;
  if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
    for (int i = 0; i < RoomManager::currentRoom->scenes().get_length(); i++) {
      scenes.push_back(&(RoomManager::currentRoom->mutable_scenes()[i]));
    }
  } else {
    for (int i = 0; i < InterfaceConfig::global_scenes.size(); i++) {
      scenes.push_back(InterfaceConfig::global_scenes[i]);
    }
  }

  if (pressed) {
    switch (component) {
    case SCENES_PAGE_SCENE1_SAVE_BUTTON_ID: {
      if (scenes.size() >= 1) {
        ScenePage::_sceneToSave = scenes[0];
        ScenePage::_doSceneSaveProgress = true;
        xTaskCreatePinnedToCore(doSceneSaveProgress, "taskSceneSave", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
      }
      break;
    }
    case SCENES_PAGE_SCENE2_SAVE_BUTTON_ID: {
      if (scenes.size() >= 2) {
        ScenePage::_sceneToSave = scenes[1];
        ScenePage::_doSceneSaveProgress = true;
        xTaskCreatePinnedToCore(doSceneSaveProgress, "taskSceneSave", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
      }
      break;
    }
    case SCENES_PAGE_SCENE3_SAVE_BUTTON_ID: {
      if (scenes.size() >= 3) {
        ScenePage::_sceneToSave = scenes[2];
        ScenePage::_doSceneSaveProgress = true;
        xTaskCreatePinnedToCore(doSceneSaveProgress, "taskSceneSave", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
      }
      break;
    }
    case SCENES_PAGE_SCENE4_SAVE_BUTTON_ID: {
      if (scenes.size() >= 4) {
        ScenePage::_sceneToSave = scenes[3];
        ScenePage::_doSceneSaveProgress = true;
        xTaskCreatePinnedToCore(doSceneSaveProgress, "taskSceneSave", 5000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
      }
      break;
    }
    default:
      break;
    }
    return;
  }

  // Release events
  switch (component) {
  case SCENES_PAGE_BACK_BUTTON_ID: {
    PageManager::GetHomePage()->show();
    break;
  }
  case SCENES_PAGE_SCENE1_LABEL_ID: {
    if (scenes.size() >= 1) {
      // scenes[0]->activate(); // TODO: Activate scene command
    }
    break;
  }
  case SCENES_PAGE_SCENE2_LABEL_ID: {
    if (scenes.size() >= 2) {
      // scenes[1]->activate(); // TODO: Activate scene command
    }
    break;
  }
  case SCENES_PAGE_SCENE3_LABEL_ID: {
    if (scenes.size() >= 3) {
      // scenes[2]->activate(); // TODO: Activate scene command
    }
    break;
  }
  case SCENES_PAGE_SCENE4_LABEL_ID: {
    if (scenes.size() >= 4) {
      // scenes[3]->activate(); // TODO: Activate scene command
    }
    break;
  }
  case SCENES_PAGE_SCENE1_SAVE_BUTTON_ID: {
    if (scenes.size() >= 1) {
      ScenePage::_sceneToSave = nullptr;
      ScenePage::_doSceneSaveProgress = false;
    }
    break;
  }
  case SCENES_PAGE_SCENE2_SAVE_BUTTON_ID: {
    if (scenes.size() >= 2) {
      ScenePage::_sceneToSave = nullptr;
      ScenePage::_doSceneSaveProgress = false;
    }
    break;
  }
  case SCENES_PAGE_SCENE3_SAVE_BUTTON_ID: {
    if (scenes.size() >= 3) {
      ScenePage::_sceneToSave = nullptr;
      ScenePage::_doSceneSaveProgress = false;
    }
    break;
  }
  case SCENES_PAGE_SCENE4_SAVE_BUTTON_ID: {
    if (scenes.size() >= 4) {
      ScenePage::_sceneToSave = nullptr;
      ScenePage::_doSceneSaveProgress = false;
    }
    break;
  }
  case SCENES_PAGE_PREVIOUS_SCENES_BUTTON_ID: {
    if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
      RoomManager::goToPreviousRoom();
      ScenePage::_setRoomLabelText(RoomManager::currentRoom->name());
    }
    break;
  }
  case SCENES_PAGE_NEXT_SCENES_BUTTON_ID: {
    if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
      RoomManager::goToNextRoom();
      ScenePage::_setRoomLabelText(RoomManager::currentRoom->name());
    }
    break;
  }
  default:
    break;
  }
}

void ScenePage::_updateDisplay() {
  std::vector<PROTOBUF_NSPANEL_SCENE *> scenes;
  if (InterfaceConfig::currentRoomMode == roomMode::room && RoomManager::hasValidCurrentRoom()) {
    for (int i = 0; i < RoomManager::currentRoom->scenes().get_size(); i++) {
      scenes.push_back(&(RoomManager::currentRoom->mutable_scenes()[i]));
    }
  } else {
    for (int i = 0; i < InterfaceConfig::global_scenes.size(); i++) {
      scenes.push_back(InterfaceConfig::global_scenes[i]);
    }
  }

  for (int i = 0; i < 4; i++) {
    switch (i) {
    case 0: {
      if (scenes.size() >= 1) {
        std::string scene_name = "   ";
        scene_name.append(scenes[0]->name());
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE1_LABEL_NAME, scene_name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, scenes[0]->can_save());
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 1: {
      if (scenes.size() >= 2) {
        std::string scene_name = "   ";
        scene_name.append(scenes[1]->name());
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE2_LABEL_NAME, scene_name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, scenes[1]->can_save());
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 2: {
      if (scenes.size() >= 3) {
        std::string scene_name = "   ";
        scene_name.append(scenes[2]->name());
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE3_LABEL_NAME, scene_name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, scenes[2]->can_save());
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 3: {
      if (scenes.size() >= 4) {
        std::string scene_name = "   ";
        scene_name.append(scenes[3]->name());
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE4_LABEL_NAME, scene_name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME, scenes[3]->can_save());
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
