#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>

#include <SDL.h>
#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/sdl2cpp_export.h>

class sdl2cpp::MainLoop {
 public:
  using SharedWindow      = std::shared_ptr<sdl2cpp::Window>;
  using WindowId          = uint32_t;
  using Name2Window       = std::map<std::string, SharedWindow>;
  using ConstNameIterator = Name2Window::const_iterator;
  using Id2Name           = std::map<WindowId, std::string>;
  using ConstIdIterator   = Id2Name::const_iterator;

  SDL2CPP_EXPORT MainLoop(bool pooling = true);
  SDL2CPP_EXPORT ~MainLoop();
  SDL2CPP_EXPORT void addWindow(std::string const& name, SharedWindow const& window);
  SDL2CPP_EXPORT void removeWindow(std::string const& name);
  SDL2CPP_EXPORT void removeWindow(uint32_t const& id);
  SDL2CPP_EXPORT bool hasWindow(std::string const& name) const;
  SDL2CPP_EXPORT SharedWindow const& getWindow(std::string const& name) const;
  SDL2CPP_EXPORT void                operator()();
  SDL2CPP_EXPORT void                stop();
  SDL2CPP_EXPORT void                setIdleCallback(std::function<void()> const& callback);
  SDL2CPP_EXPORT bool                hasIdleCallback() const;
  SDL2CPP_EXPORT void setEventHandler(std::function<bool(SDL_Event const&)> const& handler);
  SDL2CPP_EXPORT void setEventCallback(Uint32 event,std::function<bool(SDL_Event const&)> const& fce);
  SDL2CPP_EXPORT bool hasEventHandler() const;
  SDL2CPP_EXPORT ConstNameIterator nameBegin() const;
  SDL2CPP_EXPORT ConstNameIterator nameEnd() const;
  SDL2CPP_EXPORT ConstIdIterator   idBegin() const;
  SDL2CPP_EXPORT ConstIdIterator   idEnd() const;
  SDL2CPP_EXPORT size_t            getNofWindows() const;

 protected:
  std::function<bool(SDL_Event const&)> eventHandler = nullptr;
  std::function<void()>                 idleCallback = nullptr;
  std::map<Uint32,std::function<bool(SDL_Event const&)>>eventCallbacks;
  bool                                  pooling      = true;
  bool                                  running      = false;
  Name2Window                           name2Window;
  Id2Name                               id2Name;
  void                                  callIdleCallback();
  bool callEventHandler(SDL_Event const& event);
};
