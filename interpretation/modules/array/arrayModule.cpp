#include "arrayModule.hpp"
#include "../../../Errors.hpp"

#define NATIVE_FN(name) RuntimeVal* name(std::vector<RuntimeVal*> args)

NATIVE_FN(len) {
  if (args.size() != 1)
    raise_error("Expected exactly one argument to array.len");
  if (args[0]->type != ValueType::Array)
    raise_error("Expected an array type");

  return MK_NUM(static_cast<ArrayVal*>(args[0])->elements.size());
}

NATIVE_FN(append) {
  if (args.size() < 2)
    raise_error("Expected at least two arguments to array.append");

  if (args[0]->type != ValueType::Array)
    raise_error("Expected the first argument to be of type Array in array.append");

  ArrayVal* array = static_cast<ArrayVal*>(args[0]);
  
  for (int i = 1; i < args.size(); i++) {
    array->elements.push_back(args[i]);
  }

  return array;
}

NATIVE_FN(pop) {
  if (args.size() != 1)
    raise_error("Expected exactly one argument to array.pop");

  if (args[0]->type != ValueType::Array)
    raise_error("Expected the first argument to be of type Array in array.pop");

  ArrayVal* array = static_cast<ArrayVal*>(args[0]);

  RuntimeVal* value = array->elements.back();
  array->elements.pop_back();
  return value;
}

Environment* makeArrayModule() {
  Environment* _module = new Environment();

  _module->declareVar("len", MK_NATIVE_FUNC(len), true);
  _module->declareVar("append", MK_NATIVE_FUNC(append), true);
  _module->declareVar("pop", MK_NATIVE_FUNC(pop), true);

  return _module;
}