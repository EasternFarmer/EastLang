#include <iostream>
#include "parsing/lexer.hpp"
#include "parsing/parser.hpp"
#include "interpretation/Environment.hpp"
#include "interpretation/GlobalEnv.hpp"
#include "interpretation/interpreter.hpp"

#include "util.hpp"

int run(int argc, char * argv[]) {

  std::string fileContent = read_file(argv[1]);

  Parser* parser = new Parser();
  Environment* env = makeGlobalEnv();
  env->declareVar("@name", MK_STRING("main"));

  auto* argArray = new ArrayVal();
  for (int i = 1; i < argc; i++) { // convert argv after the interpreter path into an array
      argArray->elements.push_back(MK_STRING(argv[i]));
  }
  env->declareVar("argv", argArray, true);

  Program* program = parser->parse_ast(fileContent);

  evaluate(program, env);
  return 0;
}

int repl() {
  Parser* parser = new Parser();
  Environment* env = makeGlobalEnv();

  std::cout << "Repl v99.99\n";

  while (true) {
    std::string input;

    std::cout << "\n> ";
    std::getline(std::cin, input);

    if (input.find("exit") != input.npos) {
      return 0;
    }
    if (input.find("clear") != input.npos) {
      std::cout << "\033[2J\033[H";
      continue;
    }

    // Produce AST From sourc-code
    Program* program = parser->parse_ast(input);

    RuntimeVal* ret = evaluate(program, env);
    if (ret->type != ValueType::Empty)
      print({ ret }); // language specific function located in interpretation\GlobalEnv.cpp
  }
  return 0;
}

int main(int argc, char * argv[]) {

  if (argc < 2) {
    return repl();
  } else {
    return run(argc, argv);
  }

  return 0;
}