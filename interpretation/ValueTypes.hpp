#pragma once
#include <vector>
#include <functional>
#include "../parsing/ast.hpp"

class Environment;

enum class ValueType {
  Empty,
  Break, // for breaking out of the loops
  Continue, // continue out of the loops
  Number,
  String,
  Boolean,
  NativeFn,
  Function,
  IfExpr,
};

class RuntimeVal {
  public:
    ValueType type;
  protected:
    RuntimeVal(ValueType t) {
      type = t;
    }
};


class EmptyVal: public RuntimeVal{
  public:
    EmptyVal(): RuntimeVal(ValueType::Empty) {
    }
};

class BreakVal: public RuntimeVal{
  public:
    BreakVal(): RuntimeVal(ValueType::Break) {
    }
};

class ContinueVal: public RuntimeVal{
  public:
    ContinueVal(): RuntimeVal(ValueType::Continue) {
    }
};

class NumberVal: public RuntimeVal{
  public:
    NumberVal(): RuntimeVal(ValueType::Number) {
    }
    double value;
};

inline NumberVal* MK_NUM(double n) {
  NumberVal* newNum = new NumberVal();

  newNum->value = n;

  return newNum;
}

class StringVal: public RuntimeVal{
  public:
    StringVal(): RuntimeVal(ValueType::String) {
    }
    std::string value;
};

inline StringVal* MK_STRING(std::string n) {
  StringVal* newString = new StringVal();

  newString->value = n;

  return newString;
}

class BooleanVal: public RuntimeVal{
  public:
    BooleanVal(): RuntimeVal(ValueType::Boolean) {
    }
    bool value;
};

inline BooleanVal* MK_BOOL(bool n) {
  BooleanVal* newBool = new BooleanVal();

  newBool->value = n;

  return newBool;
}

class NativeFnVal: public RuntimeVal{
  public:
    NativeFnVal(): RuntimeVal(ValueType::NativeFn) {
    }
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
    FunctionVal(): RuntimeVal(ValueType::Function) {
    }
    std::vector<std::string> parameters;
    Environment* declarationEnv;
    std::vector<Stmt*> body;
};