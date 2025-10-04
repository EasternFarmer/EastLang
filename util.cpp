#include "util.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

std::string pwd() {
  std::filesystem::path cwd = std::filesystem::current_path();
  return cwd.string();
}

std::string read_file(const char * path) {
  std::ifstream file(path);
  if (!file) {
    std::cerr << "Could not open file " << path << "\n";
    std::exit(1);
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();

  std::string fileContent = buffer.str();
  return fileContent;
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
    case TokenType::BitwiseShift:
      std::cout << "BitwiseShift Token\n";
      break;
    case TokenType::LogicalExpr:
      std::cout << "LogicalExpr Token\n";
      break;
    case TokenType::ElseIf:
      std::cout << "ElseIf Token\n";
      break;
    case TokenType::Monkey:
      std::cout << "Monkey Token\n";
      break;
    case TokenType::Not:
      std::cout << "Not Token\n";
      break;
    case TokenType::ComparisonExpr:
      std::cout << "ComparisonExpr Token\n";
      break;
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
    case NodeType::ArrayLiteral:
      std::cout << "Array node\n";
      break;
    case NodeType::BitShiftExpr:
      std::cout << "BitShift node\n";
      break;
    case NodeType::SpecialExpr:
      std::cout << "SpecialExpr node\n";
      break;
    case NodeType::MemberExpr:
      std::cout << "MemberExpr node\n";
      break;
    case NodeType::SubscriptExpr: {
      std::cout << "SubscriptExpr node\n";
      break;
    }
    default:
      std::cout << "Unsupported node\n";
    }
}