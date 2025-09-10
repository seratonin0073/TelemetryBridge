#ifndef TELEMETRYBRIDGE_MAVLINK_CONVERTER_H
#define TELEMETRYBRIDGE_MAVLINK_CONVERTER_H


#ifndef MAVLINK_CONVERTER_H
#define MAVLINK_CONVERTER_H

#include <vector>
#include <cstdint>
#include "msp_parser.h"

class MavlinkConverter {
public:
    MavlinkConverter();

    std::vector<uint8_t> convertAttitude(const MSPAttitude& attitude);
    std::vector<uint8_t> convertRCChannels(const MSPRCChannels& channels);
    std::vector<uint8_t> convertBattery(const MSPBattery& battery);
    std::vector<uint8_t> createHeartbeat();
    std::vector<uint8_t> createSystemStatus();

    void setSystemId(uint8_t systemId) { this->systemId = systemId; }
    void setComponentId(uint8_t componentId) { this->componentId = componentId; }
    uint8_t getSystemId() const { return systemId; }
    uint8_t getComponentId() const { return componentId; }

    struct Statistics {
        uint32_t attitudeMessages;
        uint32_t rcMessages;
        uint32_t batteryMessages;
        uint32_t totalBytes;
    };

    Statistics getStatistics() const;

private:
    uint8_t systemId;
    uint8_t componentId;
    uint8_t sequenceNumber;
    Statistics stats;

    uint8_t getNextSequence();
    void updateStatistics(size_t bytesSent);
};

#endif // MAVLINK_CONVERTER_H
#endif //TELEMETRYBRIDGE_MAVLINK_CONVERTER_H