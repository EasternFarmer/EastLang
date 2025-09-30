cmake_minimum_required(VERSION 4.1)
project(EastLang VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  EastLangInterpreter
  main.cpp
  util.cpp
  Errors.cpp
  parsing/lexer.cpp
  parsing/parser.cpp
  interpretation/Environment.cpp
  interpretation/GlobalEnv.cpp
  interpretation/interpreter.cpp
)

set(CPACK_PACKAGE_NAME "EastLang")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EastLang Interpreter")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_GENERATOR "ZIP")

include(CPack)

install(TARGETS EastLangInterpreter)