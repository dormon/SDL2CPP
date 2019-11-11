cmake_minimum_required(VERSION 3.13.0)

project(
  SDL2CPP
  VERSION 1.1.0
  )

#set these variables to *.cpp, *.c, ..., *.h, *.hpp, ...
set(SOURCES 
  src/${PROJECT_NAME}/Window.cpp
  src/${PROJECT_NAME}/MainLoop.cpp
  )
set(PRIVATE_INCLUDES )
set(PUBLIC_INCLUDES 
  src/${PROJECT_NAME}/Fwd.h
  src/${PROJECT_NAME}/Window.h
  src/${PROJECT_NAME}/MainLoop.h
  src/${PROJECT_NAME}/Exception.h
  )
set(INTERFACE_INCLUDES )

#list of libraries to find
#it should contain list of "lists"
#format:
#set(ExternLibraries
#  A\\ B\\ C\\ D
#  E\\ F\\ G
#  )
#It will be converted into:
#find_package(A B C D)
#find_package(E F G)
#If version is specified, it has to be the second parameter (B)
set(ExternPrivateLibraries )
set(ExternPublicLibraries SDL2\\ 2.0.9\\ CONFIG\\ REQUIRED)
set(ExternInterfaceLibraries )

#set these variables to targets
set(PrivateTargets )
set(PublicTargets SDL2::SDL2 SDL2::SDL2main)
set(InterfaceTargets )

#set these libraries to variables that are provided by libraries that does not support configs
set(PrivateIncludeVariables )
set(PublicIncludeVariables )
set(InterfaceIncludeVariables )
set(PrivateReleaseLibraryVariables )
set(PublicReleaseLibraryVariables )
set(InterfaceReleaseLibraryVariables )
set(PrivateDebugLibraryVariables )
set(PublicDebugLibraryVariables )
set(InterfaceDebugLibraryVariables )

SET(CMAKE_CXX_STANDARD 14)

include(CMakeUtils.cmake)
