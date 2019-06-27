#include "Schmi/stm32.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Schmi/binary_file_std.hpp"
#include "Schmi/error_handler_interface.hpp"
#include "Schmi/error_handler_std.hpp"
#include "mock_serial_interface.hpp"

#include <istream>
#include <iterator>
#include <string>

// Add useful testing functions to namespace
using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Args;
using ::testing::Return;

using ::testing::SetArrayArgument;
using ::testing::InSequence;
using ::testing::AtLeast;
using ::testing::DoAll;

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
  // uint8_t message[1] = {Schmi::CMD::USART_INIT};
  uint8_t message[2];
  memcpy(message, Schmi::CMD::USART_INIT, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK Check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));
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
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));
    // Read Version and read protection message
    EXPECT_CALL(mock_ser_, Read(_, 4))
        .Times(1)
        .WillOnce(Return(0));
  }
  stm32_->GetVersionAndReadProtection();
}

TEST_F(Stm32Test, GetVersionAndReadProtection_Return) {
  //for ACK
  uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
  ON_CALL(mock_ser_, Read(_, 1))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

  uint8_t incoming_bytes[4] = {0x32, 0x00, 0x00, 0x79};  //4th byte is ACK
  ON_CALL(mock_ser_, Read(_, 4))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_bytes, incoming_bytes + 4), Return(0)));

  Schmi::VersionAndReadProtectionData vrp_expected = {incoming_bytes[0], incoming_bytes[1], incoming_bytes[2]};
  Schmi::VersionAndReadProtectionData vrp = stm32_->GetVersionAndReadProtection();

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
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    // Read Version and read protection message
    EXPECT_CALL(mock_ser_, Read(_, 4))
        .Times(1)
        .WillOnce(Return(0));
  }
  stm32_->GetID();
}

TEST_F(Stm32Test, GetID_Return) {
  //for ACK
  uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
  ON_CALL(mock_ser_, Read(_, 1))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

  uint8_t incoming_bytes[4] = {0x01, 0x04, 0x12, 0x79};  //4th byte is ACK
  ON_CALL(mock_ser_, Read(_, 4))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_bytes, incoming_bytes + 4), Return(0)));

  uint16_t id_expected = (incoming_bytes[1] << 8) | incoming_bytes[2];
  uint16_t id = stm32_->GetID();

  EXPECT_EQ(id_expected, id);
}

TEST_F(Stm32Test, ReadoutUnprotected_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::READOUT_UNPROTECT, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(2)
        .WillRepeatedly(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));
  }

  stm32_->ReadoutUnprotect();
}

