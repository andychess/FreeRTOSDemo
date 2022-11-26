#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"

#define UART_NUM UART_NUM_0
#define ECHO_TEST_TXD GPIO_NUM_1  
#define ECHO_TEST_RXD  GPIO_NUM_3
#define BUF_SIZE 1024
#define BAUD_RATE 115200
#define RX_FLW_CTL_THR 122

extern "C" int app_main();

//an example of echo test without hardware flow control on UART1
void echoTask(void *params)
{
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    
        .rx_flow_ctrl_thresh = RX_FLW_CTL_THR,
        .source_clk = UART_SCLK_APB,
    };

    //Configure UART1 parameters
    uart_param_config(UART_NUM, &uart_config);
    //Set UART1 pins(TX: IO4, RX: I05)
    uart_set_pin(UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    //Install UART driver (we don't need an event queue here)
    //In this example we don't even use a buffer for sending data.
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    while(true) {
        //Read data from UART
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //Write data back to UART
        uart_write_bytes(UART_NUM, (const char*) data, len);
    }
}

int app_main()
{
    //A uart read/write example without event queue;
    xTaskCreate(echoTask, "uart_echo_task", 1024, NULL, 10, NULL);
    return 1;
}