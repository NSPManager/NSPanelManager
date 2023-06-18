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

void ScenePage::show() {
  NSPanel::instance->goToPage(SCENES_PAGE_NAME);
}

void ScenePage::setRoomLabelText(const char *text) {
  NSPanel::instance->setComponentText(SCENES_PAGE_CURRENT_SCENES_LABEL_NAME, text);
}

void ScenePage::showScenes(std::vector<Scene *> &scenes) {
  // Hide any elements that wont be used
  // Update the correct name and enable components that are to be used
  // 4 is the number of scene we can display on the screen
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

void ScenePage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  if (pressed) {
    return; // We only care about when the user releases the finger from the panel
  }

  switch (component) {
  case SCENES_PAGE_BACK_BUTTON_ID: {
    // PageManager::GoBack();
    // TODO: Use GoBack function from PageManager
    PageManager::GetHomePage()->show();
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
      ScenePage::setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_SCENE2_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 2) {
      (*RoomManager::currentRoom)->scenes[1]->save();
      ScenePage::setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_SCENE3_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 3) {
      (*RoomManager::currentRoom)->scenes[2]->save();
      ScenePage::setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_SCENE4_SAVE_BUTTON_ID: {
    if ((*RoomManager::currentRoom)->scenes.size() >= 4) {
      (*RoomManager::currentRoom)->scenes[3]->save();
      ScenePage::setRoomLabelText("Saved");
      vTaskDelay(500 / portTICK_PERIOD_MS);
      ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    }
    break;
  }
  case SCENES_PAGE_PREVIOUS_SCENES_BUTTON_ID: {
    RoomManager::goToPreviousRoom();
    ScenePage::showScenes((*RoomManager::currentRoom)->scenes);
    ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    break;
  }
  case SCENES_PAGE_NEXT_SCENES_BUTTON_ID: {
    RoomManager::goToNextRoom();
    ScenePage::showScenes((*RoomManager::currentRoom)->scenes);
    ScenePage::setRoomLabelText((*RoomManager::currentRoom)->name.c_str());
    break;
  }
  default:
    break;
  }
}
