#pragma once
#include <vector>

class PageManager {
public:
  enum available_pages {
    FIRST_PAGE_IGNORE, // This page does nothing, it's more of a flag that "Hey, this value is not set!"
    LOADING,
    HOME,
    SCENES,
    ENTITIES,
    SCREENSAVER,
  };

  /**
   * Initialize the PageManager
   */
  static void init();

  /**
   * Update what page is currently shown
   */
  static void set_current_page(PageManager::available_pages page);

  /**
   * Change to the previously shown page
   */
  static void go_back();

  /**
   * Get the currently shown page
   */
  static PageManager::available_pages *get_current_page();

private:
  // Vars:
  // History of shown pages
  static inline std::vector<PageManager::available_pages> _page_history;

  // Pointer to the currently shown page.
  static inline PageManager::available_pages _current_page;
};