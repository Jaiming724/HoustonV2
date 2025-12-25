#include "Dashboard.h"
#include "COBS.h"

static char alertBuffer[kAlertBufSize] = {0};

Dashboard_Status_t Dashboard_Alert(Dashboard_t *dashboard, const char *str) {
    uint32_t s = kAlertBufSize - sizeof(DashboardPacketHeader_t) - sizeof(DashboardPacketTail_t);
    if (cobs_encode((uint8_t *) str, strlen(str), (uint8_t *) &alertBuffer[sizeof(DashboardPacketHeader_t)], &s) !=
        COBS_ENCODING_OK) {
        return DASHBOARD_ERR_COBS_FAILED;
    }
    if (s + sizeof(DashboardPacketHeader_t) + sizeof(DashboardPacketTail_t) > kAlertBufSize) {
        return DASHBOARD_ERR_NO_SPACE;
    }
    DashboardPacketHeader_t header;
    header.packetType = ID_Alert;
    header.packetContentType = TYPE_STRING;
    header.payloadKeySize = 0;
    header.payloadValueSize = s;
    header.timestamp = 0; // Could add timestamping if needed
    header.checksum = crc32((char *) &header, sizeof(DashboardPacketHeader_t) - sizeof(uint32_t));
    uint32_t writeSize = sizeof(DashboardPacketHeader_t);
    memcpy(&alertBuffer[0], &header, sizeof(DashboardPacketHeader_t));
    writeSize += s;
    DashboardPacketTail_t tail;
    tail.payloadChecksum = crc32(str, strlen(str));
    memcpy(&alertBuffer[writeSize], &tail, sizeof(DashboardPacketTail_t));
    writeSize += sizeof(DashboardPacketTail_t);
    if (!dashboard->sendData(alertBuffer, writeSize)) {
        return DASHBOARD_ERR_SEND_FAIL;
    }
    return DASHBOARD_OK;
}


uint32_t crc32(const char *s, uint32_t n) {
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < n; i++) {
        char ch = s[i];
        for (uint32_t j = 0; j < 8; j++) {
            uint32_t b = (ch ^ crc) & 1;
            crc >>= 1;
            if (b) crc = crc ^ 0xEDB88320;
            ch >>= 1;
        }
    }

    return ~crc;
}