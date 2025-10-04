#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include "ValueTypes.hpp"

class Environment {
  private:
    Environment* parentEnv;
  public:
    // for build-ins
    std::set<std::string> constants;
    std::unordered_map<std::string, RuntimeVal*> values;

    Environment(Environment* pe = nullptr);
    RuntimeVal* declareVar(std::string varname, RuntimeVal* value, bool constant = false);
    RuntimeVal* overrideVar(std::string varname, RuntimeVal* value);
    RuntimeVal* assignVar(std::string varname, RuntimeVal* value, bool local = false);
    RuntimeVal* lookupVar(std::string varname);
    Environment* resolve(std::string varname);
};