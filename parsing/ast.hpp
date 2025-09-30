#pragma once
#include <vector>
#include <string>

enum class NodeType {
  // STATEMENTS
  Program,
  VariableDeclaration,
  FunctionDeclaration,
  IfStatement,
  WhileStatement,
  // EXPRESSIONS
  AssignmentExpr,
  CallExpr,
  // Literals
  StringLiteral,
  NumberLiteral,
  Identifier,
  BinaryExpr
};

enum class OperatorType {
  add,        // +
  substract,  // -
  multiply,   // *
  divide,     // /
  modulo,     // %
};

class Stmt {
  public:
    NodeType kind;
    Stmt(NodeType k) {
      kind = k;
    }
};


class Program: public Stmt {
  public:
    Program(): Stmt(NodeType::Program) {
    }
    std::vector<Stmt*> body;
};


/*
EXPRESSIONS
*/
class Expr: public Stmt {
  protected:
    Expr(NodeType k): Stmt(k) {
    }
};

class VariableDeclaration: public Expr {
  public:
    VariableDeclaration(): Expr(NodeType::VariableDeclaration) {
    }
    std::string identifier;
    bool constant = false;
    Expr* value; // value always is defined because `x = 10` is treaded as declaration if x doesn't exist
};

class AssignmentExpr: public Expr {
  public:
    AssignmentExpr(): Expr(NodeType::AssignmentExpr) {
    }
    Expr* identifier;
    Expr* value;
};

class CallExpr: public Expr {
  public:
    CallExpr(): Expr(NodeType::CallExpr) {
    }
    Expr* caller;
    std::vector<Expr*> args;
};

class FunctionDeclaration: public Expr {
  public:
    FunctionDeclaration(): Expr(NodeType::FunctionDeclaration) {
    }
    std::vector<std::string> parameters;
    std::vector<Stmt*> body;
};

class IfStatement: public Expr {
  public:
    IfStatement(): Expr(NodeType::IfStatement) {
    }
    Expr* check;
    std::vector<Stmt*> body;
    std::vector<Stmt*> else_body;
};

class WhileStatement: public Expr {
  public:
    WhileStatement(): Expr(NodeType::WhileStatement) {
    }
    Expr* check;
    std::vector<Stmt*> body;
};

// Literals
class NumberLiteral: public Expr {
  public:
    NumberLiteral(): Expr(NodeType::NumberLiteral) {
    }
    int value;
};

class StringLiteral: public Expr {
  public:
    StringLiteral(): Expr(NodeType::StringLiteral) {
    }
    std::string value;
};

class Identifier: public Expr {
  public:
    Identifier(): Expr(NodeType::Identifier) {
    }
    std::string value;
};

class BinaryExpr: public Expr {
  public:
    BinaryExpr(): Expr(NodeType::BinaryExpr) {
    }
    Expr* left;
    Expr* right;
    OperatorType expr_operator;
};