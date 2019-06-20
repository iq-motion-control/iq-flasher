#include <iostream>

#include "Schmi/error_handler_std.hpp"
#include "Schmi/serial_interface.hpp"
#include "Schmi/serial_posix.hpp"
#include "Schmi/stm32.hpp"

int main(int argc, char* argv[]) {
  std::cout << "HELLO WORLD\n\n";

  Schmi::SerialPosix ser0("/dev/ttyUSB0");

  ser0.Init();

  Schmi::ErrorHandlerStd error;

  Schmi::Stm32 stm32(ser0, error);
  stm32.InitUsart();

  // Schmi::VersionAndReadProtection vrp = stm32.GetVersionAndReadProtection();
  // stm32.GetID();

  return EXIT_SUCCESS;
}
