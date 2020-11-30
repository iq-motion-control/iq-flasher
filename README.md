![schmi_image](misc/Anakinandmom.jpg)
# Schmi

This is a C++ tool to flash firmware to our boards over UART.  
It is called Schmi as the firmware is the force, and Schmi is Anakin's mother, Schmi indirectly gave the force to Anakin.
  
This branch contains all the source code and tests with implemenations to run with the "std" libraries and posix serial.

## Getting Started

### Prerequisites

You will need Cmake in order to build and run this repository.

### Running

Here is an example on how to use the library:

```
std::string binary_file = "binaries/0x100016_iq2306_2200kv.bin";

std::cout << "Binary to flash: " << binary_file << "\n\n";

Schmi::ErrorHandlerStd error;
Schmi::BinaryFileStd bin(binary_file);
Schmi::SerialPosix ser("/dev/ttyUSB0");
Schmi::LoadingBarStd bar;

Schmi::FlashLoader fl(&ser, &bin, &error, &bar);

fl.Init();
fl.Flash(true, false);

return EXIT_SUCCESS;
```

## Implementing Schmi In Other Software

Schmi is build without the use of the std library. This allows you to implment schmi on nearly any platform.  
To do so you just need to implement a few interfaces specific to your platform:

- binary_file_interface.hpp
- error_handler_interface.hpp
- loading_bar_interface.hpp
- serial_interface.hpp

These interfaces are already implemented if you can use the std library and Posix. You can use these implementations as example.

## Running Tests

After building the repository with Cmake, a test binary should be automatically created.
This also integrates automatically with the Cmake extension of vscode.

## coding style 

we use a .clang-format for coding style (from the Cpp style guide)

## Versioning

We use [SemVer](http://semver.org/) for versioning.  

## Authors

* **Raphael Van Hoffelen** 