#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define UART_NUM UART_NUM_0
#define ECHO_TEST_TXD GPIO_NUM_1  
#define ECHO_TEST_RXD  GPIO_NUM_3
#define BUF_SIZE 1024
#define BAUD_RATE 115200
#define RX_FLW_CTL_THR 122

static const char *TAG = "UART TEST";

extern "C" int app_main();

static uint8_t *data;
static int len = 0;

void initHW(){
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    
        .rx_flow_ctrl_thresh = RX_FLW_CTL_THR,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
}

void readTask(void *params)
{
    std::cout << uart_is_driver_installed(UART_NUM) << std::endl;

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while(true) {
        //Read data from UART
        len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        //Write data back to UART
        uart_write_bytes(UART_NUM, (const char *) data, len);
        if (len) {
            data[len] = '\0';
            ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        }
    }
}

void writeTask(void *params)
{
    while(true) {
        //Write data back to UART
        uart_write_bytes(UART_NUM, (const char *) data, len);
    }
}

int app_main()
{
    initHW();
    //A uart read/write example without event queue;
    xTaskCreate(readTask, "uart_read_task", 2048, NULL, 10, NULL);
    //xTaskCreate(writeTask, "uart_write_task",2044, NULL, 10, NULL);
    return 1;
}
