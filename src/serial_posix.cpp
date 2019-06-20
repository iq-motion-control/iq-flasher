#include "Schmi/serial_posix.hpp"

namespace Schmi {

int SerialPosix::Write(uint8_t* buffer, const uint8_t& buffer_length) {
  try {
    int num_bytes_written = write(usb_flag_, buffer, buffer_length);
    tcdrain(usb_flag_);

    if (num_bytes_written != buffer_length) {
      std::stringstream err_message;
      err_message << "Error writting bytes: " << num_bytes_written;
      err_message << " / wanted to write: " << (int)buffer_length;
      throw Schmi::StdException(err_message.str());
    }
  } catch (const StdException& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  std::cout << "WRITE - ";
  DisplayBytes(buffer, buffer_length);
  return 0;
}

int SerialPosix::Read(uint8_t* buffer, const uint8_t& num_bytes) {
  try {
    uint8_t bytes_left = num_bytes;
    while (bytes_left) {
      int num_bytes_read = read(usb_flag_, buffer, bytes_left);

      if (num_bytes_read < 1) {
        std::stringstream err_message;
        err_message << "Error reading bytes: " << num_bytes_read;
        throw Schmi::StdException(err_message.str());
      }

      bytes_left -= num_bytes_read;
      buffer += num_bytes_read;
    }

  } catch (const StdException& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  std::cout << "READ - ";
  DisplayBytes(buffer - num_bytes, num_bytes);
  return 0;
}

int SerialPosix::CheckReadQueue() {
  int num_bytes_in_queue;
  ioctl(usb_flag_, FIONREAD, &num_bytes_in_queue);

  return num_bytes_in_queue;
}

void SerialPosix::Init() {
  try {
    int usb_flag = OpenPort();
    SetAttributes(usb_flag);
    usb_flag_ = usb_flag;

  } catch (const StdException& e) {
    std::cerr << e.what() << '\n';
  }

  return;
}

int SerialPosix::OpenPort() {
  int usb_flag = open(usb_handle_.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

  if (usb_flag < 0) {
    std::stringstream err_message;
    err_message << "Could not open USB : " << usb_flag;
    throw Schmi::StdException(err_message.str());
  }

  return usb_flag;
}

void SerialPosix::SetAttributes(const int& usb_flag) {
  struct termios tty;

  GetTerminalAttributes(usb_flag, tty);
  SetBaudRate(tty);
  SetTerminalAttributes(usb_flag, tty);

  return;
}

void SerialPosix::GetTerminalAttributes(const int& usb_flag, struct termios& tty) {
  if (tcgetattr(usb_flag, &tty) < 0) {
    std::stringstream err_message;
    err_message << "Error from tcgetattr: " << std::strerror(errno);
    throw Schmi::StdException(err_message.str());
  }

  return;
}

void SerialPosix::SetBaudRate(struct termios& tty) {
  cfsetospeed(&tty, SerialConst::BAUD_RATE);
  cfsetispeed(&tty, SerialConst::BAUD_RATE);

  return;
}

void SerialPosix::SetTerminalAttributes(const int& usb_flag, struct termios& tty) {
  // These settings are similar to cfmakeraw(tty) with parity bit set
  // I wrote them explicitly for better understanding

  tty.c_cflag |= (CLOCAL | CREAD);             // ignore modem controls
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
  tty.c_cflag |= PARENB;                       // even parity bit
  tty.c_cflag &= ~CSTOPB;                      // only need 1 stop bit
  tty.c_cflag &= ~CRTSCTS;                     //no hardware flowcontrol

  // setup for non-canonical mode
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tty.c_oflag &= ~OPOST;

  tty.c_cc[VMIN] = 0;   // read doesn't block
  tty.c_cc[VTIME] = 1;  // 0.5 seconds read timeout

  if (tcsetattr(usb_flag, TCSANOW, &tty) != 0) {
    std::stringstream err_message;
    err_message << "Error from tcsetattr: " << std::strerror(errno);
    throw Schmi::StdException(err_message.str());
  }

  return;
}

void SerialPosix::DisplayBytes(uint8_t* bytes, const uint8_t& num_bytes) {
  uint8_t* buffer;
  int buffer_length = num_bytes;
  printf("bytes %d: ", buffer_length);
  for (buffer = bytes; buffer_length-- > 0; buffer++)
    printf(" 0x%x", *buffer);
  printf("\n\n");
}
}
