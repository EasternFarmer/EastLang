#include "regexModule.hpp"
#include "../../../Errors.hpp"

#define NATIVE_FN(name) RuntimeVal* name(std::vector<RuntimeVal*> args)

NATIVE_FN(compile) {
  if (args.size() != 1)
    raise_error("Expected exactly one argument to regex.compile");
  if (args[0]->type != ValueType::String)
    raise_error("Expected a String type to regex.compile");

  try {
    return MK_REGEX(static_cast<StringVal*>(args[0])->value);
  } catch (std::regex_error& err) {
    raise_error("Regex complitation error: " + (std::string)err.what());
  }
}

NATIVE_FN(match) {
  if (args.size() != 2)
    raise_error("Expected two arguments to regex.match");

  RegexPattern* regexPattern;
  if (args[0]->type == ValueType::RegexPattern) {
    regexPattern = static_cast<RegexPattern*>(args[0]);
  } else if (args[0]->type == ValueType::String) {
    regexPattern = MK_REGEX(static_cast<StringVal*>(args[0])->value);
  } else {
    raise_error("Expected a String or RegexPattern type to the 1st argument in to regex.match");
  }
  
  if (args[1]->type != ValueType::String) {
    raise_error("Expected a String type to the 2nd argument in regex.match");
  }

  std::smatch match;
  std::regex_search(static_cast<StringVal*>(args[1])->value, match, regexPattern->pattern);

  std::vector<RuntimeVal*> ret;

  for (auto value : match) {
    ret.push_back(MK_STRING(value));
  }

  return MK_ARRAY(ret);
}

NATIVE_FN(replace) {
  if (args.size() != 3)
    raise_error("Expected tree arguments to regex.replace");

  RegexPattern* regexPattern;
  if (args[0]->type == ValueType::RegexPattern) {
    regexPattern = static_cast<RegexPattern*>(args[0]);
  } else if (args[0]->type == ValueType::String) {
    regexPattern = MK_REGEX(static_cast<StringVal*>(args[0])->value);
  } else {
    raise_error("Expected a String or RegexPattern type to the 1st argument in to regex.replace");
  }
  
  if (args[1]->type != ValueType::String) {
    raise_error("Expected a String type to the 2nd argument in regex.replace");
  }
  
  if (args[2]->type != ValueType::String) {
    raise_error("Expected a String type to the 3rd argument in regex.replace");
  }

  std::string ret = regex_replace(static_cast<StringVal*>(args[1])->value, regexPattern->pattern, static_cast<StringVal*>(args[2])->value);
  return MK_STRING(ret);
}

Environment* makeRegexModule() {
  Environment* _module = new Environment();

  _module->declareVar("compile", MK_NATIVE_FUNC(compile), true);
  _module->declareVar("match", MK_NATIVE_FUNC(match), true);
  _module->declareVar("replace", MK_NATIVE_FUNC(replace), true);

  return _module;
}