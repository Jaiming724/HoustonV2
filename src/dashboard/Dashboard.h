#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DASHBOARD_OK = 0,
    DASHBOARD_ERR_INVALID_ARG,
    DASHBOARD_ERR_SEND_FAIL,
    DASHBOARD_ERR_NO_SPACE,
    DASHBOARD_ERR_COBS_FAILED,
} Dashboard_Status_t;
typedef enum {
    Dashboard_Packet_WAITING = 0,
    Dashboard_Packet_HEADER_RECEIVED,
    Dashboard_Packet_RECEIVING_PAYLOAD,
    Dashboard_Packet_TAIL_RECEIVED,
} Dashboard_Packet_Status_t;

typedef bool (*fpSendData)(const char *, uint32_t);

typedef uint32_t (*fpReadData)(char *buf, uint32_t size);

typedef bool (*fpHasData)(uint32_t *size);


typedef enum {
    ID_Telemetry = 0x30,
    ID_Alert = 0x31,
    ID_Modify = 0x32,
    ID_Request_LiveData = 0x33,
    ID_Response_LiveData = 0x34,
} PacketID_t;

typedef enum {
    TYPE_UINT32 = 0x00,
    TYPE_INT32 = 0x01,
    TYPE_FLOAT = 0x02,
    TYPE_BOOL = 0x03,
    TYPE_STRING = 0x04
} ValueType_t;

#ifdef _MSC_VER
#pragma pack(push, 1)
typedef struct {
    uint8_t packetType;
    uint8_t packetContentType;
    uint16_t payloadKeySize;
    uint16_t payloadValueSize;
    uint32_t timestamp;
    uint16_t checksum;
} DashboardPacketHeader_t;
typedef struct {
    uint16_t packetID;
    uint16_t valueType;
    union {
        uint32_t uint32Value;
        int32_t int32Value;
        float floatValue;
        bool boolValue;
    };
} LiveDataPacket_t;
typedef struct {
    uint32_t payloadChecksum;
} DashboardPacketTail_t;
#pragma pack(pop)

// Otherwise, assume GCC/Clang
#else
typedef struct __attribute__((packed)) {
    uint8_t packetType;
    uint8_t packetContentType;
    uint16_t payloadKeySize;
    uint16_t payloadValueSize;
    uint32_t timestamp;
    uint16_t checksum;
} DashboardPacketHeader_t;
typedef struct __attribute__((packed)) {
    uint16_t packetID;
    uint16_t valueType;
    union {
        uint32_t uint32Value;
        int32_t int32Value;
        float floatValue;
        bool boolValue;
    };
} LiveDataPacket_t;
typedef struct __attribute__((packed)) {
    uint32_t payloadChecksum;
} DashboardPacketTail_t;
#endif

typedef struct Dashboard {
    uint16_t liveDataCount;
    fpSendData sendData;
    fpReadData readData;
    fpHasData hasData;
    Dashboard_Packet_Status_t packetStatus;
    DashboardPacketHeader_t currentPacketHeader;

} Dashboard_t;

uint32_t crc32(const char *s, uint32_t n, uint32_t crc);
uint16_t crc16(const char *s, uint32_t n, uint16_t crc);

void
craftDashboardHeaderPacket(DashboardPacketHeader_t *packetHeader, uint8_t packetType, uint8_t contentType, uint16_t keyLen,
                           uint16_t valueSize);

Dashboard_Status_t Dashboard_Init(Dashboard_t *dashboard, fpSendData sendData,
                                  fpReadData readData, fpHasData hasData);

Dashboard_Status_t Dashboard_Alert(Dashboard_t *dashboard, const char *str);

Dashboard_Status_t Dashboard_Telemetry_Float(Dashboard_t *dashboard, const char *key, float value);

Dashboard_Status_t Dashboard_Telemetry_Int32(Dashboard_t *dashboard, const char *key, int32_t value);

Dashboard_Status_t Dashboard_Telemetry_Uint32(Dashboard_t *dashboard, const char *key, uint32_t value);

Dashboard_Status_t Dashboard_Telemetry_Str(Dashboard_t *dashboard, const char *key, const char *value);

Dashboard_Status_t Dashboard_Register_LiveData(Dashboard_t *dashboard, uint16_t key, void *data, ValueType_t type);

Dashboard_Status_t Dashboard_Update(Dashboard_t *dashboard);

#ifdef __cplusplus
}
#endif
