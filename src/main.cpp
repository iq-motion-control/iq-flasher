
#include "Schmi/binary_file_std.hpp"
#include "Schmi/error_handler_std.hpp"
#include "Schmi/flash_loader.hpp"
#include "Schmi/loading_bar_std.hpp"
#include "Schmi/serial_posix.hpp"

#include <fstream>
#include <iostream>
#include <string>

std::string GetTextFileContents(std::ifstream& file);
void DisplayAsciiArt(const std::string& file_name);

int main(int argc, char* argv[]) {
  DisplayAsciiArt("./misc/schmi_ascii_art.txt");

  std::string binary_file = "./1048583_V6-3.bin";

  std::cout << "Binary to flash: " << binary_file << "\n\n";

  Schmi::ErrorHandlerStd error;
  Schmi::BinaryFileStd bin("./1048583_V6-3.bin");
  Schmi::SerialPosix ser("/dev/ttyUSB0");
  Schmi::LoadingBarStd bar;

  Schmi::FlashLoader fl(&ser, &bin, &error, &bar);

  fl.Init();
  fl.Flash();

  return EXIT_SUCCESS;
}

void DisplayAsciiArt(const std::string& file_name) {
  std::ifstream reader(file_name);

  std::string Art = GetTextFileContents(reader);

  std::cout << Art << std::endl;

  reader.close();

  return;
}

std::string GetTextFileContents(std::ifstream& file) {
  std::string lines = "";

  if (file) {
    while (file.good()) {
      std::string temp_line;
      std::getline(file, temp_line);
      temp_line += "\n";

      lines += temp_line;
    }
    return lines;
  } else {
    return "ERROR File does not exist.";
  }
}
