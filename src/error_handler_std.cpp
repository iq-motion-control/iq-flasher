#include "Schmi/error_handler_std.hpp"

namespace Schmi {
void ErrorHandlerStd::Init(const Error& error) {
  error_ = error;
}

void ErrorHandlerStd::Display() {
  std::cerr << error_.error_location << ": ";
  std::cerr << error_.error_string << " - ";
  std::cerr << error_.err_num << "\n\n";
}

void ErrorHandlerStd::DisplayAndDie() {
  Display();
  exit(EXIT_FAILURE);
}
};