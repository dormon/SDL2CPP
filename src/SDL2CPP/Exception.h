#pragma once

#include <sstream>
#include <string>
#include <stdexcept>

#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/sdl2cpp_export.h>

class sdl2cpp::ex::Exception : public std::runtime_error {
 public:
  Exception(std::string const& msg = "") : std::runtime_error(msg) {
    prefix = "SDL2CPP";
    createMessage(msg);
  }
  virtual ~Exception() throw() {}
  virtual char const* what() const throw() override{
    return message.c_str();
  }
  void createMessage(std::string const&msg){
    if(msg == "")return;
    message = prefix + " - " + msg;
  }
  std::string message;
  std::string prefix;
};

class sdl2cpp::ex::Class : public Exception {
 public:
  Class(std::string const& className, std::string const& msg)
      : Exception()
  {
    prefix += "::" + className;
    createMessage(msg);
  }
 protected:
};

class sdl2cpp::ex::Window : public Class {
 public:
  Window(std::string const& msg = "") : Class("Window", msg) {}
};

class sdl2cpp::ex::MainLoop : public Class {
 public:
  MainLoop(std::string const& msg = "") : Class("MainLoop", msg) {}
};

class sdl2cpp::ex::MainLoopMethod : public MainLoop {
 public:
  MainLoopMethod(std::string const& method, std::string const& msg)
      : MainLoop()
  {
    prefix += "::" + method + "()";
    createMessage(msg);
  }
};

class sdl2cpp::ex::WindowMethod : public Window {
 public:
  WindowMethod(std::string const& method, std::string const& msg)
      : Window()
  {
    prefix += "::" + method + "()";
    createMessage(msg);
  }
};

class sdl2cpp::ex::CreateContext : public WindowMethod {
 public:
  CreateContext(std::string const& msg) : WindowMethod("createContext",msg) {}
};
