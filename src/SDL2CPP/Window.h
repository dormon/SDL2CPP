#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/sdl2cpp_export.h>

class SDL2CPP_EXPORT sdl2cpp::Window {
  friend class MainLoop;

 public:
  using WindowId  = uint32_t;
  using EventType = uint32_t;
  enum Profile {
    CORE          = SDL_GL_CONTEXT_PROFILE_CORE,
    COMPATIBILITY = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
    ES            = SDL_GL_CONTEXT_PROFILE_ES,
  };
  enum Flag {
    NONE               = 0,
    DEBUG              = SDL_GL_CONTEXT_DEBUG_FLAG,
    FORWARD_COMPATIBLE = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG,
    ROBUST_ACCESS      = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG,
    RESET_ISOLATION    = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG,
  };
  enum Fullscreen {
    WINDOW             = 0,
    FULLSCREEN         = SDL_WINDOW_FULLSCREEN,
    FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
  };
  Window(uint32_t width = 1024, uint32_t height = 768);
  ~Window();
  void     createContext(std::string const& name    = "context",
                         uint32_t           version = 450u,
                         Profile            profile = CORE,
                         Flag               flags   = NONE);
  void     setContext(std::string const& name,
                      Window const&      other,
                      std::string const& otherName);
  void     makeCurrent(std::string const& name) const;
  void     swap() const;
  WindowId getId() const;
  void     setEventCallback(
          EventType const&                             eventType,
          std::function<bool(SDL_Event const&)> const& callback = nullptr);
  void setWindowEventCallback(
      uint8_t const&                               eventType,
      std::function<bool(SDL_Event const&)> const& callback = nullptr);
  bool          hasEventCallback(EventType const& eventType) const;
  bool          hasWindowEventCallback(uint8_t const& eventType) const;
  void          setSize(uint32_t width, uint32_t height);
  uint32_t      getWidth() const;
  uint32_t      getHeight() const;
  void          setFullscreen(Fullscreen const& type);
  Fullscreen    getFullscreen();
  SDL_Window*   getWindow() const;
  SDL_GLContext getContext(std::string const& name) const;

 protected:
  using SharedSDLContext = std::shared_ptr<SDL_GLContext>;
  SDL_Window*                                                m_window = nullptr;
  std::map<std::string, SharedSDLContext>                    m_contexts;
  std::map<EventType, std::function<bool(SDL_Event const&)>> m_eventCallbacks;
  std::map<uint8_t, std::function<bool(SDL_Event const&)>>
            m_windowEventCallbacks;
  MainLoop* m_mainLoop;
  bool      m_defaultCloseCallback(SDL_Event const&);
  bool      m_callEventCallback(EventType const& eventType,
                                SDL_Event const& eventData);
  bool      m_callWindowEventCallback(uint8_t const&   eventType,
                                      SDL_Event const& eventData);
};

