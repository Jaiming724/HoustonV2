#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
enum{
    DashboardMagicNumber = 0xADEC
};
typedef enum {
    DASHBOARD_OK = 0,
    DASHBOARD_ERR_INVALID_ARG,
    DASHBOARD_ERR_SEND_FAIL,
    DASHBOARD_ERR_NO_SPACE,
    DASHBOARD_ERR_COBS_FAILED,
} Dashboard_Status_t;

typedef bool (*fpSendData)(const char *, uint32_t);

typedef uint32_t (*fpReadData)(char *buf, uint32_t size);

typedef bool (*fpHasData)(uint32_t *size);

typedef struct Dashboard {
    fpSendData sendData;
    fpReadData readData;
    fpHasData hasData;
} Dashboard_t;
typedef enum {
    ID_Telemetry = 0x30,
    ID_Alert = 0x31,
    ID_Modify = 0x32,
} PacketID_t;

typedef enum {
    TYPE_INT32 = 0x01,
    TYPE_FLOAT = 0x02,
    TYPE_BOOL = 0x03,
    TYPE_STRING = 0x04
} ValueType_t;

#ifdef _MSC_VER
#pragma pack(push, 1)
typedef struct {
    uint16_t magicNumber;
    uint8_t packetType;
    uint8_t packetContentType;
    uint16_t payloadKeySize;
    uint16_t payloadValueSize;
    uint32_t timestamp;
    uint32_t checksum;
} DashboardPacketHeader_t;

typedef struct {
    uint32_t payloadChecksum;
} DashboardPacketTail_t;
#pragma pack(pop)

// Otherwise, assume GCC/Clang
#else
typedef struct __attribute__((packed)) {
        uint16_t magicNumber;
        uint8_t packetType;
        uint8_t packetContentType;
        uint16_t payloadKeySize;
        uint16_t payloadValueSize;
        uint32_t timestamp;
        uint32_t checksum;
    } DashboardPacketHeader_t;

    typedef struct __attribute__((packed)) {
        uint32_t payloadChecksum;
    } DashboardPacketTail_t;
#endif

uint32_t crc32(const char *s, uint32_t n);

Dashboard_Status_t Dashboard_Init(Dashboard_t *dashboard, fpSendData sendData,
                                  fpReadData readData, fpHasData hasData);

Dashboard_Status_t Dashboard_Alert(Dashboard_t *dashboard, const char *str);

#ifdef __cplusplus
}
#endif
