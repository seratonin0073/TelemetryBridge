//
// Created by danylo on 9/3/25.
//

#ifndef TELEMETRYBRIDGE_MSP_PARSER_H
#define TELEMETRYBRIDGE_MSP_PARSER_H

#include <vector>
#include <cstdint>
#include <string>

struct MSPAttitude {
    float roll;
    float pitch;
    float yaw;
};

struct MSPRCChannels {
    uint16_t channels[18];
    uint8_t count;
};

struct MSPBattery {
    float voltage;
    float current;
    uint8_t capacity;
    uint16_t mah_draw;
};

enum MSPComand : uint8_t {
    MSP_ATTITUDE = 108,
    MSP_RC = 105,
    MSP_ANALOG = 110,
    MSP_BATTERY_STATE = 130,
    MSP_RAW_GPS = 106,
    MSP_MOTOR = 104
};

class MSPParser {
public:
    MSPParser();
    bool parcerMSPPacket(const std::vector<uint8_t> packet);
    void reset();

    MSPAttitude getAttitude() const;
    MSPRCChannels getRCChannels() const;
    MSPBattery getBattery() const;

    struct Statistic {
        uint32_t totalPackets;
        uint32_t validPackets;
        uint32_t crcErrors;
        uint32_t attitudePackets;
        uint32_t rcPackets;
        uint32_t batteryPackets;
    };

    Statistic getStatistic() const;
    std::string getLastError() const;
private:
    MSPAttitude currentAttitude;
    MSPRCChannels currentRCChannels;
    MSPBattery currentBattery;

    Statistic stats;
    std::string lastError;

    bool validatePacket(const std::vector<uint8_t>& packet) const;
    uint8_t calculateChecksum(const std::vector<uint8_t>& packet) const;

    bool parseMSP_ATTITUDE(const std::vector<uint8_t>& data);
    bool parseMSP_RC(const std::vector<uint8_t>& data);
    bool parseMSP_ANALOG(const std::vector<uint8_t>& data);
    bool parseMSP_BATTERY_STATE(const std::vector<uint8_t>& data);

};


#endif //TELEMETRYBRIDGE_MSP_PARSER_H