#include "Schmi/flash_loader.hpp"

namespace Schmi {

void FlashLoader::Init() {
  ser_->Init();
  stm32_ = new Stm32(*ser_, *err_);

  bin_->Init();
  total_num_bytes_ = bin_->GetBinaryFileSize();

  return;
}

void FlashLoader::Flash() {
  //Create array first so there is serial lag while copying bytes into the array
  //using arrays because it needs to work on multiple platforms with no std.
  //"bytes" not define as private varible to stay away from malloc() ect;
  uint8_t bytes[total_num_bytes_];
  bin_->GetBytesArray(bytes);

  stm32_->InitUsart();

  stm32_->SpecialExtendedErase(0xFFFF);  // global erase

  FlashBytes(bytes);

  stm32_->GoToAddress(start_address_);

  return;
}

void FlashLoader::FlashBytes(uint8_t* bytes) {
  FlashBytesData bytes_data = {bytes, start_address_, total_num_bytes_};

  bar_->StartLoadingBar(total_num_bytes_);

  while (bytes_data.bytes_left) {
    uint16_t num_bytes = CheckNumBytesToWrite(bytes_data.bytes_left);

    stm32_->WriteMemory(bytes_data.current_byte_pos, num_bytes, bytes_data.current_memory_address);

    UpdateWriteHelpers(bytes_data, num_bytes);

    bar_->UpdateLoadingBar(bytes_data.bytes_left);
  }

  bar_->EndLoadingBar();

  return;
}

uint16_t FlashLoader::CheckNumBytesToWrite(const uint64_t& bytes_left) {
  uint16_t num_bytes_to_write = 0;

  if (bytes_left < MAX_WRITE_SIZE) {
    num_bytes_to_write = bytes_left;
  } else {
    num_bytes_to_write = MAX_WRITE_SIZE;
  }

  return num_bytes_to_write;
}

void FlashLoader::UpdateWriteHelpers(FlashBytesData& bytes_data, const uint16_t& num_bytes) {
  bytes_data.current_byte_pos += num_bytes;
  bytes_data.current_memory_address += num_bytes;
  bytes_data.bytes_left -= num_bytes;
}
}