//
// Created by danylo on 9/3/25.
//

#ifndef TELEMETRYBRIDGE_SERIAL_INTERFACE_H
#define TELEMETRYBRIDGE_SERIAL_INTERFACE_H

#include <libserial/SerialPort.h>
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

class SerialInterface {
public:
    SerialInterface();
    ~SerialInterface();

    bool open(const std::string& port, int badurate);
    void close();
    bool isOpen() const;

    ssize_t write(const std::vector<uint8_t>& data);
    ssize_t read(std::vector<uint8_t>& buffer, size_t size);

    bool setTimeout(int timeout_ms);
    void flush();
    int available() const;

private:
    LibSerial::SerialPort serialPort;
    bool portOpen;
    std::string lastError;
};
#endif //TELEMETRYBRIDGE_SERIAL_INTERFACE_H