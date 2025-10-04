#include "interpreter.hpp"
#include "../Errors.hpp"
#include "../util.hpp"
#include "../parsing/parser.hpp" // @import()
#include "GlobalEnv.hpp"
#include <cmath>
#include "modules/main.hpp"


RuntimeVal* evaluate(Stmt* astNode, Environment* env) {
  switch (astNode->kind) {
    case NodeType::Program: {
      return eval_program(static_cast<Program*>(astNode), env);
    }
    case NodeType::AssignmentExpr: {
      return eval_assignment(static_cast<AssignmentExpr*>(astNode), env);
    }
    case NodeType::BinaryExpr: {
      return eval_binary_expr(static_cast<BinaryExpr*>(astNode), env);
    }
    case NodeType::NumberLiteral: {
      return MK_NUM(static_cast<NumberLiteral*>(astNode)->value);
    }
    case NodeType::ArrayLiteral: {
      ArrayLiteral* arrayExpr = static_cast<ArrayLiteral*>(astNode);

      ArrayVal* array = new ArrayVal();
      for (auto elem : arrayExpr->elements) {
        array->elements.push_back(evaluate(elem, env));
      }

      return array;
    }
    case NodeType::StringLiteral: {
      return MK_STRING(static_cast<StringLiteral*>(astNode)->value);
    }
    case NodeType::Identifier: {
      return env->lookupVar(static_cast<Identifier*>(astNode)->value);
    }
    case NodeType::CallExpr: {
      return eval_call_expr(static_cast<CallExpr*>(astNode), env);
    }
    case NodeType::IfStatement: {
      return eval_if_expr(static_cast<IfStatement*>(astNode), env);
    }
    case NodeType::WhileStatement: {
      return eval_while_expr(static_cast<WhileStatement*>(astNode), env);
    }
    case NodeType::FunctionDeclaration: {
      FunctionDeclaration* funcDec = static_cast<FunctionDeclaration*>(astNode);

      FunctionVal* func = new FunctionVal();

      func->declarationEnv = env;
      func->parameters = funcDec->parameters;
      func->body = funcDec->body;

      return func;
    }
    case NodeType::VariableDeclaration: {
      VariableDeclaration* varDec = static_cast<VariableDeclaration*>(astNode);
      return env->declareVar(varDec->identifier, evaluate(varDec->value, env), varDec->constant);
    }
    case NodeType::NegateExpr: {
      NegateExpr* negExpr = static_cast<NegateExpr*>(astNode);

      RuntimeVal* ret = evaluate(negExpr->expr, env);

      if (ret->type == ValueType::Boolean) {
        BooleanVal* boolVal = static_cast<BooleanVal*>(ret);
        return MK_BOOL(!(boolVal->value));

      } else if (ret->type == ValueType::Number) {
        NumberVal* numVal = static_cast<NumberVal*>(ret);
        return MK_BOOL(!(numVal->value));

      } else if (ret->type == ValueType::Empty) {
        return MK_BOOL(true);
      }
      return MK_BOOL(false);
    }
    case NodeType::ComparisonExpr: {
      ComparisonExpr* compExpr = static_cast<ComparisonExpr*>(astNode);

      auto* left = evaluate(compExpr->left, env);
      auto* right = evaluate(compExpr->right, env);
      auto op = compExpr->op;

      return eval_comparison_expr(left, right, op);
    }
    case NodeType::LogicalExpr: {
      LogicalExpr* logExpr = static_cast<LogicalExpr*>(astNode);

      auto* left = evaluate(logExpr->left, env);
      auto* right = evaluate(logExpr->right, env);
      auto op = logExpr->op;

      return eval_logical_expr(left, right, op);
    }
    case NodeType::SubscriptExpr: {
      SubscriptExpr* subExpr = static_cast<SubscriptExpr*>(astNode);

      RuntimeVal* left = evaluate(subExpr->left, env);

      RuntimeVal* index = evaluate(subExpr->value, env);

      switch (left->type) {
        case ValueType::Array: {
          ArrayVal* arrayExpr = static_cast<ArrayVal*>(left);

          if (index->type != ValueType::Number) raise_error("array index must be a number");

          int arrayIndex = (int)(static_cast<NumberVal*>(index)->value);

          return arrayExpr->elements[arrayIndex];
        }
        case ValueType::String: {
          StringVal* stringExpr = static_cast<StringVal*>(left);

          if (index->type != ValueType::Number) raise_error("string index must be a number");

          int stringIndex = (int)(static_cast<NumberVal*>(index)->value);

          return MK_STRING(std::string(1, stringExpr->value.at(stringIndex)));
        }
        default:
          raise_error("can't substring this type");
      }
    }
    case NodeType::BitShiftExpr: {
      BitShiftExpr* bitShift = static_cast<BitShiftExpr*>(astNode);

      RuntimeVal* left = evaluate(bitShift->left, env);
      RuntimeVal* right = evaluate(bitShift->right, env);

      if (left->type != ValueType::Number || left->type != ValueType::Number)
        raise_error("can't use bitshift with non-number values");

      auto* leftCast = static_cast<NumberVal*>(left);
      auto* rightCast = static_cast<NumberVal*>(right);

      if (bitShift->shiftRight) {
        return MK_NUM((int)leftCast->value >> (int)rightCast->value);
      } else {
        return MK_NUM((int)leftCast->value << (int)rightCast->value);
      }
    }
    case NodeType::SpecialExpr: {
      return eval_special_expr(static_cast<SpecialExpr*>(astNode), env);
    }
    case NodeType::MemberExpr: {
      return eval_member_expr(static_cast<MemberExpr*>(astNode), env);
    }
    default:
      print_node_type(astNode->kind);
      raise_error("invalid Node Type");
  }
}

