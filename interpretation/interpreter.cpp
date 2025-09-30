#include "interpreter.hpp"
#include "../Errors.hpp"
#include <cmath>

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
    default:
      raise_error("invalid Node Type");
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
  } else {
    RuntimeVal* last_returned = new EmptyVal();
    for (auto stmt : ifExpr->else_body) {
      last_returned = evaluate(stmt, scope);
    }
    return last_returned;
  }
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
  if (name->kind != NodeType::Identifier) {
    raise_error("Left hand side of assignment expected to be a identifier");
  }

  RuntimeVal* val = evaluate(assign->value, env);
  return env->assignVar(static_cast<Identifier*>(name)->value, val);
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
  // TODO: fix math

  if (left->type == ValueType::Number && right->type == ValueType::Number) {
    return eval_binary_math(
      static_cast<NumberVal*>(left),
      static_cast<NumberVal*>(right),
      binary->expr_operator
    );
  }

  return new EmptyVal();
}