#include "mavlink_converter.h"
#include "common/mavlink.h"
#include <cstring>
#include <cmath>

MavlinkConverter::MavlinkConverter()
    : systemId(1), componentId(1), sequenceNumber(0), stats{0} {}

uint8_t MavlinkConverter::getNextSequence() {
    return sequenceNumber++ % 256;
}

void MavlinkConverter::updateStatistics(size_t bytesSent) {
    stats.totalBytes += bytesSent;
}

std::vector<uint8_t> MavlinkConverter::createHeartbeat() {
    mavlink_heartbeat_t heartbeat;
    heartbeat.type = MAV_TYPE_QUADROTOR;
    heartbeat.autopilot = MAV_AUTOPILOT_INVALID;
    heartbeat.base_mode = 0;
    heartbeat.custom_mode = 0;
    heartbeat.system_status = MAV_STATE_ACTIVE;

    mavlink_message_t msg;
    mavlink_msg_heartbeat_encode(systemId, componentId, &msg, &heartbeat);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
    buffer.resize(len);

    updateStatistics(len);
    return buffer;
}

std::vector<uint8_t> MavlinkConverter::createSystemStatus() {
    mavlink_sys_status_t sys_status;
    sys_status.onboard_control_sensors_present = 0;
    sys_status.onboard_control_sensors_enabled = 0;
    sys_status.onboard_control_sensors_health = 0;
    sys_status.load = 0;
    sys_status.voltage_battery = 0;
    sys_status.current_battery = 0;
    sys_status.battery_remaining = -1;
    sys_status.drop_rate_comm = 0;
    sys_status.errors_comm = 0;
    sys_status.errors_count1 = 0;
    sys_status.errors_count2 = 0;
    sys_status.errors_count3 = 0;
    sys_status.errors_count4 = 0;

    mavlink_message_t msg;
    mavlink_msg_sys_status_encode(systemId, componentId, &msg, &sys_status);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
    buffer.resize(len);

    updateStatistics(len);
    return buffer;
}

std::vector<uint8_t> MavlinkConverter::convertAttitude(const MSPAttitude& attitude) {
    mavlink_attitude_t mav_attitude;

    mav_attitude.roll = attitude.roll * (M_PI / 1800.0f);
    mav_attitude.pitch = attitude.pitch * (M_PI / 1800.0f);
    mav_attitude.yaw = attitude.yaw * (M_PI / 1800.0f);

    mav_attitude.rollspeed = 0;
    mav_attitude.pitchspeed = 0;
    mav_attitude.yawspeed = 0;
    mav_attitude.time_boot_ms = 0;

    mavlink_message_t msg;
    mavlink_msg_attitude_encode(systemId, componentId, &msg, &mav_attitude);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
    buffer.resize(len);

    stats.attitudeMessages++;
    updateStatistics(len);
    return buffer;
}

std::vector<uint8_t> MavlinkConverter::convertRCChannels(const MSPRCChannels& channels) {
    mavlink_rc_channels_t mav_rc;

    mav_rc.chancount = channels.count;
    mav_rc.time_boot_ms = 0;
    mav_rc.rssi = 255;

    mav_rc.chan1_raw = (channels.count > 0) ? channels.channels[0] : UINT16_MAX;
    mav_rc.chan2_raw = (channels.count > 1) ? channels.channels[1] : UINT16_MAX;
    mav_rc.chan3_raw = (channels.count > 2) ? channels.channels[2] : UINT16_MAX;
    mav_rc.chan4_raw = (channels.count > 3) ? channels.channels[3] : UINT16_MAX;
    mav_rc.chan5_raw = (channels.count > 4) ? channels.channels[4] : UINT16_MAX;
    mav_rc.chan6_raw = (channels.count > 5) ? channels.channels[5] : UINT16_MAX;
    mav_rc.chan7_raw = (channels.count > 6) ? channels.channels[6] : UINT16_MAX;
    mav_rc.chan8_raw = (channels.count > 7) ? channels.channels[7] : UINT16_MAX;

    mav_rc.chan9_raw = UINT16_MAX;
    mav_rc.chan10_raw = UINT16_MAX;
    mav_rc.chan11_raw = UINT16_MAX;
    mav_rc.chan12_raw = UINT16_MAX;
    mav_rc.chan13_raw = UINT16_MAX;
    mav_rc.chan14_raw = UINT16_MAX;
    mav_rc.chan15_raw = UINT16_MAX;
    mav_rc.chan16_raw = UINT16_MAX;
    mav_rc.chan17_raw = UINT16_MAX;
    mav_rc.chan18_raw = UINT16_MAX;

    mavlink_message_t msg;
    mavlink_msg_rc_channels_encode(systemId, componentId, &msg, &mav_rc);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
    buffer.resize(len);

    stats.rcMessages++;
    updateStatistics(len);
    return buffer;
}

std::vector<uint8_t> MavlinkConverter::convertBattery(const MSPBattery& battery) {
    // SYS_STATUS
    mavlink_sys_status_t sys_status;
    sys_status.voltage_battery = battery.voltage * 1000;  // V → mV
    sys_status.current_battery = battery.current * 100;   // A → 10mA
    sys_status.battery_remaining = battery.capacity;

    // BATTERY_STATUS
    mavlink_battery_status_t bat_status;
    bat_status.id = 0;
    bat_status.battery_function = MAV_BATTERY_FUNCTION_ALL;
    bat_status.type = MAV_BATTERY_TYPE_LIPO;
    bat_status.temperature = INT16_MAX;
    bat_status.voltages[0] = battery.voltage * 1000;  // mV
    for (int i = 1; i < 10; i++) bat_status.voltages[i] = UINT16_MAX;
    bat_status.current_battery = battery.current * 100; // 10mA
    bat_status.current_consumed = -1;
    bat_status.energy_consumed = -1;
    bat_status.battery_remaining = battery.capacity;
    bat_status.time_remaining = 0;
    bat_status.charge_state = MAV_BATTERY_CHARGE_STATE_UNDEFINED;

    std::vector<uint8_t> result;

    mavlink_message_t msg;

    // SYS_STATUS
    mavlink_msg_sys_status_encode(systemId, componentId, &msg, &sys_status);
    std::vector<uint8_t> buffer1(MAVLINK_MAX_PACKET_LEN);
    uint16_t len1 = mavlink_msg_to_send_buffer(buffer1.data(), &msg);
    buffer1.resize(len1);

    // BATTERY_STATUS
    mavlink_msg_battery_status_encode(systemId, componentId, &msg, &bat_status);
    std::vector<uint8_t> buffer2(MAVLINK_MAX_PACKET_LEN);
    uint16_t len2 = mavlink_msg_to_send_buffer(buffer2.data(), &msg);
    buffer2.resize(len2);

    result.insert(result.end(), buffer1.begin(), buffer1.end());
    result.insert(result.end(), buffer2.begin(), buffer2.end());

    stats.batteryMessages++;
    updateStatistics(len1 + len2);
    return result;
}

MavlinkConverter::Statistics MavlinkConverter::getStatistics() const {
    return stats;
}