#pragma once

#include <sstream>
#include <string>

#include <SDL2CPP/Fwd.h>
#include <SDL2CPP/sdl2cpp_export.h>

class sdl2cpp::Exception {
 public:
  Exception(std::string const& msg) : message(msg) {}
  virtual ~Exception() {}
  virtual std::string what() const { return decorator() + " - " + message; }

 protected:
  std::string decorator() const { return "SDL2CPP"; }
  std::string message;
};

class sdl2cpp::ClassException : public Exception {
 public:
  ClassException(std::string const& className, std::string const& msg)
      : Exception(msg), className(className)
  {
  }
  virtual std::string what() const override
  {
    return decorator() + "::" + className + " - " + message;
  }

 protected:
  std::string className;
};

class sdl2cpp::WindowException : public ClassException {
 public:
  WindowException(std::string const& w) : ClassException("Window", w) {}
};

class sdl2cpp::WindowFunctionException : public WindowException {
 public:
  WindowFunctionException(std::string const& fceName, std::string const& msg)
      : WindowException(msg), functionName(fceName)
  {
  }
  virtual std::string what() const override
  {
    return decorator() + "::" + className + "::" + functionName + "() - " +
           message;
  }

 protected:
  std::string functionName;
};

class sdl2cpp::CreateContext : public WindowFunctionException {
 public:
  CreateContext(std::string const& w)
      : WindowFunctionException(w, "createContext")
  {
  }
};
