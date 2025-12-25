#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COBS_ENCODING_OK = 0,
    COBS_ENCODING_NOT_ENOUGH_SPACE
} COBS_Encoding_Status_t;
typedef enum {
    COBS_DECODING_OK = 0,
    COBS_DECODING_INVALID_BUFFER_SIZE,
    COBS_DECODE_INVALID_DATA_SIZE
} COBS_Decoding_Status_t;

COBS_Encoding_Status_t cobs_encode(uint8_t *buf, uint32_t buf_size, uint8_t *dest, uint32_t *dest_size);

COBS_Decoding_Status_t cobs_decode(uint8_t *buf, uint32_t buf_size, uint32_t *write_size);

#ifdef __cplusplus
}
#endif

