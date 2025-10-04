#pragma once
#include <string>
#include <deque>
#include <unordered_map>

enum class TokenType {
  // types
  String,
	Number,
  // List,

  Identifier,
  Callable,
  Const,
  Local,
  BinaryOperator,
  BitwiseShift,
  Equals,
  
  If,
  Else,
  ElseIf,
  While,

  LogicalExpr,
  Not,
  ComparisonExpr,

  OpenParen,
  ClosedParen,

  OpenBrace, // {
  ClosedBrace, // }

  OpenBracket, // [
  ClosedBracket, // ]

  Comma,
  Dot,
  Monkey, // @

  EndOfFile,
};

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
  { "callable", TokenType::Callable },
  { "const", TokenType::Const },
  { "local", TokenType::Local },
  { "if", TokenType::If },
  { "else", TokenType::Else },
  { "else_if", TokenType::ElseIf },
  { "while", TokenType::While },

  { "and", TokenType::LogicalExpr },
  { "or", TokenType::LogicalExpr },
  { "xor", TokenType::LogicalExpr },
  { "not", TokenType::Not },
};

class Token {
  public:
    std::string value;
    TokenType type;
    
    Token(std::string v, TokenType t) {
      value = v;
      type = t;
    }

    Token(char v, TokenType t) {
      value = std::string(1, v);
      type = t;
    }
};

std::deque<Token> tokenize(std::string sourceCode);