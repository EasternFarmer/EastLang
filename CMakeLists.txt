cmake_minimum_required(VERSION 4.1)
project(EastLang VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  EastLangInterpreter
  src/main.cpp
  src/util.cpp
  src/Errors.cpp
  # parsing
  src/parsing/lexer.cpp
  src/parsing/parser.cpp
  # interpretation
  src/interpretation/interpreter.cpp
  ## Env
  src/interpretation/Environment.cpp
  src/interpretation/GlobalEnv.cpp
  ## built-in modules
  src/interpretation/modules/main.cpp
  src/interpretation/modules/array/arrayModule.cpp
  src/interpretation/modules/regex/regexModule.cpp
)

set(CPACK_PACKAGE_NAME "EastLang")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EastLang Interpreter")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_GENERATOR "ZIP")

include(CPack)

install(TARGETS EastLangInterpreter)