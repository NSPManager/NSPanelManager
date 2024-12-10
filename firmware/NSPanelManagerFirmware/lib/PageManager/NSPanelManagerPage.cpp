#include <NSPanel.hpp>
#include <NSPanelManagerPage.hpp>
#include <PageManager.hpp>
#include <TftDefines.h>

void NSpanelManagerPage::show() {

  PageManager::SetCurrentPage(this);
  NSPanel::instance->goToPage(NSPANELMANAGER_PAGE_NAME);
}

void NSpanelManagerPage::update() {
}

void NSpanelManagerPage::unshow() {
}

void NSpanelManagerPage::processTouchEvent(uint8_t page, uint8_t component, bool pressed) {
  LOG_DEBUG("Got touch event, component ", page, ".", component, " ", pressed ? "pressed" : "released");
}

void NSpanelManagerPage::setText(std::string &text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_NAME, text.c_str());
}

void NSpanelManagerPage::setText(const char *text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_NAME, text);
}

void NSpanelManagerPage::setSecondaryText(std::string &text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_IP_NAME, text.c_str());
}

void NSpanelManagerPage::setSecondaryText(const char *text) {
  NSPanel::instance->setComponentText(NSPANELMANAGER_TEXT_IP_NAME, text);
}
