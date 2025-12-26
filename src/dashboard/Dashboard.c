#include "Dashboard.h"
#include "COBS.h"

enum {
    kAlertBufSize = 50,
    kAlertBufCOBSize = kAlertBufSize + (kAlertBufSize / 254) + 1,
};
static char alertBuffer[kAlertBufSize] = {0};
static char alertBufferCOBS[kAlertBufCOBSize] = {0};

static void
craftTelemetryPacket(DashboardPacketHeader_t *packetHeader, uint8_t contentType, uint16_t keyLen, uint16_t valueSize) {
    packetHeader->magicNumber = DashboardMagicNumber;
    packetHeader->packetType = ID_Telemetry;
    packetHeader->packetContentType = contentType;
    packetHeader->payloadKeySize = keyLen;
    packetHeader->payloadValueSize = valueSize;
    packetHeader->timestamp = 0; // Could add timestamping if needed
    packetHeader->checksum = crc32((char *) packetHeader, sizeof(DashboardPacketHeader_t) - sizeof(uint32_t), 0);
}

static Dashboard_Status_t
copyData(Dashboard_t *dashboard, DashboardPacketHeader_t *packetHeader, uint8_t *key, uint8_t *value, uint16_t keySize,
         uint16_t valueSize) {
    uint32_t writeSize = sizeof(DashboardPacketHeader_t);
    memcpy(&alertBuffer[0], packetHeader, sizeof(DashboardPacketHeader_t));
    memcpy(&alertBuffer[writeSize], key, keySize);
    writeSize += keySize;
    memcpy(&alertBuffer[writeSize], value, valueSize);
    writeSize += valueSize;
    DashboardPacketTail_t tail;

    tail.payloadChecksum = crc32((char *) key, keySize, 0);
    tail.payloadChecksum = crc32((char *) value, valueSize, tail.payloadChecksum);

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

Dashboard_Status_t Dashboard_Telemetry_Str(Dashboard_t *dashboard, const char *key, const char *value) {
    if (key == NULL || value == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = strlen(value);
    DashboardPacketHeader_t header;
    craftTelemetryPacket(&header, TYPE_STRING, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) value, keyLen, valueLen);

}

Dashboard_Status_t Dashboard_Telemetry_Int32(Dashboard_t *dashboard, const char *key, int32_t value) {
    if (key == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = sizeof(int32_t);
    DashboardPacketHeader_t header;
    craftTelemetryPacket(&header, TYPE_INT32, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) &value, keyLen, valueLen);
}

Dashboard_Status_t Dashboard_Telemetry_Uint32(Dashboard_t *dashboard, const char *key, uint32_t value) {
    if (key == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = sizeof(uint32_t);
    DashboardPacketHeader_t header;
    craftTelemetryPacket(&header, TYPE_UINT32, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) &value, keyLen, valueLen);
}

Dashboard_Status_t Dashboard_Telemetry_Float(Dashboard_t *dashboard, const char *key, float value) {
    if (key == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = sizeof(float);
    DashboardPacketHeader_t header;
    craftTelemetryPacket(&header, TYPE_UINT32, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) &value, keyLen, valueLen);
}

Dashboard_Status_t Dashboard_Alert(Dashboard_t *dashboard, const char *str) {
    if (str == NULL || dashboard == NULL) {
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
    header.checksum = crc32((char *) &header, sizeof(DashboardPacketHeader_t) - sizeof(uint32_t), 0);
    uint32_t writeSize = sizeof(DashboardPacketHeader_t);
    memcpy(&alertBuffer[0], &header, sizeof(DashboardPacketHeader_t));
    memcpy(&alertBuffer[writeSize], str, strLen);
    writeSize += strLen;
    DashboardPacketTail_t tail;
    tail.payloadChecksum = crc32(str, strLen, 0);
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


uint32_t crc32(const char *s, uint32_t n, uint32_t crc) {

    for (uint32_t i = 0; i < n; i++) {
        char ch = s[i];
        for (uint32_t j = 0; j < 8; j++) {
            uint32_t b = (ch ^ crc) & 1;
            crc >>= 1;
            if (b) crc = crc ^ 0xEDB88320;
            ch >>= 1;
        }
    }

    return crc;
}