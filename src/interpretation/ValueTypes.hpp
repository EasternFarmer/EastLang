#pragma once
#include <vector>
#include <functional>
#include <regex>
#include "../parsing/ast.hpp"

class Environment;

enum class ValueType {
  Module,
  Empty,
  Break, // for breaking out of the loops
  Continue, // continue out of the loops
  Array,
  Number,
  String,
  Boolean,
  NativeFn,
  Function,
  RegexPattern,
};

class RuntimeVal {
  public:
    ValueType type;
  protected:
    RuntimeVal(ValueType t) {
      type = t;
    }
};


class ModuleVal: public RuntimeVal{
  public:
    ModuleVal(): RuntimeVal(ValueType::Module) { }
    Environment* moduleEnv;
};

enum class ModuleName {
  Array,
  Regex,
};

inline ModuleVal* MK_MODULE(Environment* env) {
  ModuleVal* _module = new ModuleVal();
  _module->moduleEnv = env;
  return _module;
}

class EmptyVal: public RuntimeVal{
  public:
    EmptyVal(): RuntimeVal(ValueType::Empty) { }
};

class BreakVal: public RuntimeVal{
  public:
    BreakVal(): RuntimeVal(ValueType::Break) { }
};

class ContinueVal: public RuntimeVal{
  public:
    ContinueVal(): RuntimeVal(ValueType::Continue) { }
};

class ArrayVal: public RuntimeVal{
  public:
    ArrayVal(): RuntimeVal(ValueType::Array) { }
    std::vector<RuntimeVal*> elements;
};

inline ArrayVal* MK_ARRAY(std::vector<RuntimeVal*> elements) {
  ArrayVal* newArray = new ArrayVal();

  newArray->elements = elements;

  return newArray;
}

class NumberVal: public RuntimeVal{
  public:
    NumberVal(): RuntimeVal(ValueType::Number) { }
    double value;
};

inline NumberVal* MK_NUM(double n) {
  NumberVal* newNum = new NumberVal();

  newNum->value = n;

  return newNum;
}

class StringVal: public RuntimeVal{
  public:
    StringVal(): RuntimeVal(ValueType::String) { }
    std::string value;
};

inline StringVal* MK_STRING(std::string n) {
  StringVal* newString = new StringVal();

  newString->value = n;

  return newString;
}

class BooleanVal: public RuntimeVal{
  public:
    BooleanVal(): RuntimeVal(ValueType::Boolean) { }
    bool value;
};

inline BooleanVal* MK_BOOL(bool n) {
  BooleanVal* newBool = new BooleanVal();

  newBool->value = n;

  return newBool;
}

class RegexPattern: public RuntimeVal{
  public:
    RegexPattern(): RuntimeVal(ValueType::RegexPattern) { }
    std::regex pattern;
    std::string original_regex;
};

inline RegexPattern* MK_REGEX(std::string str) {
  RegexPattern* newRegex = new RegexPattern();

  newRegex->pattern = std::regex(str);
  newRegex->original_regex = str;

  return newRegex;
}

class NativeFnVal: public RuntimeVal{
  public:
    NativeFnVal(): RuntimeVal(ValueType::NativeFn) { }
    std::function<RuntimeVal*(std::vector<RuntimeVal*>)> call;
};

inline NativeFnVal* MK_NATIVE_FUNC(
  std::function<RuntimeVal*(std::vector<RuntimeVal*>)> call
) {
  NativeFnVal* newNativeFunc = new NativeFnVal();

  newNativeFunc->call = call;

  return newNativeFunc;
}

class FunctionVal: public RuntimeVal{
  public:
    FunctionVal(): RuntimeVal(ValueType::Function) { }
    std::vector<std::string> parameters;
    Environment* declarationEnv;
    std::vector<Stmt*> body;
};