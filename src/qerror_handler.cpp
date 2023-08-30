#include "iq_flasher/include/Schmi/qerror_handler.hpp"

namespace Schmi {
void ErrorHandlerStd::Init(const Error& error) { error_ = error; }

void ErrorHandlerStd::Display() {
  std::ostringstream error;
  error << error_.error_location << ": ";
  error << error_.error_string << " - ";

  iv.label_message->setText(QString::fromStdString(error.str()));
  iv.pcon->AddToLog(QString::fromStdString(error.str()));
}

void ErrorHandlerStd::DisplayAndDie() {
  std::ostringstream error;
  error << error_.error_location << ": ";
  error << error_.error_string << " - ";

  iv.pcon->AddToLog(QString::fromStdString(error.str()));
  throw QString::fromStdString(error.str());
}
};  // namespace Schmi
