#include "GlobalEnv.hpp"
#include "../Errors.hpp"
#include "Environment.hpp"

#define CONST_PI 3.14159265358979323846
#define CONST_E 2.71828182845904523536

#define NATIVE_FN(name) RuntimeVal* name(std::vector<RuntimeVal*> args)

#ifdef _WIN32
    #include <windows.h>

    void sleep_(unsigned milliseconds)
    {
        Sleep(milliseconds);
    }
#else
    #include <unistd.h>

    void sleep_(unsigned milliseconds)
    {
        usleep(milliseconds * 1000); // takes microseconds
    }
#endif


void print_runtime_val(RuntimeVal* var) {
  switch (var->type) {
    case ValueType::Boolean: {
      BooleanVal* Bool = static_cast<BooleanVal*>(var);
      if (Bool->value) {
        std::cout << "true";
      } else {
        std::cout << "false";
      }
      break;
    }
    case ValueType::Empty: {
      std::cout << "Empty";
      break;
    }
    case ValueType::Number: {
      NumberVal* Num = static_cast<NumberVal*>(var);
      std::cout << Num->value;
      break;
    }
    case ValueType::Function: {
      FunctionVal* Func = static_cast<FunctionVal*>(var);
      std::cout << "callable(";
      for (auto param : Func->parameters) {
        std::cout << param << ", ";
      }
      std::cout << ")";
      break;
    }
    case ValueType::Array: {
      ArrayVal* Func = static_cast<ArrayVal*>(var);
      std::cout << "[";
      for (auto param : Func->elements) {
        print_runtime_val(param);
        std::cout << ", ";
      }
      std::cout << "]";
      break;
    }
    case ValueType::NativeFn: {
      std::cout << "built-in";
      break;
    }
    case ValueType::String: {
      StringVal* str = static_cast<StringVal*>(var);
      std::cout << str->value;
      break;
    }
    case ValueType::Module: {
      std::cout << "module";
      break;
    }
    default:
      raise_error("invalid runtime type");
  }
}

NATIVE_FN(print) {
  for (auto val : args) {
    // std::string str = static_cast<StringVal*>(val)->value; // everything has a value field
    print_runtime_val(val);
    std::cout << " ";
  }
  std::cout << "\n";
  return new EmptyVal();
}

NATIVE_FN(type) {
  if (args.size() > 1) {
    raise_error("Too Many Arguments to the type built-in");
  } else if (args.size() < 1) {
    raise_error("Too Few Arguments to the type built-in");
  }
  switch (args[0]->type) {
    case ValueType::Boolean: {
      return MK_STRING("bool");
    }
    case ValueType::Empty: {
      return MK_STRING("empty");
    }
    case ValueType::Number: {
      return MK_STRING("number");
    }
    case ValueType::Function: {
      return MK_STRING("callable");
    }
    case ValueType::NativeFn: {
      return MK_STRING("built-in");
    }
    case ValueType::String: {
      return MK_STRING("string");
    }
    case ValueType::Array: {
      return MK_STRING("array");
    }
    case ValueType::Module: {
      return MK_STRING("module");
    }
    default:
      raise_error("invalid runtime type");
  }
}

NATIVE_FN(sleep2) {
  if (args.size() > 1) {
    raise_error("Too Many Arguments to the sleep built-in");
  } else if (args.size() < 1) {
    raise_error("Too Few Arguments to the sleep built-in");
  } else if (args[0]->type != ValueType::Number) {
    raise_error("sleep built-in requires Numbers");
  }
  NumberVal* Num = static_cast<NumberVal*>(args[0]);

  sleep_(Num->value * 1000); // *1000 to take in s instead of ms

  return new EmptyVal();
}

NATIVE_FN(DEBUG_list_all) {
  for (int i = 0; i <= std::stoi("777", nullptr, 8); i++) {
    std::cout << std::oct << i << ": " << static_cast<char>(i) << '\n';
  }
  return new EmptyVal();
}

NATIVE_FN(input) {
  if (args.size() > 0) {
    for (auto arg : args) {
      print_runtime_val(arg);
      std::cout << "";
    }
  }
  std::string out;
  std::getline(std::cin, out);
  return MK_STRING(out);
}

NATIVE_FN(ord) {
  if (args[0]->type != ValueType::String)
    raise_error("ord can only be used with a one lenght string");

  StringVal* str = static_cast<StringVal*>(args[0]);

  if (str->value.length() != 1)
    raise_error("ord can only be used with a one lenght string");

  return MK_NUM(str->value.at(0));
}

NATIVE_FN(chr) {
  if (args[0]->type != ValueType::Number)
    raise_error("chr can only be used with a number");

  NumberVal* num = static_cast<NumberVal*>(args[0]);
  try {
    return MK_STRING(std::string(1, (char)((int)num->value)));
  } catch (const std::exception& e) {
    raise_error("couldn't turn number to char\n");
  }
}

NATIVE_FN(dir) {
  if (args.size() != 1)
    raise_error("Expected only one argument");

  if (args[0]->type != ValueType::Module)
    raise_error("currently only Modules have members");

  ModuleVal* _module = static_cast<ModuleVal*>(args[0]);

  std::vector<RuntimeVal*> values;
  for (auto var : _module->moduleEnv->values) {
    values.push_back(MK_STRING(var.first));
  }
  return MK_ARRAY(values);
}

Environment* makeGlobalEnv() {
  Environment* env = new Environment(nullptr);

  env->declareVar("true", MK_BOOL(true), true);
  env->declareVar("false", MK_BOOL(false), true);

  env->declareVar("empty", new EmptyVal(), true);
  env->declareVar("break", new BreakVal(), true);
  env->declareVar("continue", new ContinueVal(), true);

  env->declareVar("pi", MK_NUM(CONST_PI), true);
  env->declareVar("e", MK_NUM(CONST_E), true);

  env->declareVar("print", MK_NATIVE_FUNC(print), true);
  env->declareVar("type", MK_NATIVE_FUNC(type), true);
  env->declareVar("sleep", MK_NATIVE_FUNC(sleep2), true);
  env->declareVar("input", MK_NATIVE_FUNC(input), true);

  env->declareVar("dir", MK_NATIVE_FUNC(dir), true);
  env->declareVar("ord", MK_NATIVE_FUNC(ord), true);
  env->declareVar("chr", MK_NATIVE_FUNC(chr), true);

  env->declareVar("DEBUG_list_all", MK_NATIVE_FUNC(DEBUG_list_all), true);

  return env;
}