#include "main.hpp"
#include "array/arrayModule.hpp"

Environment* importBuiltInModule(ModuleName moduleName) {
  switch (moduleName) {
    case ModuleName::Array: {
      return makeArrayModule();
    }
    default:
     raise_error("invalid module name");
  }
}