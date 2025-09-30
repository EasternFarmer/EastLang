#include <string>
#include <deque>
#include "lexer.hpp"
#include <iostream>

std::deque<Token> tokenize(std::string sourceCode) {
  std::deque<Token> tokens;
  std::deque<char> str(sourceCode.begin(), sourceCode.end());
  
  while (str.size() > 0) {
    if (str[0] == '(') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::OpenParen));
      
    } else if (str[0] == ')') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::ClosedParen));
      
    } else if (str[0] == '{') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::OpenBrace));
      
    } else if (str[0] == '}') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::ClosedBrace));
      
    } else if (str[0] == '[') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::OpenBracket));
      
    } else if (str[0] == ']') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::ClosedBracket));
      
    } else if (str[0] == '.') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::Dot));
      
    } else if (str[0] == ',') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::Comma));

    } else if (str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/' || str[0] == '%') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::BinaryOperator));
      
    } else if (str[0] == '=') {
      char value = str.front();
      str.pop_front();
      tokens.push_back(Token(value, TokenType::Equals));
    
    } else if (str[0] == '`') {
      str.pop_front();
      while (str[0] != '`') {
        str.pop_front();
      }
      str.pop_front();

    } else {
      if (std::isdigit(str[0])) {
        std::string ret = "";
        while (std::isdigit(str[0]) || str[0] == '.') {
          ret += str.front();
          str.pop_front();
        }
        tokens.push_back(Token(ret, TokenType::Number));

      } else if (std::isalnum(str[0])) {
        std::string ret = "";
        while (std::isalnum(str[0])) {
          ret += str.front();
          str.pop_front();
        }
        auto itt = KEYWORDS.find(ret);
        if (itt == KEYWORDS.end()) {
          tokens.push_back(Token(ret, TokenType::Identifier));
        } else {
          tokens.push_back(Token(ret, itt->second));
        };

      } else if (str[0] == '\'' || str[0] == '"') {
        std::string ret = "";
        char end = str.front();
        str.pop_front();
        while (str[0] != end) {
          ret += str.front();
          str.pop_front();
        }
        str.pop_front();
        tokens.push_back(Token(ret, TokenType::String));

      } else {
        str.pop_front();
      }
    }
  }

  tokens.push_back(Token("EOF", TokenType::EndOfFile));

  return tokens;
};