#include <GUI_data.hpp>
#include <LoadingPage.hpp>
#include <Nextion.hpp>
#include <PageManager.hpp>

void LoadingPage::show() {
  PageManager::set_current_page(PageManager::available_pages::LOADING);
  Nextion::go_to_page(GUI_LOADING_PAGE::page_name, 250);
}

void LoadingPage::unshow() {
}

void LoadingPage::set_loading_text(std::string text) {
  Nextion::set_component_text(GUI_LOADING_PAGE::component_text_name, text.c_str(), pdMS_TO_TICKS(100));
}

void LoadingPage::set_secondary_text(std::string text) {
  Nextion::set_component_text(GUI_LOADING_PAGE::component_text_ip_text, text.c_str(), pdMS_TO_TICKS(100));
}