#include "Schmi/serial_posix.hpp"

namespace Schmi {

int SerialPosix::Write(uint8_t* buffer, const uint16_t& buffer_length) {
  try {
    int num_bytes_written = write(usb_flag_, buffer, buffer_length);
    tcdrain(usb_flag_);

    CheckNumBytesWritten(num_bytes_written, buffer_length);

  } catch (const StdException& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  return 0;
}

void SerialPosix::CheckNumBytesWritten(const int& num_bytes_written, const uint16_t& buffer_length) {
  if (num_bytes_written != buffer_length) {
    std::stringstream err_message;
    err_message << "Error writting bytes: " << num_bytes_written;
    err_message << " / wanted to write: " << (int)buffer_length;
    throw Schmi::StdException(err_message.str());
  }
}

int SerialPosix::Read(uint8_t* buffer, const uint16_t& num_bytes, const uint16_t& timeout_ms) {
  SerialReadData read_data = {buffer, num_bytes};
  try {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (read_data.bytes_left) {
      if (std::chrono::steady_clock::now() - start > std::chrono::milliseconds(timeout_ms)) {
        std::stringstream err_message;
        err_message << "Read Timout: " << timeout_ms;
        throw Schmi::StdException(err_message.str());
      }

      int num_bytes_read = read(usb_flag_, read_data.buffer, read_data.bytes_left);

      CheckNumBytesRead(num_bytes_read);

      UpdateSerialReadData(read_data, num_bytes_read);
    }

  } catch (const StdException& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  return 0;
}

void SerialPosix::CheckNumBytesRead(const int& num_bytes_read) {
  if (num_bytes_read < 0) {
    std::stringstream err_message;
    err_message << "Error reading bytes: " << num_bytes_read;
    throw Schmi::StdException(err_message.str());
  }

  return;
}

void SerialPosix::UpdateSerialReadData(SerialReadData& read_data, const int& num_bytes_read) {
  read_data.bytes_left -= num_bytes_read;
  read_data.buffer += num_bytes_read;

  return;
}

void SerialPosix::Init() {
  try {
    int usb_flag = OpenPort();
    SetAttributes(usb_flag);
    usb_flag_ = usb_flag;

  } catch (const StdException& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    exit(EXIT_FAILURE);
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

void SerialPosix::DisplayBytes(uint8_t* bytes, const uint16_t& num_bytes) {
  uint8_t* buffer;
  uint16_t buffer_length = num_bytes;
  printf("bytes %d: ", buffer_length);
  for (buffer = bytes; buffer_length-- > 0; buffer++) {
    if (*buffer == 0x79) {
      printf("ACK:");
    } else if (*buffer == 0x1F) {
      printf("NACK:");
    }
    printf("0x%x ", *buffer);
  }
  printf("\n\n");
}
}  // namespace Schmi
