#ifndef QSERIALSCHMI_H
#define QSERIALSCHMI_H

#include "QSerialPort"

#include "iq_flasher/include/Schmi/serial_interface.hpp"
#include "iq_flasher/include/Schmi/std_exception.hpp"

namespace Schmi {

namespace SerialConst {
const qint32 BAUD_RATE = QSerialPort::Baud115200;
};

struct SerialReadData {
  uint8_t* buffer;
  uint16_t bytes_left;
};

class QSerial : public SerialInterface {
 public:
  QSerial(const QString& usb_handle) : usb_handle_(usb_handle){};
  QSerial(){}; //Default constructor that will let us tie QSerial in with QSerialInterface easily
  ~QSerial() override;

  int Write(uint8_t* buffer, const uint16_t& buffer_length) override;
  int Read(uint8_t* buffer, const uint16_t& num_bytes, const uint16_t& timeout_ms = 500) override;
  void Init() override;

  void LinkSerialPort(QSerialPort* ser);
 private:
  QString usb_handle_;
  QSerialPort* qser_port_;

  void CheckNumBytesWritten(const qint64& num_bytes_written, const uint16_t& buffer_length);
  void CheckNumBytesRead(const qint64& num_bytes_read);
  void UpdateSerialReadData(SerialReadData& read_data, const qint64& num_bytes_read);
};
}  // namespace Schmi

#endif  // QSERIALSCHMI_H
