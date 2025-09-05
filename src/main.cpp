#include <iostream>
#include "telemetry_bridge.h"
#include "test_file.h"

SerialInterface serial;

//socat -d -d pty,raw,echo=0 pty, raw, echo=0
void test_serial() {
    const std::string PORT1 = "/dev/pts/1";//enter port1
    const std::string PORT2 = "/dev/pts/2";//enter port2
    const std::string INVALID_PORT = "/dev/invalid_port";
    const int BUAD = 115200;
    Test test;
    test.serialInterfaceTestBasicIO(PORT1, PORT2, BUAD);
    test.serialInterfaceTestBasicIO(INVALID_PORT, PORT2, BUAD);
    test.serialInterfaceTestBasicIO(PORT1, INVALID_PORT, BUAD);
}

void test_msp_parser() {
    Test test;
    test.testMSPParser();
}


int main()
{
    return 0;
}