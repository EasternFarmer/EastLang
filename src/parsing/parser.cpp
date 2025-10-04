/*
Orders Of Precedence https://en.cppreference.com/w/cpp/language/operator_precedence.html

  - Assignment
  - Logic operators (and, or, xor, not)
  - Comparison operators (==, !=, >, <, >=, <=)
  - Bitwise shift
  - AdditiveExpr
  - MultiplicitaveExpr
  - Call   - ~~Member~~ // no members as of now `a().b.c()` - Subscript
  - PrimaryExpr // callable, if, while definitions (because if everything is an expression, these need to be evaled first)

TODOs:

  - OOP (far future)
  - Pattern matching (maybe a new type)
  - Maybe objects
  - Maybe FFI
  - some sort of way to interact with RuntimeVal's (maybe methods `"".join(list)` or modules for it `string.join("", list)`)
*/

#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "../Errors.hpp"
#include "../util.hpp"

bool Parser::not_eof() {
  return tokens[0].type != TokenType::EndOfFile;
}

Token Parser::curr() {
  return tokens[0];
}

Token Parser::look_ahead(int n) {
  return tokens[n];
}

Token Parser::advance() {
  Token value = tokens.front();
  tokens.pop_front();

  return value;
}

Token Parser::expect(TokenType expected_token, std::string error_message) {
  Token value = advance();

  if (value.type != expected_token) {
    raise_error(error_message);
  }

  return value;
}

OperatorType Parser::get_math_operator(std::string operatorLiteral) {
  if (operatorLiteral == "+") {
    return OperatorType::add;
  } else if (operatorLiteral == "-") {
    return OperatorType::substract;
  } else if (operatorLiteral == "*") {
    return OperatorType::multiply;
  } else if (operatorLiteral == "/") {
    return OperatorType::divide;
  } else if (operatorLiteral == "%") {
    return OperatorType::modulo;
  } else {
    raise_error("Unexpected operator, " + operatorLiteral);
  }
}

LogicalOperatorType Parser::get_logical_operator(std::string operatorLiteral) {
  if (operatorLiteral == "and") {
    return LogicalOperatorType::And;
  } else if (operatorLiteral == "or") {
    return LogicalOperatorType::Or;
  } else if (operatorLiteral == "xor") {
    return LogicalOperatorType::Xor;
  } else {
    raise_error("Unexpected logical operator, " + operatorLiteral);
  }
}

ComparisonOperatorType Parser::get_comparison_operator(std::string operatorLiteral) {
  if (operatorLiteral == "==") {
    return ComparisonOperatorType::equal;
  } else if (operatorLiteral == ">") {
    return ComparisonOperatorType::greater;
  } else if (operatorLiteral == ">=") {
    return ComparisonOperatorType::greater_equal;
  } else if (operatorLiteral == "<") {
    return ComparisonOperatorType::less;
  } else if (operatorLiteral == "<=") {
    return ComparisonOperatorType::less_equal;
  } else if (operatorLiteral == "!=") {
    return ComparisonOperatorType::not_equal;
  } else {
    raise_error("Unexpected logical operator, " + operatorLiteral);
  }
}

Program* Parser::parse_ast(std::string& sourceCode) {
  tokens = tokenize(sourceCode);
  Program* program = new Program();
  while (not_eof()) {
    program->body.push_back(parse_expr()); // everything is an expression :clueless:
  }

  return program;
}

Expr* Parser::parse_expr() {
  return parse_assignment_expr();
}

Expr* Parser::parse_assignment_expr() {
  if (curr().type == TokenType::Const) {
    advance(); // eat const keyword
    Expr* iden = parse_primary_expr();

    if (iden->kind != NodeType::Identifier) {
      raise_error("Expected a identifier after a const keyword");
    }
    expect(TokenType::Equals, "an Equal sign is needed after a const declaration"); // remove the equal sign

    VariableDeclaration* var = new VariableDeclaration();
    var->constant = true;
    var->identifier = static_cast<Identifier*>(iden)->value;
    var->value = parse_expr();
    return var;
  } else if (curr().type == TokenType::Local) {
    advance(); // eat local keyword
    Expr* iden = parse_primary_expr();

    if (iden->kind != NodeType::Identifier) {
      raise_error("Expected a identifier after a const keyword");
    }

    Expr* value;

    if (curr().type == TokenType::Equals) {
      advance();
      value = parse_expr();
    } else {
      Identifier* empty = new Identifier();
      empty->value = "empty";

      value = empty;
    }

    AssignmentExpr* var = new AssignmentExpr();
    var->local = true;
    var->identifier = iden;
    var->value = value;
    return var;
  }
  Expr* left = parse_logical_expr();

  if (curr().type == TokenType::Equals) {
    advance(); // eat the curr equal sign
    Expr* right = parse_expr();

    AssignmentExpr* assign = new AssignmentExpr();
    assign->identifier = left;
    assign->value = right;
    return assign;
  }
  return left;
}

