#ifndef INTERFACE_PAGE_HPP
#define INTERFACE_PAGE_HPP

class Page {
  /// @brief Show the intended page and do everything need to get it working
  virtual void show();
  /// @brief Update the page according to new values
  virtual void update();
  /// @brief Remove and event handlers or simular
  virtual void unshow();
};

#endif
