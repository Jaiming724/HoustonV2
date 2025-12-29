#include <stdio.h>
#include <esp_check.h>
#include <math.h>
#include <esp_rom_uart.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#include "Dashboard.h"
#include "MCP4725.h"
#include "ADS1115.h"
#include "DeltaTime.h"
#include "Util.h"
#include "Servo.h"

static const char *TAG = "Main";

#define I2C_PORT            0                   // I2C_NUM_0
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define I2C_FREQ_HZ         100000
#define UART_PORT UART_NUM_0
#define BUF_SIZE  1024
#define V_SNS1_Ratio 15.87f
#define Step_Ratio 0.000244f
enum {
    ADS1115_I2C_Addr = 0x48, // ADDR -> GND
    DAC1_I2C_ADDR = 0x60,
    DAC2_I2C_ADDR = 0x61,
    Conversion_Register_Addr = 00,
    Config_Register_Addr = 01,
    Sample_Time_MS = 4,
    Infinite_Resistance = 999999,
};

bool send_data(const char *data, uint32_t size) {
    uart_write_bytes(UART_PORT, data, size);
    return true;
}

uint32_t read_uart(char *buf, uint32_t size) {
    return uart_read_bytes(UART_PORT, buf, size, pdMS_TO_TICKS(100));
}

bool get_uart_data_size(uint32_t *size) {
    uart_get_buffered_data_len(UART_PORT, (size_t *) size);
    return true;
}

float f1 = 3.3f;
uint32_t u1 = 123456;
bool b1 = true;

void app_main(void) {


    uart_config_t uart_config = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
    };


    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE, BUF_SIZE, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    Dashboard_t dashboard;
    Dashboard_Init(&dashboard, send_data, read_uart, get_uart_data_size);
    if (Dashboard_Register_LiveData(&dashboard, 1, &f1, TYPE_FLOAT) != DASHBOARD_OK) {
        ESP_LOGE(TAG, "Failed to register live data");
    }
    if (Dashboard_Register_LiveData(&dashboard, 2, &u1, TYPE_UINT32) != DASHBOARD_OK) {
        ESP_LOGE(TAG, "Failed to register live data");
    }
    if (Dashboard_Register_LiveData(&dashboard, 3, &b1, TYPE_BOOL) != DASHBOARD_OK) {
        ESP_LOGE(TAG, "Failed to register live data");
    }

    while (1) {
        Dashboard_Telemetry_Uint32(&dashboard, "LiveCount", dashboard.liveDataCount);
        if (Dashboard_Telemetry_Float(&dashboard, "Temperature", f1) != DASHBOARD_OK) {
            ESP_LOGE(TAG, "Failed to send telemetry");
        }
        if (Dashboard_Telemetry_Uint32(&dashboard, "Pressure", u1) != DASHBOARD_OK) {
            ESP_LOGE(TAG, "Failed to send telemetry");
        }
        if (Dashboard_Telemetry_Str(&dashboard, "Status", "All systems nominal") != DASHBOARD_OK) {
            ESP_LOGE(TAG, "Failed to send telemetry");
        }
        if(b1){
            Dashboard_Telemetry_Str(&dashboard, "Alert", "True");
        }else{
            Dashboard_Telemetry_Str(&dashboard, "Alert", "False");

        }
        if (Dashboard_Telemetry_Int32(&dashboard, "Altitude", -50) != DASHBOARD_OK) {
            ESP_LOGE(TAG, "Failed to send telemetry");
        }
        Dashboard_Update(&dashboard);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // ESP_ERROR_CHECK(i2c_master_bus_rm_device(ads));
    // ESP_ERROR_CHECK(i2c_del_master_bus(bus));
}
