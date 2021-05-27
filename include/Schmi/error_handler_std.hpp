#ifndef SCHMI_ERROR_HANDLER_STD_HPPP
#define SCHMI_ERROR_HANDLER_STD_HPPP

#include "Schmi/error_handler_interface.hpp"

#include <iostream>

namespace Schmi {

class ErrorHandlerStd : public ErrorHandlerInterface {
 public:
  ErrorHandlerStd(){};
  ~ErrorHandlerStd(){};

  void Init(const Schmi::Error& error) override;
  void Display() override;
  void DisplayAndDie() override;

 private:
  Error error_;
};
}

#endif  //SCHMI_ERROR_HANDLER_STD_HPPP
