#include <HomePage.hpp>
#include <LoadingPage.hpp>
#include <PageManager.hpp>
#include <PageManager_event.hpp>
#include <esp_log.h>

ESP_EVENT_DEFINE_BASE(PAGEMANAGER_EVENT);

void PageManager::init() {
  PageManager::_page_history.resize(5);
  for (int i = 0; i < PageManager::_page_history.size(); i++) {
    PageManager::_page_history[i] = PageManager::available_pages::FIRST_PAGE_IGNORE;
  }
}

void PageManager::set_current_page(PageManager::available_pages page) {
  ESP_LOGI("PageManager", "Setting currently shown page to: %ld", static_cast<uint32_t>(PageManager::_page_history[0]));
  switch (PageManager::_current_page) {
  case PageManager::available_pages::LOADING:
    LoadingPage::unshow();
    break;
  case PageManager::available_pages::HOME:
    HomePage::unshow();
    break;

  default:
    ESP_LOGE("PageManager", "Tried to unshow unknown page: %ld", static_cast<uint32_t>(PageManager::_page_history[0]));
    break;
  }

  // Shift all data forward so that we can set the new page to PageManager::_page_history[0]
  for (int i = PageManager::_page_history.size() - 1; i > 1; i--) {
    PageManager::_page_history[i] = PageManager::_page_history[i - 1];
  }
  PageManager::_page_history[0] = page;
}

void PageManager::go_back() {
  for (int i = 0; i < PageManager::_page_history.size() - 1; i++) {
    PageManager::_page_history[i] = PageManager::_page_history[i + 1];
  }

  switch (PageManager::_page_history[0]) {
  case PageManager::available_pages::LOADING:
    LoadingPage::show();
    break;
  case PageManager::available_pages::HOME:
    HomePage::show();
    break;

  default:
    ESP_LOGE("PageManager", "Tried to go back to unknown page: %ld", static_cast<uint32_t>(PageManager::_page_history[0]));
    break;
  }
}