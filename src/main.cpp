#include <iostream>

#include "Schmi/binary_file_std.hpp"
#include "Schmi/error_handler_std.hpp"
#include "Schmi/flash_loader.hpp"
#include "Schmi/loading_bar_std.hpp"
#include "Schmi/serial_posix.hpp"

int main(int argc, char* argv[]) {
  std::cout << "HELLO WORLD\n\n";

  Schmi::ErrorHandlerStd error;
  Schmi::BinaryFileStd bin("./1048583_V6-3.bin");
  Schmi::SerialPosix ser("/dev/ttyUSB0");
  Schmi::LoadingBarStd bar;

  Schmi::FlashLoader fl(&ser, &bin, &error, &bar);

  fl.Init();
  fl.Flash();

  return EXIT_SUCCESS;
}
