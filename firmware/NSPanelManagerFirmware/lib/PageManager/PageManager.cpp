#include <NSPanel.hpp>
#include <PageManager.hpp>

void PageManager::init() {
  NSPanel::instance->attachTouchEventCallback(&PageManager::ProcessTouchEventOnCurrentPage);
}

void PageManager::GoBack() {
  // Remove current page from history stack
  if (PageManager::_page_history.size() > 1) {
    PageManager::_page_history.pop_front();
  }

  if (PageManager::_page_history.size() > 0) {
    // PageManager::UnshowCurrentPage(); Unshow will be done when the new page calls setCurrentPage
    PageBase *show_page = PageManager::_page_history.front();
    PageManager::_page_history.front()->show();
    PageManager::_page_history.front()->update();
    PageManager::_page_history.pop_front(); // Do not add page to history again
  }
}

PageBase *PageManager::GetCurrentPage() {
  return PageManager::_current_page;
}

void PageManager::UpdateCurrentPage() {
  if (PageManager::GetCurrentPage() != nullptr) {
    PageManager::GetCurrentPage()->update();
  } else {
    LOG_ERROR("Trying to update current page but no current page is set.");
  }
}

void PageManager::UnshowCurrentPage() {
  if (PageManager::GetCurrentPage() != nullptr) {
    PageManager::GetCurrentPage()->unshow();
  } else {
    LOG_ERROR("Trying to unshow current page but no current page is set.");
  }
}

void PageManager::ProcessTouchEventOnCurrentPage(uint8_t page, uint8_t component, bool pressed) {
  if (PageManager::GetCurrentPage() != nullptr) {
    PageManager::GetCurrentPage()->processTouchEvent(page, component, pressed);
  } else {
    LOG_ERROR("Trying to process touch event on current page but no current page is set.");
  }
}

void PageManager::SetCurrentPage(PageBase *page) {
  PageManager::UnshowCurrentPage();
  PageManager::_current_page = page;
  PageManager::_page_history.push_front(page);

  while (PageManager::_page_history.size() > 5) {
    PageManager::_page_history.pop_back();
  }
}

LightPage *PageManager::GetLightPage() {
  if (PageManager::_lightPage == nullptr) {
    PageManager::_lightPage = new LightPage;
  }
  return PageManager::_lightPage;
}

NSpanelManagerPage *PageManager::GetNSPanelManagerPage() {
  if (PageManager::_nspanel_manager_page == nullptr) {
    PageManager::_nspanel_manager_page = new NSpanelManagerPage;
  }
  return PageManager::_nspanel_manager_page;
}

HomePage *PageManager::GetHomePage() {
  if (PageManager::_home_page == nullptr) {
    PageManager::_home_page = new HomePage;
  }
  return PageManager::_home_page;
}

ScenePage *PageManager::GetScenePage() {
  if (PageManager::_scene_page == nullptr) {
    PageManager::_scene_page = new ScenePage;
  }
  return PageManager::_scene_page;
}

RoomPage *PageManager::GetRoomPage() {
  if (PageManager::_room_page == nullptr) {
    PageManager::_room_page = new RoomPage;
  }
  return PageManager::_room_page;
}

ScreensaverPage *PageManager::GetScreensaverPage() {
  if (PageManager::_screensaver_page == nullptr) {
    PageManager::_screensaver_page = new ScreensaverPage;
  }
  return PageManager::_screensaver_page;
}
