#include "Dashboard.h"
#include "COBS.h"

enum {
    kAlertBufSize = 100,
    kAlertBufCOBSize = kAlertBufSize + (kAlertBufSize / 254) + 1,
    kLiveDataBufSize = 5,
};


static char alertBuffer[kAlertBufSize] = {0};
static char alertBufferCOBS[kAlertBufCOBSize] = {0};
static LiveDataPacket_t liveDataBuffer[kLiveDataBufSize] = {0};
static char rxBuffer[kAlertBufCOBSize] = {0};
static void *liveDataPtr[kLiveDataBufSize] = {0};

void
craftDashboardHeaderPacket(DashboardPacketHeader_t *packetHeader, uint8_t packetType, uint8_t contentType,
                           uint16_t keyLen,
                           uint16_t valueSize) {
    packetHeader->packetType = packetType;
    packetHeader->packetContentType = contentType;
    packetHeader->payloadKeySize = keyLen;
    packetHeader->payloadValueSize = valueSize;
    packetHeader->timestamp = 0; // Could add timestamping if needed
    packetHeader->checksum = crc16((char *) packetHeader, sizeof(DashboardPacketHeader_t) - sizeof(packetHeader->checksum), 0);
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

static Dashboard_Status_t processPacket(Dashboard_t *dashboard) {
    DashboardPacketHeader_t *header = (DashboardPacketHeader_t *) rxBuffer;
    if (header->packetType == ID_Request_LiveData) {
        Dashboard_Alert(dashboard, "Live Data Requested");
        DashboardPacketHeader_t responseHeader;
        craftDashboardHeaderPacket(&responseHeader, ID_Response_LiveData, TYPE_UINT32, 0,
                                   dashboard->liveDataCount * sizeof(LiveDataPacket_t));
        uint32_t writeSize = sizeof(DashboardPacketHeader_t);
        memcpy(&alertBuffer[0], &responseHeader, sizeof(DashboardPacketHeader_t));
        for (uint32_t i = 0; i < dashboard->liveDataCount; i++) {
            if (liveDataBuffer[i].valueType == TYPE_FLOAT) {
                liveDataBuffer[i].floatValue = *((float *) liveDataPtr[i]);
            } else if (liveDataBuffer[i].valueType == TYPE_BOOL) {
                liveDataBuffer[i].boolValue = *((bool *) liveDataPtr[i]);
            } else if (liveDataBuffer[i].valueType == TYPE_INT32) {
                liveDataBuffer[i].int32Value = *((int32_t *) liveDataPtr[i]);
            } else if (liveDataBuffer[i].valueType == TYPE_UINT32) {
                liveDataBuffer[i].uint32Value = *((uint32_t *) liveDataPtr[i]);
            }
            memcpy(&alertBuffer[writeSize], (char *) &liveDataBuffer[i], sizeof(LiveDataPacket_t));
            writeSize += sizeof(LiveDataPacket_t);
        }
        DashboardPacketTail_t tail;
        tail.payloadChecksum = crc32((char *) alertBuffer + sizeof(DashboardPacketHeader_t),
                                     dashboard->liveDataCount * sizeof(LiveDataPacket_t), 0);
        memcpy(&alertBuffer[writeSize], &tail, sizeof(DashboardPacketTail_t));
        writeSize += sizeof(DashboardPacketTail_t);
        uint32_t encodeSize = kAlertBufCOBSize;
        if (cobs_encode((uint8_t *) alertBuffer, writeSize, (uint8_t *) alertBufferCOBS, &encodeSize) !=
            COBS_ENCODING_OK) {
            return DASHBOARD_ERR_COBS_FAILED;
        }
        if (!dashboard->sendData(alertBufferCOBS, encodeSize)) {
            return DASHBOARD_ERR_SEND_FAIL;
        }
    } else if (header->packetType == ID_Modify) {
        Dashboard_Alert(dashboard, "Live Data Modify Received");
        LiveDataPacket_t *packet = (LiveDataPacket_t *) (rxBuffer + sizeof(DashboardPacketHeader_t));

        for (uint32_t i = 0; i < dashboard->liveDataCount; i++) {
            if (liveDataBuffer[i].packetID == packet->packetID) {
                if (liveDataBuffer[i].valueType != packet->valueType) {
                    Dashboard_Alert(dashboard, "Live Data Type Mismatch");
                    return DASHBOARD_OK;
                }
                if (packet->valueType == TYPE_FLOAT) {
                    *((float *) liveDataPtr[i]) = packet->floatValue;
                } else if (packet->valueType == TYPE_BOOL) {
                    *((bool *) liveDataPtr[i]) = packet->boolValue;
                } else if (packet->valueType == TYPE_INT32) {
                    *((int32_t *) liveDataPtr[i]) = packet->int32Value;
                } else if (packet->valueType == TYPE_UINT32) {
                    *((uint32_t *) liveDataPtr[i]) = packet->uint32Value;
                }
            }
        }
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
    dashboard->liveDataCount = 0;
    dashboard->packetStatus = Dashboard_Packet_WAITING;
    return DASHBOARD_OK;
}

Dashboard_Status_t
Dashboard_Register_LiveData(Dashboard_t *dashboard, uint16_t key, void *data, ValueType_t type) {
    if (dashboard == NULL || data == NULL || dashboard->liveDataCount >= kLiveDataBufSize) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    liveDataBuffer[dashboard->liveDataCount].packetID = key;
    liveDataBuffer[dashboard->liveDataCount].valueType = type;
    liveDataPtr[dashboard->liveDataCount] = data;

    dashboard->liveDataCount++;

    return DASHBOARD_OK;
}

Dashboard_Status_t Dashboard_Telemetry_Str(Dashboard_t *dashboard, const char *key, const char *value) {
    if (key == NULL || value == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = strlen(value);
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Telemetry, TYPE_STRING, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) value, keyLen, valueLen);

}

Dashboard_Status_t Dashboard_Telemetry_Int32(Dashboard_t *dashboard, const char *key, int32_t value) {
    if (key == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = sizeof(int32_t);
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Telemetry, TYPE_INT32, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) &value, keyLen, valueLen);
}

