#include "Errors.hpp"

[[ noreturn ]] void raise_error(std::string error) {
  std::cerr << "\033[31m" << error << "\033[0m\n";
  std::exit(1);
}