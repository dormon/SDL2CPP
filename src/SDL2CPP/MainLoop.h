#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/sdl2cpp_export.h>

class SDL2CPP_EXPORT sdl2cpp::MainLoop {
 public:
  using SharedWindow      = std::shared_ptr<sdl2cpp::Window>;
  using WindowId          = uint32_t;
  using Name2Window       = std::map<std::string, SharedWindow>;
  using ConstNameIterator = Name2Window::const_iterator;
  using Id2Name           = std::map<WindowId, std::string>;
  using ConstIdIterator   = Id2Name::const_iterator;

  MainLoop(bool pooling = true);
  ~MainLoop();
  void addWindow(std::string const& name, SharedWindow const& window);
  void removeWindow(std::string const& name);
  void removeWindow(uint32_t const& id);
  bool hasWindow(std::string const& name) const;
  SharedWindow const& getWindow(std::string const& name) const;
  void                operator()();
  void                setIdleCallback(std::function<void()> const& callback);
  bool                hasIdleCallback() const;
  void setEventHandler(std::function<bool(SDL_Event const&)> const& handler);
  bool hasEventHandler() const;
  ConstNameIterator nameBegin() const;
  ConstNameIterator nameEnd() const;
  ConstIdIterator   idBegin() const;
  ConstIdIterator   idEnd() const;
  size_t            getNofWindows() const;

 protected:
  std::function<bool(SDL_Event const&)> m_eventHandler = nullptr;
  std::function<void()>                 m_idleCallback = nullptr;
  bool                                  m_pooling      = true;
  bool                                  m_running      = false;
  Name2Window                           m_name2Window;
  Id2Name                               m_id2Name;
  void                                  m_callIdleCallback();
  bool m_callEventHandler(SDL_Event const& event);
};
