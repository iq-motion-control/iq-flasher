#include "Schmi/stm32.hpp"

namespace Schmi {

void Stm32::InitUsart() {
  uint8_t message_length = 1;
  uint8_t message[message_length] = {CMD::USART_INIT};

  SendMessage(message, message_length);

  return;
}

VersionAndReadProtection Stm32::GetVersionAndReadProtection() {
  uint8_t message_length = 2;
  uint8_t message[message_length];
  memcpy(message, CMD::GET_VER_PROTECT_STATUS, message_length);

  SendMessage(message, message_length);

  uint8_t num_incoming_bytes = 4;
  uint8_t incoming_bytes[num_incoming_bytes];
  ReadBytes(incoming_bytes, num_incoming_bytes);

  return CreateVersionAndReadProtection(incoming_bytes);
}

uint16_t Stm32::GetID() {
  uint8_t message_length = 2;
  uint8_t message[message_length];
  memcpy(message, CMD::GET_ID, message_length);

  SendMessage(message, message_length);

  uint8_t num_incoming_bytes = 4;
  uint8_t incoming_bytes[num_incoming_bytes];
  ReadBytes(incoming_bytes, num_incoming_bytes);
  uint16_t Id = (incoming_bytes[1] << 8) | incoming_bytes[2];
  return Id;
}

VersionAndReadProtection Stm32::CreateVersionAndReadProtection(uint8_t* bytes) {
  VersionAndReadProtection version_read_protection;
  version_read_protection.version = bytes[0];
  version_read_protection.option1 = bytes[1];
  version_read_protection.option2 = bytes[2];

  return version_read_protection;
}

void Stm32::SendMessage(uint8_t* message, const size_t& message_length) {
  SendBytes(message, message_length);
  CheckForAck();
}

void Stm32::CheckForAck() {
  uint8_t num_bytes_to_read = 1;
  uint8_t buffer[num_bytes_to_read];
  ReadBytes(buffer, num_bytes_to_read);

  if (*buffer != CMD::ACK) {
    Schmi::Error err = {"CheckForAck", "Not ACK", *buffer};
    error_handler_.Init(err);
    error_handler_.Display();
  }

  return;
}

void Stm32::SendBytes(uint8_t* buffer, const size_t& buffer_length) {
  //TODO: iterate with 256 bytes
  if (ser_.Write(buffer, buffer_length) != 0) {
    Schmi::Error err = {"SendBytes", "Failed to send Bytes", -1};
    error_handler_.Init(err);
    error_handler_.Display();
  }
  return;
}

void Stm32::ReadBytes(uint8_t* buffer, const size_t& num_bytes) {
  //TODO: iterate with 256 bytes maybe ?
  int num_read = ser_.Read(buffer, num_bytes);

  return;
}

uint8_t Stm32::CalculateCheckSum(uint8_t* buffer, const size_t& num_bytes) {
  uint8_t checksum = 0;
  for (uint8_t ii = 0; ii < num_bytes; ii++) {
    checksum = buffer[ii] ^ checksum;
  }

  return checksum;
}
}