Dashboard_Status_t Dashboard_Telemetry_Uint32(Dashboard_t *dashboard, const char *key, uint32_t value) {
    if (key == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = sizeof(uint32_t);
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Telemetry, TYPE_UINT32, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) &value, keyLen, valueLen);
}

Dashboard_Status_t Dashboard_Telemetry_Float(Dashboard_t *dashboard, const char *key, float value) {
    if (key == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint16_t keyLen = strlen(key);
    uint16_t valueLen = sizeof(float);
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Telemetry, TYPE_FLOAT, keyLen, valueLen);
    return copyData(dashboard, &header, (uint8_t *) key, (uint8_t *) &value, keyLen, valueLen);
}

Dashboard_Status_t Dashboard_Alert(Dashboard_t *dashboard, const char *str) {
    if (str == NULL || dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint32_t strLen = strlen(str);
    DashboardPacketHeader_t header;
    craftDashboardHeaderPacket(&header, ID_Alert, TYPE_STRING, 0, strLen);
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

Dashboard_Status_t Dashboard_Update(Dashboard_t *dashboard) {
    if (dashboard == NULL) {
        return DASHBOARD_ERR_INVALID_ARG;
    }
    uint32_t count;
    bool status = dashboard->hasData(&count);
    DashboardPacketHeader_t *header = (DashboardPacketHeader_t *) rxBuffer;
    if (dashboard->packetStatus == Dashboard_Packet_WAITING && status && count > sizeof(DashboardPacketHeader_t)) {
        dashboard->readData(rxBuffer, sizeof(DashboardPacketHeader_t));
        if (crc16((char *) header, sizeof(DashboardPacketHeader_t) - sizeof(header->checksum), 0) == header->checksum) {
            uint32_t totalPacketSize = sizeof(DashboardPacketHeader_t) +
                                       header->payloadKeySize + header->payloadValueSize +
                                       sizeof(DashboardPacketTail_t);
            if (totalPacketSize < kAlertBufCOBSize) {
                Dashboard_Alert(dashboard, "Received packet header");

                dashboard->packetStatus = Dashboard_Packet_HEADER_RECEIVED;
            } else {
                Dashboard_Alert(dashboard, "Received packet too large");
            }

        }
    }
    if (dashboard->packetStatus == Dashboard_Packet_HEADER_RECEIVED) {
        status = dashboard->hasData(&count);
        uint32_t expectedSize = header->payloadKeySize + header->payloadValueSize + sizeof(DashboardPacketTail_t);
        if (status && count >= expectedSize) {
            dashboard->readData(&rxBuffer[sizeof(DashboardPacketHeader_t)], expectedSize);
            uint32_t expectedPayloadSize = header->payloadKeySize + header->payloadValueSize;
            DashboardPacketTail_t *tail = (DashboardPacketTail_t *) (rxBuffer + sizeof(DashboardPacketHeader_t) +
                                                                     expectedPayloadSize);
            uint32_t payloadChecksum = crc32(rxBuffer + sizeof(DashboardPacketHeader_t), header->payloadKeySize, 0);
            payloadChecksum = crc32(rxBuffer + sizeof(DashboardPacketHeader_t) + header->payloadKeySize,
                                    header->payloadValueSize, payloadChecksum);

            dashboard->packetStatus = Dashboard_Packet_WAITING;
            Dashboard_Telemetry_Uint32(dashboard, "CalculuatedCRC", payloadChecksum);
            Dashboard_Telemetry_Uint32(dashboard, "ReceivedCRC", tail->payloadChecksum);
            if (payloadChecksum == tail->payloadChecksum) {
                Dashboard_Alert(dashboard, "Processing packet");
                return processPacket(dashboard);
            } else {
                Dashboard_Alert(dashboard, "Payload checksum mismatch");
            }

        }
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

uint16_t crc16(const char *s, uint32_t n, uint16_t crc) {
    const uint8_t *p = (const uint8_t *)s;

    for (uint32_t i = 0; i < n; i++) {
        crc ^= p[i];
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}