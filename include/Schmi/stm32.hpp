#ifndef SCHMI_STM32_HPP
#define SCHMI_STM32_HPP

#include "Schmi/error_handler_interface.hpp"
#include "Schmi/serial_interface.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

namespace Schmi {

//All these values are from the AN3155 file
namespace CMD {
const uint8_t USART_INIT[2] = {0x7F, 0x00};  //added dummy 0x00 byte so all commands are 2 bytes long
const uint8_t ACK = 0x79;
const uint8_t NACK = 0x1F;
const uint8_t GET = 0x00;
const uint8_t GET_VER_PROTECT_STATUS[2] = {0x01, 0xFE};
const uint8_t GET_ID[2] = {0x02, 0xFD};
const uint8_t READ_MEMORY = 0x11;
const uint8_t GO[2] = {0x21, 0xDE};
const uint8_t WRITE_MEMORY[2] = {0x31, 0xCE};
const uint8_t ERASE[2] = {0x43, 0xBC};
const uint8_t EXTEND_ERASE[2] = {0x44, 0xBB};
const uint8_t WRITE_PROTECT = 0x63;
const uint8_t WRITE_UNPROTECT = 0x73;
const uint8_t READOUT_PROTECT = 0x82;
const uint8_t READOUT_UNPROTECT[2] = {0x92, 0x6D};
}

struct VersionAndReadProtectionData {
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

  VersionAndReadProtectionData GetVersionAndReadProtection();

  uint16_t GetID();

  void ReadoutUnprotect();

  void Erase(uint8_t* page_codes, const uint8_t& num_of_pages);

  void ExtendedErase(uint16_t* page_codes, const uint16_t& num_of_pages);

  void SpecialExtendedErase(const uint16_t& special_extended_erase_code);

  void GoToAddress(const uint32_t& address);

  void WriteMemory(uint8_t* bytes, const uint16_t& num_bytes, const uint32_t& start_address);

 private:
  SerialInterface& ser_;
  ErrorHandlerInterface& error_handler_;

  void SendAddressMessage(const uint32_t& address);
  void SendMemoryBytesMessage(uint8_t* bytes, const uint16_t& num_bytes);

  void SendCmd(const uint8_t* cmd);
  void CheckForAck();

  void SendMessage(uint8_t* message, const size_t& message_length);

  void SendBytes(uint8_t* buffer, const size_t& buffer_lenght);
  void ReadBytes(uint8_t* buffer, const size_t& num_bytes);

  void AddCheckSum(uint8_t* message, const size_t& num_bytes);
  uint8_t CalculateCheckSum(uint8_t* buffer, const size_t& num_bytes);
  uint8_t SpecialExtendedEraseCheckSum(const uint16_t& special_extended_erase_code);

  VersionAndReadProtectionData CreateVersionAndReadProtection(uint8_t* bytes);
};
}

#endif  // SCHMI_STM32_HPP