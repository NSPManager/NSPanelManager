#pragma once
#include <string>

class LoadingPage {
public:
  static void show();
  static void unshow();
  static void set_loading_text(std::string text);
  static void set_secondary_text(std::string text);
};