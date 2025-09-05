//
// Created by danylo on 9/3/25.
//

#ifndef TELEMETRYBRIDGE_TESTFILE_H
#define TELEMETRYBRIDGE_TESTFILE_H

#include "serial_interface.h"
#include <string>
#include <atomic>

class Test {
public:
    bool serialInterfaceTestBasicIO(const std::string& port1, const std::string& port2, int baud);
};


#endif //TELEMETRYBRIDGE_TESTFILE_H