TEST_F(Stm32Test, Erase_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::ERASE, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint8_t num_of_pages = 2;
    uint8_t page_codes[num_of_pages] = {3, 4};

    uint8_t message_without_checksum[num_of_pages + 1];
    message_without_checksum[0] = num_of_pages - 1;
    memcpy(message_without_checksum + 1, page_codes, num_of_pages);

    uint8_t checksum = 0x06;

    //need to write size explicitely for EXPECT_CALL, ElementsAreArray()
    uint8_t full_message[4];  //size = num_of_pages + 2
    memcpy(full_message, message_without_checksum, num_of_pages + 1);
    full_message[num_of_pages + 1] = checksum;

    // write num pages, locations and checksum;
    EXPECT_CALL(mock_ser_, Write(_, num_of_pages + 2))
        .With(Args<0, 1>(ElementsAreArray(full_message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->Erase(page_codes, num_of_pages);
  }
}

TEST_F(Stm32Test, ExtendedErase_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::EXTEND_ERASE, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint16_t number_of_pages = 3;
    uint16_t page_codes[number_of_pages] = {0, 1, 5};

    uint8_t checksum = 0x06;
    uint8_t message[9];
    message[0] = ((number_of_pages - 1) >> 8);
    message[1] = (number_of_pages - 1) & 0xff;
    for (int ii = 0; ii < number_of_pages; ii++) {
      message[ii * 2 + 2] = (page_codes[ii] >> 8);
      message[ii * 2 + 3] = page_codes[ii] & 0xff;
    }
    message[8] = checksum;

    EXPECT_CALL(mock_ser_, Write(_, 9))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->ExtendedErase(page_codes, number_of_pages);
  }
}

TEST_F(Stm32Test, SpecialExtendedErase0xFFFF_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::EXTEND_ERASE, 2);
  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint16_t code1 = 0xFFFF;

    uint8_t message1[3];
    message1[0] = (code1 >> 8);
    message1[1] = (code1 & 0xff);
    message1[2] = 0x00;
    EXPECT_CALL(mock_ser_, Write(_, 3))
        .With(Args<0, 1>(ElementsAreArray(message1)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->SpecialExtendedErase(code1);
  }
}

TEST_F(Stm32Test, SpecialExtendedErase0xFFFE_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::EXTEND_ERASE, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint16_t code2 = 0xFFFE;

    uint8_t message2[3];
    message2[0] = (code2 >> 8);
    message2[1] = (code2 & 0xff);
    message2[2] = 0x01;
    EXPECT_CALL(mock_ser_, Write(_, 3))
        .With(Args<0, 1>(ElementsAreArray(message2)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->SpecialExtendedErase(code2);
  }
}

TEST_F(Stm32Test, SpecialExtendedErase0xFFFD_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::EXTEND_ERASE, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint16_t code3 = 0xFFFD;

    uint8_t message3[3];
    message3[0] = (code3 >> 8);
    message3[1] = (code3 & 0xff);
    message3[2] = 0x02;
    EXPECT_CALL(mock_ser_, Write(_, 3))
        .With(Args<0, 1>(ElementsAreArray(message3)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->SpecialExtendedErase(code3);
  }
}

TEST_F(Stm32Test, WriteMemory_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::WRITE_MEMORY, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint32_t start_address = 0x08000000;
    uint8_t checksum1 = 0x08;
    uint8_t address_message[5];
    address_message[4] = checksum1;
    address_message[3] = start_address & 0xFF;
    address_message[2] = (start_address >> 8) & 0xFF;
    address_message[1] = (start_address >> 16) & 0xFF;
    address_message[0] = (start_address >> 24) & 0xFF;

    EXPECT_CALL(mock_ser_, Write(_, 5))
        .With(Args<0, 1>(ElementsAreArray(address_message)))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    Schmi::BinaryFileStd test_binary("../../test.bin");
    test_binary.Init();

    uint64_t binary_file_size = test_binary.GetBinaryFileSize();
    uint8_t bytes[binary_file_size];
    test_binary.GetBytesArray(bytes);

    //need to write size explicitely for EXPECT_CALL, ElementsAreArray()
    uint8_t bytes_message[32 + 2];
    uint8_t checksum2 = 0x3F;
    bytes_message[0] = (uint8_t)binary_file_size - 1;
    std::copy(bytes, bytes + binary_file_size, bytes_message + 1);
    bytes_message[33] = checksum2;

    EXPECT_CALL(mock_ser_, Write(_, 34))
        .With(Args<0, 1>(ElementsAreArray(bytes_message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->WriteMemory(bytes, binary_file_size, start_address);
  }
}

TEST_F(Stm32Test, Go_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::GO, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint32_t start_address = 0x08000000;
    uint8_t checksum = 0x08;
    uint8_t message[5];
    message[4] = checksum;
    message[3] = start_address & 0xFF;
    message[2] = (start_address >> 8) & 0xFF;
    message[1] = (start_address >> 16) & 0xFF;
    message[0] = (start_address >> 24) & 0xFF;

    EXPECT_CALL(mock_ser_, Write(_, 5))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    stm32_->GoToAddress(start_address);
  }
}

// TEST_F(Stm32Test, CheckForACK) {
//   EXPECT_CALL(mock_ser_, Read(_, 1))
//       .Times(1);

//   uint8_t test_buffer_NACK[1] = {Schmi::CMD::NACK};
//   ON_CALL(mock_ser_, Read(_, _))
//       .WillByDefault(testing::SetArrayArgument<0>(test_buffer_NACK, test_buffer_NACK + 1));

//   testing::internal::CaptureStderr();
//   stm32_->InitUsart();
//   std::string output = testing::internal::GetCapturedStderr();

//   Schmi::Error err = {"CheckForAck", "Not ACK", *test_buffer_NACK};
//   std::string error_message = MakeErrorMessage(err);

//   EXPECT_STREQ(error_message.c_str(), output.c_str());
// }

// TEST_F(Stm32Test, SendBytes) {
//   uint8_t test_buffer[1] = {0};

//   EXPECT_CALL(mock_ser_, Write(_, 1))
//       .With(Args<0, 1>(ElementsAreArray(test_buffer)))
//       .Times(1)
//       .WillOnce(Return(1));

//   testing::internal::CaptureStderr();
//   stm32_->SendBytes(test_buffer, 1);
//   std::string output = testing::internal::GetCapturedStderr();

//   Schmi::Error err = {"SendBytes", "Failed to send Bytes", -1};
//   std::string error_message = MakeErrorMessage(err);
//   EXPECT_STREQ(error_message.c_str(), output.c_str());
// }

// TEST_F(Stm32Test, ReadBytes) {
//   EXPECT_CALL(mock_ser_, Read(_, 1))
//       .Times(1);

//   uint8_t test_buffer[1];
//   stm32_->ReadBytes(test_buffer, 1);
// }

// TEST_F(Stm32Test, CalculateCheckSum) {
//   uint8_t test_array_length = 5;
//   uint8_t test_array[test_array_length] = {2, 4, 128, 45, 201};
//   uint8_t expected_checksum = 98;

//   uint8_t checksum = stm32_->CalculateCheckSum(test_array, test_array_length);
//   EXPECT_EQ(expected_checksum, checksum);
// }
