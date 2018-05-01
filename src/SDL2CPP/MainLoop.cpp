#include <SDL2CPP/Exception.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <cassert>

using namespace sdl2cpp;

/**
 * @brief Creates new main loop
 *
 * @param pooling if set to false, idle will be called only when new event
 * arrives
 */
MainLoop::MainLoop(bool pooling) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw ex::MainLoop(SDL_GetError());
  m_pooling = pooling;
}

/**
 * @brief Destroys main loop
 */
MainLoop::~MainLoop() { SDL_Quit(); }

/**
 * @brief Adds window to this main loop
 *
 * @param name name identificator of window
 * @param window SDLWindow
 */
void MainLoop::addWindow(std::string const& name, SharedWindow const& window) {
  m_name2Window[name] = window;
  m_id2Name[window->getId()] = name;
  window->m_mainLoop = this;
}

/**
 * @brief Removes window from this main loop by window id
 *
 * @param id window id
 */
void MainLoop::removeWindow(uint32_t const& id) {
  assert(m_id2Name.count(id) != 0);
  auto name = m_id2Name.at(id);
  m_id2Name.erase(id);
  m_name2Window.erase(name);
}

/**
 * @brief Removes window from this main loop
 *
 * @param name name identificator of window
 */
void MainLoop::removeWindow(std::string const& name) {
  m_id2Name.erase(getWindow(name)->getId());
  m_name2Window.erase(name);
}

/**
 * @brief Has this main loop window with this name?
 *
 * @param name name of window
 *
 * @return true if this main loop has that window
 */
bool MainLoop::hasWindow(std::string const& name) const {
  return m_name2Window.count(name) != 0;
}

/**
 * @brief Gets window by its name
 *
 * @param name name of window
 *
 * @return return SDLWindow
 */
MainLoop::SharedWindow const& MainLoop::getWindow(
    std::string const& name) const {
  assert(m_name2Window.count(name) != 0);
  return m_name2Window.find(name)->second;
}

/**
 * @brief Starts main loop
 */
void MainLoop::operator()() {
  m_running = true;
  SDL_Event event;
  while (m_running) {
    if (m_name2Window.size() == 0) {
      m_running = false;
      break;
    }
    if (!m_pooling)
      if (SDL_WaitEvent(&event) == 0) {
        throw ex::MainLoop(SDL_GetError());
        return;
      }
    while (true) {
      if (m_pooling)
        if (!SDL_PollEvent(&event)) break;

      bool handledByEventHandler = false;

      if (hasEventHandler())
        handledByEventHandler = m_callEventHandler(event);

      if (!handledByEventHandler) {
        auto windowIter = m_id2Name.find(event.window.windowID);
        bool handledByEventCallback = false;
        if (windowIter != m_id2Name.end()) {
          auto const& window = m_name2Window[windowIter->second];
          if (window->hasEventCallback(event.type))
            handledByEventCallback =
                window->m_callEventCallback(event.type, event);
        }
        if (!handledByEventCallback) {
          if (event.type == SDL_WINDOWEVENT) {
            bool handledByWindowEventCallback = false;
            if (windowIter != m_id2Name.end()) {
              auto const& window = m_name2Window.at(windowIter->second);
              if (window->hasWindowEventCallback(event.window.event))
                handledByWindowEventCallback =
                    window->m_callWindowEventCallback(event.window.event,
                                                      event);
            }
            (void)handledByWindowEventCallback;
          }
        }
      }

      if (!m_pooling)
        if (!SDL_PollEvent(&event)) break;
    }
    if (hasIdleCallback()) m_callIdleCallback();
  }
}

/**
 * @brief sets idle callback
 *
 * @param callback idle callback
 */
void MainLoop::setIdleCallback(std::function<void()> const& callback) {
  m_idleCallback = callback;
}

/**
 * @brief has idle callback
 *
 * @return true if this main loop has idle callback
 */
bool MainLoop::hasIdleCallback() const {
  return m_idleCallback != nullptr;
}

/**
 * @brief sets event handler callback
 * Event handler can serve every event, it is usefull for imgui, AntTweakBar
 * Event handlere has to return true if it served particular event
 *
 * @param handler callback
 */
void MainLoop::setEventHandler(
    std::function<bool(SDL_Event const&)> const& handler) {
  m_eventHandler = handler;
}

/**
 * @brief has event handler callback
 *
 * @return event handler callback
 */
bool MainLoop::hasEventHandler() const {
  return m_eventHandler != nullptr;
}

/**
 * @brief begin iterator of names of registered windows
 *
 * @return begin iterator of names of registered windows
 */
MainLoop::ConstNameIterator MainLoop::nameBegin() const {
  return m_name2Window.begin();
}

/**
 * @brief end iterator of names of registered windows
 *
 * @return end iterator of names of registered windows
 */
MainLoop::ConstNameIterator MainLoop::nameEnd() const {
  return m_name2Window.end();
}

/**
 * @brief begin iterator of ids of registered windows
 *
 * @return begin iterator of ids of registerd windows
 */
MainLoop::ConstIdIterator MainLoop::idBegin() const {
  return m_id2Name.begin();
}

/**
 * @brief end iterator of ids of registered windows
 *
 * @return end iterator of ids of registered windows
 */
MainLoop::ConstIdIterator MainLoop::idEnd() const {
  return m_id2Name.end();
}

/**
 * @brief gets number of registered windows
 *
 * @return number of registered windows
 */
size_t MainLoop::getNofWindows() const {
  return m_name2Window.size();
}

void MainLoop::m_callIdleCallback() {
  assert(m_idleCallback != nullptr);
  m_idleCallback();
}

bool MainLoop::m_callEventHandler(SDL_Event const& event) {
  assert(m_eventHandler != nullptr);
  return m_eventHandler(event);
}