RuntimeVal* eval_member_expr(MemberExpr* memberExpr, Environment* env) {
  RuntimeVal* left = evaluate(memberExpr->left, env);
  if (left->type == ValueType::Module) {
    ModuleVal* moduleVal = static_cast<ModuleVal*>(left);
    return moduleVal->moduleEnv->lookupVar(memberExpr->identifier);
    
  } else {
    raise_error("Unsuported type for member (dot) Expr");
  }
}

ModuleName strToModuleName(std::string name) {
  if (name == "<array>") {
    return ModuleName::Array;
  } else {
    raise_error("Invalid built-in module name: "+ name);
  }
}

RuntimeVal* eval_special_expr(SpecialExpr* specialExpr, Environment* env) {
  if (specialExpr->identifier == "import") {
    if (specialExpr->args.size() != 1) {
      raise_error("cant import more than 1 module in a single expresion");
    }

    std::string moduleName = static_cast<StringLiteral*>(specialExpr->args[0])->value;

    if (moduleName.at(0) == '<') {
      return MK_MODULE(importBuiltInModule(strToModuleName(moduleName)));
    }
    
    ModuleVal* moduleVal = new ModuleVal();
    moduleVal->moduleEnv = makeGlobalEnv();

    std::string sourceCode = read_file((pwd() + "/" + moduleName).c_str());

    evaluate(Parser().parse_ast(sourceCode), moduleVal->moduleEnv);

    return moduleVal;

  } else {
    raise_error("Invalid special Expr identifier");
  }
}

RuntimeVal* eval_comparison_expr(RuntimeVal* left, RuntimeVal* right, ComparisonOperatorType op) {
  switch (op) {
    case ComparisonOperatorType::equal: {
      if (left->type == ValueType::Boolean && right->type == ValueType::Boolean) {
        return MK_BOOL(
          static_cast<BooleanVal*>(left)->value == static_cast<BooleanVal*>(right)->value
        );
      } else if (left->type == ValueType::Number && right->type == ValueType::Number) {
        return MK_BOOL(
          static_cast<NumberVal*>(left)->value == static_cast<NumberVal*>(right)->value
        );
      } else if (left->type == ValueType::String && right->type == ValueType::String) {
        return MK_BOOL(
          static_cast<StringVal*>(left)->value == static_cast<StringVal*>(right)->value
        );
      } else if (left->type == ValueType::Empty && right->type == ValueType::Empty) {
        return MK_BOOL(true);
      }
      return MK_BOOL(false);
    }
    case ComparisonOperatorType::not_equal: {
      if (left->type == ValueType::Boolean && right->type == ValueType::Boolean) {
        return MK_BOOL(
          static_cast<BooleanVal*>(left)->value != static_cast<BooleanVal*>(right)->value
        );
      } else if (left->type == ValueType::Number && right->type == ValueType::Number) {
        return MK_BOOL(
          static_cast<NumberVal*>(left)->value != static_cast<NumberVal*>(right)->value
        );
      } else if (left->type == ValueType::String && right->type == ValueType::String) {
        return MK_BOOL(
          static_cast<StringVal*>(left)->value != static_cast<StringVal*>(right)->value
        );
      } else if (left->type == ValueType::Empty && right->type == ValueType::Empty) {
        return MK_BOOL(false);
      }
      return MK_BOOL(true);
    }
    case ComparisonOperatorType::greater: {
      if (left->type == ValueType::Number && right->type == ValueType::Number) {
        return MK_BOOL(
          static_cast<NumberVal*>(left)->value > static_cast<NumberVal*>(right)->value
        );
      }
      raise_error("can't use '>' on this data type");
    }
    case ComparisonOperatorType::greater_equal: {
      if (left->type == ValueType::Number && right->type == ValueType::Number) {
        return MK_BOOL(
          static_cast<NumberVal*>(left)->value >= static_cast<NumberVal*>(right)->value
        );
      }
      raise_error("can't use '>=' on this data type");
    }
    case ComparisonOperatorType::less: {
      if (left->type == ValueType::Number && right->type == ValueType::Number) {
        return MK_BOOL(
          static_cast<NumberVal*>(left)->value < static_cast<NumberVal*>(right)->value
        );
      }
      raise_error("can't use '<' on this data type");
    }
    case ComparisonOperatorType::less_equal: {
      if (left->type == ValueType::Number && right->type == ValueType::Number) {
        return MK_BOOL(
          static_cast<NumberVal*>(left)->value <= static_cast<NumberVal*>(right)->value
        );
      }
      raise_error("can't use '<=' on this data type");
    }
    default:
      raise_error("invalid operator");
  }
}

