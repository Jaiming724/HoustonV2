#include "Dashboard.h"
#include "COBS.h"

enum {
    kAlertBufSize = 50,
    kAlertBufCOBSize = kAlertBufSize + (kAlertBufSize / 254) + 1,
};
static char alertBuffer[kAlertBufSize] = {0};
static char alertBufferCOBS[kAlertBufCOBSize] = {0};

Dashboard_Status_t Dashboard_Init(Dashboard_t *dashboard,
                                  fpSendData sendData,
                                  fpReadData readData,
                                  fpHasData hasData) {
    if (dashboard == NULL || sendData == NULL || readData == NULL || hasData == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    dashboard->sendData = sendData;
    dashboard->readData = readData;
    dashboard->hasData = hasData;
    return DASHBOARD_OK;
}

Dashboard_Status_t Dashboard_Alert(Dashboard_t *dashboard, const char *str) {
    if(str == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint32_t strLen = strlen(str);
    DashboardPacketHeader_t header;
    header.packetType = ID_Alert;
    header.magicNumber = DashboardMagicNumber;
    header.packetContentType = TYPE_STRING;
    header.payloadKeySize = 0;
    header.payloadValueSize = strLen;
    header.timestamp = 0; // Could add timestamping if needed
    header.checksum = crc32((char *) &header, sizeof(DashboardPacketHeader_t) - sizeof(uint32_t));
    uint32_t writeSize = sizeof(DashboardPacketHeader_t);
    memcpy(&alertBuffer[0], &header, sizeof(DashboardPacketHeader_t));
    memcpy(&alertBuffer[writeSize], str, strLen);
    writeSize += strLen;
    DashboardPacketTail_t tail;
    tail.payloadChecksum = crc32(str, strLen);
    memcpy(&alertBuffer[writeSize], &tail, sizeof(DashboardPacketTail_t));
    writeSize += sizeof(DashboardPacketTail_t);
    uint32_t encodeSize = kAlertBufCOBSize;
    if (cobs_encode((uint8_t *) alertBuffer, writeSize, (uint8_t *) alertBufferCOBS, &encodeSize) != COBS_ENCODING_OK) {
        return DASHBOARD_ERR_COBS_FAILED;
    }
    if (!dashboard->sendData(alertBufferCOBS, encodeSize)) {
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