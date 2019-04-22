#include <SDL2CPP/Exception.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>

#include <cassert>
#include <iostream>

using namespace sdl2cpp;
using namespace std;

/**
 * @brief Creates new Window
 *
 * @param width width of new window
 * @param height height of new window
 */
Window::Window(uint32_t width, uint32_t height)
{
  initSDL2();

  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  window     = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, flags);
  if (!window) throw ex::Window(SDL_GetError());
  setWindowEventCallback(
      SDL_WINDOWEVENT_CLOSE,
      bind(&Window::defaultCloseCallback, this, placeholders::_1));
}

/**
 * @brief Destroys Window
 */
Window::~Window()
{
  // free contexts, otherwise it would cause memory leak on gpu (according to
  // CodeXL)
  contexts.clear();
  SDL_DestroyWindow(window);
}

void setContextMajorVersion(uint32_t version)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version / 100) >= 0)
    return;
  throw ex::CreateContext(string("SDL_GL_CONTEXT_MAJOR_VERISON - ") +
                          SDL_GetError());
}

void setContextMinorVersion(uint32_t version)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, (version % 100) / 10) >=
      0)
    return;
  throw ex::CreateContext(string("SDL_GL_CONTEXT_MINOR_VERISON - ") +
                          SDL_GetError());
}

void setContextProfile(Window::Profile profile)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile) >= 0) return;
  throw ex::CreateContext(string("SDL_GL_CONTEXT_PROFILE_MASK - ") +
                          SDL_GetError());
}

void setContextFlags(Window::Flag flags)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags) >= 0) return;
  throw ex::CreateContext(string("SDL_GL_CONTEXT_FLAGS - ") +
                          SDL_GetError());
}

/**
 * @brief Creates new (GL) context for this window
 *
 * @param name new of new context
 * @param version context version 450, 440, 430, ...
 * @param profile context profile
 * @param flags context flags, debug context ...
 *
 */
void Window::createContext(string const& name,
                           uint32_t           version,
                           Profile            profile,
                           Flag               flags)
{
  setContextMajorVersion(version);
  setContextMinorVersion(version);
  setContextProfile(profile);
  setContextFlags(flags);
  SharedSDLContext ctx = shared_ptr<SDL_GLContext>(
      new SDL_GLContext, [&](SDL_GLContext* ctx) {
        if (*ctx) SDL_GL_DeleteContext(*ctx);
        delete ctx;
      });
  *ctx = SDL_GL_CreateContext(window);
  if (*ctx == nullptr) throw ex::CreateContext(SDL_GetError());
  contexts[name] = ctx;
}

/**
 * @brief Sets context in this window with other window context
 *
 * @param name name of this windows context
 * @param other other window
 * @param otherName name of other window context
 */
void Window::setContext(string const& name,
                        Window const&      other,
                        string const& otherName)
{
  assert(other.contexts.count(otherName) != 0);
  contexts[name] = other.contexts.find(otherName)->second;
}

/**
 * @brief Makes context current for this window
 *
 * @param name name of context that will be current
 */
void Window::makeCurrent(string const& name) const
{
  assert(contexts.count(name) != 0);
  if (SDL_GL_MakeCurrent(window, *contexts.find(name)->second) < 0)
    throw ex::WindowMethod("makeCurrent", SDL_GetError());
}

/**
 * @brief Swaps buffers (front and back buffers)
 */
void Window::swap() const { SDL_GL_SwapWindow(window); }

/**
 * @brief Returns window id
 *
 * @return window id
 */
Window::WindowId Window::getId() const { return SDL_GetWindowID(window); }

/**
 * @brief Sets callback for particular event in this event
 *
 * @param eventType event type (SDL_KEYDOWN, SDL_MOUSEMOTION, ...)
 * @param callback callback, callback has to return true if event was served
 */
void Window::setEventCallback(
    EventType const&                             eventType,
    function<bool(SDL_Event const&)> const& callback)
{
  if (callback == nullptr) {
    eventCallbacks.erase(eventType);
    return;
  }
  eventCallbacks[eventType] = callback;
}

/**
 * @brief Sets callback for window event
 *
 * @param eventType (SDL_WINDOWEVENT_CLOSE, ...)
 * @param callback callback, callback has to return true if event was served
 */
void Window::setWindowEventCallback(
    uint8_t const&                               eventType,
    function<bool(SDL_Event const&)> const& callback)
{
  if (callback == nullptr) {
    windowEventCallbacks.erase(eventType);
    return;
  }
  windowEventCallbacks[eventType] = callback;
}

/**
 * @brief Gets true if callback for particular event is present
 *
 * @param eventType event type (SDL_KEYDOWN, ...)
 *
 * @return returns true if this callback is present
 */
bool Window::hasEventCallback(EventType const& eventType) const
{
  auto ii = eventCallbacks.find(eventType);
  return ii != eventCallbacks.end() && ii->second != nullptr;
}

/**
 * @brief Gets true if callback for particular window event is present
 *
 * @param eventType window event type (SDL_WINDOW_EVENT_CLOSE, ...)
 *
 * @return true if this window callback is present
 */
bool Window::hasWindowEventCallback(uint8_t const& eventType) const
{
  auto ii = windowEventCallbacks.find(eventType);
  return ii != windowEventCallbacks.end() && ii->second != nullptr;
}

/**
 * @brief sets window size
 *
 * @param width new width of this window
 * @param height new height of this window
 */
void Window::setSize(uint32_t width, uint32_t heght)
{
  SDL_SetWindowSize(window, width, heght);
}

/**
 * @brief gets window width
 *
 * @return window width
 */
uint32_t Window::getWidth() const
{
  int size[2];
  SDL_GetWindowSize(window, size + 0, size + 1);
  return size[0];
}

/**
 * @brief gets window height
 *
 * @return window height
 */
uint32_t Window::getHeight() const
{
  int size[2];
  SDL_GetWindowSize(window, size + 0, size + 1);
  return size[1];
}

/**
 * @brief sets fullscreen
 *
 * @param type fullscreen type
 */
void Window::setFullscreen(Fullscreen const& type)
{
  if (SDL_SetWindowFullscreen(window, type))
    throw ex::WindowMethod("setFullscreen", SDL_GetError());
}

/**
 * @brief gets fullscreen type
 *
 * @return fullscreen type
 */
Window::Fullscreen Window::getFullscreen()
{
  auto flags = SDL_GetWindowFlags(window);
  if (flags & FULLSCREEN) return FULLSCREEN;
  if (flags & FULLSCREEN_DESKTOP) return FULLSCREEN_DESKTOP;
  return WINDOW;
}

SDL_GLContext Window::getContext(string const& name) const
{
  auto const it = contexts.find(name);
  if (it == contexts.end()) return nullptr;
  return *it->second;
}

/**
 * @brief gets window object as implemented by SDL
 *
 * @return window object
 */
bool Window::defaultCloseCallback(SDL_Event const&)
{
  assert(mainLoop != nullptr);
  mainLoop->removeWindow(getId());
  return true;
}

bool Window::callEventCallback(EventType const& eventType,
                                 SDL_Event const& event)
{
  assert(eventCallbacks.count(eventType) != 0);
  assert(eventCallbacks.at(eventType) != nullptr);
  return eventCallbacks.at(eventType)(event);
}

bool Window::callWindowEventCallback(uint8_t const&   eventType,
                                       SDL_Event const& eventData)
{
  assert(windowEventCallbacks.count(eventType) != 0);
  assert(windowEventCallbacks.at(eventType) != nullptr);
  return windowEventCallbacks.at(eventType)(eventData);
}

SDL_Window* Window::getWindow() const { return window; }
