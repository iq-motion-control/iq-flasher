#ifndef SCHMI_FLASH_LOADER_HPP
#define SCHMI_FLASH_LOADER_HPP

#include "Schmi/binary_file_interface.hpp"
#include "Schmi/error_handler_interface.hpp"
#include "Schmi/loading_bar_interface.hpp"
#include "Schmi/serial_interface.hpp"
#include "Schmi/stm32.hpp"

namespace Schmi {

struct FlashBytesData {
  uint32_t current_byte_pos;
  uint32_t current_memory_address;
  uint32_t bytes_left;
};

class FlashLoader {
 public:
  FlashLoader(SerialInterface* ser, BinaryFileInterface* bin, ErrorHandlerInterface* err, LoadingBarInterface* bar) : ser_(ser),
                                                                                                                      bin_(bin),
                                                                                                                      err_(err),
                                                                                                                      bar_(bar){};
  ~FlashLoader() {
    delete stm32_;
  };

  void Init();

  bool Flash();

 private:
  const uint32_t start_address_ = 0x08000000;
  const uint16_t MAX_WRITE_SIZE = 256;

  SerialInterface* ser_;
  BinaryFileInterface* bin_;
  ErrorHandlerInterface* err_;
  LoadingBarInterface* bar_;
  Stm32* stm32_;

  uint32_t total_num_bytes_ = 0;

  bool FlashBytes();

  uint16_t CheckNumBytesToWrite(const uint64_t& bytes_left);

  void UpdateWriteHelpers(FlashBytesData& bytes_data, const uint16_t& num_bytes);
};
}

#endif  // SCHMI_FLASH_LOADER_HPP
