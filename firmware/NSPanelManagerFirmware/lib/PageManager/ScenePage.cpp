#include <InterfaceConfig.hpp>
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

void ScenePage::doSceneSaveProgress(void *param) {
  unsigned long countStarted = millis();
  NSPanel::instance->setComponentVisible(SCENES_PAGE_SAVE_SLIDER_NAME, true);
  uint8_t lastSaveProgress = 255;
  while (millis() - countStarted < 3000 && ScenePage::_doSceneSaveProgress) { // TODO: Make timeout configurable
    uint8_t saveProgress = (millis() - countStarted) / 30;
    if (saveProgress > 100) {
      saveProgress = 100;
    }
    if (saveProgress != lastSaveProgress) {
      NSPanel::instance->setComponentVal(SCENES_PAGE_SAVE_SLIDER_NAME, saveProgress);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  if (ScenePage::_doSceneSaveProgress) {
    // Finger is still held at save button after timeout. Save the page.
    if (ScenePage::_sceneToSave != nullptr) {
      ScenePage::_sceneToSave->save();
      PageManager::GetScenePage()->_setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }

  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    PageManager::GetScenePage()->_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
  } else {
    PageManager::GetScenePage()->_setRoomLabelText("<--ALL-->");
  }
  NSPanel::instance->setComponentVisible(SCENES_PAGE_SAVE_SLIDER_NAME, false);
  vTaskDelete(NULL);
}

void ScenePage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  std::vector<Scene *> scenes;
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    scenes = (*RoomManager::currentRoom)->scenes;
  } else {
    scenes = InterfaceConfig::global_scenes;
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
    LOG_DEBUG("Touch event on component: ", page, ".", component);
    return;
  }

  // Release events
  switch (component) {
  case SCENES_PAGE_BACK_BUTTON_ID: {
    PageManager::GoBack();
    break;
  }
  case SCENES_PAGE_SCENE1_LABEL_ID: {
    if (scenes.size() >= 1) {
      scenes[0]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE2_LABEL_ID: {
    if (scenes.size() >= 2) {
      scenes[1]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE3_LABEL_ID: {
    if (scenes.size() >= 3) {
      scenes[2]->activate();
    }
    break;
  }
  case SCENES_PAGE_SCENE4_LABEL_ID: {
    if (scenes.size() >= 4) {
      scenes[3]->activate();
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
    if (InterfaceConfig::currentRoomMode == roomMode::room) {
      RoomManager::goToPreviousRoom();
      ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_NEXT_SCENES_BUTTON_ID: {
    if (InterfaceConfig::currentRoomMode == roomMode::room) {
      RoomManager::goToNextRoom();
      ScenePage::_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  default:
    break;
  }
}

void ScenePage::_updateDisplay() {
  std::vector<Scene *> scenes;
  if (InterfaceConfig::currentRoomMode == roomMode::room) {
    scenes = (*RoomManager::currentRoom)->scenes;
    this->_setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
  } else {
    scenes = InterfaceConfig::global_scenes;
    this->_setRoomLabelText("<--ALL-->");
  }

  for (int i = 0; i < 4; i++) {
    switch (i) {
    case 0: {
      if (scenes.size() >= 1) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE1_LABEL_NAME, scenes[0]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", scenes[0]->name.c_str(), " in slot 1");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 1: {
      if (scenes.size() >= 2) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE2_LABEL_NAME, scenes[1]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", scenes[1]->name.c_str(), " in slot 2");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 2: {
      if (scenes.size() >= 3) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE3_LABEL_NAME, scenes[2]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", scenes[2]->name.c_str(), " in slot 3");
      } else {
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_LABEL_NAME, false);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME, false);
      }
      break;
    }
    case 3: {
      if (scenes.size() >= 4) {
        NSPanel::instance->setComponentText(SCENES_PAGE_SCENE4_LABEL_NAME, scenes[3]->name.c_str());
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_LABEL_NAME, true);
        NSPanel::instance->setComponentVisible(SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME, true);
        LOG_DEBUG("Showing scene: ", scenes[3]->name.c_str(), " in slot 1");
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
