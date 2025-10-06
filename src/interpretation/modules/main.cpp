#include "main.hpp"
#include "array/arrayModule.hpp"
#include "regex/regexModule.hpp"

Environment* importBuiltInModule(ModuleName moduleName) {
  switch (moduleName) {
    case ModuleName::Array: {
      return makeArrayModule();
    }
    case ModuleName::Regex: {
      return makeRegexModule();
    }
    default:
     raise_error("invalid module name");
  }
}