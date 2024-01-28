#pragma once

#include <Arduino.h>

#define buffer_size 400
#define mapSize 5
#define mapBufferSize 100
#define alertBufferSize 100
#define numBufferSize 12
#define floatBufferSize 20
struct Packet {
    uint8_t packet_id;
    uint16_t packet_length;
    char data_buf[16];
    union {
        float float_data;
        //Questionable, technically the server can send a 4 byte integer
        int int_data;
    };
    uint32_t checksum;
};
enum DataState {
    IDLE,
    HEADER_RECEIVED,
    SIZE_RECEIVED,
    STRING_DATA_RECEIVED,
    NUMERIC_DATA_RECEIVED,
    CHECKSUM_RECEIVED
};

class Dashboard {
private:
    uint32_t crc32(const char *s, size_t n) {
        uint32_t crc = 0xFFFFFFFF;

        for (size_t i = 0; i < n; i++) {
            char ch = s[i];
            for (size_t j = 0; j < 8; j++) {
                uint32_t b = (ch ^ crc) & 1;
                crc >>= 1;
                if (b) crc = crc ^ 0xEDB88320;
                ch >>= 1;
            }
        }

        return ~crc;
    }

public:
    struct Packet received_packet;
    DataState dataState = IDLE;

    char buffer[buffer_size] = {0};
    char alertBuffer[alertBufferSize] = {0};
    char numBuffer[numBufferSize] = {0};
    char floatBuffer[floatBufferSize] = {0};

    int floatCount = 0;
    int intCount = 0;
    char floatMapKeys[mapSize][4] = {{0}};
    char integerMapKeys[mapSize][4] = {{0}};
    int *intMapValues[mapSize] = {nullptr};
    float *floatMapValues[mapSize] = {nullptr};
    char mapKeysBuffer[mapBufferSize] = {0};


    Dashboard() {
        memset(buffer, 0, buffer_size);
        memset(alertBuffer, 0, alertBufferSize);
        memset(mapKeysBuffer, 0, mapBufferSize);
        strcat(buffer, "CWC!");
        strcat(alertBuffer, "CWCA!");
        strcat(mapKeysBuffer, "CWCM!");
    }

    void alert(const char *s) {
        if (strlen(alertBuffer) + strlen(s) >= alertBufferSize) {
            memset(alertBuffer, 0, alertBufferSize);
            strcat(alertBuffer, "ERROR:TOO MUCH DATA;");
        } else {
            strcat(alertBuffer, s);
            strcat(alertBuffer, ";");
        }
    }

    void alert(int num) {
        integer_to_string(num);
        alert(numBuffer);
        memset(numBuffer, 0, numBufferSize);
    }

    void integer_to_string(int x) {
        sprintf(numBuffer, "%d", x);
    }

    void float_to_string(float x, int precision) {
        dtostrf(x, -1, precision, floatBuffer);
    }

    void telemetry(const char *s, const char *t) {
        if (strlen(s) + strlen(t) + strlen(buffer) >= buffer_size) {
            memset(buffer, 0, buffer_size);
            strcat(buffer, "ERROR:TOO MUCH DATA;");
            send();
        } else {
            strcat(buffer, s);
            strcat(buffer, ":");
            strcat(buffer, t);
            strcat(buffer, ";");
        }
    }

    void telemetry(const char *s, int t) {
        if (t > 32767 || t < -32768) {
            telemetry(s, "NUMBER OUT OF RANGE");
        } else {
            integer_to_string(t);
            telemetry(s, numBuffer);
            memset(numBuffer, 0, numBufferSize);
        }
    }

    void telemetry(const char *s, float t, int precision) {
        int currentFloatBufferSize = 1 + log10(abs(t)) + 1 + precision + 1;
        if (currentFloatBufferSize > floatBufferSize) {
            telemetry(s, "NUMBER OUT OF RANGE");
            return;
        }
        float_to_string(t, precision);
        telemetry(s, floatBuffer);
        memset(floatBuffer, 0, floatBufferSize);
    }

    void addLiveBoolean(const char *key, const char *v, bool *ptr) {
        if (intCount >= mapSize) {
            alert("Live Integer Value Buffer is filled");
            return;
        }
        memcpy(integerMapKeys[intCount], "B", 1);
        memcpy(integerMapKeys[intCount] + 1, key, 2);
        intMapValues[intCount] = reinterpret_cast<int *>(ptr);
        strcat(mapKeysBuffer, integerMapKeys[intCount]);
        strcat(mapKeysBuffer, ":");
        strcat(mapKeysBuffer, v);
        strcat(mapKeysBuffer, ";");
        intCount += 1;
    }

    void addLiveInteger(const char *key, const char *v, int *ptr) {
        if (intCount >= mapSize) {
            alert("Live Integer Value Buffer is filled");
            return;
        }
        memcpy(integerMapKeys[intCount], "I", 1);
        memcpy(integerMapKeys[intCount] + 1, key, 2);
        intMapValues[intCount] = ptr;

        strcat(mapKeysBuffer, integerMapKeys[intCount]);
        strcat(mapKeysBuffer, ":");
        strcat(mapKeysBuffer, v);
        strcat(mapKeysBuffer, ";");
        intCount += 1;
    }

