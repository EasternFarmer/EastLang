#pragma once
#include <string>
#include "parsing/lexer.hpp"
#include "parsing/ast.hpp"

std::string pwd();

std::string read_file(const char * path);

void print_token_type(TokenType tk);

void print_node_type(NodeType node);