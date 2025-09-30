#include "GlobalEnv.hpp"
#include "../Errors.hpp"
#include "Environment.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

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
      std::cout << Bool->value;
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
    case ValueType::NativeFn: {
      std::cout << "built-in";
      break;
    }
    case ValueType::String: {
      StringVal* str = static_cast<StringVal*>(var);
      std::cout << str->value;
      break;
    }
    default:
      raise_error("invalid runtime type");
  }
}

RuntimeVal* print(std::vector<RuntimeVal*> args) {
  for (auto val : args) {
    // std::string str = static_cast<StringVal*>(val)->value; // everything has a value field
    print_runtime_val(val);
    std::cout << " ";
  }
  std::cout << "\n";
  return new EmptyVal();
}

RuntimeVal* type(std::vector<RuntimeVal*> args) {
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
    default:
      raise_error("invalid runtime type");
  }
}

RuntimeVal* sleep2(std::vector<RuntimeVal*> args) {
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

RuntimeVal* input(std::vector<RuntimeVal*> args) {
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

// comparison funcs

RuntimeVal* not_equal(std::vector<RuntimeVal*> args) {
  if (args.size() > 2) {
    raise_error("Too Many Arguments to the built-in");
  } else if (args.size() < 2) {
    raise_error("Too Few Arguments to the built-in");
  } 
  
  if (
    args[0]->type == ValueType::Number && args[1]->type == ValueType::Number
  ) {
    double left = static_cast<NumberVal*>(args[0])->value;
    double right = static_cast<NumberVal*>(args[1])->value;
  
    if (left == right) {
      return MK_BOOL(true);
    }
    return MK_BOOL(false);
  } else if (
    args[0]->type == ValueType::String && args[1]->type == ValueType::String
  ) {
    std::string left = static_cast<StringVal*>(args[0])->value;
    std::string right = static_cast<StringVal*>(args[1])->value;
    if (left != right) {
      return MK_BOOL(true);
    }
    return MK_BOOL(false);
  } else if (
    args[0]->type == ValueType::Boolean && args[1]->type == ValueType::Boolean
  ) {
    bool left = static_cast<BooleanVal*>(args[0])->value;
    bool right = static_cast<BooleanVal*>(args[1])->value;
    if (left != right) {
      return MK_BOOL(true);
    }
    return MK_BOOL(false);
  } else if (
    args[0]->type == ValueType::Empty && args[1]->type == ValueType::Empty
  ) {
    return MK_BOOL(false);
  }
  return MK_BOOL(true);
}

RuntimeVal* equal(std::vector<RuntimeVal*> args) {
  if (args.size() > 2) {
    raise_error("Too Many Arguments to the built-in");
  } else if (args.size() < 2) {
    raise_error("Too Few Arguments to the built-in");
  } 
  
  if (
    args[0]->type == ValueType::Number && args[1]->type == ValueType::Number
  ) {
    double left = static_cast<NumberVal*>(args[0])->value;
    double right = static_cast<NumberVal*>(args[1])->value;
  
    if (left == right) {
      return MK_BOOL(true);
    }
    return MK_BOOL(false);
  } else if (
    args[0]->type == ValueType::String && args[1]->type == ValueType::String
  ) {
    std::string left = static_cast<StringVal*>(args[0])->value;
    std::string right = static_cast<StringVal*>(args[1])->value;
    if (left == right) {
      return MK_BOOL(true);
    }
    return MK_BOOL(false);
  } else if (
    args[0]->type == ValueType::Boolean && args[1]->type == ValueType::Boolean
  ) {
    bool left = static_cast<BooleanVal*>(args[0])->value;
    bool right = static_cast<BooleanVal*>(args[1])->value;
    if (left == right) {
      return MK_BOOL(true);
    }
    return MK_BOOL(false);
  } else if (
    args[0]->type == ValueType::Empty && args[1]->type == ValueType::Empty
  ) {
    return MK_BOOL(true);
  }
  std::cout << "111 ";
  return MK_BOOL(false);
}

RuntimeVal* greater(std::vector<RuntimeVal*> args) {
  if (args.size() > 2) {
    raise_error("Too Many Arguments to the built-in");
  } else if (args.size() < 2) {
    raise_error("Too Few Arguments to the built-in");
  } else if (args[0]->type != ValueType::Number) {
    raise_error("built-in requires Numbers");
  } else if (args[1]->type != ValueType::Number) {
    raise_error("built-in requires Numbers");
  }
  double left = static_cast<NumberVal*>(args[0])->value;
  double right = static_cast<NumberVal*>(args[1])->value;

  if (left > right) {
    return MK_BOOL(true);
  }
  return MK_BOOL(false);
}

RuntimeVal* less(std::vector<RuntimeVal*> args) {
  if (args.size() > 2) {
    raise_error("Too Many Arguments to the built-in");
  } else if (args.size() < 2) {
    raise_error("Too Few Arguments to the built-in");
  } else if (args[0]->type != ValueType::Number) {
    raise_error("built-in requires Integers or Floats");
  } else if (args[1]->type != ValueType::Number) {
    raise_error("built-in requires Integers or Floats");
  }
  double left = static_cast<NumberVal*>(args[0])->value;
  double right = static_cast<NumberVal*>(args[1])->value;

  if (left < right) {
    return MK_BOOL(true);
  }
  return MK_BOOL(false);
}

RuntimeVal* greater_equal(std::vector<RuntimeVal*> args) {
  if (args.size() > 2) {
    raise_error("Too Many Arguments to the built-in");
  } else if (args.size() < 2) {
    raise_error("Too Few Arguments to the built-in");
  } else if (args[0]->type != ValueType::Number) {
    raise_error("built-in requires Integers or Floats");
  } else if (args[1]->type != ValueType::Number) {
    raise_error("built-in requires Integers or Floats");
  }
  double left = static_cast<NumberVal*>(args[0])->value;
  double right = static_cast<NumberVal*>(args[1])->value;

  if (left >= right) {
    return MK_BOOL(true);
  }
  return MK_BOOL(false);
}

RuntimeVal* less_equal(std::vector<RuntimeVal*> args) {
  if (args.size() > 2) {
    raise_error("Too Many Arguments to the built-in");
  } else if (args.size() < 2) {
    raise_error("Too Few Arguments to the built-in");
  } else if (args[0]->type != ValueType::Number) {
    raise_error("built-in requires Integers or Floats");
  } else if (args[1]->type != ValueType::Number) {
    raise_error("built-in requires Integers or Floats");
  }
  double left = static_cast<NumberVal*>(args[0])->value;
  double right = static_cast<NumberVal*>(args[1])->value;

  if (left <= right) {
    return MK_BOOL(true);
  }
  return MK_BOOL(false);
}

Environment* makeGlobalEnv() {
  Environment* env = new Environment(nullptr);

  env->declareVar("true", MK_BOOL(true), true);
  env->declareVar("false", MK_BOOL(false), true);
  
  env->declareVar("empty", new EmptyVal(), true);
  env->declareVar("break", new BreakVal(), true);
  
  env->declareVar("pi", MK_NUM(M_PI), true);
  env->declareVar("e", MK_NUM(M_E), true);

  env->declareVar("print", MK_NATIVE_FUNC(print), true);
  env->declareVar("type", MK_NATIVE_FUNC(type), true);
  env->declareVar("sleep", MK_NATIVE_FUNC(sleep2), true); // broken
  env->declareVar("input", MK_NATIVE_FUNC(input), true);

  // comparison funcs
  env->declareVar("not_equal", MK_NATIVE_FUNC(not_equal), true);
  env->declareVar("equal", MK_NATIVE_FUNC(equal), true);
  env->declareVar("greater", MK_NATIVE_FUNC(greater), true);
  env->declareVar("less", MK_NATIVE_FUNC(less), true);
  env->declareVar("greater_equal", MK_NATIVE_FUNC(greater_equal), true);
  env->declareVar("less_equal", MK_NATIVE_FUNC(less_equal), true);

  return env;
}