Expr* Parser::parse_logical_expr() {
  if (curr().type == TokenType::Not) {
    advance();
    Expr* negatedExpr = parse_logical_expr();

    NegateExpr* neg = new NegateExpr();
    neg->expr = negatedExpr;
    return neg;
  }

  Expr* left = parse_comparison_expr();
  while (curr().type == TokenType::LogicalExpr) {
    LogicalExpr* logicExpr = new LogicalExpr();

    logicExpr->op = get_logical_operator(curr().value);
    advance();
    logicExpr->left = left;
    logicExpr->right = parse_comparison_expr();


    left = logicExpr;
  }
  return left;
}

Expr* Parser::parse_comparison_expr() {
  Expr* left = parse_bitwise_shift_expr();
  while (curr().type == TokenType::ComparisonExpr) {
    ComparisonExpr* compExpr = new ComparisonExpr();

    compExpr->op = get_comparison_operator(curr().value);
    advance();
    compExpr->left = left;
    compExpr->right = parse_bitwise_shift_expr();

    left = compExpr;
  }
  return left;
}

Expr* Parser::parse_bitwise_shift_expr() {
  Expr* left = parse_additive_expr();
  while (curr().type == TokenType::BitwiseShift) {
    BitShiftExpr* bitShiftExpr = new BitShiftExpr();

    if (advance().value == ">>") {
      bitShiftExpr->shiftRight = true;
    } else {
      bitShiftExpr->shiftRight = false;
    }

    bitShiftExpr->left = left;
    bitShiftExpr->right = parse_additive_expr();

    left = bitShiftExpr;
  }
  return left;
}

Expr* Parser::parse_additive_expr() {
  Expr* left = parse_multiplicative_expr();
  while (curr().value == "+" || curr().value == "-") {
    std::string operatorType = advance().value;
    Expr* right = parse_multiplicative_expr();

    BinaryExpr* binary = new BinaryExpr();
    binary->expr_operator = get_math_operator(operatorType);
    binary->left = left;
    binary->right = right;

    left = binary;
  }
  return left;
}

Expr* Parser::parse_multiplicative_expr() {
  Expr* left = parse_call_member_expr();
  while (curr().value == "*" || curr().value == "/" || curr().value == "%") {
    std::string operatorType = advance().value;
    Expr* right = parse_call_member_expr();

    BinaryExpr* binary = new BinaryExpr();
    binary->expr_operator = get_math_operator(operatorType);
    binary->left = left;
    binary->right = right;

    left = binary;
  }
  return left;
}

Expr* Parser::parse_call_member_expr() {
  Expr* left = parse_primary_expr(); // no members as of now
  while (curr().type == TokenType::OpenParen || curr().type == TokenType::OpenBracket || curr().type == TokenType::Dot) {
    switch (curr().type) {
      case TokenType::OpenParen:
        left = parse_call_expr(left);
        break;
      case TokenType::OpenBracket:
        left = parse_subscript_expr(left);
        break;
      case TokenType::Dot:
        left = parse_member_expr(left);
        break;
      default:
        raise_error("wut2");
    }
  }
  return left;
}

Expr* Parser::parse_subscript_expr(Expr* left) {
  if (curr().type != TokenType::OpenBracket) return left;
  advance();

  SubscriptExpr* sub = new SubscriptExpr();
  sub->left = left;
  sub->value = parse_expr();

  expect(TokenType::ClosedBracket, "expected closing braket on subscript end");
  return sub;
}

Expr* Parser::parse_member_expr(Expr* left) {
  advance(); // eat the dot
  Expr* iden = parse_primary_expr();
  if (iden->kind != NodeType::Identifier)
    raise_error("expected an identifier after the dot expr");

  MemberExpr* memberExpr = new MemberExpr();
  memberExpr->left = left;
  memberExpr->identifier = static_cast<Identifier*>(iden)->value;

  return memberExpr;
}

Expr* Parser::parse_call_expr(Expr* caller) {
  CallExpr* callExpr = new CallExpr();
  callExpr->caller = caller;
  callExpr->args = parse_call_args();

  if (curr().type == TokenType::OpenParen) {
    return parse_call_expr(callExpr);
  }
  return callExpr;
}

std::vector<Expr*> Parser::parse_call_args() {
  advance(); // eat the open paren
  std::vector<Expr*> args = {};
  if (curr().type != TokenType::ClosedParen) {
    Expr* arg = parse_expr();
    args.push_back(arg);

    while (curr().type == TokenType::Comma) {
      advance(); // eat the comma
      Expr* arg = parse_expr();
      args.push_back(arg);
    }
  }
  expect(TokenType::ClosedParen, "Expected a closing paren"); // eat the closing paren
  return args;
}

std::vector<Expr*> Parser::parse_list_elements() {
  advance(); // eat the open braket
  std::vector<Expr*> args;
  if (curr().type != TokenType::ClosedBracket) {
    Expr* arg = parse_expr();
    args.push_back(arg);

    while (curr().type == TokenType::Comma) {
      advance(); // eat the comma
      Expr* arg = parse_expr();
      args.push_back(arg);
    }
  }
  expect(TokenType::ClosedBracket, "Expected a closing bracket"); // eat the closing bracket
  return args;
}

