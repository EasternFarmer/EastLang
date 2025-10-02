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
  SubscriptExpr,

  NegateExpr,
  LogicalExpr,
  ComparisonExpr,
  // Literals
  StringLiteral,
  ArrayLiteral,
  NumberLiteral,
  Identifier,
  BinaryExpr,
  BitShiftExpr,
};

enum class OperatorType {
  add,        // +
  substract,  // -
  multiply,   // *
  divide,     // /
  modulo,     // %
};

enum class ComparisonOperatorType {
  equal,          // ==
  not_equal,      // !=
  greater_equal,  // >=
  greater,        // >
  less_equal,     // <=
  less,           // <
};

enum class LogicalOperatorType {
  And,
  Or,
  Xor,
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
    Program(): Stmt(NodeType::Program) {}
    std::vector<Stmt*> body;
};


/*
EXPRESSIONS
*/
class Expr: public Stmt {
  protected:
    Expr(NodeType k): Stmt(k) {}
};

class VariableDeclaration: public Expr {
  public:
    VariableDeclaration(): Expr(NodeType::VariableDeclaration) {}
    std::string identifier;
    bool constant = false;
    Expr* value; // value always is defined because `x = 10` is treaded as declaration if x doesn't exist
};

class AssignmentExpr: public Expr {
  public:
    AssignmentExpr(): Expr(NodeType::AssignmentExpr) {}
    Expr* identifier;
    Expr* value;
};

class CallExpr: public Expr {
  public:
    CallExpr(): Expr(NodeType::CallExpr) {}
    Expr* caller;
    std::vector<Expr*> args;
};

class BinaryExpr: public Expr {
  public:
    BinaryExpr(): Expr(NodeType::BinaryExpr) {}
    Expr* left;
    Expr* right;
    OperatorType expr_operator;
};

class FunctionDeclaration: public Expr {
  public:
    FunctionDeclaration(): Expr(NodeType::FunctionDeclaration) {}
    std::vector<std::string> parameters;
    std::vector<Stmt*> body;
};

class IfStatement: public Expr {
  public:
    IfStatement(): Expr(NodeType::IfStatement) {}
    Expr* check;
    std::vector<Stmt*> body;
    std::vector<std::pair<Expr*, std::vector<Stmt*>>> else_if_chain;
    std::vector<Stmt*> else_body;
};

class WhileStatement: public Expr {
  public:
    WhileStatement(): Expr(NodeType::WhileStatement) {}
    Expr* check;
    std::vector<Stmt*> body;
};

class NegateExpr: public Expr {
  public:
    NegateExpr(): Expr(NodeType::NegateExpr) {}
    Expr* expr;
};

class LogicalExpr: public Expr {
  public:
    LogicalExpr(): Expr(NodeType::LogicalExpr) {}
    Expr* left;
    Expr* right;
    LogicalOperatorType op;
};

class ComparisonExpr: public Expr {
  public:
    ComparisonExpr(): Expr(NodeType::ComparisonExpr) {}
    Expr* left;
    Expr* right;
    ComparisonOperatorType op;
};

class BitShiftExpr: public Expr {
  public:
    BitShiftExpr(): Expr(NodeType::BitShiftExpr) {}
    Expr* left;
    Expr* right;
};

class SubscriptExpr: public Expr {
  public:
    SubscriptExpr(): Expr(NodeType::SubscriptExpr) {}
    Expr* left;
    Expr* value;
};

// Literals
class NumberLiteral: public Expr {
  public:
    NumberLiteral(): Expr(NodeType::NumberLiteral) {}
    double value;
};

class StringLiteral: public Expr {
  public:
    StringLiteral(): Expr(NodeType::StringLiteral) {}
    std::string value;
};

class ArrayLiteral: public Expr {
  public:
    ArrayLiteral(): Expr(NodeType::ArrayLiteral) {}
    std::vector<Expr*> elements;
};

class Identifier: public Expr {
  public:
    Identifier(): Expr(NodeType::Identifier) {}
    std::string value;
};