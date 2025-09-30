#include "util.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

std::string pwd() {
  std::filesystem::path cwd = std::filesystem::current_path();
  return cwd.string();
}

std::string read_file(char * path) {
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