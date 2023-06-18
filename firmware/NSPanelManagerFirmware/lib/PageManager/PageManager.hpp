#ifndef PAGEMANAGER_HPP
#define PAGEMANAGER_HPP

#include <Arduino.h>
#include <HomePage.hpp>
#include <LightPage.hpp>
#include <NSPanelManagerPage.hpp>
#include <PageBase.hpp>
#include <ScreensaverPage.hpp>
#include <list>

class PageManager {
public:
  static void UnshowCurrentPage();
  static void UpdateCurrentPage();
  static void GoBack();
  static void SetCurrentPage(PageBase *page);
  static PageBase *GetCurrentPage();

  static LightPage *GetLightPage();
  static HomePage *GetHomePage();
  static ScreensaverPage *GetScreensaverPage();
  static NSpanelManagerPage *GetNSPanelManagerPage();

private:
  static inline std::list<PageBase *> _page_history;
  static inline PageBase *_current_page;

  static inline LightPage *_lightPage;
  static inline NSpanelManagerPage *_nspanel_manager_page;
  static inline HomePage *_home_page;
  static inline ScreensaverPage *_screensaver_page;
};

#endif
