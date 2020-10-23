#include "Schmi/binary_file_std.hpp"
#include "Schmi/error_handler_std.hpp"
#include "Schmi/flash_loader.hpp"
#include "Schmi/loading_bar_std.hpp"
#include "Schmi/serial_posix.hpp"

#include <iostream>
#include <string>

void DisplayAsciiArt(const std::string& file_name);
std::string GetTextFileContents(std::ifstream& file);

int main(int argc, char* argv[]) {
  // DisplayAsciiArt("misc/schmi_ascii_art.txt");

  // std::string binary_file = "binaries/0x100016_iq2306_2200kv.bin";
  std::string binary_file = "binaries/0x20000A_iq2306_190kv.bin";
  // std::string binary_file = "./1048583_V6-3.bin";
  // std::string binary_file = "./1048583_V6-3.bin";
  // std::string binary_file = "./1048583_V6-3.bin";

  const uint16_t num_of_pages = 29;
  uint16_t page_codes[num_of_pages];
  for (int ii = 0; ii < num_of_pages; ++ii) {
    page_codes[ii] = ii;
  }

  std::cout << "Binary to flash: " << binary_file << "\n\n";

  Schmi::ErrorHandlerStd error;
  Schmi::BinaryFileStd bin(binary_file);
  Schmi::SerialPosix ser("/dev/ttyUSB0");
  Schmi::LoadingBarStd bar;

  Schmi::FlashLoader fl(&ser, &bin, &error, &bar);

  fl.Init();
  // fl.Flash(page_codes, num_of_pages);
  fl.Flash();

  return EXIT_SUCCESS;
}

void DisplayAsciiArt(const std::string& file_name) {
  try {
    std::ifstream reader(file_name);
    std::string Art = GetTextFileContents(reader);
    std::cout << Art << std::endl;
    reader.close();

  } catch (std::exception const& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    exit(EXIT_FAILURE);
  }

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
    throw std::runtime_error("ASCII art file does not exist, check path");
  }
  file.close();
  return lines;
}
