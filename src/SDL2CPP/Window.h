#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

#include <SDL.h>

#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/sdl2cpp_export.h>

class sdl2cpp::Window {
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
  SDL2CPP_EXPORT Window(uint32_t width = 1024, uint32_t height = 768);
  SDL2CPP_EXPORT ~Window();
  SDL2CPP_EXPORT void     createContext(std::string const& name    = "context",
                                        uint32_t           version = 450u,
                                        Profile            profile = CORE,
                                        Flag               flags   = NONE);
  SDL2CPP_EXPORT void     setContext(std::string const& name,
                                     Window const&      other,
                                     std::string const& otherName);
  SDL2CPP_EXPORT void     makeCurrent(std::string const& name) const;
  SDL2CPP_EXPORT void     swap() const;
  SDL2CPP_EXPORT WindowId getId() const;
  SDL2CPP_EXPORT void     setEventCallback(
          EventType const&                             eventType,
          std::function<bool(SDL_Event const&)> const& callback = nullptr);
  SDL2CPP_EXPORT void setWindowEventCallback(
      uint8_t const&                               eventType,
      std::function<bool(SDL_Event const&)> const& callback = nullptr);
  SDL2CPP_EXPORT bool          hasEventCallback(EventType const& eventType) const;
  SDL2CPP_EXPORT bool          hasWindowEventCallback(uint8_t const& eventType) const;
  SDL2CPP_EXPORT void          setSize(uint32_t width, uint32_t height);
  SDL2CPP_EXPORT uint32_t      getWidth() const;
  SDL2CPP_EXPORT uint32_t      getHeight() const;
  SDL2CPP_EXPORT void          setFullscreen(Fullscreen const& type);
  SDL2CPP_EXPORT Fullscreen    getFullscreen();
  SDL2CPP_EXPORT SDL_Window*   getWindow() const;
  SDL2CPP_EXPORT SDL_GLContext getContext(std::string const& name) const;

 protected:
  using SharedSDLContext = std::shared_ptr<SDL_GLContext>;
  SDL_Window*                                                window = nullptr;
  std::map<std::string, SharedSDLContext>                    contexts;
  std::map<EventType, std::function<bool(SDL_Event const&)>> eventCallbacks;
  std::map<uint8_t, std::function<bool(SDL_Event const&)>>
            windowEventCallbacks;
  MainLoop* mainLoop;
  bool      defaultCloseCallback(SDL_Event const&);
  bool      callEventCallback(EventType const& eventType,
                                SDL_Event const& eventData);
  bool      callWindowEventCallback(uint8_t const&   eventType,
                                      SDL_Event const& eventData);
};