Expr* Parser::parse_special_expr() {
  if (curr().type != TokenType::Identifier)
    raise_error("missing identifier after @");

  Identifier* iden = static_cast<Identifier*>(parse_primary_expr());

  SpecialExpr* specialExpr = new SpecialExpr();
  specialExpr->identifier = iden->value;

  if (curr().type != TokenType::OpenParen) { // if i want to add special value or sum
    raise_error("Expected a opening paren after special function identifier " + iden->value);
  }
  advance();

  specialExpr->isFunction = true;

  while (not_eof() && curr().type != TokenType::ClosedParen) {
    if (curr().type != TokenType::String)
      raise_error("param to the special function should be a string");

    specialExpr->args.push_back(static_cast<StringLiteral*>(parse_primary_expr()));
  }

  expect(TokenType::ClosedParen, "Missing closing paren for special function " + iden->value);

  return specialExpr;
}

Expr* Parser::parse_primary_expr() {
  TokenType token = curr().type;

  switch (token) {
    case TokenType::Identifier: {
      Identifier* iden = new Identifier();
      iden->value = advance().value;

      return iden;
    }
    case TokenType::Number: {
      NumberLiteral* numLit = new NumberLiteral();
      numLit->value = std::stod(advance().value);

      return numLit;
    }
    case TokenType::String: {
      StringLiteral* strLit = new StringLiteral();
      strLit->value = advance().value;

      return strLit;
    }
    case TokenType::OpenParen: {
      advance();
      Expr* value = parse_expr();
      expect(TokenType::ClosedParen, "Expected a closing Parenthesis");
      return value;
    }
    case TokenType::Callable: {
      advance(); // eat the keyword
      if (curr().type != TokenType::OpenParen)
        raise_error("Expected open paren after callable keyword");

      std::vector<std::string> params = {};
      std::vector<Expr*> args = parse_call_args();
      for (auto arg : args) {
        if (arg->kind != NodeType::Identifier) // check if every param is an identifier
          raise_error("Only identifiers in callable definition");
        params.push_back(static_cast<Identifier*>(arg)->value);
      }

      FunctionDeclaration* func = new FunctionDeclaration();
      func->parameters = params;

      expect(TokenType::OpenBrace, "Expected an open Brace for callable body declaration");

      while (not_eof() && curr().type != TokenType::ClosedBrace) {
        Stmt* stmt = parse_expr();
        func->body.push_back(stmt);
      }
      expect(TokenType::ClosedBrace, "Expected a closing brace to close the callable body definition");

      return func;
    }
    case TokenType::If: {
      advance();
      IfStatement* IfExpr = new IfStatement();
      IfExpr->check = parse_expr();

      expect(TokenType::OpenBrace, "Expected an open Brace for if body declaration");

      while (not_eof() && curr().type != TokenType::ClosedBrace) {
        Stmt* stmt = parse_expr();
        IfExpr->body.push_back(stmt);
      }
      expect(TokenType::ClosedBrace, "Expected a closing brace to close the if body definition");

      while (curr().type == TokenType::ElseIf) {
        advance();

        Expr* check = parse_expr();
        std::vector<Stmt*> else_if_body;

        expect(TokenType::OpenBrace, "Expected an open Brace for else_if body declaration");

        while (not_eof() && curr().type != TokenType::ClosedBrace) {
          Stmt* stmt = parse_expr();
          else_if_body.push_back(stmt);
        }
        expect(TokenType::ClosedBrace, "Expected a closing brace to close the else_if body definition");

        IfExpr->else_if_chain.push_back({check, else_if_body});
      }

      if (curr().type == TokenType::Else) {
        advance();
        expect(TokenType::OpenBrace, "Expected an open Brace for else body declaration");

        while (not_eof() && curr().type != TokenType::ClosedBrace) {
          Stmt* stmt = parse_expr();
          IfExpr->else_body.push_back(stmt);
        }
        expect(TokenType::ClosedBrace, "Expected a closing brace to close the else body definition");
      }

      return IfExpr;
    }
    case TokenType::While: {
      advance();
      Expr* check = parse_expr();
      WhileStatement* WhileExpr = new WhileStatement();

      WhileExpr->check = check;

      expect(TokenType::OpenBrace, "Expected an open Brace for if body declaration");

      while (not_eof() && curr().type != TokenType::ClosedBrace) {
        Stmt* stmt = parse_expr();
        WhileExpr->body.push_back(stmt);
      }
      expect(TokenType::ClosedBrace, "Expected a closing brace to close the if body definition");

      return WhileExpr;
    }
    case TokenType::OpenBracket: {
      ArrayLiteral* array = new ArrayLiteral();
      array->elements = parse_list_elements();
      return array;
    }
    case TokenType::Monkey: {
      advance();
      return parse_special_expr();
    }
    default:
      print_token_type(token);
      raise_error("Unexpected token: " + curr().value);
  }
}