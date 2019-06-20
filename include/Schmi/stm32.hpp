#ifndef SCHMI_STM32_HPP
#define SCHMI_STM32_HPP

#include "Schmi/error_handler_std.hpp"
#include "Schmi/serial_interface.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

namespace Schmi {

//All these values are from the AN3155 file
namespace CMD {
const uint8_t USART_INIT = 0x7F;
const uint8_t ACK = 0x79;
const uint8_t NACK = 0x1F;
const uint8_t GET = 0x00;
const uint8_t GET_VER_PROTECT_STATUS[2] = {0x01, 0xFE};
const uint8_t GET_ID[2] = {0x02, 0xFD};
const uint8_t READ_MEMORY = 0x11;
const uint8_t GO = 0x21;
const uint8_t WRITE_MEMORY = 0x31;
const uint8_t ERASE = 0x43;
const uint8_t EXTEND_ERASE = 0x44;
const uint8_t WRITE_PROTECT = 0x63;
const uint8_t WRITE_UNPROTECT = 0x73;
const uint8_t READOUT_PROTECT = 0x82;
const uint8_t READOUT_UNPROTECT = 0x92;
}

struct VersionAndReadProtection {
  uint8_t version;
  uint8_t option1;
  uint8_t option2;
};

class Stm32 {
 public:
  Stm32(SerialInterface& ser, ErrorHandlerInterface& error) : ser_(ser), error_handler_(error){};
  ~Stm32(){};

  void InitUsart();

  void Get();  //this is kinda useless for now so no implementation

  VersionAndReadProtection GetVersionAndReadProtection();

  uint16_t GetID();

  void SendMessage(uint8_t* message, const size_t& message_length);

  void CheckForAck();

  void SendBytes(uint8_t* buffer, const size_t& buffer_lenght);

  void ReadBytes(uint8_t* buffer, const size_t& num_bytes);

  uint8_t CalculateCheckSum(uint8_t* buffer, const size_t& num_bytes);

 private:
  SerialInterface& ser_;
  ErrorHandlerInterface& error_handler_;

  VersionAndReadProtection CreateVersionAndReadProtection(uint8_t* bytes);
};
}

#endif  // SCHMI_STM32_HPP