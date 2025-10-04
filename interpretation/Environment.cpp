#include <string>
#include <set>
#include <unordered_map>
#include "ValueTypes.hpp"
#include "Environment.hpp"
#include "../Errors.hpp"
#include "GlobalEnv.hpp"

Environment::Environment(Environment* pe) {
  parentEnv = pe;
}

RuntimeVal* Environment::declareVar(std::string varname, RuntimeVal* value, bool constant) {
  if (values.find(varname) != values.end()) { // value exists
    raise_error("Can't declare a value that already exists");
  }
  if (constant) {
    constants.insert(varname);
  }
  values[varname] = value;
  return value;
};

RuntimeVal* Environment::overrideVar(std::string varname, RuntimeVal* value) {
  Environment* env = resolve(varname);

  env->values[varname] = value;

  return value;
};

RuntimeVal* Environment::assignVar(std::string varname, RuntimeVal* value) {
  Environment* env = resolve(varname);

  if (env == nullptr || env->values.find(varname) == env->values.end()) { // if undefined
    return this->declareVar(varname, value);
  }
  if (env->constants.find(varname) != env->constants.end()) { // if defined AND constant
    raise_error("Attempted assignment on a constant variable");
  }
  env->values[varname] = value;

  return value;
};

RuntimeVal* Environment::lookupVar(std::string varname) {
  auto env = this->resolve(varname);
  if (env == nullptr) {
    raise_error("Variable " + varname + " doesn't exist");
  }

  auto value = env->values.find(varname);

  return value->second;
};

Environment* Environment::resolve(std::string varname) {
  if (values.find(varname) == values.end()) { // no value in this scope
    if (parentEnv != nullptr) {
      return parentEnv->resolve(varname); // look in the parent env
    } else {
      return nullptr; // return nullptr if no above scope is found 
    }
  }
  return this; // we found the variable in this scope so we return 
};