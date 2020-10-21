#ifndef SCHMI_TEST_MOCK_SERIAL_INTERFACE_HPP
#define SCHMI_TEST_MOCK_SERIAL_INTERFACE_HPP

#include "Schmi/serial_interface.hpp"

#include "gmock/gmock.h"

class MockSerialInterface : public Schmi::SerialInterface {
 public:
  MOCK_METHOD0(Init, void());
  MOCK_METHOD2(Write, int(uint8_t* buffer, const uint16_t& buffer_length));
  MOCK_METHOD3(Read, int(uint8_t* buffer, const uint16_t& buffer_length, const uint16_t& timeout_ms));
};

#endif  // SCHMI_TEST_MOCK_SERIAL_INTERFACE_HPP