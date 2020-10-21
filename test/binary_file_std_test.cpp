#include "Schmi/binary_file_std.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using ::testing::ContainerEq;
using ::testing::Eq;

class BinaryFileStdTest : public ::testing::Test {
 protected:
  BinaryFileStdTest() {
    iq_bin_ = new Schmi::BinaryFileStd("../test_files/1048583_V6-3.bin");
    test_bin_ = new Schmi::BinaryFileStd("../test_files/test.bin");
  };

  ~BinaryFileStdTest() {
    delete iq_bin_;
    delete test_bin_;
  };

  void SetUp() override{};

  void TearDown() override{};

  Schmi::BinaryFileStd* iq_bin_;
  Schmi::BinaryFileStd* test_bin_;
};

TEST_F(BinaryFileStdTest, GetFileSize) {
  iq_bin_->Init();
  test_bin_->Init();

  uint64_t size = test_bin_->GetBinaryFileSize();
  EXPECT_EQ(32, test_bin_->GetBinaryFileSize());
  EXPECT_EQ(53776, iq_bin_->GetBinaryFileSize());
};

TEST_F(BinaryFileStdTest, GetBytesArray_testbin) {
  std::ifstream test_binary_file("../test_files/test.bin", std::ios::binary);

  test_binary_file.seekg(0, test_binary_file.end);
  uint64_t test_binary_file_size = test_binary_file.tellg();
  test_binary_file.seekg(0, test_binary_file.beg);

  char raw_data[test_binary_file_size];
  test_binary_file.read(raw_data, test_binary_file_size);
  std::vector<uint8_t> test_bytes(raw_data, raw_data + test_binary_file_size);

  test_binary_file.close();

  test_bin_->Init();

  uint16_t binary_file_size = test_bin_->GetBinaryFileSize();
  //  256 is the max amount of bytes you can flash
  uint8_t bytes_array[256];
  Schmi::BytesData bytes_data = {binary_file_size, 0};
  test_bin_->GetBytesArray(bytes_array, bytes_data);
  std::vector<uint8_t> bytes(bytes_array, bytes_array + bytes_data.num_bytes);

  EXPECT_THAT(bytes, ContainerEq(test_bytes));
};

TEST_F(BinaryFileStdTest, GetBytesArray_iqbin) {
  std::ifstream test_binary_file("../test_files/1048583_V6-3.bin", std::ios::binary);

  test_binary_file.seekg(0, test_binary_file.end);
  uint64_t test_binary_file_size = test_binary_file.tellg();
  test_binary_file.seekg(0, test_binary_file.beg);

  char raw_data[test_binary_file_size];
  test_binary_file.read(raw_data, test_binary_file_size);
  std::vector<uint8_t> test_bytes(raw_data, raw_data + 256);
  test_binary_file.close();

  iq_bin_->Init();

  //  256 is the max amount of bytes you can flash
  uint8_t bytes_array[256];
  Schmi::BytesData bytes_data = {256, 0};
  iq_bin_->GetBytesArray(bytes_array, bytes_data);
  std::vector<uint8_t> bytes(bytes_array, bytes_array + bytes_data.num_bytes);

  // //Eq() instead of ContainerEq() so that it runs faster (huge .bin file)
  // //it's less failure info but runs same test
  EXPECT_THAT(bytes, Eq(test_bytes));
}