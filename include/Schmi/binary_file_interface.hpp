#ifndef SCHMI_BINARY_FILE_INTERFACE_HPP
#define SCHMI_BINARY_FILE_INTERFACE_HPP
//test 2
#include <cstdint>

namespace Schmi {

class BinaryFileInterface {
 public:
  virtual ~BinaryFileInterface(){};

  virtual void Init() = 0;
  virtual uint64_t GetBinaryFileSize() = 0;
  virtual void GetBytesArray(uint8_t* bytes) = 0;
};
}

#endif  // SCHMI_BINARY_FILE_INTERFACE