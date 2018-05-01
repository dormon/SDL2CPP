#include <SDL2CPP/Exception.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>

#include <cassert>
#include <iostream>

using namespace sdl2cpp;

/**
 * @brief Creates new Window
 *
 * @param width width of new window
 * @param height height of new window
 */
Window::Window(uint32_t width, uint32_t height)
{
  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  m_window     = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, flags);
  if (!m_window)
    throw WindowException(SDL_GetError());
  setWindowEventCallback(
      SDL_WINDOWEVENT_CLOSE,
      std::bind(&Window::m_defaultCloseCallback, this, std::placeholders::_1));
}

/**
 * @brief Destroys Window
 */
Window::~Window()
{
  // free contexts, otherwise it would cause memory leak on gpu (according to
  // CodeXL)
  m_contexts.clear();
  SDL_DestroyWindow(m_window);
}

void setContextMajorVersion(uint32_t version)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version / 100) == 0)
    return;
  throw CreateContext(std::string("SDL_GL_CONTEXT_MAJOR_VERISON - ") +
                      SDL_GetError());
}

void setContextMinorVersion(uint32_t version)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, (version % 100) / 10) ==
      0)
    return;
  throw CreateContext(std::string("SDL_GL_CONTEXT_MINOR_VERISON - ") +
                      SDL_GetError());
}

void setContextProfile(Window::Profile profile)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile) == 0) return;
  throw CreateContext(std::string("SDL_GL_CONTEXT_PROFILE_MASK - ") +
                      SDL_GetError());
}

void setContextFlags(Window::Flag flags)
{
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags) < 0) return;
  throw CreateContext(std::string("SDL_GL_CONTEXT_FLAGS - ") + SDL_GetError());
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
void Window::createContext(std::string const& name,
                           uint32_t           version,
                           Profile            profile,
                           Flag               flags)
{
  setContextMajorVersion(version);
  setContextMinorVersion(version);
  setContextProfile(profile);
  setContextFlags(flags);
  SharedSDLContext ctx = std::shared_ptr<SDL_GLContext>(
      new SDL_GLContext, [&](SDL_GLContext* ctx) {
        if (*ctx) SDL_GL_DeleteContext(*ctx);
        delete ctx;
      });
  *ctx = SDL_GL_CreateContext(m_window);
  if (*ctx == nullptr)
    throw CreateContext(SDL_GetError());
  m_contexts[name] = ctx;
}

/**
 * @brief Sets context in this window with other window context
 *
 * @param name name of this windows context
 * @param other other window
 * @param otherName name of other window context
 */
void Window::setContext(std::string const& name,
                        Window const&      other,
                        std::string const& otherName)
{
  assert(other.m_contexts.count(otherName) != 0);
  m_contexts[name] = other.m_contexts.find(otherName)->second;
}

/**
 * @brief Makes context current for this window
 *
 * @param name name of context that will be current
 */
void Window::makeCurrent(std::string const& name) const
{
  assert(m_contexts.count(name) != 0);
  if (SDL_GL_MakeCurrent(m_window, *m_contexts.find(name)->second) < 0)
    throw WindowFunctionException("makeCurrent",SDL_GetError());
}

/**
 * @brief Swaps buffers (front and back buffers)
 */
void Window::swap() const { SDL_GL_SwapWindow(m_window); }

/**
 * @brief Returns window id
 *
 * @return window id
 */
Window::WindowId Window::getId() const { return SDL_GetWindowID(m_window); }

/**
 * @brief Sets callback for particular event in this event
 *
 * @param eventType event type (SDL_KEYDOWN, SDL_MOUSEMOTION, ...)
 * @param callback callback, callback has to return true if event was served
 */
void Window::setEventCallback(
    EventType const&                             eventType,
    std::function<bool(SDL_Event const&)> const& callback)
{
  if (callback == nullptr) {
    m_eventCallbacks.erase(eventType);
    return;
  }
  m_eventCallbacks[eventType] = callback;
}

/**
 * @brief Sets callback for window event
 *
 * @param eventType (SDL_WINDOWEVENT_CLOSE, ...)
 * @param callback callback, callback has to return true if event was served
 */
void Window::setWindowEventCallback(
    uint8_t const&                               eventType,
    std::function<bool(SDL_Event const&)> const& callback)
{
  if (callback == nullptr) {
    m_windowEventCallbacks.erase(eventType);
    return;
  }
  m_windowEventCallbacks[eventType] = callback;
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
  auto ii = m_eventCallbacks.find(eventType);
  return ii != m_eventCallbacks.end() && ii->second != nullptr;
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
  auto ii = m_windowEventCallbacks.find(eventType);
  return ii != m_windowEventCallbacks.end() && ii->second != nullptr;
}

/**
 * @brief sets window size
 *
 * @param width new width of this window
 * @param height new height of this window
 */
void Window::setSize(uint32_t width, uint32_t heght)
{
  SDL_SetWindowSize(m_window, width, heght);
}

/**
 * @brief gets window width
 *
 * @return window width
 */
uint32_t Window::getWidth() const
{
  int size[2];
  SDL_GetWindowSize(m_window, size + 0, size + 1);
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
  SDL_GetWindowSize(m_window, size + 0, size + 1);
  return size[1];
}

/**
 * @brief sets fullscreen
 *
 * @param type fullscreen type
 */
void Window::setFullscreen(Fullscreen const& type)
{
  if (SDL_SetWindowFullscreen(m_window, type))
    throw WindowFunctionException("setFullscreen",SDL_GetError());
}

/**
 * @brief gets fullscreen type
 *
 * @return fullscreen type
 */
Window::Fullscreen Window::getFullscreen()
{
  auto flags = SDL_GetWindowFlags(m_window);
  if (flags & FULLSCREEN) return FULLSCREEN;
  if (flags & FULLSCREEN_DESKTOP) return FULLSCREEN_DESKTOP;
  return WINDOW;
}

SDL_GLContext Window::getContext(std::string const& name) const
{
  auto const it = m_contexts.find(name);
  if (it == m_contexts.end()) return nullptr;
  return *it->second;
}

/**
 * @brief gets window object as implemented by SDL
 *
 * @return window object
 */
bool Window::m_defaultCloseCallback(SDL_Event const&)
{
  assert(m_mainLoop != nullptr);
  m_mainLoop->removeWindow(getId());
  return true;
}

bool Window::m_callEventCallback(EventType const& eventType,
                                 SDL_Event const& event)
{
  assert(m_eventCallbacks.count(eventType) != 0);
  assert(m_eventCallbacks.at(eventType) != nullptr);
  return m_eventCallbacks.at(eventType)(event);
}

bool Window::m_callWindowEventCallback(uint8_t const&   eventType,
                                       SDL_Event const& eventData)
{
  assert(m_windowEventCallbacks.count(eventType) != 0);
  assert(m_windowEventCallbacks.at(eventType) != nullptr);
  return m_windowEventCallbacks.at(eventType)(eventData);
}

SDL_Window* Window::getWindow() const { return m_window; }