bool eval_runtimeval_to_bool(RuntimeVal* var) {
  switch (var->type) {
    case ValueType::Boolean:
      return static_cast<BooleanVal*>(var)->value;
    case ValueType::Number:
      return static_cast<NumberVal*>(var)->value != 0.0;
    case ValueType::String:
      return (static_cast<StringVal*>(var)->value).length() > 0;
    case ValueType::Function:
      return true;
    case ValueType::NativeFn:
      return true;
    case ValueType::Empty:
      return false;
    default:
      raise_error("invalid RuntimeVal value in logical expr");
  }
}

RuntimeVal* eval_logical_expr(RuntimeVal* left, RuntimeVal* right, LogicalOperatorType op) {
  switch (op) {
    case LogicalOperatorType::And: {
      return MK_BOOL(
        eval_runtimeval_to_bool(left)
        && eval_runtimeval_to_bool(right)
      );
    }
    case LogicalOperatorType::Or: {
      return MK_BOOL(
        eval_runtimeval_to_bool(left)
        || eval_runtimeval_to_bool(right)
      );
    }
    case LogicalOperatorType::Xor: {
      return MK_BOOL(
        eval_runtimeval_to_bool(left)
        ^ eval_runtimeval_to_bool(right)
      );
    }
    default:
      raise_error("Invalid Logical operator");
  }
}

RuntimeVal* eval_call_expr(CallExpr* callexpr, Environment* env) {
  RuntimeVal* caller = evaluate(callexpr->caller, env);

  if (caller->type == ValueType::NativeFn) {
    NativeFnVal* nativefn = static_cast<NativeFnVal*>(caller);
    return nativefn->call(eval_args(callexpr->args, env));
  }
  if (caller->type == ValueType::Function) {
    FunctionVal* func = static_cast<FunctionVal*>(caller);

    Environment* scope = new Environment(func->declarationEnv);
    std::vector<RuntimeVal*> args = eval_args(callexpr->args, env);

    for (int i = 0; i < func->parameters.size(); i++) {
      scope->declareVar(func->parameters[i], args[i], false);
    }

    RuntimeVal* last_returned = new EmptyVal();
    for (auto stmt : func->body) {
      last_returned = evaluate(stmt, scope);
    }
    return last_returned;
  }
  raise_error("Cannot call a non-callable value");
}

RuntimeVal* eval_if_expr(IfStatement* ifExpr, Environment* env) {

  Environment* scope = new Environment(env);
  RuntimeVal* checkRet = evaluate(ifExpr->check, env);

  if (checkRet->type != ValueType::Boolean) {
    raise_error("if checks only support boolean values");
  }
  bool passed = static_cast<BooleanVal*>(checkRet)->value;
  if (passed) {
    RuntimeVal* last_returned = new EmptyVal();
    for (auto stmt : ifExpr->body) {
      last_returned = evaluate(stmt, scope);
    }
    return last_returned;
  }

  // else if's
  for (auto check_body_pair : ifExpr->else_if_chain) {
    RuntimeVal* elseIfCheckRet = evaluate(check_body_pair.first, env);

    bool elseIfpassed = static_cast<BooleanVal*>(elseIfCheckRet)->value;
    if (elseIfpassed) {
      RuntimeVal* elseIfLast_returned = new EmptyVal();
      for (auto stmt : check_body_pair.second) {
        elseIfLast_returned = evaluate(stmt, scope);
      }
      return elseIfLast_returned;
    }
  }

  // else
  RuntimeVal* last_returned = new EmptyVal();
  for (auto stmt : ifExpr->else_body) {
    last_returned = evaluate(stmt, scope);
  }
  return last_returned;
}

