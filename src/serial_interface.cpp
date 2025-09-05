//
// Created by danylo on 9/3/25.
//

#include "serial_interface.h"
#include <libserial/SerialStream.h>
#include <system_error>
#include <iostream>
#include <chrono>

SerialInterface::SerialInterface() :portOpen(false) {}

SerialInterface::~SerialInterface() {
     close();
}

bool SerialInterface::open(const std::string &port, int badurate) {
     try {
          serialPort.Open(port);
          serialPort.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
          serialPort.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
          serialPort.SetParity(LibSerial::Parity::PARITY_NONE);
          serialPort.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

          portOpen = true;
          lastError.clear();
          return true;
     }
     catch (const LibSerial::OpenFailed& e) {
          lastError = "Open failed: " + std::string(e.what());
          std::cout << lastError << std::endl;
          return false;
     }
     catch (const std::exception& e) {
          lastError = "Exception: " + std::string(e.what());
          std::cout << lastError << std::endl;
          return false;
     }
}
void SerialInterface::close() {
     if (portOpen) {
          serialPort.Close();
          portOpen = false;
     }
}

bool SerialInterface::isOpen() const {
     return portOpen;
}

ssize_t SerialInterface::write(const std::vector<uint8_t> &data) {
     if (!portOpen) return -1;

     try {
          serialPort.Write(data);
          return data.size();
     }
     catch (const std::exception& e) {
          lastError = "Write error: " + std::string(e.what());
          std::cout << lastError << std::endl;
          return -1;
     }
}

ssize_t SerialInterface::read(std::vector<uint8_t> &buffer, size_t size) {
     if (!portOpen) return -1;

     try {
          buffer.resize(size);

          serialPort.Read(buffer, size,100);

          size_t bytesAvailable = serialPort.GetNumberOfBytesAvailable();
          size_t bytesRead = size - bytesAvailable;

          buffer.resize(bytesRead);
          return buffer.size();
     }
     catch (const LibSerial::ReadTimeout&) {
          if (buffer.empty()) return -1;
          return buffer.size();
     }
     catch (const std::exception& e) {
          lastError = "Read error: " + std::string(e.what());
          std::cout << lastError << std::endl;
          return -1;
     }
}

/*bool SerialInterface::setTimeout(int timeout_ms) {
     if (!portOpen) return false;

     try {
          auto timeout = std::chrono::milliseconds(timeout_ms);
          serialPort.SetTimeout(timeout);
          return true;
     }
     catch (const std::exception& e) {
          lastError = "Timeout error: " + std::string(e.what());
          std::cout << lastError << std::endl;
          return false;
     }
}*/

void SerialInterface::flush() {
     if (portOpen) serialPort.FlushIOBuffers();
}

int SerialInterface::available() const {
     if (!portOpen) return 0;

     try {
          LibSerial::SerialPort& port = const_cast<LibSerial::SerialPort&>(serialPort);
          return port.GetNumberOfBytesAvailable();
     }
     catch (const std::exception& e) {
          return 0;
     }
}





