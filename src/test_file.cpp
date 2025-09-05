//
// Created by danylo on 9/3/25.
//

#include "test_file.h"
#include <iostream>
#include <bits/this_thread_sleep.h>

bool Test::serialInterfaceTestBasicIO(const std::string &port1, const std::string &port2, int baud) {
    std::cout << "\n==== Test basic serial IO ====" << std::endl;
    SerialInterface serial1;
    SerialInterface serial2;

    std::string testMessage = "Serial test 1234!";
    std::vector<uint8_t> sendData(testMessage.begin(), testMessage.end());

    if (!serial1.open(port1, baud)) {
        std::cout << "BasicIO open port1 false!" << std::endl;
        return false;
    }
    if (!serial2.open(port2, baud)) {
        std::cout << "BasicIO open port2 false!" << std::endl;
        return false;
    }

    ssize_t written = serial1.write(sendData);
    if (written != static_cast<ssize_t>(sendData.size())) {
        std::cout << "Failed to write data!" << std::endl;
        return false;
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::vector<uint8_t> receiveData;
    ssize_t read = serial2.read(receiveData, 256);

    std::string receivedText(receiveData.begin(),receiveData.end());
    if (read != written) {
        std::cout << "Data size mismatch" << std::endl;
        //std::cout << "read: " << read << " " << receivedText << " written: " << written << " " << testMessage << std::endl;
        return false;
    }

    //std::string receivedText(receiveData.begin(),receiveData.end());

    if (testMessage != receivedText) {
        std::cout << "Content mismatch!" << std::endl;
        return false;
    }

    std::cout << "\n==== All checks passed ====\n" << std::endl;
    return true;

}
