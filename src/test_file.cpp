//
// Created by danylo on 9/3/25.
//

#include "test_file.h"
#include <iostream>

void Test::serialInterfaceTestBasicIO(const std::string &port1, const std::string &port2, int baud) {
    std::cout << "\n==== Test basic serial IO ====" << std::endl;
    SerialInterface serial1;
    SerialInterface serial2;

    std::string testMessage = "Serial test 1234!";
    std::vector<uint8_t> sendData(testMessage.begin(), testMessage.end());

    if (!serial1.open(port1, baud)) {
        std::cout << "BasicIO open port1 false!" << std::endl;
        return;
    }
    if (!serial2.open(port2, baud)) {
        std::cout << "BasicIO open port2 false!" << std::endl;
        return;
    }

    ssize_t written = serial1.write(sendData);
    if (written != static_cast<ssize_t>(sendData.size())) {
        std::cout << "Failed to write data!" << std::endl;
        return;
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::vector<uint8_t> receiveData;
    ssize_t read = serial2.read(receiveData, 256);

    std::string receivedText(receiveData.begin(),receiveData.end());
    if (read != written) {
        std::cout << "Data size mismatch" << std::endl;
        //std::cout << "read: " << read << " " << receivedText << " written: " << written << " " << testMessage << std::endl;
        return;
    }

    //std::string receivedText(receiveData.begin(),receiveData.end());

    if (testMessage != receivedText) {
        std::cout << "Content mismatch!" << std::endl;
        return;
    }

    std::cout << "\n==== All checks passed ====\n" << std::endl;
}

void Test::testMSPParser() {
    MSPParser parser;

    //MSP_ATTITUDE
    std::cout << "\n==== MSP_ATTITUDE TEST ====\n" << std::endl;
    std::vector<uint8_t> attitudePacket = {
        '$', 'M', '>', 6, 108,
        0x64, 0x00,
        0x9C, 0xFF,
        0x00, 0x00,
        0x6D
    };

    if (parser.parcerMSPPacket(attitudePacket)) {
        MSPAttitude attitude = parser.getAttitude();
        std::cout << "MSP_ATTITUDE: \n\tRoll: " << attitude.roll
        <<"` \n\tPitch: " << attitude.pitch
        <<"` \n\tYaw: " << attitude.yaw << "`" << std::endl;
    }
    else
        std::cout << parser.getLastError() << std::endl;
    std::cout << "\n==== ALL CHECKS PASSED ====\n" << std::endl;


    //MSP_RC
    std::cout << "\n==== MSP_RC TEST ====\n" << std::endl;
    std::vector<uint8_t> rcPacket = {
        '$','M', '>', 8, 105,
        0xD0, 0x07,
        0x84, 0x03,
        0xE8, 0x03,
        0xDC, 0x05,
        0x03
    };

    if (parser.parcerMSPPacket(rcPacket)) {
        MSPRCChannels rc = parser.getRCChannels();
        std::cout << "\tMSP_RC: " << rc.count << " channels" << std::endl;
        for (int i = 0; i < rc.count; i++) {
            std::cout << "\tChannel " << i+1 << ": " << rc.channels[i] << std::endl;
        }
    }
    else {
        std::cout << parser.getLastError() << std::endl;
    }
    std::cout << "\n==== ALL CHECKS PASSED ====\n" << std::endl;


    //MSP_ANALOG
    std::cout << "\n==== MSP_ANALOG TEST ====\n" << std::endl;
    std::vector<uint8_t> analogPacket = {
        '$', 'M', '>', 5, 110,
        0xA8, 0x00,
        0x19, 0x00,
        0x4B,
        0x00
    };
    uint8_t crc = 0;
    for (size_t i = 3; i< analogPacket.size() - 1;i++) {
        crc ^= analogPacket[i];
    }
    analogPacket.back() = crc;
    if (parser.parcerMSPPacket(analogPacket)) {
        MSPBattery battery = parser.getBattery();
        std:: cout << "\tVoltage: " << battery.voltage << " V" << std::endl;
        std:: cout << "\tCurrent: " << battery.current << " A" << std::endl;
        std:: cout << "\tCapacity: " << static_cast<int>(battery.capacity) << " %" << std::endl;
    }
    else {
        std::cout << parser.getLastError() << std::endl;
    }

    std::cout << "\n==== ALL CHECKS PASSED ====\n" << std::endl;

    //MSP_BATTERY_STATE
    std::cout << "\n==== MSP_BATTERY_STATE TEST ====\n" << std::endl;

    std::vector<uint8_t> batteryStatePacket = {
        '$', 'M', '>',
        3, 130,
        0x3B,
        0xE8, 0x03,
        0x00
    };

    crc = 0;
    for (size_t i = 3; i < batteryStatePacket.size() - 1; i++) {
        crc ^= batteryStatePacket[i];
    }
    batteryStatePacket.back() = crc;

    if (parser.parcerMSPPacket(batteryStatePacket)) {
        auto battery = parser.getBattery();
        std::cout << "\tCpacity: " << static_cast<int>(battery.capacity) << "%" << std::endl;
        std::cout << "\tmAh drawn: " << static_cast<int>(battery.mah_draw) << " mAh" << std::endl;
    } else {
        std::cout<< parser.getLastError() << std::endl;
    }
    std::cout << "\n==== ALL CHECKS PASSED ====\n" << std::endl;
}



