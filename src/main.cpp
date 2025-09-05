#include <iostream>
#include "telemetry_bridge.h"
#include "test_file.h"

SerialInterface serial;

void test_serial() {
    const std::string PORT1 = "/dev/pts/1";
    const std::string PORT2 = "/dev/pts/2";
    const std::string INVALID_PORT = "/dev/invalid_port";
    const int BUAD = 115200;
    Test test;
    test.serialInterfaceTestBasicIO(PORT1, PORT2, BUAD);
}


int main()
{
    test_serial();
    return 0;
}