    void addLiveFloat(const char *key, const char *v, float *ptr) {
        if (floatCount >= mapSize) {
            alert("Live Float Value Buffer is filled");
            return;
        }
        memcpy(floatMapKeys[floatCount], "F", 1);

        memcpy(floatMapKeys[floatCount] + 1, key, 2);
        floatMapValues[floatCount] = ptr;
        strcat(mapKeysBuffer, floatMapKeys[floatCount]);
        strcat(mapKeysBuffer, ":");
        strcat(mapKeysBuffer, v);
        strcat(mapKeysBuffer, ";");
        floatCount += 1;
    }

    void resetPacket() {
        received_packet.packet_id = 0;
        received_packet.packet_length = 0;
        received_packet.checksum = 0;
        memset(received_packet.data_buf, 0, 16);
    }

    void processData() {
        if (dataState == IDLE) {
            resetPacket();
            if (Serial.available() > 0) {
                uint8_t temp = Serial.read();
                if (temp == 13) {
                    received_packet.packet_id = temp;
                    dataState = HEADER_RECEIVED;
                    alert("ID received");
                } else {
                    alert("ID failed");
                }
            }
        } else if (dataState == HEADER_RECEIVED) {
            if (Serial.available() >= 2) {
                uint8_t least = Serial.read();
                uint8_t most = Serial.read();
                uint16_t res = most << 8 | least;
                if (res >= 15) {
                    dataState = IDLE;
                    alert("size corruption");
                } else {
                    received_packet.packet_length = res;
                    dataState = SIZE_RECEIVED;
                    alert("size received");
                }
            }
        } else if (dataState == SIZE_RECEIVED) {
            if (Serial.available() >= received_packet.packet_length) {
                for (uint16_t i = 0; i < received_packet.packet_length; i++) {
                    received_packet.data_buf[i] = Serial.read();
                }
                alert("string received");
                dataState = STRING_DATA_RECEIVED;
            }
        } else if (dataState == STRING_DATA_RECEIVED) {
            if (Serial.available() >= 4) {
                for (uint8_t i = 0; i < 4; i++) {
                    received_packet.data_buf[received_packet.packet_length + i] = Serial.read();
                }
                float f;
                memcpy(&f, received_packet.data_buf + received_packet.packet_length, 4);
                received_packet.float_data = f;
                alert(received_packet.int_data);
                alert(received_packet.data_buf);
                dataState = NUMERIC_DATA_RECEIVED;
            }
        } else if (dataState == NUMERIC_DATA_RECEIVED) {
            if (Serial.available() >= 4) {
                uint32_t result = 0;
                for (uint8_t i = 0; i < 4; i++) {
                    uint8_t t = Serial.read();
                    result |= ((uint32_t) t) << i * 8;
                }
                uint32_t calcCRC32 = crc32(received_packet.data_buf, received_packet.packet_length + 4);
                if (calcCRC32 == result) {
                    received_packet.checksum = result;
                    dataState = CHECKSUM_RECEIVED;
                    alert("checksum passed");

                } else {
                    alert("checksum failed");
                    dataState = IDLE;
                }

            }
        }
        if (dataState == CHECKSUM_RECEIVED) {
            switch (received_packet.packet_id) {
                case 13:
                    char type = received_packet.data_buf[0];
                    char tempBuf[4] = {0};
                    memcpy(tempBuf, received_packet.data_buf, 3);
                    if (type == 'I') {
                        for (int i = 0; i < intCount; i++) {
                            if (strcmp(integerMapKeys[i], tempBuf) == 0) {
                                *intMapValues[i] = received_packet.int_data;
                            }
                        }
                    } else if (type == 'F') {
                        for (int i = 0; i < floatCount; i++) {
                            if (strcmp(floatMapKeys[i], tempBuf) == 0) {
                                *floatMapValues[i] = received_packet.float_data;
                            }
                        }
                    } else {
                        for (int i = 0; i < intCount; i++) {
                            if (strcmp(integerMapKeys[i], tempBuf) == 0) {
                                if (received_packet.int_data == 0) {
                                    *((bool *) intMapValues[i]) = false;
                                } else {
                                    *((bool *) intMapValues[i]) = true;
                                }
                            }
                        }
                    }
                    break;
            }

            dataState = IDLE;
        }

    }

    void send() {
        processData();
        //alert(mapKeysBuffer);
        strcat(buffer, "@");
        Serial.print(buffer);
        strcat(alertBuffer, "@");
        Serial.print(alertBuffer);
        int len = strlen(mapKeysBuffer);
        if (mapKeysBuffer[len - 1] != '@') {
            strcat(mapKeysBuffer, "@");
        }
        Serial.print(mapKeysBuffer);
        memset(buffer, 0, buffer_size);
        memset(alertBuffer, 0, alertBufferSize);

        //memset(mapKeysBuffer, 0, mapBufferSize);
        strcat(buffer, "CWC!");
        strcat(alertBuffer, "CWCA!");
        //strcat(mapKeysBuffer, "CWCM!");
    }
};

inline Dashboard dashboard;
