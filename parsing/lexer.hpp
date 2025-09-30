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
  BinaryOperator,
  Equals,
  If,
  Else,
  While,

  OpenParen,
  ClosedParen,

  OpenBrace, // {
  ClosedBrace, // }

  OpenBracket, // [
  ClosedBracket, // ]

  Comma,
  Dot,

  EndOfFile,
};

inline const std::unordered_map<std::string, TokenType> KEYWORDS = {
  { "callable", TokenType::Callable },
  { "const", TokenType::Const },
  { "if", TokenType::If },
  { "else", TokenType::Else },
  { "while", TokenType::While },
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