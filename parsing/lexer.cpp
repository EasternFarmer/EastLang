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
      
    } else if (str[0] == '!') {
      str.pop_front();
      if (str[0] == '=') {
        str.pop_front();
        tokens.push_back(Token("!=", TokenType::ComparisonExpr));
      } else {
        tokens.push_back(Token("not", TokenType::Not));
      }
    } else if (str[0] == '=') {
      str.pop_front();
      if (str[0] == '=') {
        str.pop_front();
        tokens.push_back(Token("==", TokenType::ComparisonExpr));
      } else {
        tokens.push_back(Token('=', TokenType::Equals));
      }
      
    } else if (str[0] == '@') {
      str.pop_front();
      tokens.push_back(Token('@', TokenType::Monkey));

    } else if (str[0] == '>') {
      str.pop_front();
      if (str[0] == '=') {
        str.pop_front();
        tokens.push_back(Token(">=", TokenType::ComparisonExpr));
      } else if (str[0] == '>') {
        str.pop_front();
        tokens.push_back(Token(">>", TokenType::BitwiseShift));
      } else {
        tokens.push_back(Token('>', TokenType::ComparisonExpr));
      }
      
    } else if (str[0] == '<') {
      str.pop_front();
      if (str[0] == '=') {
        str.pop_front();
        tokens.push_back(Token("<=", TokenType::ComparisonExpr));
      } else if (str[0] == '<') {
        str.pop_front();
        tokens.push_back(Token("<<", TokenType::BitwiseShift));
      } else {
        tokens.push_back(Token('<', TokenType::ComparisonExpr));
      }
    
    } else if (str[0] == '`') { // block comments
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
        while (std::isalnum(str[0]) || str[0] == '_') {
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
          if (str[0] == '\\') { // ESCAPE CODES
            str.pop_front(); // pop \ from the deque
            if (str[0] == '\'' || str[0] == '"') {
              ret += str.front();
              str.pop_front(); // pop ' or " from the deque
              continue;

            } else if (str[0] == '\\') {
              ret += '\\';
              str.pop_front();
              continue;

            } else if (str[0] == 'n') {
              str.pop_front();
              ret += '\n';
              continue;

            } else if (str[0] == 't') {
              str.pop_front();
              ret += '\t';
              continue;

            } else if (str[0] == 'r') {
              str.pop_front();
              ret += '\r';
              continue;

            } else if (str[0] == 'v') {
              str.pop_front();
              ret += '\v';
              continue;

            } else if (std::isdigit(str[0])) {
              std::string esc = "";
              esc += str.front(); // escape codes \nnn where n is a digit
              str.pop_front();

              esc += str.front();
              str.pop_front();
              
              esc += str.front();
              str.pop_front();

              char val = static_cast<char>(std::stoi(esc, nullptr, 8));
              ret += val;
              continue;

            } else if (str[0] == 'x') {
              str.pop_front(); // pop x

              std::string esc = "";
              esc += str.front(); // escape codes \xnn where n is a digit
              str.pop_front();

              esc += str.front();
              str.pop_front();

              char val = static_cast<char>(std::stoi(esc, nullptr, 16));
              ret += val;
              continue;
            }
            str.pop_front(); // remove the char after the slash
          }
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