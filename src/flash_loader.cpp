#include "Schmi/flash_loader.hpp"

namespace Schmi {

void FlashLoader::Init() {
  ser_->Init();
  stm32_ = new Stm32(*ser_, *err_);

  bin_->Init();
  total_num_bytes_ = bin_->GetBinaryFileSize();

  return;
}

bool FlashLoader::Flash() {
  if (!stm32_->InitUsart()) {
    return 0;
  }

  if (!stm32_->SpecialExtendedErase(0xFFFF)) {
    return 0;
  }  // global erase

  if (!FlashBytes()) {
    return 0;
  }

  if (!stm32_->GoToAddress(start_address_)) {
    return 0;
  }

  return 1;
}

bool FlashLoader::FlashBytes() {
  FlashBytesData flash_data = {0, start_address_, total_num_bytes_};

  bar_->StartLoadingBar(total_num_bytes_);

  while (flash_data.bytes_left) {
    uint16_t num_bytes = CheckNumBytesToWrite(flash_data.bytes_left);

    uint8_t bytes[num_bytes];
    bin_->GetBytesArray(bytes, {num_bytes, flash_data.current_byte_pos});

    if (!stm32_->WriteMemory(bytes, num_bytes, flash_data.current_memory_address)) {
      return 0;
    }

    UpdateWriteHelpers(flash_data, num_bytes);

    bar_->UpdateLoadingBar(flash_data.bytes_left);
  }

  bar_->EndLoadingBar();

  return 1;
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

  return;
}
}