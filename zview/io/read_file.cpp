#include "zview/io/read_file.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string readFile(const std::string& filename) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  std::stringstream file_stream;
  try {
    file.open(filename.c_str());
    file_stream << file.rdbuf();
    file.close();
  } catch (const std::ifstream::failure& e) {
    std::cout << "Error reading Shader File!" << std::endl;
  }
  return file_stream.str();
}
