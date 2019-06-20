#include "Schmi/stm32.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Schmi/error_handler_interface.hpp"
#include "Schmi/error_handler_std.hpp"
#include "mock_serial_interface.hpp"

#include <istream>
#include <string>

// Add useful testing functions to namespace
using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Args;
using ::testing::Return;

using ::testing::SetArgPointee;
using ::testing::InSequence;
// using ::testing::AtLeast;
// using ::testing::Exactly;

class Stm32Test : public ::testing::Test {
 protected:
  Stm32Test() {
    stm32_ = new Schmi::Stm32(mock_ser_, error_);
  };

  ~Stm32Test() {
    delete stm32_;
  };

  void SetUp() override{};

  void TearDown() override{};

  std::string MakeErrorMessage(const Schmi::Error& err) {
    std::stringstream error_message;
    error_message << err.error_location << ": ";
    error_message << err.error_string << " - ";
    error_message << err.err_num << "\n\n";
    return error_message.str();
  }

  Schmi::Stm32* stm32_;
  MockSerialInterface mock_ser_;
  Schmi::ErrorHandlerStd error_;
};

TEST_F(Stm32Test, InitUsart) {
  uint8_t message[1] = {Schmi::CMD::USART_INIT};

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 1))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK Check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(Return(0));
  }

  stm32_->InitUsart();
}

TEST_F(Stm32Test, GetVersionAndReadProtection_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::GET_VER_PROTECT_STATUS, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(Return(0));

    // Read Version and read protection message
    EXPECT_CALL(mock_ser_, Read(_, 4))
        .Times(1)
        .WillOnce(Return(0));
  }
  stm32_->GetVersionAndReadProtection();
}

TEST_F(Stm32Test, GetVersionAndReadProtection_Return) {
  uint8_t incoming_bytes[4] = {0x32, 0x00, 0x00, 0x79};  //4th byte is ACK
  ON_CALL(mock_ser_, Read(_, 4))
      .WillByDefault(testing::SetArrayArgument<0>(incoming_bytes, incoming_bytes + 4));

  Schmi::VersionAndReadProtection vrp_expected = {incoming_bytes[0], incoming_bytes[1], incoming_bytes[2]};
  Schmi::VersionAndReadProtection vrp = stm32_->GetVersionAndReadProtection();

  EXPECT_EQ(vrp.version, vrp_expected.version);
  EXPECT_EQ(vrp.option1, vrp_expected.option1);
  EXPECT_EQ(vrp.option2, vrp_expected.option2);
}

TEST_F(Stm32Test, GetID_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::GET_ID, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(Return(0));

    // Read Version and read protection message
    EXPECT_CALL(mock_ser_, Read(_, 4))
        .Times(1)
        .WillOnce(Return(0));
  }
  stm32_->GetID();
}

TEST_F(Stm32Test, GetID_Return) {
  uint8_t incoming_bytes[4] = {0x01, 0x04, 0x12, 0x79};  //4th byte is ACK
  ON_CALL(mock_ser_, Read(_, 4))
      .WillByDefault(testing::SetArrayArgument<0>(incoming_bytes, incoming_bytes + 4));

  uint16_t id_expected = (incoming_bytes[1] << 8) | incoming_bytes[2];
  uint16_t id = stm32_->GetID();

  EXPECT_EQ(id_expected, id);
}

TEST_F(Stm32Test, CheckForACK) {
  EXPECT_CALL(mock_ser_, Read(_, 1))
      .Times(1);

  uint8_t test_buffer_NACK[1] = {Schmi::CMD::NACK};
  ON_CALL(mock_ser_, Read(_, _))
      .WillByDefault(testing::SetArrayArgument<0>(test_buffer_NACK, test_buffer_NACK + 1));

  testing::internal::CaptureStderr();
  stm32_->InitUsart();
  std::string output = testing::internal::GetCapturedStderr();

  Schmi::Error err = {"CheckForAck", "Not ACK", *test_buffer_NACK};
  std::string error_message = MakeErrorMessage(err);

  EXPECT_STREQ(error_message.c_str(), output.c_str());
}

TEST_F(Stm32Test, SendBytes) {
  uint8_t test_buffer[1] = {0};

  EXPECT_CALL(mock_ser_, Write(_, 1))
      .With(Args<0, 1>(ElementsAreArray(test_buffer)))
      .Times(1)
      .WillOnce(Return(1));

  testing::internal::CaptureStderr();
  stm32_->SendBytes(test_buffer, 1);
  std::string output = testing::internal::GetCapturedStderr();

  Schmi::Error err = {"SendBytes", "Failed to send Bytes", -1};
  std::string error_message = MakeErrorMessage(err);
  EXPECT_STREQ(error_message.c_str(), output.c_str());
}

TEST_F(Stm32Test, ReadBytes) {
  EXPECT_CALL(mock_ser_, Read(_, 1))
      .Times(1);

  uint8_t test_buffer[1];
  stm32_->ReadBytes(test_buffer, 1);
}

TEST_F(Stm32Test, CalculateCheckSum) {
  uint8_t test_array_length = 5;
  uint8_t test_array[test_array_length] = {2, 4, 128, 45, 201};
  uint8_t expected_checksum = 98;

  uint8_t checksum = stm32_->CalculateCheckSum(test_array, test_array_length);
  EXPECT_EQ(expected_checksum, checksum);
}
