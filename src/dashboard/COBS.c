#include "COBS.h"

COBS_Encoding_Status_t cobs_encode(uint8_t *buf, uint32_t buf_size, uint8_t *dest, uint32_t *dest_size) {
    uint32_t read_index = 0;
    uint32_t write_index = 1; // Start at 1, index 0 is reserved for the first code
    uint32_t code_index = 0;  // Location of the overhead byte we need to patch later
    uint8_t counter = 1;
    if (*dest_size < 1 || buf_size < 1) return COBS_ENCODING_NOT_ENOUGH_SPACE;

    while (read_index < buf_size) {
        if (buf[read_index] == 0) {
            dest[code_index] = counter;
            code_index = write_index;
            counter = 1;
            if (write_index >= *dest_size) return COBS_ENCODING_NOT_ENOUGH_SPACE;
            write_index++;
        } else if (counter == 0xFF) {
            dest[code_index] = counter;
            counter = 1;

            code_index = write_index;
            if (write_index >= *dest_size) return COBS_ENCODING_NOT_ENOUGH_SPACE;
            write_index++;

            if (write_index >= *dest_size) return COBS_ENCODING_NOT_ENOUGH_SPACE;
            dest[write_index] = buf[read_index];
            write_index++;
            counter++;
        } else {
            if (write_index >= *dest_size) return COBS_ENCODING_NOT_ENOUGH_SPACE;
            dest[write_index] = buf[read_index];
            write_index++;
            counter++;
        }
        read_index++;
    }
    dest[code_index] = counter;
    if (write_index >= *dest_size) return COBS_ENCODING_NOT_ENOUGH_SPACE;
    dest[write_index] = 0;
    write_index++;
    *dest_size = write_index;
    return COBS_ENCODING_OK;
}

COBS_Decoding_Status_t cobs_decode(uint8_t *buf, uint32_t buf_size, uint32_t *write_size) {
    if (buf_size <= 1) {
        return COBS_DECODING_INVALID_BUFFER_SIZE;
    }
    uint32_t read_idx = 1;
    uint32_t write_idx = 0;
    uint8_t counter = buf[0];
    while (counter != 0) {

        if (read_idx + counter - 1 >= buf_size) {
            return COBS_DECODE_INVALID_DATA_SIZE;
        }

        for (uint32_t i = 0; i < counter - 1; i++) {
            buf[write_idx] = buf[read_idx + i];
            write_idx++;
        }

        read_idx = read_idx + counter - 1;

        uint8_t next_counter = buf[read_idx];
        read_idx++;

        if (counter != 0xFF && next_counter != 0) {
            buf[write_idx] = 0;
            write_idx++;
        }
        counter = next_counter;
    }
    if (read_idx != buf_size) {
        return COBS_DECODE_INVALID_DATA_SIZE;
    }
    *write_size = write_idx;
    return COBS_DECODING_OK;
}