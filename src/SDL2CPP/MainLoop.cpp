#include <SDL2CPP/Exception.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <cassert>

using namespace sdl2cpp;
using namespace std;

void sdl2cpp::initSDL2(){
  if(SDL_WasInit(SDL_INIT_EVERYTHING)&SDL_INIT_EVERYTHING)
    return;
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    throw ex::Exception(SDL_GetError());
}

/**
 * @brief Creates new main loop
 *
 * @param pooling if set to false, idle will be called only when new event
 * arrives
 */
MainLoop::MainLoop(bool pooling) {
  initSDL2();
  pooling = pooling;
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
void MainLoop::addWindow(string const& name, SharedWindow const& window) {
  if(!window)
    throw ex::MainLoopMethod("addWindow","window cannot be nullptr");
  name2Window[name] = window;
  id2Name[window->getId()] = name;
  window->mainLoop = this;
}

/**
 * @brief Removes window from this main loop by window id
 *
 * @param id window id
 */
void MainLoop::removeWindow(uint32_t const& id) {
  assert(id2Name.count(id) != 0);
  auto name = id2Name.at(id);
  id2Name.erase(id);
  name2Window.erase(name);
}

/**
 * @brief Removes window from this main loop
 *
 * @param name name identificator of window
 */
void MainLoop::removeWindow(string const& name) {
  id2Name.erase(getWindow(name)->getId());
  name2Window.erase(name);
}

/**
 * @brief Has this main loop window with this name?
 *
 * @param name name of window
 *
 * @return true if this main loop has that window
 */
bool MainLoop::hasWindow(string const& name) const {
  return name2Window.count(name) != 0;
}

/**
 * @brief Gets window by its name
 *
 * @param name name of window
 *
 * @return return SDLWindow
 */
MainLoop::SharedWindow const& MainLoop::getWindow(
    string const& name) const {
  assert(name2Window.count(name) != 0);
  return name2Window.find(name)->second;
}

/**
 * @brief Starts main loop
 */
void MainLoop::operator()() {
  running = true;
  SDL_Event event;
  while (running) {
    if (name2Window.size() == 0) {
      running = false;
      break;
    }
    if (!pooling)
      if (SDL_WaitEvent(&event) == 0) {
        throw ex::MainLoop(SDL_GetError());
        return;
      }
    while (true) {
      if (pooling)
        if (!SDL_PollEvent(&event)) break;

      bool handledByEventHandler = false;

      if (hasEventHandler())
        handledByEventHandler = callEventHandler(event);

      if (!handledByEventHandler) {
        bool handledByMainLoopEventCallback = false;
        auto it = eventCallbacks.find(event.type);
        if(it != eventCallbacks.end())
          handledByMainLoopEventCallback = it->second(event);

        if(!handledByMainLoopEventCallback){
          auto const& winID = event.window.windowID;
          auto windowIter = id2Name.find(winID);
          bool handledByEventCallback = false;
          if (windowIter != id2Name.end()) {
            auto const& window = name2Window[windowIter->second];
            if (window->hasEventCallback(event.type))
              handledByEventCallback =
                  window->callEventCallback(event.type, event);
          }
          if (!handledByEventCallback) {
            if (event.type == SDL_WINDOWEVENT) {
              bool handledByWindowEventCallback = false;
              if (windowIter != id2Name.end()) {
                auto const& window = name2Window.at(windowIter->second);
                if (window->hasWindowEventCallback(event.window.event))
                  handledByWindowEventCallback =
                      window->callWindowEventCallback(event.window.event,
                                                        event);
              }
              (void)handledByWindowEventCallback;
            }
          }
        }
      }

      if (!pooling)
        if (!SDL_PollEvent(&event)) break;
    }
    if (hasIdleCallback()) callIdleCallback();
  }
}

void MainLoop::stop(){
  running = false;
}

/**
 * @brief sets idle callback
 *
 * @param callback idle callback
 */
void MainLoop::setIdleCallback(function<void()> const& callback) {
  idleCallback = callback;
}

/**
 * @brief has idle callback
 *
 * @return true if this main loop has idle callback
 */
bool MainLoop::hasIdleCallback() const {
  return idleCallback != nullptr;
}

/**
 * @brief sets event handler callback
 * Event handler can serve every event, it is usefull for imgui, AntTweakBar
 * Event handlere has to return true if it served particular event
 *
 * @param handler callback
 */
void MainLoop::setEventHandler(
    function<bool(SDL_Event const&)> const& handler) {
  eventHandler = handler;
}

/**
 * @brief This function sets main loop event callback (SDL_QUIT forexample)
 *
 * @param event event type
 * @param fce callback
 */
void MainLoop::setEventCallback(Uint32 event,std::function<bool(SDL_Event const&)> const& fce){
  if(fce == nullptr){
    eventCallbacks.erase(event);
    return;
  }
  eventCallbacks[event] = fce;
}

/**
 * @brief has event handler callback
 *
 * @return event handler callback
 */
bool MainLoop::hasEventHandler() const {
  return eventHandler != nullptr;
}

/**
 * @brief begin iterator of names of registered windows
 *
 * @return begin iterator of names of registered windows
 */
MainLoop::ConstNameIterator MainLoop::nameBegin() const {
  return name2Window.begin();
}

/**
 * @brief end iterator of names of registered windows
 *
 * @return end iterator of names of registered windows
 */
MainLoop::ConstNameIterator MainLoop::nameEnd() const {
  return name2Window.end();
}

/**
 * @brief begin iterator of ids of registered windows
 *
 * @return begin iterator of ids of registerd windows
 */
MainLoop::ConstIdIterator MainLoop::idBegin() const {
  return id2Name.begin();
}

/**
 * @brief end iterator of ids of registered windows
 *
 * @return end iterator of ids of registered windows
 */
MainLoop::ConstIdIterator MainLoop::idEnd() const {
  return id2Name.end();
}

/**
 * @brief gets number of registered windows
 *
 * @return number of registered windows
 */
size_t MainLoop::getNofWindows() const {
  return name2Window.size();
}

void MainLoop::callIdleCallback() {
  assert(idleCallback != nullptr);
  idleCallback();
}

bool MainLoop::callEventHandler(SDL_Event const& event) {
  assert(eventHandler != nullptr);
  return eventHandler(event);
}
