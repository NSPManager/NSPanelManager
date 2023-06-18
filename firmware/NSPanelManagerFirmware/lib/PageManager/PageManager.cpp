#include <PageManager.hpp>

void PageManager::GoBack() {
}

PageBase *PageManager::GetCurrentPage() {
  return PageManager::_current_page;
}

void PageManager::UnshowCurrentPage() {
}

void PageManager::SetCurrentPage(PageBase *page) {
  PageManager::_current_page = page;
  PageManager::_page_history.push_front(page);
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

ScreensaverPage *PageManager::GetScreensaverPage() {
  if (PageManager::_screensaver_page == nullptr) {
    PageManager::_screensaver_page = new ScreensaverPage;
  }
  return PageManager::_screensaver_page;
}
