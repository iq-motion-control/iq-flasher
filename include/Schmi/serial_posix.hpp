#ifndef SCHMI_SERIAL_POSIX_HPP
#define SCHMI_SERIAL_POSIX_HPP

#include "Schmi/serial_interface.hpp"
#include "Schmi/std_exception.hpp"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#include <clocale>
#include <cstddef>
#include <cstring>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace Schmi {

namespace SerialConst {
const speed_t BAUD_RATE = B115200;
}

class SerialPosix : public SerialInterface {
 public:
  SerialPosix(const std::string& usb_handle) : usb_handle_(usb_handle){};
  ~SerialPosix() { close(usb_flag_); };

  int Write(uint8_t* buffer, const uint8_t& buffer_length) override;
  int Read(uint8_t* buffer, const uint8_t& num_bytes) override;

  void Init();

 private:
  std::string usb_handle_;
  int usb_flag_ = -1;

  int CheckReadQueue();

  int OpenPort();

  void SetAttributes(const int& usb_flag);
  void GetTerminalAttributes(const int& usb_flag, struct termios& tty);
  void SetBaudRate(struct termios& tty);
  void SetTerminalAttributes(const int& usb_flag, struct termios& tty);

  //Private interal debugging functions
  void DisplayBytes(uint8_t* bytes, const uint8_t& num_bytes);
};
}

#endif  //SCHMI_SERIAL_POSIX_HPP