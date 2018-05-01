#pragma once

#include <sstream>
#include <string>

#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/sdl2cpp_export.h>

class sdl2cpp::ex::Exception : public std::runtime_error {
 public:
  Exception(std::string const& msg) : std::runtime_error(msg) {}
  virtual ~Exception() throw() {}
  virtual char const* what() const throw() override
  {
    return std::string(std::string("SDL2CPP - ") + std::runtime_error::what())
        .c_str();
  }
};

class sdl2cpp::ex::Class : public Exception {
 public:
  Class(std::string const& className, std::string const& msg)
      : Exception(msg), className(className)
  {
  }
  virtual char const* what() const throw() override
  {
    return std::string(std::string("SDL2CPP::") + className + " - " +
                       std::runtime_error::what())
        .c_str();
  }

 protected:
  std::string className;
};

class sdl2cpp::ex::Window : public Class {
 public:
  Window(std::string const& msg) : Class("Window", msg) {}
};

class sdl2cpp::ex::MainLoop : public Class {
 public:
  MainLoop(std::string const& msg) : Class("MainLoop", msg) {}
};

class sdl2cpp::ex::WindowMethod : public Window {
 public:
  WindowMethod(std::string const& method, std::string const& m)
      : Window(m), methodName(method)
  {
  }
  virtual char const* what() const throw() override
  {
    return std::string(std::string("SDL2CPP::" + className + "::" + methodName +
                                   "() - ") +
                       std::runtime_error::what())
        .c_str();
  }

 protected:
  std::string methodName;
};

class sdl2cpp::ex::CreateContext : public WindowMethod {
 public:
  CreateContext(std::string const& w) : WindowMethod(w, "createContext") {}
};
