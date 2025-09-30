#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "../Errors.hpp"

bool Parser::not_eof() {
  return tokens[0].type != TokenType::EndOfFile;
}

Token Parser::curr() {
  return tokens[0];
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

OperatorType Parser::get_operator(std::string operatorLiteral) {
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

void print_node_type(NodeType node) {
  switch (node) {
    case NodeType::AssignmentExpr:
      std::cout << "assing node\n";
      break;
    case NodeType::BinaryExpr:
      std::cout << "binary node\n";
      break;
    case NodeType::CallExpr:
      std::cout << "callexpr node\n";
      break;
    case NodeType::NumberLiteral:
      std::cout << "Number node\n";
      break;
    case NodeType::FunctionDeclaration:
      std::cout << "func node\n";
      break;
    case NodeType::Identifier:
      std::cout << "iden node\n";
      break;
    case NodeType::Program:
      std::cout << "program node\n";
      break;
    case NodeType::StringLiteral:
      std::cout << "str node\n";
    case NodeType::VariableDeclaration:
      std::cout << "varDec node\n";
      break;
    case NodeType::IfStatement:
      std::cout << "IfStmt node\n";
      break;
    case NodeType::WhileStatement:
      std::cout << "WhileStmt node\n";
      break;
    }
}

void print_token_type(TokenType tk) {
  switch (tk) {
    case TokenType::String:
      std::cout << "str Token\n";
      break;
    case TokenType::Number:
      std::cout << "Number Token\n";
      break;
    case TokenType::Identifier:
      std::cout << "iden Token\n";
      break;
    case TokenType::Callable:
      std::cout << "callable Token\n";
      break;
    case TokenType::Const:
      std::cout << "const Token\n";
      break;
    case TokenType::BinaryOperator:
      std::cout << "binary Token\n";
      break;
    case TokenType::Equals:
      std::cout << "equal Token\n";
      break;
    case TokenType::OpenParen:
      std::cout << "( Token\n";
      break;
    case TokenType::ClosedParen:
      std::cout << ") Token\n";
      break;
    case TokenType::OpenBrace:
      std::cout << "{ Token\n";
      break;
    case TokenType::ClosedBrace:
      std::cout << "} Token\n";
      break;
    case TokenType::OpenBracket:
      std::cout << "[ Token\n";
      break;
    case TokenType::ClosedBracket:
      std::cout << "] Token\n";
      break;
    case TokenType::Comma:
      std::cout << ", Token\n";
      break;
    case TokenType::Dot:
      std::cout << ". Token\n";
      break;
    case TokenType::EndOfFile:
      std::cout << "EOF Token\n";
      break;
    case TokenType::If:
      std::cout << "IF Token\n";
      break;
    case TokenType::Else:
      std::cout << "Else Token\n";
      break;
    case TokenType::While:
      std::cout << "While Token\n";
      break;
  }
}

// Orders Of Prescidence
//
// Assignment
// AdditiveExpr
// MultiplicitaveExpr
// ~Member~
// Call
// PrimaryExpr // callable declaration too!
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
  }
  Expr* left = parse_additive_expr();

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

Expr* Parser::parse_additive_expr() {
  Expr* left = parse_multiplicative_expr();
  while (curr().value == "+" || curr().value == "-") {
    std::string operatorType = advance().value;
    Expr* right = parse_multiplicative_expr();

    BinaryExpr* binary = new BinaryExpr();
    binary->expr_operator = get_operator(operatorType);
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
    binary->expr_operator = get_operator(operatorType);
    binary->left = left;
    binary->right = right;

    left = binary;
  }
  return left;
}

Expr* Parser::parse_call_member_expr() {
  Expr* left = parse_primary_expr();
  if (curr().type == TokenType::OpenParen) {
    left = parse_call_expr(left);
  }
  return left;
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
      Expr* check = parse_expr();
      IfStatement* IfExpr = new IfStatement();

      IfExpr->check = check;

      expect(TokenType::OpenBrace, "Expected an open Brace for if body declaration");

      while (not_eof() && curr().type != TokenType::ClosedBrace) {
        Stmt* stmt = parse_expr();
        IfExpr->body.push_back(stmt);
      }
      expect(TokenType::ClosedBrace, "Expected a closing brace to close the if body definition");

      if (curr().type == TokenType::Else) {
        advance();
        expect(TokenType::OpenBrace, "Expected an open Brace for if body declaration");

        while (not_eof() && curr().type != TokenType::ClosedBrace) {
          Stmt* stmt = parse_expr();
          IfExpr->else_body.push_back(stmt);
        }
        expect(TokenType::ClosedBrace, "Expected a closing brace to close the if body definition");
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
    default:
      raise_error("Unexpected token: " + curr().value);
  }
}