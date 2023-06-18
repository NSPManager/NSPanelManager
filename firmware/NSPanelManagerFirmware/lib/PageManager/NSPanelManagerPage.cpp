#include <NSPanel.hpp>
#include <NSPanelManagerPage.hpp>
#include <TftDefines.h>

void NSpanelManagerPage::show() {
  NSPanel::instance->goToPage(NSPANELMANAGER_PAGE_NAME);
}

void NSpanelManagerPage::setText(std::string &text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_NAME, text.c_str());
}

void NSpanelManagerPage::setText(const char *text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_NAME, text);
}
