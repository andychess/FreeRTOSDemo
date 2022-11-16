#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

using namespace std;

#define MS portTICK_PERIOD_MS

#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

//Simple que example.
static const uint8_t msg_queue_lgth = 5;

static QueueHandle_t msg_queue;

void printMessage(void *param){
    int item = 1;
    while(true){
        if (xQueueReceive(msg_queue, (void *)&item, 0) == pdTRUE) 
            std::cout << item << std::endl;
    }
    vTaskDelay(1000 / MS);
}

extern "C" void app_main();

void app_main() {
    std::cout << std::endl;
    std::cout << "---FreeRTOS Demo---" << std::endl;
    std::cout << std::endl;
    std::cout << "Setup and loop task running on core: ";
    std::cout << xPortGetCoreID();
    std::cout << " with priority ";
    std::cout << uxTaskPriorityGet(NULL) << std::endl;

    msg_queue = xQueueCreate(msg_queue_lgth, sizeof(int));

    xTaskCreatePinnedToCore(printMessage,
                            "Print Msg",
                            1500,
                            NULL,
                            tskIDLE_PRIORITY,
                            NULL,
                            app_cpu);

    while (true){
        static int num = 0;

        if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE)
            std::cout << "Queue full." << std::endl;

        num++;
        vTaskDelay(1000 / MS);   
    }
}