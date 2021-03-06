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
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Return;

using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::SetArrayArgument;

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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));
  }

  ASSERT_TRUE(stm32_->InitUsart());
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    // Read Version and read protection message
    EXPECT_CALL(mock_ser_, Read(_, 4, 500))
        .Times(1)
        .WillOnce(Return(0));
  }
  Schmi::VersionAndReadProtectionData vrpd;
  ASSERT_TRUE(stm32_->GetVersionAndReadProtection(vrpd));
}

TEST_F(Stm32Test, GetVersionAndReadProtection_Return) {
  //for ACK
  uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
  ON_CALL(mock_ser_, Read(_, 1, 500))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

  uint8_t incoming_bytes[4] = {0x32, 0x00, 0x00, 0x79};  //4th byte is ACK
  ON_CALL(mock_ser_, Read(_, 4, 500))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_bytes, incoming_bytes + 4), Return(0)));

  Schmi::VersionAndReadProtectionData vrp_expected = {incoming_bytes[0], incoming_bytes[1], incoming_bytes[2]};
  Schmi::VersionAndReadProtectionData vrpd;
  EXPECT_TRUE(stm32_->GetVersionAndReadProtection(vrpd));

  EXPECT_EQ(vrpd.version, vrp_expected.version);
  EXPECT_EQ(vrpd.option1, vrp_expected.option1);
  EXPECT_EQ(vrpd.option2, vrp_expected.option2);
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    // Read Version and read protection message
    EXPECT_CALL(mock_ser_, Read(_, 4, 500))
        .Times(1)
        .WillOnce(Return(0));
  }
  uint16_t id;
  ASSERT_TRUE(stm32_->GetID(id));
}

TEST_F(Stm32Test, GetID_Return) {
  //for ACK
  uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
  ON_CALL(mock_ser_, Read(_, 1, 500))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

  uint8_t incoming_bytes[4] = {0x01, 0x04, 0x12, 0x79};  //4th byte is ACK
  ON_CALL(mock_ser_, Read(_, 4, 500))
      .WillByDefault(DoAll(SetArrayArgument<0>(incoming_bytes, incoming_bytes + 4), Return(0)));

  uint16_t id_expected = (incoming_bytes[1] << 8) | incoming_bytes[2];
  uint16_t id;
  stm32_->GetID(id);

  EXPECT_EQ(id_expected, id);
}

TEST_F(Stm32Test, ReadMemory_SerialCalls) {
  uint8_t message[2];
  memcpy(message, Schmi::CMD::READ_MEMORY, 2);

  {
    InSequence dummy;
    //CMD send
    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    uint8_t incoming_ACK[1] = {Schmi::CMD::ACK};
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint32_t start_address = 0x08000000;
    uint8_t checksum = 0x08;
    uint8_t address_message[5];
    address_message[4] = checksum;
    address_message[3] = start_address & 0xFF;
    address_message[2] = (start_address >> 8) & 0xFF;
    address_message[1] = (start_address >> 16) & 0xFF;
    address_message[0] = (start_address >> 24) & 0xFF;

    EXPECT_CALL(mock_ser_, Write(_, 5))
        .With(Args<0, 1>(ElementsAreArray(address_message)))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    checksum = ~1;
    uint8_t num_bytes_message[2];
    num_bytes_message[1] = checksum;
    num_bytes_message[0] = 1;

    EXPECT_CALL(mock_ser_, Write(_, 2))
        .With(Args<0, 1>(ElementsAreArray(num_bytes_message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint16_t num_bytes_to_read = 2;
    uint8_t bytes_read_buffer[2];
    bytes_read_buffer[0] = 2;
    EXPECT_CALL(mock_ser_, Read(_, num_bytes_to_read, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(bytes_read_buffer, bytes_read_buffer + 1), Return(0)));

    ASSERT_TRUE(stm32_->ReadMemory(bytes_read_buffer, num_bytes_to_read, start_address));
  }
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(2)
        .WillRepeatedly(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));
  }

  ASSERT_TRUE(stm32_->ReadoutUnprotect());
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 1000))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    ASSERT_TRUE(stm32_->ExtendedErase(page_codes, number_of_pages));
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    uint16_t code1 = 0xFFFF;

    uint8_t message[3];
    message[0] = (code1 >> 8);
    message[1] = (code1 & 0xff);
    message[2] = 0x00;
    EXPECT_CALL(mock_ser_, Write(_, 3))
        .With(Args<0, 1>(ElementsAreArray(message)))
        .Times(1)
        .WillOnce(Return(0));

    // ACK check
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    ASSERT_TRUE(stm32_->SpecialExtendedErase(code1));
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    ASSERT_TRUE(stm32_->SpecialExtendedErase(code2));
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    ASSERT_TRUE(stm32_->SpecialExtendedErase(code3));
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
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

    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    Schmi::BinaryFileStd test_binary("../test_files/test.bin");
    test_binary.Init();

    uint16_t binary_file_size = test_binary.GetBinaryFileSize();
    uint8_t bytes[binary_file_size];
    Schmi::BytesData bytes_data = {binary_file_size, 0};
    test_binary.GetBytesArray(bytes, bytes_data);

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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    ASSERT_TRUE(stm32_->WriteMemory(bytes, binary_file_size, start_address));
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
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
    EXPECT_CALL(mock_ser_, Read(_, 1, 500))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(incoming_ACK, incoming_ACK + 1), Return(0)));

    ASSERT_TRUE(stm32_->GoToAddress(start_address));
  }
}
