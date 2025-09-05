//
// Created by danylo on 9/3/25.
//

#include "msp_parser.h"
#include <iostream>
#include <cstring>


MSPParser::MSPParser() {
    reset();
    stats = {0};
}

void MSPParser::reset() {
    currentAttitude = {0};
    currentRCChannels = {0};
    currentBattery = {0};
    lastError.clear();

}

bool MSPParser::parcerMSPPacket(const std::vector<uint8_t> packet) {
    stats.totalPackets++;

    //min size 6
    if (packet.size() < 6) {
        lastError = "Packet too small: " + std::to_string(packet.size()) + " bytes";
        return false;
    }
    if (packet[0] != '$' || packet[1] != 'M' || packet[2] != '>') {
        lastError = "Invalid MSP header";
        return false;
    }

    if (!validatePacket(packet)) {
        stats.crcErrors++;
        lastError = "CRC mismatch";
        return false;
    }

    uint8_t dataLenght = packet[3];
    uint8_t command = packet[4];
    if (packet.size() != 6 + dataLenght) {
        lastError = "Data lenght mismatch";
        std::cout << lastError << std::endl;
        return false;
    }

    std::vector<uint8_t> data(packet.begin() + 5, packet.end() - 1);

    bool parseResult = false;
    switch (command) {
        case MSP_ATTITUDE: {
            parseResult = parseMSP_ATTITUDE(data);
            if (parseResult) stats.attitudePackets++;
            break;
        }
        case MSP_RC:
            parseResult = parseMSP_RC(data);
            if (parseResult) stats.rcPackets++;
            break;
        case MSP_ANALOG:
            parseResult = parseMSP_ANALOG(data);
            if (parseResult) stats.batteryPackets++;
            break;
        case MSP_BATTERY_STATE:
            parseResult = parseMSP_BATTERY_STATE(data);
            if (parseResult) stats.batteryPackets++;
            break;
        default:
            lastError = "Unknown MSP command: " + std::to_string(command);
            std::cout << lastError << std::endl;
            return false;
    }

    return parseResult;
}

bool MSPParser::validatePacket(const std::vector<uint8_t>& packet) const {
    if (packet.size() < 6) return false;

    uint8_t calculatedCrc = 0;

    for (size_t i = 3; i < packet.size() - 1; i++) {
        calculatedCrc ^= packet[i];
    }
    return calculatedCrc == packet.back();
}

uint8_t MSPParser::calculateChecksum(const std::vector<uint8_t>& packet) const {
    uint8_t crc = 0;
    for (size_t i = 3; i < packet.size();i++) {
        crc ^= packet[i];
    }
    return crc;
}

bool MSPParser::parseMSP_ATTITUDE(const std::vector<uint8_t>& data) {
    if (data.size() < 6) {
        lastError = "MSP_ATTITUDE: insufficient data";
        std::cout << lastError << std::endl;
        return false;
    }

    int16_t roll = (data[1] << 8) | data[0];
    int16_t pitch = (data[3] << 8) | data[2];
    int16_t yaw = (data[5] << 8) | data[4];

    currentAttitude.roll = roll/10.0f;
    currentAttitude.pitch = pitch/10.0f;
    currentAttitude.yaw = yaw/10.0f;
    return true;
}

bool MSPParser::parseMSP_RC(const std::vector<uint8_t>& data) {
    size_t channelCount = data.size()/2;
    if (channelCount > 18 || data.size() % 2 != 0) {
        lastError = "MSP_RC: invalid data size";
        std::cout << lastError << std::endl;
        return false;
    }

    for (size_t i = 0; i < channelCount; i++) {
        currentRCChannels.channels[i] = (data[i*2+1] << 8 | data[i*2]);
    }

    currentRCChannels.count = channelCount;
    return true;
}

bool MSPParser::parseMSP_ANALOG(const std::vector<uint8_t>& data) {
    if (data.size() < 5) {
        lastError = "MSP_ANALOG: insufficient data";
        return false;
    }

    uint16_t voltage = (data[1] << 8) | data[0];
    uint16_t current_val = (data[3] << 8) | data[2];
    uint16_t capacity = data[4];

    currentBattery.voltage = voltage/10.0f;
    currentBattery.current = current_val/10.0f;
    currentBattery.capacity = capacity;

    return true;
}

bool MSPParser::parseMSP_BATTERY_STATE(const std::vector<uint8_t>& data) {
    if (data.size() < 3) {
        lastError = "MSP_BATTARY_STATE: insufficient data";
        return false;
    }

    currentBattery.capacity = data[0];
    currentBattery.mah_draw = (data[2] << 8) | data[1];

    return true;
}

MSPAttitude MSPParser::getAttitude() const {return currentAttitude;}
MSPRCChannels MSPParser::getRCChannels() const {return currentRCChannels;}
MSPBattery MSPParser::getBattery() const {return currentBattery;}

MSPParser::Statistic MSPParser::getStatistic() const {return stats;}
std::string MSPParser::getLastError() const {return lastError;}