RuntimeVal* eval_while_expr(WhileStatement* whileExpr, Environment* env) {

  Environment* scope = new Environment(env);
  RuntimeVal* checkRet = evaluate(whileExpr->check, env);

  if (checkRet->type != ValueType::Boolean) {
    raise_error("if checks only support boolean values");
  }
  bool passed = static_cast<BooleanVal*>(checkRet)->value;
  RuntimeVal* last_returned = new EmptyVal();
  bool break_flag = false;
  while (passed) {
    for (auto stmt : whileExpr->body) {
      auto returned = evaluate(stmt, scope);
      if (returned->type == ValueType::Break) { break_flag = true; break; }
      if (returned->type == ValueType::Continue) { break; }
      last_returned = returned;
    }
    if (break_flag) {break;}
    passed = static_cast<BooleanVal*>(evaluate(whileExpr->check, env))->value;
  }
  return last_returned;
}

std::vector<RuntimeVal*> eval_args(std::vector<Expr*> args, Environment* env) {
  std::vector<RuntimeVal*> ret;
  for (auto arg : args) {
    ret.push_back(evaluate(arg, env));
  }
  return ret;
}

RuntimeVal* eval_program(Program* program, Environment* env) {
  RuntimeVal* lastEvaluated = new EmptyVal();

	for (auto stmt : program->body) {
		lastEvaluated = evaluate(stmt, env);
	}

	return lastEvaluated;
}

RuntimeVal* eval_assignment(AssignmentExpr* assign, Environment* env) {
  Expr* name = assign->identifier;
  if (name->kind == NodeType::Identifier) {
    RuntimeVal* val = evaluate(assign->value, env);
    return env->assignVar(static_cast<Identifier*>(name)->value, val, assign->local);

  } else if (name->kind == NodeType::SubscriptExpr) {
    SubscriptExpr* subs = static_cast<SubscriptExpr*>(name);

    RuntimeVal* left = evaluate(subs->left, env);
    if (left->type != ValueType::Array)
      raise_error("cannot subscript assing a non-array");
    
    RuntimeVal* num = evaluate(subs->value, env);
    if (num->type != ValueType::Number)
      raise_error("cannot subscript using a non-number");

    auto value = evaluate(assign->value, env);

    ArrayVal* leftArray = static_cast<ArrayVal*>(left);
    int index = (int)(static_cast<NumberVal*>(num)->value);
    
    if (leftArray->elements.size() < index)
      raise_error("array index out of range");

    leftArray->elements[index] = value;
    if (subs->left->kind == NodeType::Identifier) {
      env->overrideVar(static_cast<Identifier*>(subs->left)->value, leftArray);
      return value;
    }

    raise_error("wut1");
  }
  raise_error("Left hand side of assignment expected to be a identifier");

  
}

NumberVal* eval_binary_math(NumberVal* a, NumberVal* b, OperatorType op) {
  NumberVal* result = new NumberVal();
  switch (op) {
    case OperatorType::add:
      result->value = a->value + b->value;
      break;
    case OperatorType::substract:
      result->value = a->value - b->value;
      break;
    case OperatorType::multiply:
      result->value = a->value * b->value;
      break;
    case OperatorType::divide:
      result->value = a->value / b->value;
      break;
    case OperatorType::modulo:
      result->value = std::fmod(a->value, b->value);
      break;
  }

  return result;
}

RuntimeVal* eval_binary_expr(BinaryExpr* binary, Environment* env) {
  RuntimeVal* left = evaluate(binary->left, env);
  RuntimeVal* right = evaluate(binary->right, env);

  if (left->type == ValueType::Number && right->type == ValueType::Number) {
    return eval_binary_math(
      static_cast<NumberVal*>(left),
      static_cast<NumberVal*>(right),
      binary->expr_operator
    );
  }
  if (left->type == ValueType::String && right->type == ValueType::String) {
    StringVal* leftStr = static_cast<StringVal*>(left);
    StringVal* rightStr = static_cast<StringVal*>(right);
    return MK_STRING(leftStr->value + rightStr->value);
  }

  return new EmptyVal();
}