#include "Schmi/stm32.hpp"

//test
namespace Schmi {

void Stm32::InitUsart() {
  SendCmd(CMD::USART_INIT);

  return;
}

VersionAndReadProtectionData Stm32::GetVersionAndReadProtection() {
  SendCmd(CMD::GET_VER_PROTECT_STATUS);

  uint8_t num_incoming_bytes = 4;
  uint8_t incoming_bytes[num_incoming_bytes];
  ReadBytes(incoming_bytes, num_incoming_bytes);

  return CreateVersionAndReadProtection(incoming_bytes);
}

uint16_t Stm32::GetID() {
  SendCmd(CMD::GET_ID);

  uint8_t num_incoming_bytes = 4;
  uint8_t incoming_bytes[num_incoming_bytes];
  ReadBytes(incoming_bytes, num_incoming_bytes);

  uint16_t Id = (incoming_bytes[1] << 8) | incoming_bytes[2];

  return Id;
}

void Stm32::ReadoutUnprotect() {
  SendCmd(CMD::READOUT_UNPROTECT);

  CheckForAck();

  return;
}

//TODO: Not working with board for some reason ?
void Stm32::Erase(uint8_t* page_codes, const uint8_t& num_of_pages) {
  SendCmd(CMD::ERASE);

  //This is how the Erase message is structured, AN3155 for more info
  uint8_t message_length = num_of_pages + 2;
  uint8_t message[message_length];
  message[0] = num_of_pages - 1;
  memcpy(message + 1, page_codes, num_of_pages);

  AddCheckSum(message, message_length);

  SendMessage(message, num_of_pages + 2);

  return;
}

void Stm32::ExtendedErase(uint16_t* page_codes, const uint16_t& num_of_pages) {
  SendCmd(CMD::EXTEND_ERASE);

  // This is how the Extend Erase message is made, (look up AN3155)
  // it's confusing but not worth making its own method
  uint8_t message_length = 3 + (2 * (num_of_pages));
  uint8_t message[message_length];

  message[0] = ((num_of_pages - 1) >> 8);
  message[1] = (num_of_pages - 1) & 0xff;
  for (int ii = 0; ii < num_of_pages; ii++) {
    message[ii * 2 + 2] = (page_codes[ii] >> 8);
    message[ii * 2 + 3] = page_codes[ii] & 0xff;
  }

  AddCheckSum(message, 9);

  SendMessage(message, 9);

  return;
}

void Stm32::SpecialExtendedErase(const uint16_t& special_extended_erase_code) {
  SendCmd(CMD::EXTEND_ERASE);

  // This is how the Extend Erase message for special codes is made, (look up AN3155)
  uint8_t message_length = 3;
  uint8_t message[message_length];

  message[0] = (special_extended_erase_code >> 8);
  message[1] = (special_extended_erase_code & 0xff);
  message[2] = SpecialExtendedEraseCheckSum(special_extended_erase_code);

  SendMessage(message, message_length);

  return;
}

void Stm32::GoToAddress(const uint32_t& address) {
  SendCmd(Schmi::CMD::GO);

  SendAddressMessage(address);

  return;
}

void Stm32::WriteMemory(uint8_t* bytes, const uint16_t& num_bytes, const uint32_t& start_address) {
  SendCmd(CMD::WRITE_MEMORY);

  SendAddressMessage(start_address);

  SendMemoryBytesMessage(bytes, num_bytes);

  return;
}

void Stm32::SendAddressMessage(const uint32_t& address) {
  // Check AN3155.pdf for message structure
  uint8_t message_length = 5;
  uint8_t message[message_length];
  message[3] = address & 0xFF;
  message[2] = (address >> 8) & 0xFF;
  message[1] = (address >> 16) & 0xFF;
  message[0] = (address >> 24) & 0xFF;

  AddCheckSum(message, message_length);

  SendMessage(message, message_length);

  return;
}

void Stm32::SendMemoryBytesMessage(uint8_t* bytes, const uint16_t& num_bytes) {
  uint16_t message_length = num_bytes + 2;  // plus first byte and checksum
  uint8_t message[message_length];

  message[0] = num_bytes - 1;
  std::copy(bytes, bytes + num_bytes, message + 1);

  AddCheckSum(message, message_length);

  SendMessage(message, message_length);

  return;
}

void Stm32::SendCmd(const uint8_t* cmd) {
  uint8_t message_length = 2;
  uint8_t message[message_length];
  memcpy(message, cmd, message_length);

  SendMessage(message, message_length);

  return;
}

void Stm32::CheckForAck() {
  uint8_t num_bytes_to_read = 1;
  uint8_t buffer[num_bytes_to_read];
  ReadBytes(buffer, num_bytes_to_read);

  if (*buffer != CMD::ACK) {
    Schmi::Error err = {"CheckForAck", "Not ACK", *buffer};
    error_handler_.Init(err);
    error_handler_.DisplayAndDie();
  }

  return;
}

void Stm32::SendMessage(uint8_t* message, const size_t& message_length) {
  SendBytes(message, message_length);
  CheckForAck();

  return;
}

void Stm32::SendBytes(uint8_t* buffer, const size_t& buffer_length) {
  if (ser_.Write(buffer, buffer_length) != 0) {
    Schmi::Error err = {"SendBytes", "Failed to send Bytes", -1};
    error_handler_.Init(err);
    error_handler_.DisplayAndDie();
  }

  return;
}

void Stm32::ReadBytes(uint8_t* buffer, const size_t& num_bytes) {
  int result = ser_.Read(buffer, num_bytes);

  if (result != 0) {
    Schmi::Error err = {"ReadBytes", "Failed to read Bytes", result};
    error_handler_.Init(err);
    error_handler_.DisplayAndDie();
  }

  return;
}

void Stm32::AddCheckSum(uint8_t* message, const size_t& message_length) {
  uint8_t checksum = CalculateCheckSum(message, message_length - 1);
  message[message_length - 1] = checksum;

  return;
}

uint8_t Stm32::CalculateCheckSum(uint8_t* buffer, const size_t& num_bytes) {
  uint8_t checksum = 0;

  for (size_t ii = 0; ii < num_bytes; ii++) {
    checksum = buffer[ii] ^ checksum;
  }

  return checksum;
}

uint8_t Stm32::SpecialExtendedEraseCheckSum(const uint16_t& special_extended_erase_code) {
  uint8_t checksum;

  switch (special_extended_erase_code) {
    case 0xFFFF:  // global errase
      checksum = 0x00;
      break;

    case 0xFFFE:  // bank 1 mass erase
      checksum = 0x01;
      break;

    case 0xFFFD:  // bank 2 mass erase
      checksum = 0x02;
      break;

    default:
      Schmi::Error err = {"SpecialExtendedErase", "Code not recognized", special_extended_erase_code};
      error_handler_.Init(err);
      error_handler_.DisplayAndDie();
      break;
  }

  return checksum;
}

VersionAndReadProtectionData Stm32::CreateVersionAndReadProtection(uint8_t* bytes) {
  VersionAndReadProtectionData version_read_protection;
  version_read_protection.version = bytes[0];
  version_read_protection.option1 = bytes[1];
  version_read_protection.option2 = bytes[2];

  return version_read_protection;
}
}